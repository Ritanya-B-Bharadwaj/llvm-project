import json
import re
import requests
from tkinter import Tk, Label, Entry, Button, Text, Scrollbar, END, messagebox
from tkinter.scrolledtext import ScrolledText
from groq import Groq
from pdfminer.high_level import extract_text

# ========== CONFIG ==========
REPO = "llvm/llvm-project"
MODEL_NAME = "llama3-70b-8192"
PDF_PATH = "OpenMPSpec.pdf"
# API = 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
# ============================
client = Groq()

# ----------------------------
def fetch_and_save_pr_data(pr_number):
    files_url = f"https://api.github.com/repos/{REPO}/pulls/{pr_number}/files"
    pr_url = f"https://api.github.com/repos/{REPO}/pulls/{pr_number}"

    files_res = requests.get(files_url)
    pr_res = requests.get(pr_url)

    if files_res.status_code != 200 or pr_res.status_code != 200:
        raise Exception(f"Failed to fetch PR data. Status codes: files={files_res.status_code}, pr={pr_res.status_code}")

    files_data = files_res.json()
    pr_data = pr_res.json()

    patch_path = f"pr_{pr_number}_files.json"
    title_path = f"pr_{pr_number}.json"

    with open(patch_path, "w") as f:
        json.dump(files_data, f, indent=2)
    with open(title_path, "w") as f:
        json.dump(pr_data, f, indent=2)

    return patch_path, title_path

# ----------------------------
def load_spec_text(pdf_path):
    return extract_text(pdf_path)

def load_patches(patch_path):
    with open(patch_path, 'r') as f:
        return json.load(f)

def load_titles(title_path, filenames):
    with open(title_path, 'r') as f:
        data = json.load(f)
        title = data.get("title", "No title provided")
        return {fname: title for fname in filenames}

def extract_keywords(text):
    return set(re.findall(r'\b\w+\b', text.lower())) - {
        'a', 'an', 'the', 'in', 'of', 'to', 'and', 'is', 'for', 'int', 'float', 'if', 'else', 'return', 'void', 'bool'
    }

def find_spec_section_by_keywords(spec_text, keywords, min_score=2):
    lines = spec_text.split('\n')
    candidates = []

    for i, line in enumerate(lines):
        line_lower = line.lower()
        match_score = sum(1 for word in keywords if word in line_lower)
        if match_score >= min_score:
            section_header = None
            for j in range(i, max(i - 10, 0), -1):
                m = re.match(r'(Section|chapter|Clause)?\s*([0-9.]+)', lines[j], re.IGNORECASE)
                if m:
                    section_header = m.group(2)
                    break
            if section_header:
                candidates.append((match_score, section_header, lines[i].strip()))

    if candidates:
        candidates.sort(reverse=True)
        return candidates[0][1]
    return "N/A"

def summarize_patch(patch, section_text):
    prompt = f"""
        You are reviewing a GitHub pull request. Below is the code patch and a relevant specification section.

        Summarize the purpose of this pull request, explaining what functionality it adds, changes, or fixes. 
        Be concise and clear.

        Patch:
        {patch}

        Relevant Spec Section:
        {section_text}
        """
    response = client.chat.completions.create(
        model=MODEL_NAME,
        messages=[{"role": "user", "content": prompt}],
        temperature=0.7,
        max_tokens=1024,
    )
    return response.choices[0].message.content

# ----------------------------
# GUI Integration
# ----------------------------
def run_analysis():
    pr_number = entry.get().strip()
    output_text.delete(1.0, END)

    if not pr_number.isdigit():
        messagebox.showerror("Input Error", "Please enter a valid numeric PR number.")
        return

    try:
        output_text.insert(END, f"Fetching PR #{pr_number}...\n")
        patch_path, title_path = fetch_and_save_pr_data(pr_number)
        patches = load_patches(patch_path)
        filenames = [p['filename'] for p in patches]
        title_lookup = load_titles(title_path, filenames)
        spec_text = load_spec_text(PDF_PATH)

        for i, patch_item in enumerate(patches):
            filename = patch_item.get('filename', 'unknown')
            patch = patch_item.get('patch', '')
            title = title_lookup.get(filename, "")

            title_keywords = extract_keywords(title)
            patch_keywords = extract_keywords(patch)

            section_id = find_spec_section_by_keywords(spec_text, title_keywords)
            if section_id == "N/A":
                section_id = find_spec_section_by_keywords(spec_text, patch_keywords)

            section_info = f"Section {section_id}" if section_id != "N/A" else "No relevant section found"
            summary = summarize_patch(patch, section_info)

            output_text.insert(END, f"\n--- Patch {i+1}: {filename} ---\n")
            output_text.insert(END, f"{summary}\n")
            output_text.insert(END, "-" * 60 + "\n")

        output_text.insert(END, "Done.\n")

    except Exception as e:
        messagebox.showerror("Error", str(e))

# ----------------------------
# Build GUI
# ----------------------------
root = Tk()
root.title("OpenMP PR Analyzer")

Label(root, text="Enter GitHub PR Number:").grid(row=0, column=0, padx=10, pady=10, sticky="w")
entry = Entry(root, width=30)
entry.grid(row=0, column=1, padx=10, pady=10)

Button(root, text="Summarize", command=run_analysis, bg="lightblue").grid(row=0, column=2, padx=10, pady=10)

output_text = ScrolledText(root, width=100, height=30, wrap="word")
output_text.grid(row=1, column=0, columnspan=3, padx=10, pady=10)

root.mainloop()
