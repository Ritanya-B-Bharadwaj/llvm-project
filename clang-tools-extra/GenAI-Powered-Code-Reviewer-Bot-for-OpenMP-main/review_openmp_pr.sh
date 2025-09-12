#!/bin/bash

# Ensure a PR number was passed
if [ -z "$1" ]; then
  echo "Usage: ./review_openmp_pr.sh <PR_NUMBER>"
  exit 1
fi

PR_NUMBER=$1

echo "🔎 Running code reviewer for PR #$PR_NUMBER"
python3 src/suggest_code_review.py "$PR_NUMBER"