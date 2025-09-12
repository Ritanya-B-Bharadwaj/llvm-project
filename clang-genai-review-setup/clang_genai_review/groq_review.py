def groq_review(filename: str, line_data: dict) -> str:
    from groq import Groq
    import os

    modified_lines = line_data.get(filename, [])
    modified_code = "\n".join(modified_lines)

    prompt = f"""
    You are an expert LLVM code reviewer familiar with LLVM's coding standards and commit best practices: https://llvm.org/docs/CodingStandards.html.

    The following diff has been submitted in a commit to the LLVM project.

    Please:
    1. Review the modified code shown below.
    2. Identify whether the changes follow LLVM's coding standards and best practices.
    3. Comment on any areas that need improvement or refactoring.
    4. Acknowledge good practices where applicable.

    ### File: {filename}
    ### Modified Code:
    ```cpp
    {modified_code}
    Respond in clear, concise English with:

    ✅ If the code is well-written and adheres to standards.

    ⚠️ Constructive comments for areas needing improvement.

    💡 Suggestions for improvements, if applicable.
    """

    client = Groq(api_key=os.environ["GROQ_API_KEY"])
    completion = client.chat.completions.create(
    model="meta-llama/llama-4-scout-17b-16e-instruct",
    messages=[{"role": "user", "content": prompt}],
    temperature=0.7,
    max_completion_tokens=1024,
    top_p=1,
    stream=False,
    )

    return completion.choices[0].message.content.strip()

