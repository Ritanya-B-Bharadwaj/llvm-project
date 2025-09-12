# scripts/fetch_pr_diffs.py

import os
import json
import re
import nltk
from nltk.corpus import stopwords
from nltk.stem import PorterStemmer

# --- Configuration ---
RAW_DATA_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "raw")
PROCESSED_DATA_FILE = os.path.join(os.path.dirname(__file__), "..", "data", "processed", "processed_prs.json")
RELEVANT_EXTENSIONS = {'.cpp', '.h', '.c', '.hpp', '.inc', '.td'}

# --- Download NLTK data if not present ---
try:
    stopwords.words('english')
except LookupError:
    nltk.download('stopwords', quiet=True)
try:
    nltk.data.find('tokenizers/punkt')
except LookupError:
    nltk.download('punkt', quiet=True)

# --- Helper Functions ---

def get_file_role(filepath):
    """Categorizes a file into a role based on its path."""
    if 'clang' in filepath:
        if any(part in filepath for part in ['/lib/Parse/', 'Parse', '/lib/Parser/']):
            return 'parse'
        if any(part in filepath for part in ['/lib/Sema/', 'Sema']):
            return 'sema'
        if any(part in filepath for part in ['/lib/CodeGen/', 'CG']):
            return 'codegen'
        if any(part in filepath for part in ['/include/clang/AST/', 'AST']):
            return 'ast'
        if any(part in filepath for part in ['/include/clang/Basic/', 'Basic']):
            return 'basic'
        if '/test/' in filepath:
            return 'test'
        return 'clang-other'
    if 'openmp/runtime/' in filepath:
        return 'runtime'
    if 'llvm/lib/Transforms/' in filepath:
        return 'llvm-transform'
    if 'flang/lib' in filepath:
        return 'flang'
    return 'other'

def preprocess_text(text):
    """Cleans and normalizes text for NLP processing."""
    if not text:
        return ""
    text = re.sub(r'```.*?```', '', text, flags=re.DOTALL)
    text = re.sub(r'`[^`]*`', '', text)
    text = re.sub(r'[^a-zA-Z\s]', '', text.lower())
    tokens = nltk.word_tokenize(text)
    stop_words = set(stopwords.words('english'))
    stemmer = PorterStemmer()
    custom_stopwords = {'clang', 'llvm', 'openmp', 'omp', 'pr', 'fix'}
    stop_words.update(custom_stopwords)
    return " ".join([stemmer.stem(w) for w in tokens if w not in stop_words and len(w) > 2])

# --- Main Execution ---

def main():
    """Processes raw PR data into a structured format for model training."""
    print("Starting to process raw PR data...")
    if not os.path.exists(RAW_DATA_DIR) or not os.listdir(RAW_DATA_DIR):
        print(f"Error: Raw data directory is empty or not found at {RAW_DATA_DIR}")
        print("Please run '01_fetch_prs.py' first.")
        return

    processed_prs = []
    filenames = [f for f in os.listdir(RAW_DATA_DIR) if f.endswith('.json')]
    
    for i, filename in enumerate(filenames, 1):
        filepath = os.path.join(RAW_DATA_DIR, filename)
        try:
            with open(filepath, 'r', encoding='utf-8') as f:
                pr_data = json.load(f)
        except (json.JSONDecodeError, IOError) as e:
            print(f"Warning: Could not read or parse {filename}. Skipping. Error: {e}")
            continue

        text_to_process = f"{pr_data.get('title', '')} {pr_data.get('body', '')}"
        processed_text = preprocess_text(text_to_process)
        
        modified_files = []
        for file in pr_data.get('files', []):
            if isinstance(file, str) and any(file.endswith(ext) for ext in RELEVANT_EXTENSIONS):
                modified_files.append({"path": file, "role": get_file_role(file)})
            elif isinstance(file, dict) and 'filename' in file and any(file['filename'].endswith(ext) for ext in RELEVANT_EXTENSIONS):
                 modified_files.append({"path": file['filename'], "role": get_file_role(file['filename'])})

        if processed_text and modified_files:
            processed_prs.append({
                "number": pr_data.get('number'),
                "processed_text": processed_text,
                "files": modified_files
            })
    
    os.makedirs(os.path.dirname(PROCESSED_DATA_FILE), exist_ok=True)
    with open(PROCESSED_DATA_FILE, 'w', encoding='utf-8') as f:
        json.dump(processed_prs, f, indent=4)

    print(f"\nProcessing complete. Saved {len(processed_prs)} PRs to {PROCESSED_DATA_FILE}")

if __name__ == "__main__":
    main()
