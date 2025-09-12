import os
import argparse
from groq import Groq

# Set your Groq API key using an environment variable
api_key = os.getenv("GROQ_API_KEY")
if not api_key:
    raise ValueError("GROQ_API_KEY environment variable not set.")

client = Groq(api_key=api_key)
model = "llama3-70b-8192"

# Step 1: Read and split into logical sections
def split_into_sections(file_path):
    with open(file_path, "r") as f:
        lines = f.readlines()

    sections = []
    current_src = None
    current_ir = []

    for line in lines:
        if line.strip().startswith("#### Line"):
            if current_src and current_ir:
                sections.append((current_src, current_ir))
                current_ir = []
            current_src = None
        elif line.strip().startswith("Source code:"):
            current_src = line.strip().replace("Source code:", "").strip(" `\n")
        elif line.strip().startswith("Mapped IR code:"):
            continue
        elif line.strip().startswith("```llvm") or line.strip().startswith("```"):
            continue
        else:
            current_ir.append(line.rstrip())

    if current_src and current_ir:
        sections.append((current_src, current_ir))

    return sections

# Step 2: Prompt Groq to explain each section
def explain_with_groq(source_line, ir_block):
    prompt = f"""You are an expert in compiler design and LLVM IR.

Given the C++ source line:
    {source_line}

And the corresponding LLVM IR block:
{chr(10).join(ir_block)}

Explain **in simple English** what this block does. Be specific about how the IR relates to the source line.

Only output the explanation in English. Do not repeat the source or IR unless absolutely necessary.
"""
    response = client.chat.completions.create(
        model=model,
        messages=[{"role": "user", "content": prompt}],
        temperature=0.3
    )
    return response.choices[0].message.content.strip()

# Step 3: Process and generate explained output
def generate_explained_md(sections, output_path):
    with open(output_path, "w") as out:
        for i, (source_line, ir_block) in enumerate(sections, start=1):
            out.write(f"### Line {i}: `{source_line}`\n\n")
            out.write("**LLVM IR Block:**\n")
            out.write("```llvm\n")
            out.write("\n".join(ir_block))
            out.write("\n```\n\n")
            out.write("**Explanation:**\n\n")
            explanation = explain_with_groq(source_line, ir_block)
            out.write(explanation + "\n\n")
            out.write("---\n\n")

# CLI Entry point
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Explain LLVM IR blocks using Groq.")
    parser.add_argument("input_file", help="Path to the input Markdown file with source and IR")
    parser.add_argument("output_file", help="Path to the output Markdown file with explanations")

    args = parser.parse_args()
    sections = split_into_sections(args.input_file)
    generate_explained_md(sections, args.output_file)
    print(f"Explained output written to: {args.output_file}")
