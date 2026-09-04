import argparse
import os
from clang_genai_review.fetch_pr_patch import fetch_patch_from_pr
from clang_genai_review.genai_review import run_review_from_patch

def main():
    parser = argparse.ArgumentParser(description="GenAI Code Review for LLVM Pull Requests")
    parser.add_argument("--pr", type=int, help="LLVM PR number to review")
    parser.add_argument("--repo", type=str, default="llvm/llvm-project", help="GitHub repo name (e.g., llvm/llvm-project)")
    parser.add_argument("--sha", type=str, required=False, help="Commit SHA to fetch the changed files")
    parser.add_argument("--show", action="store_true", help="Show previously saved review if available")
    args = parser.parse_args()

    pr_number = args.pr
    repo = args.repo
    commit_sha = args.sha
    output_dir = "clang_genai_review/reviews"
    review_file = f"{output_dir}/pr_{pr_number}.md"

    if args.show:
        if os.path.exists(review_file):
            with open(review_file, "r", encoding="utf-8") as f:
                print(f.read())
        else:
            print(f"[❌] No saved review found for PR #{pr_number}")
        return

    if not pr_number:
        print("[❌] Please provide a PR number with --pr")
        return

    if not commit_sha:
        print("[❌] Please provide a commit SHA with --sha (required for file fetching)")
        return

    print(f"[🔍] Fetching patch for PR #{pr_number}...")
    patch = fetch_patch_from_pr(pr_number)

    print("[🧠] Running GenAI review...")
    os.makedirs(output_dir, exist_ok=True)
    run_review_from_patch(
        patch_text=patch,
        output_dir=output_dir,
        pr_number=pr_number,
        repo=repo,
        commit_sha=commit_sha
    )

    print(f"[✅] Review saved to {review_file}")

if __name__ == "__main__":
    main()
