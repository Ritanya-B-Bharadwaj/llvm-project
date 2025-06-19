# import json
# import os
# import faiss
# from dotenv import load_dotenv
# from sentence_transformers import SentenceTransformer
# import google.generativeai as genai
# import re

# # === Load environment ===
# load_dotenv()
# GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
# if not GEMINI_API_KEY:
#     raise ValueError("Missing GEMINI_API_KEY in .env")

# # === Setup Gemini ===
# genai.configure(api_key=GEMINI_API_KEY)
# gemini = genai.GenerativeModel("gemini-pro")

# # === Config ===
# INDEX_PATH = "data/faiss_index.idx"
# META_PATH = "data/comments_lookup.json"
# EMBED_MODEL_NAME = "all-MiniLM-L6-v2"
# TOP_K = 5

# # === Load components ===
# print("📦 Loading FAISS index and metadata...")
# index = faiss.read_index(INDEX_PATH)
# with open(META_PATH, "rb") as f:
#     metadata = json.load(f)

# embedder = SentenceTransformer(EMBED_MODEL_NAME)

# # === Helper: Filter out low-quality comments ===
# def is_high_quality(comment: str) -> bool:
#     if not comment.strip():
#         return False
#     if re.search(r'\bLGTM\b|\blooks good\b|\bthank', comment, re.IGNORECASE):
#         return False
#     if len(comment.split()) < 4:
#         return False
#     return True

# # === Suggest review comments for new hunks ===
# def suggest_comments(pr_hunks: list):
#     hunk_texts = [h["hunk"] for h in pr_hunks]
#     hunk_embeds = embedder.encode(hunk_texts, convert_to_numpy=True)
#     D, I = index.search(hunk_embeds, TOP_K)

#     all_results = []

#     for i, hunk in enumerate(pr_hunks):
#         similar = [metadata[j] for j in I[i]]
#         examples = "\n".join(
#             f"<diff>\n{e['hunk']}\n</diff>\n→ {e['comment']}" for e in similar
#         )

#         prompt = f"""
# You are reviewing this diff:
# <diff>
# {hunk['hunk']}
# </diff>

# Similar comments from past:
# {examples}

# Write a useful code review comment:
# """.strip()

#         print(f"🔍 Generating comment for {hunk['file']}:{hunk['line_start']}")
#         try:
#             response = gemini.generate_content(prompt)
#             suggestion = response.text.strip()
#             if is_high_quality(suggestion):
#                 all_results.append({
#                     "file": hunk["file"],
#                     "line": hunk["line_start"],
#                     "comment": suggestion
#                 })
#         except Exception as e:
#             print(f"⚠️ Gemini generation failed: {e}")

#     return all_results

import os
import json
import faiss
import numpy as np
from pathlib import Path
from sentence_transformers import SentenceTransformer
import google.generativeai as genai
from diff_utils import fetch_pr_diff_files  # Replace with your actual function

# --- CONFIG ---
DATA_PATH = "data/comments_lookup.json"
INDEX_PATH = "data/faiss_index.idx"
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
index = faiss.read_index(INDEX_PATH)
with open(DATA_PATH) as f:
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

# def generate_review_comment(diff, similar_examples):
#     prompt = f"""You are reviewing this diff:\n{diff}\n\n"""
#     for i, (file_path, comment_block) in enumerate(similar_examples, 1):
#         prompt += f"Similar past example {i} from `{file_path}`:\n{comment_block}\n\n"
#     prompt += "Write a useful code review comment."

#     response = model.generate_content(prompt)
#     return response.text.strip()
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

# python src/suggest_code_review.py 98547 143221
