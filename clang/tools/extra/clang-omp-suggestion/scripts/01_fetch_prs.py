# scripts/fetch_prs.py

import os
import requests
import json
from datetime import datetime, timedelta

# --- Configuration ---
# You can generate a token here: https://github.com/settings/tokens

GITHUB_TOKEN = os.environ.get("GITHUB_TOKEN", "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")
REPO_OWNER = "llvm"
REPO_NAME = "llvm-project"
SEARCH_QUERY = "OpenMP OR OMP in:title,body is:pr is:merged"
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "raw")

# --- Helper Functions ---

def get_headers():
    """Returns the appropriate headers for the GitHub API request."""
    headers = {"Accept": "application/vnd.github.v3+json"}
    if GITHUB_TOKEN:
        headers["Authorization"] = f"token {GITHUB_TOKEN}"
    return headers

def fetch_pull_requests(session, page):
    """Fetches a single page of pull requests."""
    url = f"https://api.github.com/search/issues"
    params = {
        "q": f"repo:{REPO_OWNER}/{REPO_NAME} {SEARCH_QUERY}",
        "sort": "created",
        "order": "desc",
        "per_page": 100,
        "page": page,
    }
    response = session.get(url, params=params, headers=get_headers())
    response.raise_for_status()
    return response.json()

def fetch_pr_details(session, pr_number):
    """Fetches detailed information for a single pull request, including files and commits."""
    base_url = f"https://api.github.com/repos/{REPO_OWNER}/{REPO_NAME}/pulls/{pr_number}"

    # Get main PR data
    pr_response = session.get(base_url, headers=get_headers())
    pr_response.raise_for_status()
    pr_data = pr_response.json()

    # Get files
    files_response = session.get(f"{base_url}/files", headers=get_headers())
    files_response.raise_for_status()
    files_data = [f["filename"] for f in files_response.json()]

    # Get commits
    commits_response = session.get(f"{base_url}/commits", headers=get_headers())
    commits_response.raise_for_status()
    commits_data = [c["commit"]["message"] for c in commits_response.json()]

    return {
        "number": pr_data["number"],
        "title": pr_data["title"],
        "body": pr_data["body"],
        "created_at": pr_data["created_at"],
        "merged_at": pr_data["merged_at"],
        "user": pr_data["user"]["login"],
        "files": files_data,
        "commits": commits_data,
    }

def save_pr_data(pr_data):
    """Saves the pull request data to a JSON file."""
    if not os.path.exists(OUTPUT_DIR):
        os.makedirs(OUTPUT_DIR)

    filepath = os.path.join(OUTPUT_DIR, f"pr_{pr_data['number']}.json")
    with open(filepath, "w", encoding="utf-8") as f:
        json.dump(pr_data, f, indent=4)
    print(f"Successfully saved data for PR #{pr_data['number']}")

# --- Main Execution ---

def main():
    """Main function to fetch and save all relevant OpenMP pull requests."""
    print("Starting to fetch OpenMP pull requests from llvm/llvm-project...")
    
    session = requests.Session()
    page = 1
    
    while True:
        try:
            print(f"\nFetching page {page}...")
            search_results = fetch_pull_requests(session, page)
            items = search_results.get("items", [])

            if not items:
                print("No more pull requests found.")
                break

            for item in items:
                pr_number = item["number"]
                output_file = os.path.join(OUTPUT_DIR, f"pr_{pr_number}.json")
                if os.path.exists(output_file):
                    print(f"Skipping PR #{pr_number} as it already exists.")
                    continue
                
                try:
                    pr_details = fetch_pr_details(session, pr_number)
                    save_pr_data(pr_details)
                except requests.exceptions.HTTPError as e:
                    print(f"Error fetching details for PR #{pr_number}: {e}")
                    if e.response.status_code == 403:
                        print("Rate limit likely exceeded. Stopping.")
                        return

            # Check if there's a next page by looking for a 'next' link in the response headers
            if 'next' not in response.links:
                 break
            
            page += 1

        except requests.exceptions.HTTPError as e:
            print(f"An error occurred: {e}")
            if e.response.status_code == 403:
                print("Rate limit likely exceeded. Stopping.")
            break
        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            break
            
    print("\nFinished fetching pull requests.")

if __name__ == "__main__":
    main()
