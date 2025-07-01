# import os
# import requests
# from dotenv import load_dotenv

# load_dotenv()
# GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
# OWNER = "llvm"
# REPO = "llvm-project"
# HEADERS = {
#     "Authorization": f"Bearer {GITHUB_TOKEN}",
#     "Accept": "application/vnd.github+json"
# }

# def get_recent_prs(limit=50):
#     prs = []
#     page = 1
#     per_page = 30

#     while len(prs) < limit:
#         url = f"https://api.github.com/repos/{OWNER}/{REPO}/pulls"
#         params = {
#             "state": "all",
#             "per_page": per_page,
#             "page": page
#         }
#         r = requests.get(url, headers=HEADERS, params=params)
#         if r.status_code != 200:
#             print("Failed to fetch PRs:", r.text)
#             break
#         page_data = r.json()
#         if not page_data:
#             break
#         prs.extend(page_data)
#         page += 1

#     return prs[:limit]

# def has_review_comments(pr_number):
#     url = f"https://api.github.com/repos/{OWNER}/{REPO}/pulls/{pr_number}/comments"
#     r = requests.get(url, headers=HEADERS)
#     if r.status_code != 200:
#         print(f"Failed to fetch review comments for PR #{pr_number}")
#         return False
#     comments = r.json()
#     for c in comments:
#         # Check that it's not a bot, has position, and path (i.e. reviewable)
#         if c.get("position") is not None and "bot" not in c["user"]["login"]:
#             return True
#     return False

# def main():
#     print("Searching for PRs with real review comments...")
#     prs = get_recent_prs(limit=100)
#     matching = []
#     for pr in prs:
#         pr_number = pr["number"]
#         print(f"Checking PR #{pr_number}...")
#         if has_review_comments(pr_number):
#             print(f"✅ PR #{pr_number} has review comments.")
#             matching.append({
#                 "number": pr["number"],
#                 "title": pr["title"],
#                 "diff_url": pr["diff_url"],
#                 "base": pr["base"],
#                 "head": pr["head"]
#             })
#         else:
#             print(f"❌ PR #{pr_number} has no review comments.")

#     print(f"\nFound {len(matching)} PRs with real review comments.")
#     for pr in matching:
#         print(f"- #{pr['number']}: {pr['title']}")

#     # Optionally, save this for further use
#     import json
#     with open("data/prs_with_review_comments.json", "w") as f:
#         json.dump(matching, f, indent=2)

# if __name__ == "__main__":
#     main()

# import os
# import requests
# from dotenv import load_dotenv
# import json

# load_dotenv()
# GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
# OWNER = "llvm"
# REPO = "llvm-project"
# HEADERS = {
#     "Authorization": f"Bearer {GITHUB_TOKEN}",
#     "Accept": "application/vnd.github+json"
# }

# START_PR = 143056   # 🔢 Set your start PR number
# END_PR = 143228     # 🔢 Set your end PR number

# def is_openmp_related(pr_number):
#     url = f"https://api.github.com/repos/{OWNER}/{REPO}/pulls/{pr_number}/files"
#     r = requests.get(url, headers=HEADERS)
#     if r.status_code != 200:
#         print(f"❌ Failed to fetch files for PR #{pr_number}: {r.text}")
#         return False
#     files = r.json()
#     for f in files:
#         filename = f.get("filename", "").lower()
#         if filename.startswith("openmp/") or "openmp" in filename or "omp" in filename:
#             return True
#     return False

# def has_review_comments(pr_number):
#     url = f"https://api.github.com/repos/{OWNER}/{REPO}/pulls/{pr_number}/comments"
#     r = requests.get(url, headers=HEADERS)
#     if r.status_code != 200:
#         print(f"❌ Failed to fetch comments for PR #{pr_number}: {r.text}")
#         return False
#     comments = r.json()
#     for c in comments:
#         if c.get("position") is not None and "bot" not in c["user"]["login"]:
#             return True
#     return False

# def get_pr_data(pr_number):
#     url = f"https://api.github.com/repos/{OWNER}/{REPO}/pulls/{pr_number}"
#     r = requests.get(url, headers=HEADERS)
#     if r.status_code != 200:
#         return None
#     return r.json()

# def main():
#     print(f"🔍 Scanning PRs from #{START_PR} to #{END_PR} for OpenMP-related content...")
#     matching = []

#     for pr_number in range(START_PR, END_PR + 1):
#         print(f"\n🔎 Checking PR #{pr_number}")
#         pr_data = get_pr_data(pr_number)
#         if not pr_data:
#             print(f"❌ PR #{pr_number} does not exist or failed to fetch.")
#             continue

#         if is_openmp_related(pr_number) and has_review_comments(pr_number):
#             print(f"✅ PR #{pr_number} is OpenMP-related and has review comments.")
#             matching.append({
#                 "number": pr_data["number"],
#                 "title": pr_data["title"],
#                 "url": pr_data["html_url"],
#                 "diff_url": pr_data["diff_url"],
#                 "base": pr_data["base"],
#                 "head": pr_data["head"]
#             })
#         else:
#             print(f"❌ PR #{pr_number} does not match criteria.")

#     print(f"\n🎯 Found {len(matching)} OpenMP-related PRs with human review comments.")
#     for pr in matching:
#         print(f"- #{pr['number']}: {pr['title']}")

#     with open("data/openmp_prs_by_range.json", "w") as f:
#         json.dump(matching, f, indent=2)

# if __name__ == "__main__":
#     main()

import os
import requests
import json
from dotenv import load_dotenv

load_dotenv()
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")
OWNER = "llvm"
REPO = "llvm-project"

HEADERS = {
    "Authorization": f"Bearer {GITHUB_TOKEN}",
    "Accept": "application/vnd.github+json"
}

def search_openmp_prs(max_pages=10):
    prs = []
    for page in range(1, max_pages + 1):
        print(f"🔍 Searching OpenMP PRs page {page}")
        url = "https://api.github.com/search/issues"
        params = {
            "q": f"repo:{OWNER}/{REPO} is:pr is:closed openmp",
            "per_page": 100,
            "page": page
        }
        r = requests.get(url, headers=HEADERS, params=params)
        if r.status_code != 200:
            print("❌ Failed:", r.text)
            break
        page_data = r.json()
        items = page_data.get("items", [])
        if not items:
            break
        prs.extend(items)
    return prs

def fetch_pr_metadata(pr_number):
    url = f"https://api.github.com/repos/{OWNER}/{REPO}/pulls/{pr_number}"
    r = requests.get(url, headers=HEADERS)
    if r.status_code != 200:
        print(f"❌ Failed to fetch PR #{pr_number}: {r.text}")
        return None
    return r.json()

def main():
    raw_prs = search_openmp_prs(max_pages=10)
    print(f"🔎 Found {len(raw_prs)} PRs mentioning 'openmp'")

    enriched = []

    for item in raw_prs:
        pr_number = item["number"]
        print(f"🔄 Enriching PR #{pr_number}")
        pr_meta = fetch_pr_metadata(pr_number)
        if not pr_meta:
            continue
        enriched.append({
            "number": pr_meta["number"],
            "title": pr_meta["title"],
            "url": pr_meta["html_url"],
            "diff_url": pr_meta["diff_url"],
            "base": pr_meta["base"],
            "head": pr_meta["head"]
        })

    os.makedirs("data", exist_ok=True)
    with open("data/openmp_indexed_closed_prs.json", "w") as f:
        json.dump(enriched, f, indent=2)

    print(f"\n✅ Saved {len(enriched)} enriched PRs to data/openmp_indexed_closed_prs.json")

if __name__ == "__main__":
    main()