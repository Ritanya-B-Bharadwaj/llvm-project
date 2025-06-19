import json
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
PR_DATA_PATH = PROJECT_ROOT / "data" / "openmp_prs.json"
OUTPUT_PATH = PROJECT_ROOT / "data" / "openmp_hunks_comments.json"

def load_prs():
    with open(PR_DATA_PATH) as f:
        return json.load(f)

def build_hunk_comment_mappings(prs):
    mappings = []
    
    for pr in prs:
        pr_number = pr["pr_number"]
        review_comments = pr.get("review_comments", [])
        issue_comments = pr.get("issue_comments", [])
        
        # Group by file for hunk mapping (simplified logic)
        file_to_comments = {}
        print(f"PR #{pr_number}: {len(review_comments)} review comments, {len(issue_comments)} issue comments")

        for c in review_comments:
            if c.get("path") and c.get("position") is not None:
                comment_obj = {
                    "body": c["body"],
                    "line": c.get("line"),
                    "position": c["position"],
                    "user": c["user"],
                    "type": "review"
                }
                file_to_comments.setdefault(c["path"], []).append(comment_obj)
                if c.get("path") and c.get("position") is not None:
                    print(f"✅ Mappable comment on {c['path']} at position {c['position']}")


        # Map each file's comments to a "hunk" (simplified as one per file)
        for file_path, comments in file_to_comments.items():
            mappings.append({
                "pr_number": pr_number,
                "file_path": file_path,
                "comments": comments
            })

        # Add issue comments under a special pseudo-path
        if issue_comments:
            issue_objs = [
                {
                    "body": c["body"],
                    "user": c["user"],
                    "type": "issue"
                }
                for c in issue_comments
            ]
            mappings.append({
                "pr_number": pr_number,
                "file_path": "[general:issue_comments]",
                "comments": issue_objs
            })

    return mappings

def main():
    prs = load_prs()
    mappings = build_hunk_comment_mappings(prs)

    with open(OUTPUT_PATH, "w") as f:
        json.dump(mappings, f, indent=2)

    print(f"✅ Saved {len(mappings)} hunk-comment mappings to {OUTPUT_PATH}")

if __name__ == "__main__":
    main()
