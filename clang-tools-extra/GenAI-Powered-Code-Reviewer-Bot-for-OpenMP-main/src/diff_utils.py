import requests
import os
from dotenv import load_dotenv

load_dotenv()
GITHUB_TOKEN = os.getenv("GITHUB_TOKEN")

HEADERS = {
    "Authorization": f"Bearer {GITHUB_TOKEN}",
    "Accept": "application/vnd.github.v3.diff"
}

def fetch_pr_diff_files(pr_number):
    url = f"https://api.github.com/repos/llvm/llvm-project/pulls/{pr_number}"
    diff_url = url + ".diff"
    response = requests.get(diff_url, headers=HEADERS)
    if response.status_code != 200:
        raise RuntimeError(f"Failed to fetch diff for PR #{pr_number}")

    raw_diff = response.text
    return parse_unified_diff(raw_diff)

def parse_unified_diff(diff_text):
    hunks = []
    current_file = None
    current_hunk = []
    recording = False
    start_line = None

    for line in diff_text.splitlines():
        if line.startswith("diff --git"):
            if current_file and current_hunk:
                hunks.append({
                    "file": current_file,
                    "hunk": "\n".join(current_hunk),
                    "line_start": start_line or 0
                })
            current_hunk = []
            current_file = None
            recording = False
            start_line = None

        elif line.startswith("+++ b/"):
            current_file = line[6:]

        elif line.startswith("@@"):
            recording = True
            current_hunk = [line]
            try:
                meta = line.split("@@")[1].strip().split(" ")[1]
                start_line = int(meta.split(",")[0].replace("+", ""))
            except Exception:
                start_line = 0

        elif recording:
            current_hunk.append(line)

    # Final one
    if current_file and current_hunk:
        hunks.append({
            "file": current_file,
            "hunk": "\n".join(current_hunk),
            "line_start": start_line or 0
        })

    return hunks
