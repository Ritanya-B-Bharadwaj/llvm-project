import os
import subprocess
import tempfile
import requests
from pathlib import Path
from clang_genai_review.parse_diff import parse_patch_string
from groq import Groq

GITHUB_RAW_BASE = "https://raw.githubusercontent.com"

def fetch_file_from_pr(repo: str, commit_sha: str, filepath: str) -> str:
    url = f"{GITHUB_RAW_BASE}/{repo}/{commit_sha}/{filepath}"
    resp = requests.get(url)
    if resp.status_code != 200:
        raise Exception(f"Failed to fetch {filepath} from GitHub at {commit_sha}. Status: {resp.status_code}")
    return resp.text

def run_clang_format(file_path):
    try:
        result = subprocess.run(
            ["clang-format", "--dry-run", "--Werror", str(file_path)],
            capture_output=True, text=True, check=False
        )
        return result.stdout.strip() + result.stderr.strip()
    except FileNotFoundError:
        return "❌ clang-format not found"

def run_clang_tidy(file_path):
    try:
        result = subprocess.run(
            ["clang-tidy", str(file_path), "--"],
            capture_output=True, text=True, check=False
        )
        return result.stdout.strip() + result.stderr.strip()
    except FileNotFoundError:
        return "❌ clang-tidy not found"

def groq_review(filename: str, line_data: dict) -> str:
    modified_lines = line_data.get(filename, [])
    modified_code = "\n".join(str(line) for line in modified_lines)

    if not modified_code.strip():
        return "⚠️ No meaningful changes found to review."

    prompt = f"""You are an expert LLVM code reviewer familiar with LLVM's official coding standards: https://llvm.org/docs/CodingStandards.html.

Analyze the following modified code in `{filename}` from a recent commit. You must:
1. Check if the code adheres to LLVM's coding and formatting practices.
2. Identify issues, including logic, performance, naming, formatting, or safety.
3. Acknowledge any good practices used.
4. Provide suggestions with clear reasoning if improvement is needed.

### Modified Code:
{modified_code}

Give your review in a clear, structured markdown-friendly format. Use:
✅ for good code,
⚠️ for minor issues,
❌ for serious problems,
💡 for suggestions.
"""

    try:
        client = Groq(api_key=os.environ["GROQ_API_KEY"])
        completion = client.chat.completions.create(
            model="meta-llama/llama-4-scout-17b-16e-instruct",
            messages=[{"role": "user", "content": prompt}],
            temperature=0.7,
            max_completion_tokens=1024,
            top_p=1,
            stream=False,
        )
        return completion.choices[0].message.content.strip()
    except Exception as e:
        return f"❌ GenAI review failed: {str(e)}"

def run_full_review(patch_text, repo, commit_sha, save_markdown=True, output_path="llvm_review.md"):
    parsed = parse_patch_string(patch_text)
    combined_reviews = {}

    with tempfile.TemporaryDirectory() as tmpdir:
        for file_diff in parsed:
            filename = file_diff.get("filename")
            modified_lines = file_diff.get("lines", [])

            if not filename or not modified_lines:
                continue

            local_file_path = Path(tmpdir) / filename
            os.makedirs(local_file_path.parent, exist_ok=True)

            try:
                file_content = fetch_file_from_pr(repo, commit_sha, filename)
                with open(local_file_path, "w", encoding="utf-8") as f:
                    f.write(file_content)
            except Exception:
                file_content = None

            if file_content:
                format_output = run_clang_format(local_file_path)
                tidy_output = run_clang_tidy(local_file_path)
            else:
                format_output = "❌ File not available locally, skipped clang-format"
                tidy_output = "❌ File not available locally, skipped clang-tidy"

            line_data = {filename: modified_lines}
            ai_review = groq_review(filename, line_data)

            combined_reviews[filename] = {
                "format": format_output,
                "tidy": tidy_output,
                "genai": ai_review
            }

    if save_markdown:
        os.makedirs(os.path.dirname(output_path), exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            f.write("# Combined LLVM Code Review\n\n")
            for file, content in combined_reviews.items():
                f.write(f"## `{file}`\n\n")
                f.write("### 🧹 clang-format Output\n```text\n")
                f.write(content["format"] + "\n```\n")
                f.write("### 🧪 clang-tidy Output\n```text\n")
                f.write(content["tidy"] + "\n```\n")
                f.write("### 🤖 GenAI Review\n```text\n")
                f.write(content["genai"] + "\n```\n\n---\n\n")

    return combined_reviews

def run_review_from_patch(patch_text: str, output_dir: str = "clang_genai_review/reviews",
                          pr_number: str = "manual", repo: str = "", commit_sha: str = "",
                          save_markdown: bool = True, output_path: str = None) -> str:
    if not repo or not commit_sha:
        raise ValueError("❌ 'repo' and 'commit_sha' are required for running clang tools.")

    if output_path is None:
        output_path = os.path.join(output_dir, f"pr_{pr_number}.md")

    run_full_review(patch_text, repo, commit_sha, save_markdown=save_markdown, output_path=output_path)

    print(f"[✅] Review saved to {os.path.abspath(output_path)}")
    return output_path
