import os
import json
import faiss
import numpy as np
from pathlib import Path
from sentence_transformers import SentenceTransformer
import google.generativeai as genai
from diff_utils import fetch_pr_diff_files  # Replace with your actual function

# --- CONFIG ---
BASE_DIR = Path(__file__).resolve().parent.parent  # Goes from /src/ to root
DATA_PATH = BASE_DIR / "data" / "comments_lookup.json"
INDEX_PATH = BASE_DIR / "data" / "faiss_index.idx"
EMBEDDING_MODEL = "all-MiniLM-L6-v2"  # Fast and small
NUM_NEIGHBORS = 5
GENERATION_MODEL = "gemini-2.0-flash"
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
assert GEMINI_API_KEY, "Missing GEMINI_API_KEY in environment"

# --- INIT ---
genai.configure(api_key=GEMINI_API_KEY)
embedder = SentenceTransformer(EMBEDDING_MODEL)
model = genai.GenerativeModel(GENERATION_MODEL)

# --- Load FAISS index + metadata ---
index = faiss.read_index(str(INDEX_PATH))
with open(str(DATA_PATH)) as f:
    raw = json.load(f)
    metadata = raw["queries"]

def embed_hunk(text):
    vec = embedder.encode([text])
    return vec.astype('float32')

def retrieve_similar_comments(hunk_text, k=NUM_NEIGHBORS):
    query_vec = embed_hunk(hunk_text)
    D, I = index.search(query_vec, k)
    results = []
    for idx in I[0]:
        if idx < len(metadata):
            entry = metadata[int(idx)]
            example_comments = f"- {entry.strip()}"
            results.append(("", example_comments))
    return results

def generate_review_comment(diff, similar_examples):
    prompt = f"""You are a concise, expert code reviewer.

Below is a code diff under review:
{diff}

Here are similar examples of past review comments:"""

    for i, (file_path, comment_block) in enumerate(similar_examples, 1):
        prompt += f"\n\nExample {i} from `{file_path}`:\n{comment_block}"

    prompt += """

Write a short, clear review comment (1–2 sentences max). Be specific and avoid repetition or unnecessary explanations."""

    response = model.generate_content(prompt)
    return response.text.strip()

def main(pr_number):
    print(f"📥 Fetching hunks for PR #{pr_number}")
    hunks = fetch_pr_diff_files(pr_number)
    for h in hunks:
        print(f"\n🔍 Hunk from {h['file']}, line {h['line_start']}")
        similar = retrieve_similar_comments(h['hunk'])
        comment = generate_review_comment(h['hunk'], similar)
        print(f"💬 Suggested Comment:\n{comment}\n{'-' * 80}")

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("pr_number", type=int, help="GitHub PR number")
    args = parser.parse_args()
    main(args.pr_number)