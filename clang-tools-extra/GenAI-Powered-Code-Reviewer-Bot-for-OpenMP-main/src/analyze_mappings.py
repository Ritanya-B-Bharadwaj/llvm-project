import json
from pathlib import Path
import matplotlib.pyplot as plt # type: ignore

PROJECT_ROOT = Path(__file__).parent.parent
MAPPINGS_PATH = PROJECT_ROOT / "data" / "openmp_hunks_comments.json"

def load_mappings():
    with open(MAPPINGS_PATH, 'r') as f:
        return json.load(f)

def analyze_mappings(mappings):
    pr_comment_counts = {}
    comment_lengths = {"review": [], "issue": []}
    comment_counts = {"review": 0, "issue": 0}
    file_extensions = {}

    for item in mappings:
        pr_num = item['pr_number']
        file_ext = item['file_path'].split('.')[-1] if '.' in item['file_path'] else 'no_ext'
        
        if pr_num not in pr_comment_counts:
            pr_comment_counts[pr_num] = {"review": 0, "issue": 0}

        for comment in item['comments']:
            ctype = comment.get("type", "review")  # default to review if not tagged
            body = comment.get("body", "")
            
            comment_lengths[ctype].append(len(body))
            comment_counts[ctype] += 1
            pr_comment_counts[pr_num][ctype] += 1

            # Count file extensions only for review comments
            if ctype == "review":
                file_extensions[file_ext] = file_extensions.get(file_ext, 0) + 1

    stats = {
        "total_hunks_with_comments": len(mappings),
        "total_comments": comment_counts["review"] + comment_counts["issue"],
        "review_comments": comment_counts["review"],
        "issue_comments": comment_counts["issue"],
        "avg_review_comment_length": (
            sum(comment_lengths["review"]) / len(comment_lengths["review"])
            if comment_lengths["review"] else 0
        ),
        "avg_issue_comment_length": (
            sum(comment_lengths["issue"]) / len(comment_lengths["issue"])
            if comment_lengths["issue"] else 0
        ),
        "avg_review_comments_per_pr": (
            sum(c["review"] for c in pr_comment_counts.values()) / len(pr_comment_counts)
            if pr_comment_counts else 0
        ),
        "avg_issue_comments_per_pr": (
            sum(c["issue"] for c in pr_comment_counts.values()) / len(pr_comment_counts)
            if pr_comment_counts else 0
        ),
        "file_extensions": sorted(file_extensions.items(), key=lambda x: x[1], reverse=True)
    }

    return stats

def main():
    print("Loading hunk-comment mappings...")
    mappings = load_mappings()
    stats = analyze_mappings(mappings)

    print("\n=== Mapping Statistics ===")
    print(f"Total hunks with comments: {stats['total_hunks_with_comments']}")
    print(f"Total comments: {stats['total_comments']}")
    print(f"Review comments: {stats['review_comments']}")
    print(f"Issue comments: {stats['issue_comments']}")
    print(f"Avg. review comment length: {stats['avg_review_comment_length']:.2f} chars")
    print(f"Avg. issue comment length: {stats['avg_issue_comment_length']:.2f} chars")
    print(f"Avg. review comments per PR: {stats['avg_review_comments_per_pr']:.2f}")
    print(f"Avg. issue comments per PR: {stats['avg_issue_comments_per_pr']:.2f}")

    print("\nTop file extensions for review comments:")
    for ext, count in stats["file_extensions"][:5]:
        print(f"  .{ext}: {count}")

    stats_path = PROJECT_ROOT / "data" / "mapping_stats.json"
    with open(stats_path, "w") as f:
        json.dump(stats, f, indent=2)
    print(f"\n✅ Statistics saved to {stats_path}")

if __name__ == "__main__":
    main()
