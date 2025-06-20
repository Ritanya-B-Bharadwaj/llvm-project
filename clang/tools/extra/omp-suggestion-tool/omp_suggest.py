import os
import argparse
import joblib
from sentence_transformers import SentenceTransformer, util
import pandas as pd
import torch

# It's good practice to have the NLTK downloads in the main script as well
# in case the user runs this without running the processing scripts.
try:
    import nltk
    from nltk.corpus import stopwords
    from nltk.stem import PorterStemmer
    stopwords.words('english')
    nltk.data.find('tokenizers/punkt')
except (ImportError, LookupError):
    print("NLTK components not found. Please run: pip install nltk")
    print("Then run the following in a Python shell:\nimport nltk\nnltk.download('stopwords')\nnltk.download('punkt')")
    exit(1)

# --- Configuration ---
MODEL_FILE = os.path.join(os.path.dirname(__file__), "data", "model.joblib")
TOP_K_PRS = 10 # Number of similar PRs to consider for suggestions

# --- Text Preprocessing (re-used from 02_process_data.py) ---
# We must use the *exact* same preprocessing on the query as on the training data.
def preprocess_text(text):
    """Cleans and normalizes text for NLP processing."""
    if not text:
        return ""
    text = text.lower()
    tokens = nltk.word_tokenize(text)
    stop_words = set(stopwords.words('english'))
    stemmer = PorterStemmer()
    custom_stopwords = {'clang', 'llvm', 'openmp', 'omp', 'pr', 'fix'}
    stop_words.update(custom_stopwords)
    filtered_tokens = [
        stemmer.stem(w) for w in tokens if w.isalpha() and w not in stop_words and len(w) > 2
    ]
    return " ".join(filtered_tokens)

# --- Core Logic ---

def find_suggestions(feature_query, module_filter=None):
    """Loads the sentence embedding model and finds file suggestions."""
    # --- 1. Load Model Artifacts ---
    if not os.path.exists(MODEL_FILE):
        print(f"Error: Model file not found at {MODEL_FILE}")
        print("Please run the data fetching and training scripts first.")
        print("1. python scripts/01_fetch_prs.py")
        print("2. python scripts/02_process_data.py")
        print("3. python scripts/03_train_model.py (with sentence-transformers)")
        return

    print("Loading pre-trained model and data... (This may take a moment)")
    model_artifacts = joblib.load(MODEL_FILE)
    model_name = model_artifacts['model_name']
    corpus_embeddings = model_artifacts['corpus_embeddings']
    pr_data = model_artifacts['pr_data']

    # --- 2. Initialize Model and Process Query ---
    model = SentenceTransformer(model_name)
    print("Analyzing feature query...")
    # No preprocessing needed, the model handles it.
    query_embedding = model.encode(feature_query, convert_to_tensor=True)

    # --- 3. Find Similar PRs ---
    # Use the sentence-transformers `util.cos_sim` for efficient similarity calculation.
    # It returns a tensor of scores.
    print("Finding semantically similar PRs...")
    cosine_scores = util.cos_sim(query_embedding, corpus_embeddings)[0]
    
    # Use torch.topk to find the top K most similar PRs.
    top_results = torch.topk(cosine_scores, k=min(TOP_K_PRS, len(cosine_scores)))

    # --- 4. Aggregate and Rank Files ---
    print("Aggregating file suggestions from similar PRs...")
    suggested_files = []
    
    for score, idx in zip(top_results[0], top_results[1]):
        pr = pr_data[idx]
        for file_info in pr['files']:
            if module_filter and file_info['role'] != module_filter:
                continue
            
            suggested_files.append({
                "path": file_info['path'],
                "role": file_info['role'],
                "from_pr": pr['number'],
                "similarity": score.item() # Convert tensor score to float
            })

    if not suggested_files:
        print("\nCould not find any relevant files. The dataset may not contain this feature.")
        return

    # --- 5. Format and Display Results ---
    df = pd.DataFrame(suggested_files)
    
    # Calculate a confidence score based on the sum of similarities for each file.
    confidence_scores = df.groupby('path')['similarity'].sum()
    max_confidence = confidence_scores.max()
    
    # Create a results DataFrame with path, role, count, and normalized confidence.
    results_df = df.groupby(['path', 'role']).agg(
        count=('path', 'size'),
        total_similarity=('similarity', 'sum')
    ).reset_index()
    
    results_df['confidence'] = (results_df['total_similarity'] / max_confidence) * 100

    # Sort and display the results.
    results_df = results_df.sort_values(
        by=['confidence', 'count'], ascending=False
    ).drop(columns='total_similarity')

    print(f"\nSuggestions for '{feature_query}':")
    print("{:<65} {:<15} {:<10} {:<10}".format("Path", "Role", "Count", "Confidence"))
    print("-" * 100)
    for _, row in results_df.iterrows():
        print("{:<65} {:<15} {:<10} {:.1f}%".format(row['path'], row['role'], row['count'], row['confidence']))
    

# --- Main Execution ---

def main():
    parser = argparse.ArgumentParser(
        description="Suggests files to modify in Clang/LLVM for a given OpenMP feature.",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument(
        "--feature",
        type=str,
        required=True,
        help="A semantic description of the OpenMP feature, e.g., 'codegen for taskwait'."
    )
    parser.add_argument(
        "--module",
        type=str,
        choices=['parse', 'sema', 'codegen', 'ast', 'basic', 'test', 'runtime', 'llvm-transform', 'flang', 'other'],
        help="Optional: Filter suggestions by a specific module/role."
    )
    args = parser.parse_args()

    find_suggestions(args.feature, args.module)

if __name__ == "__main__":
    main() 