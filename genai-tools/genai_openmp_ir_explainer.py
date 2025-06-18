import re
import sys
import json
import openai

# 👉 Replace this with your real Groq API key
openai.api_key = "gsk_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
openai.api_base = "https://api.groq.com/openai/v1"
MODEL_NAME = "llama3-8b-8192"

def build_prompt(directive, ir_snippet):
    return f"""You are a compiler assistant.

Given this OpenMP directive and its corresponding LLVM IR snippet, explain:

Directive:
#pragma omp {directive}

LLVM IR:
{ir_snippet}

Respond in valid JSON format as:
{{
  "directive": "{directive}",
  "description": "Plain English explanation of what it does",
  "ir_translation": "How it was lowered into LLVM IR (e.g. __kmpc_* calls)",
  "notes": "Any useful insights about threading or data movement"
}}
"""

def extract_directives(source_file):
    with open(source_file, 'r') as f:
        code = f.read()
    return list(set(re.findall(r"#pragma omp ([a-zA-Z0-9_ ]+)", code)))

def extract_ir_snippets(ir_file):
    with open(ir_file, 'r') as f:
        lines = f.readlines()g

    snippets = []
    block = []
    for line in lines:
        if "!omp.annotation" in line:
            block.append(line)
        elif block:
            block.append(line)
            snippets.append("".join(block).strip())
            block = []
    return snippets

def query_groq(prompt):
    client = openai.OpenAI(
    api_key="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",
    base_url="https://api.groq.com/openai/v1" 
   )

    response = client.chat.completions.create(
        model=MODEL_NAME,
        messages=[{"role": "user", "content": prompt}],
        temperature=0.3,
        max_tokens=512
    )
    return response.choices[0].message.content

def main():
    if len(sys.argv) != 4:
        print("Usage: python genai_openmp_ir_explainer.py <input.cpp> <annotated.ll> <output.json>")
        return

    cpp_file, ir_file, output_file = sys.argv[1:4]

    directives = extract_directives(cpp_file)
    ir_snippets = extract_ir_snippets(ir_file)

    results = []
    for i, directive in enumerate(directives):
        ir = ir_snippets[i] if i < len(ir_snippets) else "// No annotated IR found"
        print(f"🔍 Explaining: {directive}")
        try:
            prompt = build_prompt(directive, ir)
            response_text = query_groq(prompt)
            json_start = response_text.find('{')
            json_data = json.loads(response_text[json_start:])
            json_data["ir_snippet"] = ir
            results.append(json_data)
        except Exception as e:
            results.append({
                "directive": directive,
                "description": "Failed to parse model output.",
                "ir_translation": "N/A",
                "notes": str(e),
                "ir_snippet": ir
            })

    with open(output_file, 'w') as out:
        json.dump(results, out, indent=2)
        print(f"\nExplanations saved to: {output_file}")

if __name__ == "__main__":
    main()
