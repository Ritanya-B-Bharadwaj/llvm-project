import requests

def fetch_patch_from_pr(pr_number, repo="llvm/llvm-project"):
    url = f"https://patch-diff.githubusercontent.com/raw/{repo}/pull/{pr_number}.patch"
    print(f"[INFO] Fetching patch from GitHub PR #{pr_number}...")
    resp = requests.get(url)

    if resp.status_code != 200:
        raise Exception(f"[❌] Failed to fetch PR #{pr_number} patch from GitHub. Status: {resp.status_code}")

    return resp.text
