import streamlit as st
import os

from clang_genai_review.fetch_pr_patch import fetch_patch_from_pr
from clang_genai_review.genai_review import run_review_from_patch

st.set_page_config(page_title="LLVM GenAI PR Reviewer", layout="wide")
st.title("🧠 LLVM PR Review (Clang + GenAI)")

pr_number = st.number_input("Enter LLVM PR Number", min_value=1,value=141116, step=1)
repo = st.text_input("GitHub Repo (e.g., llvm/llvm-project)", value="llvm/llvm-project")
commit_sha = st.text_input("Commit SHA (from PR)", placeholder="e.g., 97194f")

run = st.button("🔍 Analyze PR")

REVIEW_DIR = "reviews"
os.makedirs(REVIEW_DIR, exist_ok=True)

review_file = os.path.join(REVIEW_DIR, f"pr_{pr_number}.md")

if run:
    if not commit_sha:
        st.error("❌ Please provide the commit SHA.")
        st.stop()

    with st.spinner(f"Fetching patch for PR #{pr_number}..."):
        try:
            patch = fetch_patch_from_pr(pr_number)
            st.success("✅ Patch fetched successfully.")
        except Exception as e:
            st.error(f"❌ Error fetching patch: {e}")
            st.stop()

    with st.expander("📄 Show Patch"):
        st.code(patch, language="diff")

    with st.spinner("Running GenAI review..."):
        try:
            # Always overwrite the file by deleting if exists
            if os.path.exists(review_file):
                os.remove(review_file)

            run_review_from_patch(
                patch_text=patch,
                output_dir=REVIEW_DIR,
                pr_number=pr_number,
                repo=repo,
                commit_sha=commit_sha,
                save_markdown=True,
                output_path=review_file
            )
            st.success("✅ Review completed and saved.")
        except Exception as e:
            st.error(f"❌ Review failed: {e}")
            st.stop()

if pr_number and os.path.exists(review_file):
    with st.expander("📋 Show AI Review"):
        with open(review_file, "r", encoding="utf-8") as f:
            st.markdown(f.read())

    with open(review_file, "r", encoding="utf-8") as f:
        st.download_button("📥 Download Markdown Review", f.read(), file_name=os.path.basename(review_file))
else:
    st.info("ℹ️ No review found yet. Click Analyze PR to generate it.")
