import json
import sys
from collections import defaultdict
import os
from openai import OpenAI
from openai.types.chat import ChatCompletionMessageParam

# ==== CONFIG ====
GROQ_API_KEY = ""
GROQ_BASE_URL = "https://api.groq.com/openai/v1"
GROQ_MODEL = "llama3-70b-8192"
OUTPUT_FILE = "output.txt"

client = OpenAI(api_key=GROQ_API_KEY, base_url=GROQ_BASE_URL)
explanation_cache = {}

def explain_ast_node_kinds(kinds: list[str]) -> str:
    if not kinds:
        return ""
    key = tuple(sorted(kinds))
    if key in explanation_cache:
        return explanation_cache[key]

    prompt = f"Explain the following C++ AST nodes in simple terms: {', '.join(key)}."
    messages: list[ChatCompletionMessageParam] = [{"role": "user", "content": prompt}]

    try:
        response = client.chat.completions.create(
            model=GROQ_MODEL,
            messages=messages,
            temperature=0.2
        )
        explanation = response.choices[0].message.content.strip()
    except Exception as e:
        explanation = f"(Groq error: {e})"

    explanation_cache[key] = explanation
    return explanation

def collect_ast_lines(node, line_map):
    if isinstance(node, dict):
        kind = node.get("kind")
        for key in ["loc", "range"]:
            if key in node and isinstance(node[key], dict):
                loc = node[key].get("start") if key == "loc" else node[key].get("begin")
                if isinstance(loc, dict):
                    line = loc.get("line")
                    if isinstance(line, int):
                        line_map[line].add(kind)
        for val in node.values():
            if isinstance(val, (dict, list)):
                collect_ast_lines(val, line_map)
    elif isinstance(node, list):
        for item in node:
            collect_ast_lines(item, line_map)

def annotate_file(source_path, ast_path):
    with open(ast_path, "r") as f:
        ast = json.load(f)

    line_map = defaultdict(set)
    collect_ast_lines(ast, line_map)

    with open(source_path, "r") as src, open(OUTPUT_FILE, "w") as out:
        for i, line in enumerate(src, start=1):
            kinds = sorted(line_map[i]) if i in line_map else []
            annotation = ", ".join(kinds)
            explanation = explain_ast_node_kinds(kinds) if kinds else ""
            out.write(f"{i:>3}: {line.rstrip()}  {'// ' + annotation if annotation else ''}\n")
            if explanation:
                out.write(f"     >> {explanation}\n")

def main():
    if len(sys.argv) != 3:
        print("Usage: main.py <source.cpp> <ast.json>")
        sys.exit(1)

    source_file, ast_file = sys.argv[1], sys.argv[2]
    annotate_file(source_file, ast_file)

    with open(OUTPUT_FILE, "r") as f:
        print(f.read())

if __name__ == "__main__":
    main()
