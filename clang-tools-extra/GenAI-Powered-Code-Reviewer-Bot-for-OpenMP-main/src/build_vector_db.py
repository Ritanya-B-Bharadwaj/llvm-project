import json
import faiss
import numpy as np
from sentence_transformers import SentenceTransformer

print("📥 Loading hunk-comment mappings...")
with open("data/openmp_hunks_comments.json", "r") as f:
    mappings = json.load(f)

model = SentenceTransformer("all-MiniLM-L6-v2")

diffs = []
comments = []

for item in mappings:
    file_path = item.get("file_path")
    pr_number = item.get("pr_number")
    comment_list = item.get("comments", [])

    for comment_obj in comment_list:
        comment_body = comment_obj.get("body")
        
        # Optional: use PR number + file path + line as context
        context = f"PR #{pr_number}, File: {file_path}"
        combined = f"{context}\n{comment_body}"

        diffs.append(combined)
        comments.append(comment_body)

print("🔢 Generating embeddings...")
embeddings = model.encode(diffs, convert_to_numpy=True)

print("📦 Creating FAISS index...")
dimension = embeddings.shape[1]
index = faiss.IndexFlatL2(dimension)
index.add(embeddings)

print("💾 Saving index + lookup...")
faiss.write_index(index, "data/faiss_index.idx")
with open("data/comments_lookup.json", "w") as f:
    json.dump({"queries": diffs, "comments": comments}, f)

print("✅ Vector DB ready: faiss_index.idx + comments_lookup.json")