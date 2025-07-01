import os
import json
import requests
from dotenv import load_dotenv
from pathlib import Path

# Load environment variables
load_dotenv()
GITHUB_TOKEN = os.getenv('GITHUB_TOKEN')

if not GITHUB_TOKEN:
    raise ValueError("GITHUB_TOKEN not found in environment variables.")

HEADERS = {
    "Authorization": f"Bearer {GITHUB_TOKEN}",
    "Accept": "application/vnd.github+json"
}

OWNER = "llvm"
REPO = "llvm-project"

def fetch_diff(pr):
    diff_url = pr['diff_url']
    response = requests.get(diff_url, headers=HEADERS)
    return response.text if response.status_code == 200 else ""

def fetch_review_comments(pr_number):
    url = f"https://api.github.com/repos/{OWNER}/{REPO}/pulls/{pr_number}/comments"
    r = requests.get(url, headers=HEADERS)
    return r.json() if r.status_code == 200 else []

def fetch_issue_comments(pr_number):
    url = f"https://api.github.com/repos/{OWNER}/{REPO}/issues/{pr_number}/comments"
    r = requests.get(url, headers=HEADERS)
    return r.json() if r.status_code == 200 else []

def main():
    data_dir = Path("data")
    data_dir.mkdir(exist_ok=True)

    # Load filtered PRs
    prs_path = data_dir / "openmp_indexed_closed_prs.json"
    if not prs_path.exists():
        raise FileNotFoundError("Missing prs_with_review_comments.json. Run find_prs_with_review_comments.py first.")

    with open(prs_path) as f:
        prs = json.load(f)

    all_data = []

    for pr in prs:
        pr_number = pr['number']
        try:
            print(f"🔄 Fetching PR #{pr_number}")
            diff = fetch_diff(pr)
            review_comments = fetch_review_comments(pr_number)
            issue_comments = fetch_issue_comments(pr_number)

            print(f"  ↳ {len(review_comments)} review comments, {len(issue_comments)} issue comments")

            all_data.append({
                "pr_number": pr_number,
                "title": pr["title"],
                "diff": diff,
                "base_sha": pr["base"]["sha"],
                "head_sha": pr["head"]["sha"],
                "review_comments": review_comments,
                "issue_comments": issue_comments
            })
        except Exception as e:
            print(f"❌ Unexpected error for PR #{pr_number}: {e}")

    output_file = data_dir / "openmp_prs.json"
    with open(output_file, "w") as f:
        json.dump(all_data, f, indent=2)

    print(f"\n✅ Saved processed data for {len(all_data)} PRs to {output_file}")

if __name__ == "__main__":
    main()
