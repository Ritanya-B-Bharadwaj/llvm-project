# run_review.py

from suggest_code_review import suggest_comments
from find_comments import fetch_review_comments  # Replace with actual module name

pr_number = 78999
pr_hunks = fetch_review_comments(pr_number)

results = suggest_comments(pr_hunks)

for r in results:
    print(f"📄 {r['file']}:{r['line']}")
    print(f"📝 {r['comment']}\n")