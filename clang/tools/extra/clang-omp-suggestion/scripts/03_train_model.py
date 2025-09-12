import os
import json
import joblib
from sentence_transformers import SentenceTransformer
import numpy as np

# --- Configuration ---
PROCESSED_DATA_FILE = os.path.join(os.path.dirname(__file__), "..", "data", "processed", "processed_prs.json")
MODEL_OUTPUT_FILE = os.path.join(os.path.dirname(__file__), "..", "data", "model.joblib")
MODEL_NAME = 'all-MiniLM-L6-v2' # A good starting point for a lightweight, high-quality model

# --- Main Execution ---

def main():
    """Trains the Sentence Transformer model and saves the artifacts."""
    print("Starting model training with Sentence Transformers...")

    # --- 1. Load Data ---
    print(f"Loading processed data from {PROCESSED_DATA_FILE}...")
    if not os.path.exists(PROCESSED_DATA_FILE):
        print(f"Error: Processed data file not found at {PROCESSED_DATA_FILE}")
        print("Please run '01_fetch_prs.py' and '02_process_data.py' first.")
        return

    with open(PROCESSED_DATA_FILE, 'r', encoding='utf-8') as f:
        processed_data = json.load(f)

    if not processed_data:
        print("Error: No data found in the processed file. Cannot train model.")
        return
        
    # The "corpus" is the text we want to create embeddings for.
    # We use the raw text here, as sentence transformers handle tokenization and normalization.
    corpus = [f"{pr.get('title', '')} {pr.get('body', '')}" for pr in processed_data]

    # --- 2. Load Model and Generate Embeddings ---
    print(f"Loading pre-trained Sentence Transformer model: {MODEL_NAME}...")
    # This will download the model from the Hugging Face Hub on its first run
    model = SentenceTransformer(MODEL_NAME)
    
    print("Generating embeddings for the PR corpus... (This may take a while)")
    # The model's `encode` method turns our text corpus into a matrix of vector embeddings.
    corpus_embeddings = model.encode(corpus, show_progress_bar=True)
    
    print("Embedding generation complete.")
    print(f"Corpus embeddings shape: {corpus_embeddings.shape}")

    # --- 3. Save Model Artifacts ---
    # For this model, we need to save the name of the transformer (to load it at inference time),
    # the generated embeddings, and the original PR data for lookups.
    model_artifacts = {
        'model_name': MODEL_NAME,
        'corpus_embeddings': corpus_embeddings,
        'pr_data': processed_data 
    }

    output_dir = os.path.dirname(MODEL_OUTPUT_FILE)
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    print(f"Saving model artifacts to {MODEL_OUTPUT_FILE}...")
    joblib.dump(model_artifacts, MODEL_OUTPUT_FILE)

    print("\nModel training and saving process completed successfully.")

if __name__ == "__main__":
    main()
