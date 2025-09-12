import argparse
import re
import requests
from generator import *
def get_pr_diff(repo: str, pr_number: int, github_token: str) -> str | None:
    """Fetches the diff of a pull request from GitHub."""
    print(f"🔎 Fetching diff for PR #{pr_number} from repository {repo}...")
    url = f"https://api.github.com/repos/{repo}/pulls/{pr_number}"
    headers = {
        "Accept": "application/vnd.github.v3.diff",
        "Authorization": f"token {github_token}"
    }
    try:
        response = requests.get(url, headers=headers)
        response.raise_for_status()
        return response.text
    except requests.exceptions.RequestException as e:
        print(f"❌ Error fetching from GitHub: {e}")
        return None


def extract_openmp_pragmas(diff_text: str, api_key: str, user_requirements: str, base_directory: str = "../llvm-project\llvm-project\clang\test\OpenMP") -> list[str]:
    pragma_pattern = re.compile(r'^\+\s*#pragma\s+omp\s+(.*)', re.IGNORECASE)
    pragmas = []

    for line in diff_text.splitlines():
        match = pragma_pattern.match(line)
        if match:
            full_clause = match.group(1).strip()

            # Basic split: pick major words like 'parallel for' or 'target simd'
            words = full_clause.split()
            base = []

            for word in words:
                # Skip clause parameters like (+:sum), (x), etc.
                clean_word = re.sub(r'\(.*?\)', '', word)
                if clean_word:
                    base.append(clean_word)

            if base:
                normalized = "_".join(base)
                pragmas.append(normalized)
    s = set(pragmas)
    for i in s:
        t = generate_openmp_test_skeleton(i, api_key, user_requirements, base_directory)
        print(t)
        if t:
            print(t)

        with open("test_" + i + ".cpp", "w") as f:
            f.write(t)
    return list(set(pragmas))  # Remove duplicates


# --- Main Execution ---
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Fetch GitHub PR diff.")
    parser.add_argument("pr_number", type=int, help="GitHub Pull Request number.")
    parser.add_argument("--repo", required=True, help="GitHub repo in format 'owner/repo' (e.g., 'llvm/llvm-project').")
    parser.add_argument("--token", required=True, help="Your GitHub personal access token.")
    parser.add_argument("--api_key", required=True, help="Your Gemini API key.")
    parser.add_argument("--user_requirements", required=True, help="User requirements for the test skeleton.")
    parser.add_argument("--base_directory", default="~/llvm-project/clang/test/OpenMP", help="Base directory for test files.")
    args = parser.parse_args()

    diff = get_pr_diff(args.repo, args.pr_number, args.token)

    if diff:
        print("\n✅ OpenMP Pragmas Detected:")
        pragmas = extract_openmp_pragmas(diff, args.api_key, args.user_requirements, args.base_directory)
        print(pragmas if pragmas else "None found.")
    else:
        print("⚠️ No diff fetched.")
