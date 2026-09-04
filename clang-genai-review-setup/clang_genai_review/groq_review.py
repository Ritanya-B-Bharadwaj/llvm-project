import os

def groq_review(file_path: str, patch_lines: list[str]) -> str:
    """
    Simulated GenAI review of a patch.

    Args:
        file_path (str): Path to the file being reviewed.
        patch_lines (list[str]): The modified lines extracted from the diff.

    Returns:
        str: Simulated GenAI review string.
    """

    if not patch_lines:
        return f"✅ No code changes detected in `{file_path}` to review."

    # Simulated GenAI feedback for now
    review_lines = [
        f"🤖 **GenAI Review for `{file_path}`**",
        "",
        "```diff"
    ]

    # Add the patch content in diff style
    for line in patch_lines:
        review_lines.append(line)
    review_lines.append("```")
    review_lines.append("")
    review_lines.append("🧠 **Feedback**:")
    review_lines.append("The above changes appear structurally sound. Consider reviewing:")
    review_lines.append("- Code readability and formatting")
    review_lines.append("- Potential edge case handling")
    review_lines.append("- Naming conventions and code reuse")

    return "\n".join(review_lines)
