# 🧠 LLVM GenAI PR Reviewer

This project adds an automated code review assistant for LLVM pull requests. It integrates:

- `clang-format` for code formatting diagnostics
- `clang-tidy` for static code analysis
- 🤖 GenAI (via LLM) for human-like review feedback:
  - Readability suggestions
  - Style and naming critique
  - Potential bugs or improvements

It fetches a GitHub PR patch, analyzes the code locally, and generates a unified review report (Markdown format). You can run it either via a CLI tool or a Streamlit-based web UI.

---

## 📦 Installation

> ⚠️ Recommended: Use a Python virtual environment

```bash
# Clone the repository (forked llvm-project)
git clone https://github.com/ritanya-b-bharadwaj/llvm-project.git
cd llvm-project/clang_genai_review

# Set up virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies in editable mode
pip install -e .
```
🛠️ Requirements
Python 3.8+

clang-format and clang-tidy installed and available in PATH

🔧 Installing Clang Tools (Ubuntu/Debian)
bash
Copy
Edit
sudo apt update
sudo apt install clang-format clang-tidy
🔧 On Windows (via LLVM pre-built binaries)
Download from: https://github.com/llvm/llvm-project/releases

Add clang-format.exe and clang-tidy.exe to your system PATH

🚀 Usage
▶️ Run via CLI
bash
Copy
Edit
clang-genai-review --pr <PR_NUMBER> --repo llvm/llvm-project --sha <COMMIT_SHA>
Example:
bash
Copy
Edit
clang-genai-review --pr 144629 --repo llvm/llvm-project --sha abcdef1234567890
✅ Output will be saved to clang_genai_review/reviews/pr_<PR_NUMBER>.md

💻 Run via Web UI (Streamlit)
bash
Copy
Edit
streamlit run clang_genai_review/app.py
Then open the link shown in terminal (usually http://localhost:8501).

Fill in:

LLVM PR Number

Repository (e.g., llvm/llvm-project)

Commit SHA

Then click "Analyze PR" to generate and view the review interactively.

📁 Folder Structure
bash
Copy
Edit
clang_genai_review/
│
├── cli.py                    # CLI entry point
├── app.py                   # Streamlit UI
├── genai_review.py          # Main logic
├── fetch_pr_patch.py        # GitHub PR fetching
├── parse_diff.py            # Diff parser
├── groq_review.py           # GenAI interface
│
└── reviews/                 # Auto-generated markdown reviews
📄 Sample Output
You can explore example reviews in the reviews/ folder — these are real outputs from past PRs like #143422 and #144629.

🧪 For Testing
To test locally using a fork:

Fork the llvm/llvm-project repo.

Make a sample commit (e.g., whitespace or formatting change).

Push and create a PR.

Run the tool with the PR number and commit SHA.

📌 Contributing
This is a prototype for automating and accelerating LLVM code review with AI. Contributions, issue reports, and suggestions are welcome.

🧾 License
This project is built as part of LLVM tooling research and follows the LLVM Project license structure.

📣 Maintained by
ritanya-b-bharadwaj

markdown
Copy
Edit

---

### ✅ What You Should Do Now:
- Add this as `README.md` in `clang_genai_review/`
- Make sure `__init__.py` exists (even if empty)
- Test `pip install -e .` and CLI call from any directory
- Push to your fork and create a PR in `llvm-project`
- Reference the corresponding issue (if there’s one open)

Let me know if you want me to help generate that sample PR, or if you need a smaller test commit idea!







