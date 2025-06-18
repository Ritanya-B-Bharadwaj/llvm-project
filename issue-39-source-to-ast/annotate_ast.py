import sys
import json
from rich.console import Console
from rich.syntax import Syntax

def load_ast(ast_file):
    with open(ast_file, "r") as f:
        return json.load(f)

def get_line_node_map(ast, line_map=None):
    if line_map is None:
        line_map = {}

    if isinstance(ast, dict):
        loc = ast.get("loc", {}).get("start", {})
        line = loc.get("line")
        if line:
            line_map.setdefault(line, []).append(ast)
        for key in ast:
            get_line_node_map(ast[key], line_map)

    elif isinstance(ast, list):
        for item in ast:
            get_line_node_map(item, line_map)

    return line_map

def explain_node(node):
    kind = node.get("kind", "Unknown")
    name = node.get("name", "")
    if kind == "VarDecl":
        return f"Declares a variable `{name}`"
    elif kind == "FunctionDecl":
        return f"Function definition: `{name}()`"
    elif kind == "CallExpr":
        return "Function call expression"
    elif kind == "ReturnStmt":
        return "Return statement"
    elif kind == "CXXMemberCallExpr":
        return "C++ Member function call"
    elif kind == "IntegerLiteral":
        return "Integer literal"
    else:
        return f"AST node of kind: {kind}"

def annotate_source(source_path, ast_map):
    with open(source_path, "r") as f:
        lines = f.readlines()

    console = Console()
    for i, line in enumerate(lines, start=1):
        syntax = Syntax(line.rstrip(), "cpp", theme="monokai", line_numbers=True)
        console.print(syntax)
        if i in ast_map:
            for node in ast_map[i]:
                console.print(f"  [yellow]- {explain_node(node)}[/]")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 annotate_ast.py <source.cpp> <ast.json>")
        sys.exit(1)

    source_file = sys.argv[1]
    ast_file = sys.argv[2]
    ast = load_ast(ast_file)
    line_map = get_line_node_map(ast)
    annotate_source(source_file, line_map)
