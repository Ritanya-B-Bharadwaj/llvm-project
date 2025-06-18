from flask import Flask, render_template, request, jsonify, send_from_directory
import tempfile
import os
import json
import subprocess
from pathlib import Path
import html

app = Flask(__name__)

class WebHardcodedExplainer:
    """Provides hardcoded explanations for AST nodes - optimized for web display"""
    
    def __init__(self):
        self.explanations = self._build_explanation_map()
        self.patterns = self._build_pattern_explanations()
    
    def _build_explanation_map(self):
        """Comprehensive mapping of AST nodes to explanations"""
        return {
            # Declarations
            'FunctionDecl': "declares a function with its signature and body",
            'CXXMethodDecl': "declares a class method or member function",
            'CXXConstructorDecl': "declares a class constructor",
            'CXXDestructorDecl': "declares a class destructor", 
            'CXXRecordDecl': "declares a class, struct, or union type",
            'ClassTemplateDecl': "declares a class template",
            'VarDecl': "declares a variable with optional initialization",
            'FieldDecl': "declares a class member variable or field",
            'ParmVarDecl': "declares a function parameter",
            'NamespaceDecl': "declares or opens a namespace",
            'UsingDecl': "brings a name from another namespace into scope",
            'TypedefDecl': "creates a type alias using typedef",
            'EnumDecl': "declares an enumeration type",
            'EnumConstantDecl': "declares an enumeration constant",
            
            # Statements
            'CompoundStmt': "represents a block of statements enclosed in braces {}",
            'IfStmt': "represents an if conditional statement",
            'ForStmt': "represents a traditional for loop",
            'WhileStmt': "represents a while loop",
            'CXXForRangeStmt': "represents a range-based for loop (C++11)",
            'SwitchStmt': "represents a switch statement for multi-way branching",
            'CaseStmt': "represents a case label in a switch statement",
            'DefaultStmt': "represents the default case in a switch statement",
            'ReturnStmt': "returns a value from a function",
            'BreakStmt': "breaks out of a loop or switch statement",
            'ContinueStmt': "continues to the next iteration of a loop",
            'DeclStmt': "represents a declaration statement",
            'ExprStmt': "represents an expression used as a statement",
            'NullStmt': "represents an empty statement (just a semicolon)",
            'DoStmt': "represents a do-while loop",
            'GotoStmt': "represents a goto statement",
            'LabelStmt': "represents a labeled statement",
            
            # Expressions
            'BinaryOperator': "performs a binary operation (e.g., +, -, ==, &&)",
            'UnaryOperator': "performs a unary operation (e.g., ++, --, !, ~)",
            'ConditionalOperator': "represents the ternary operator (condition ? true : false)",
            'CallExpr': "represents a function call",
            'CXXOperatorCallExpr': "represents an overloaded operator call in C++",
            'CXXMemberCallExpr': "represents a call to a class member function",
            'MemberExpr': "accesses a member of a struct/class (e.g., obj.member)",
            'ArraySubscriptExpr': "accesses an array element using [] operator",
            'DeclRefExpr': "references a previously declared variable or function",
            'IntegerLiteral': "represents an integer constant (e.g., 42, 0xFF)",
            'FloatingLiteral': "represents a floating-point constant (e.g., 3.14)",
            'StringLiteral': "represents a string literal (e.g., \"hello\")",
            'CharacterLiteral': "represents a character literal (e.g., 'a')",
            'CXXBoolLiteralExpr': "represents a boolean literal (true or false)",
            'CXXNullPtrLiteralExpr': "represents a nullptr literal",
            'AssignmentOperator': "assigns a value to a variable (=, +=, -=, etc.)",
            'CompoundAssignOperator': "performs compound assignment (+=, -=, *=, etc.)",
            
            # C++ Specific Expressions
            'CXXNewExpr': "allocates memory using the new operator",
            'CXXDeleteExpr': "deallocates memory using the delete operator",
            'CXXThisExpr': "represents the 'this' pointer in a class method",
            'CXXThrowExpr': "throws an exception",
            'CXXTryStmt': "represents a try-catch block for exception handling",
            'CXXCatchStmt': "represents a catch block in exception handling",
            'CXXConstructExpr': "constructs an object using a constructor",
            'CXXTemporaryObjectExpr': "creates a temporary object",
            'CXXFunctionalCastExpr': "performs a functional-style cast",
            'CXXStaticCastExpr': "performs a static_cast",
            'CXXDynamicCastExpr': "performs a dynamic_cast",
            'CXXReinterpretCastExpr': "performs a reinterpret_cast",
            'CXXConstCastExpr': "performs a const_cast",
            
            # Type-related
            'ImplicitCastExpr': "performs an implicit type conversion",
            'CStyleCastExpr': "performs a C-style cast",
            'ParenExpr': "represents parenthesized expression",
            'SizeOfExpr': "calculates the size of a type or expression",
            
            # Control Flow Helpers
            'MaterializeTemporaryExpr': "materializes a temporary object",
            'ExprWithCleanups': "manages cleanup of temporary objects",
            'CXXBindTemporaryExpr': "binds a temporary object for cleanup",
        }
    
    def _build_pattern_explanations(self):
        """Pattern-based explanations for combinations of AST nodes"""
        return [
            # Function patterns
            (['FunctionDecl', 'CompoundStmt'], "defines a function with a body containing statements"),
            (['FunctionDecl', 'ParmVarDecl'], "defines a function that takes parameters"),
            (['CallExpr', 'DeclRefExpr'], "calls a previously declared function"),
            
            # Variable patterns
            (['VarDecl', 'IntegerLiteral'], "declares an integer variable with a literal value"),
            (['VarDecl', 'CallExpr'], "declares a variable initialized by a function call"),
            (['AssignmentOperator', 'DeclRefExpr'], "assigns a value to an existing variable"),
            
            # Control flow patterns
            (['IfStmt', 'BinaryOperator'], "conditional statement with a comparison"),
            (['ForStmt', 'BinaryOperator'], "for loop with a comparison condition"),
            (['WhileStmt', 'BinaryOperator'], "while loop with a comparison condition"),
            
            # Class patterns
            (['CXXRecordDecl', 'CXXMethodDecl'], "defines a class with member methods"),
            (['CXXRecordDecl', 'FieldDecl'], "defines a class with member variables"),
            (['CXXConstructorDecl', 'MemberExpr'], "constructor that initializes member variables"),
        ]
    
    def explain_ast_nodes(self, ast_nodes, source_line=""):
        """Generate explanation for a set of AST nodes"""
        if not ast_nodes:
            return ""
        
        # Sort nodes for consistent pattern matching
        sorted_nodes = sorted(ast_nodes)
        
        # Try pattern matching first
        for pattern_nodes, explanation in self.patterns:
            if all(node in sorted_nodes for node in pattern_nodes):
                return explanation
        
        # Fall back to individual node explanations
        explanations = []
        for node in sorted_nodes:
            if node in self.explanations:
                explanations.append(self.explanations[node])
            else:
                explanations.append(f"AST node: {node}")
        
        # Combine explanations intelligently
        if len(explanations) == 1:
            return explanations[0]
        elif len(explanations) == 2:
            return f"{explanations[0]} and {explanations[1]}"
        else:
            return f"combines {', '.join(explanations[:-1])}, and {explanations[-1]}"

def run_clang_ast_dump(file_path):
    """Run clang AST dump and return parsed JSON"""
    try:
        result = subprocess.run(
            ["clang", "-Xclang", "-ast-dump=json", "-fsyntax-only", file_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=30
        )
        
        if result.returncode != 0:
            return None
            
        if not result.stdout.strip():
            return None
            
        return json.loads(result.stdout)
        
    except (subprocess.TimeoutExpired, json.JSONDecodeError, FileNotFoundError):
        return None

def should_include_node(kind, filter_config="clean"):
    """Determine if a node kind should be included based on filter configuration"""
    if filter_config == "minimal":
        important_nodes = {
            'FunctionDecl', 'CXXMethodDecl', 'CXXConstructorDecl', 'CXXDestructorDecl',
            'CXXRecordDecl', 'ClassTemplateDecl', 'VarDecl', 'FieldDecl',
            'IfStmt', 'ForStmt', 'WhileStmt', 'CXXForRangeStmt', 'SwitchStmt',
            'ReturnStmt', 'BreakStmt', 'ContinueStmt', 'CompoundStmt'
        }
        return kind in important_nodes
    
    elif filter_config == "clean":
        noise_nodes = {
            'MaterializeTemporaryExpr', 'CXXBindTemporaryExpr', 'ExprWithCleanups',
            'ParenExpr', 'AlignedAttr', 'VisibilityAttr',
            'TypedefDecl', 'CXXRecordDecl', 'ConstantExpr'
        }
        return kind not in noise_nodes
    
    else:  # "all"
        return True

def get_node_category(kind):
    """Return category for different node types for styling"""
    if kind in ['FunctionDecl', 'CXXMethodDecl', 'CXXConstructorDecl', 'CXXDestructorDecl']:
        return 'function'
    elif kind in ['CXXRecordDecl', 'ClassTemplateDecl']:
        return 'class'
    elif kind in ['VarDecl', 'FieldDecl', 'ParmVarDecl']:
        return 'variable'
    elif kind in ['IfStmt', 'ForStmt', 'WhileStmt', 'CXXForRangeStmt', 'SwitchStmt']:
        return 'control'
    elif kind in ['ReturnStmt', 'BreakStmt', 'ContinueStmt']:
        return 'flow'
    else:
        return 'other'

def collect_line_annotations(node, line_map, target_file_path, filter_config="clean"):
    """Recursively collect AST node annotations for each line"""
    def process_location(loc_info, node_kind):
        if not loc_info or not isinstance(loc_info, dict):
            return
            
        file_path = loc_info.get("file", "")
        line = loc_info.get("line")
        
        if line and isinstance(line, int) and node_kind:
            target_abs = os.path.abspath(target_file_path)
            if not file_path:
                if should_include_node(node_kind, filter_config):
                    line_map.setdefault(line, set()).add(node_kind)
            else:
                file_abs = os.path.abspath(file_path) if os.path.isabs(file_path) else file_path
                if (file_abs == target_abs or 
                    os.path.basename(file_abs) == os.path.basename(target_abs)):
                    if should_include_node(node_kind, filter_config):
                        line_map.setdefault(line, set()).add(node_kind)

    if isinstance(node, dict):
        kind = node.get("kind")
        
        loc = node.get("loc")
        if loc:
            process_location(loc, kind)

        range_info = node.get("range")
        if range_info and isinstance(range_info, dict):
            begin = range_info.get("begin")
            end = range_info.get("end")
            if begin:
                process_location(begin, kind)
            if end and end != begin:
                process_location(end, kind)

        for value in node.values():
            collect_line_annotations(value, line_map, target_file_path, filter_config)
            
    elif isinstance(node, list):
        for item in node:
            collect_line_annotations(item, line_map, target_file_path, filter_config)

def annotate_source_for_web(source_code, filter_config="clean", detailed=False):
    """
    Annotate C/C++ source code and return structured data for web display
    """
    # Write the source code to a temporary file
    with tempfile.NamedTemporaryFile(suffix=".cpp", delete=False, mode="w", encoding="utf-8") as tmp:
        tmp.write(source_code)
        tmp_path = tmp.name

    try:
        file_path = os.path.abspath(tmp_path)
        explainer = WebHardcodedExplainer()

        ast_json = run_clang_ast_dump(file_path)
        if ast_json is None:
            return {
                'success': False,
                'error': 'Failed to generate AST. Please check if clang is installed.',
                'lines': []
            }

        line_map = {}
        collect_line_annotations(ast_json, line_map, file_path, filter_config)

        # Process source lines
        lines = source_code.split('\n')
        annotated_lines = []
        
        for i, line in enumerate(lines, start=1):
            line_data = {
                'number': i,
                'code': line,
                'annotations': [],
                'explanation': ''
            }
            
            if i in line_map:
                sorted_kinds = sorted(list(line_map[i]))
                
                # Add annotations with categories for styling
                for kind in sorted_kinds:
                    line_data['annotations'].append({
                        'kind': kind,
                        'category': get_node_category(kind)
                    })
                
                # Generate explanation
                explanation = explainer.explain_ast_nodes(sorted_kinds, line)
                line_data['explanation'] = explanation
            
            annotated_lines.append(line_data)

        total_annotations = sum(len(line_map.get(i, [])) for i in range(1, len(lines) + 1))
        
        return {
            'success': True,
            'lines': annotated_lines,
            'total_annotations': total_annotations,
            'annotated_lines_count': len(line_map)
        }
        
    finally:
        os.remove(tmp_path)

# Routes
@app.route("/")
def index():
    """Main AST Annotator tool page"""
    return render_template("index.html")

@app.route("/guide")
def guide():
    """Compiler Design guide page"""
    return render_template("guide.html")

@app.route("/annotate", methods=["POST"])
def annotate():
    """API endpoint for analyzing code"""
    data = request.get_json()
    source_code = data.get('code', '')
    filter_config = data.get('filter', 'clean')
    detailed = data.get('detailed', False)
    
    if not source_code.strip():
        return jsonify({'success': False, 'error': 'Please provide source code'})
    
    result = annotate_source_for_web(source_code, filter_config, detailed)
    return jsonify(result)

# Static files route (if needed for CSS/JS files)
@app.route('/static/<path:filename>')
def static_files(filename):
    """Serve static files (CSS, JS, images)"""
    return send_from_directory('static', filename)

if __name__ == "__main__":
    app.run(debug=True)