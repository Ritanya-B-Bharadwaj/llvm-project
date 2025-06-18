from flask import Flask, render_template, request, jsonify, send_from_directory
import tempfile
import os
import json
import subprocess
from pathlib import Path
import html
import traceback

app = Flask(__name__)

class WebHardcodedExplainer:
    """Provides hardcoded explanations for AST nodes - optimized for web display"""
    
    def __init__(self):
        self.explanations = self._build_explanation_map()
    
    def _build_explanation_map(self):
        """Comprehensive mapping of AST nodes to explanations"""
        return {
            # Declarations
            'FunctionDecl': "Function Declaration",
            'CXXMethodDecl': "C++ Method Declaration",
            'CXXConstructorDecl': "C++ Constructor Declaration",
            'CXXDestructorDecl': "C++ Destructor Declaration", 
            'CXXRecordDecl': "C++ Class/Struct Declaration",
            'ClassTemplateDecl': "Class Template Declaration",
            'VarDecl': "Variable Declaration",
            'FieldDecl': "Field Declaration",
            'ParmVarDecl': "Parameter Variable Declaration",
            'NamespaceDecl': "Namespace Declaration",
            'UsingDecl': "Using Declaration",
            'TypedefDecl': "Typedef Declaration",
            'EnumDecl': "Enum Declaration",
            'EnumConstantDecl': "Enum Constant Declaration",
            
            # Statements
            'CompoundStmt': "Compound Statement (Block)",
            'IfStmt': "If Statement",
            'ForStmt': "For Loop Statement",
            'WhileStmt': "While Loop Statement",
            'CXXForRangeStmt': "C++ Range-based For Loop",
            'SwitchStmt': "Switch Statement",
            'CaseStmt': "Case Statement",
            'DefaultStmt': "Default Statement",
            'ReturnStmt': "Return Statement",
            'BreakStmt': "Break Statement",
            'ContinueStmt': "Continue Statement",
            'DeclStmt': "Declaration Statement",
            'ExprStmt': "Expression Statement",
            'NullStmt': "Null Statement",
            'DoStmt': "Do-While Statement",
            'GotoStmt': "Goto Statement",
            'LabelStmt': "Label Statement",
            
            # Expressions
            'BinaryOperator': "Binary Operator",
            'UnaryOperator': "Unary Operator",
            'ConditionalOperator': "Conditional Operator (?:)",
            'CallExpr': "Function Call Expression",
            'CXXOperatorCallExpr': "C++ Operator Call Expression",
            'CXXMemberCallExpr': "C++ Member Function Call",
            'MemberExpr': "Member Access Expression",
            'ArraySubscriptExpr': "Array Subscript Expression",
            'DeclRefExpr': "Declaration Reference Expression",
            'IntegerLiteral': "Integer Literal",
            'FloatingLiteral': "Floating Point Literal",
            'StringLiteral': "String Literal",
            'CharacterLiteral': "Character Literal",
            'CXXBoolLiteralExpr': "Boolean Literal",
            'CXXNullPtrLiteralExpr': "Null Pointer Literal",
            'AssignmentOperator': "Assignment Operator",
            'CompoundAssignOperator': "Compound Assignment Operator",
            
            # C++ Specific
            'CXXNewExpr': "C++ New Expression",
            'CXXDeleteExpr': "C++ Delete Expression",
            'CXXThisExpr': "C++ This Expression",
            'CXXThrowExpr': "C++ Throw Expression",
            'CXXTryStmt': "C++ Try Statement",
            'CXXCatchStmt': "C++ Catch Statement",
            'CXXConstructExpr': "C++ Constructor Expression",
            'CXXTemporaryObjectExpr': "C++ Temporary Object Expression",
            'CXXFunctionalCastExpr': "C++ Functional Cast Expression",
            'CXXStaticCastExpr': "C++ Static Cast Expression",
            'CXXDynamicCastExpr': "C++ Dynamic Cast Expression",
            'CXXReinterpretCastExpr': "C++ Reinterpret Cast Expression",
            'CXXConstCastExpr': "C++ Const Cast Expression",
            
            # Type-related
            'ImplicitCastExpr': "Implicit Cast Expression",
            'CStyleCastExpr': "C-Style Cast Expression",
            'ParenExpr': "Parenthesized Expression",
            'SizeOfExpr': "Sizeof Expression",
            
            # Control Flow Helpers
            'MaterializeTemporaryExpr': "Materialize Temporary Expression",
            'ExprWithCleanups': "Expression with Cleanups",
            'CXXBindTemporaryExpr': "C++ Bind Temporary Expression",
        }
    
    def get_node_description(self, kind):
        """Get a short description for an AST node kind"""
        return self.explanations.get(kind, kind)

def run_clang_ast_dump(file_path):
    """Run clang AST dump and return parsed JSON with better error handling"""
    try:
        # Try different clang command variations
        clang_commands = [
            ["clang++", "-Xclang", "-ast-dump=json", "-fsyntax-only"],
            ["clang", "-Xclang", "-ast-dump=json", "-fsyntax-only"],
            ["clang-14", "-Xclang", "-ast-dump=json", "-fsyntax-only"],
            ["clang-13", "-Xclang", "-ast-dump=json", "-fsyntax-only"],
        ]
        
        for cmd in clang_commands:
            try:
                result = subprocess.run(
                    cmd + [file_path],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                    timeout=30
                )
                
                if result.returncode == 0 and result.stdout.strip():
                    return json.loads(result.stdout)
                    
            except (FileNotFoundError, subprocess.TimeoutExpired):
                continue
                
        return None
        
    except json.JSONDecodeError as e:
        print(f"JSON parsing error: {e}")
        return None
    except Exception as e:
        print(f"Unexpected error: {e}")
        return None

def should_include_node(kind, filter_config="clean"):
    if filter_config == "minimal":
        important_nodes = {
            'FunctionDecl', 'CXXMethodDecl', 'CXXConstructorDecl', 'CXXDestructorDecl',
            'CXXRecordDecl', 'VarDecl', 'ParmVarDecl', 'FieldDecl',
            'IfStmt', 'ForStmt', 'WhileStmt', 'ReturnStmt', 'CallExpr', 'BinaryOperator',
            'DeclStmt', 'CompoundStmt', 'DeclRefExpr', 'IntegerLiteral', 'StringLiteral',
            'CXXOperatorCallExpr'
        }
        return kind in important_nodes
        
    elif filter_config == "clean":
        # Filter out noise nodes that don't add much value
        noise_nodes = {
            'MaterializeTemporaryExpr', 'CXXBindTemporaryExpr', 'ExprWithCleanups',
            'ParenExpr', 'AlignedAttr', 'VisibilityAttr', 'ImplicitCastExpr',
            'CXXFunctionalCastExpr'
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
    elif kind in ['BinaryOperator', 'UnaryOperator', 'CallExpr', 'DeclRefExpr']:
        return 'expression'
    else:
        return 'other'

def extract_node_name(node):
    """Extract a meaningful name from an AST node"""
    if not isinstance(node, dict):
        return ""
    
    # Try to get the name from various fields
    name = node.get('name', '')
    if name:
        return name
    
    # For literals, get the value
    if node.get('kind') == 'IntegerLiteral':
        return str(node.get('value', ''))
    elif node.get('kind') == 'StringLiteral':
        return f'"{node.get("value", "")}"'
    elif node.get('kind') == 'CharacterLiteral':
        return f"'{node.get('value', '')}'"
    elif node.get('kind') in ['CXXBoolLiteralExpr']:
        return str(node.get('value', ''))
    
    # For operators, get the opcode
    if 'opcode' in node:
        return node['opcode']
    
    # For types, get the type
    if 'type' in node and isinstance(node['type'], dict):
        return node['type'].get('qualType', '')
    
    return ""

def normalize_file_path(file_path, target_file):
    """Normalize and compare file paths"""
    if not file_path:
        return True  # If no file specified, assume it's from target file
    
    # Convert to absolute paths for comparison
    try:
        target_abs = os.path.abspath(target_file)
        
        # Handle relative paths
        if not os.path.isabs(file_path):
            # For relative paths, check if basename matches
            return os.path.basename(file_path) == os.path.basename(target_abs)
        
        file_abs = os.path.abspath(file_path)
        return file_abs == target_abs
        
    except Exception:
        # If path operations fail, fall back to basename comparison
        return os.path.basename(file_path) == os.path.basename(target_file)

def extract_location_info(node):
    """Extract all location information from a node"""
    locations = []
    
    if not isinstance(node, dict):
        return locations
    
    # Check main 'loc' field
    if 'loc' in node:
        loc = node['loc']
        if isinstance(loc, dict):
            # Handle different location formats
            if 'line' in loc:
                # Direct line format
                locations.append({
                    'line': loc.get('line'),
                    'file': loc.get('file', ''),
                    'col': loc.get('col')
                })
            elif 'start' in loc:
                # Start format
                start = loc['start']
                if isinstance(start, dict) and 'line' in start:
                    locations.append({
                        'line': start.get('line'),
                        'file': start.get('file', ''),
                        'col': start.get('col')
                    })
    
    # Check 'range' field
    if 'range' in node:
        range_info = node['range']
        if isinstance(range_info, dict):
            # Process begin and end locations
            for loc_key in ['begin', 'end']:
                if loc_key in range_info:
                    loc_data = range_info[loc_key]
                    if isinstance(loc_data, dict) and 'line' in loc_data:
                        locations.append({
                            'line': loc_data.get('line'),
                            'file': loc_data.get('file', ''),
                            'col': loc_data.get('col')
                        })
    
    return locations

def collect_line_annotations(ast_root, line_map, target_file_path, filter_config="clean", explainer=None):
    """Collect line annotations with improved accuracy and debugging"""
    
    def add_annotation(line_num, node):
        """Add annotation to a specific line"""
        if not line_num or not isinstance(line_num, int) or line_num <= 0:
            return
            
        kind = node.get('kind')
        if not kind or not should_include_node(kind, filter_config):
            return
        
        if line_num not in line_map:
            line_map[line_num] = {'nodes': [], 'details': []}
        
        # Avoid duplicates
        existing_kinds = [n['kind'] for n in line_map[line_num]['nodes']]
        if kind not in existing_kinds:
            name = extract_node_name(node)
            description = explainer.get_node_description(kind) if explainer else kind
            
            line_map[line_num]['nodes'].append({
                'kind': kind,
                'name': name,
                'category': get_node_category(kind),
                'description': description,
                'inline': f"{kind}({name})" if name else kind
            })
    
    def traverse_ast(node):
        """Recursively traverse AST nodes"""
        if not isinstance(node, dict):
            return
        
        # Extract location information
        locations = extract_location_info(node)
        
        # Add annotations for all valid locations
        for loc_info in locations:
            line_num = loc_info.get('line')
            file_path = loc_info.get('file', '')
            
            # Check if this location is from our target file
            if line_num and normalize_file_path(file_path, target_file_path):
                add_annotation(line_num, node)
        
        # Recursively process child nodes
        if 'inner' in node and isinstance(node['inner'], list):
            for child in node['inner']:
                traverse_ast(child)
    
    # Start traversal
    if isinstance(ast_root, dict):
        traverse_ast(ast_root)
    elif isinstance(ast_root, list):
        for item in ast_root:
            traverse_ast(item)

def format_annotation_string(nodes):
    """Format the annotation string like 'VarDecl → DeclStmt' """
    if not nodes:
        return ""
    
    # Sort nodes by importance/hierarchy
    def get_priority(kind):
        priorities = {
            'FunctionDecl': 1,
            'CXXMethodDecl': 1,
            'CXXRecordDecl': 1,
            'VarDecl': 2,
            'IfStmt': 2,
            'ForStmt': 2,
            'WhileStmt': 2,
            'ReturnStmt': 2,
            'DeclStmt': 3,
            'CompoundStmt': 3,
            'BinaryOperator': 4,
            'CallExpr': 4,
            'CXXOperatorCallExpr': 4,
            'DeclRefExpr': 5,
            'IntegerLiteral': 6,
            'StringLiteral': 6,
        }
        return priorities.get(kind, 10)
    
    # Sort by priority, then alphabetically
    sorted_nodes = sorted(nodes, key=lambda x: (get_priority(x['kind']), x['kind']))
    
    # Create the arrow-separated string
    node_names = [node['kind'] for node in sorted_nodes]
    return " → ".join(node_names)

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
                'error': 'Failed to generate AST. Please check if clang is installed and the code is valid.',
                'lines': [],
                'tree': None
            }

        # Handle case where AST is returned as array
        if isinstance(ast_json, list) and ast_json:
            ast_json = ast_json[0]

        if not isinstance(ast_json, dict):
            return {
                'success': False,
                'error': 'AST root node is not a valid dictionary.',
                'lines': [],
                'tree': None
            }

        line_map = {}
        collect_line_annotations(ast_json, line_map, file_path, filter_config, explainer)

        # Process source lines
        lines = source_code.split('\n')
        annotated_lines = []
        total_annotations = 0
        
        for i, line in enumerate(lines, start=1):
            annotation_text = ""
            
            if i in line_map and line_map[i]['nodes']:
                nodes = line_map[i]['nodes']
                total_annotations += len(nodes)
                annotation_text = format_annotation_string(nodes)
            
            # Format the line exactly as requested: "line_number  code_content    annotation"
            formatted_line = f"{i}  {line}"
            if annotation_text:
                # Calculate padding to align annotations
                padding = max(40 - len(formatted_line), 4)  # Minimum 4 spaces
                formatted_line += " " * padding + annotation_text
            
            line_data = {
                'number': i,
                'code': line,
                'formatted_display': formatted_line,
                'annotations': line_map[i]['nodes'] if i in line_map else [],
                'annotation_text': annotation_text,
                'has_annotations': bool(annotation_text)
            }
            
            annotated_lines.append(line_data)

            def filter_user_ast_subtree(ast, target_file_path):
                """Extract only the subtree related to user's file"""
                if isinstance(ast, dict):
                    loc = ast.get("loc", {})
                    file_path = loc.get("file") or loc.get("start", {}).get("file")
                    if file_path and normalize_file_path(file_path, target_file_path):
                        return ast  # This is the root node of user's code
                    # Recurse into children
                    if 'inner' in ast:
                        for child in ast['inner']:
                            result = filter_user_ast_subtree(child, target_file_path)
                            if result:
                                return result
                elif isinstance(ast, list):
                    for item in ast:
                        result = filter_user_ast_subtree(item, target_file_path)
                        if result:
                            return result
                return None


        return {
            'success': True,
            'lines': annotated_lines,
            'total_annotations': total_annotations,
            'annotated_lines_count': len([l for l in line_map.values() if l['nodes']]),
            'tree': filter_user_ast_subtree(ast_json, file_path)
        }
        
    except Exception as e:
        print(f"Error: {e}")
        print(traceback.format_exc())
        return {
            'success': False,
            'error': f'Error processing code: {str(e)}',
            'lines': [],
            'tree': None
        }
        
    finally:
        try:
            os.remove(tmp_path)
        except:
            pass

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
    try:
        data = request.get_json()
        source_code = data.get('code', '')
        filter_config = data.get('filter', 'clean')
        detailed = data.get('detailed', False)
        
        if not source_code.strip():
            return jsonify({'success': False, 'error': 'Please provide source code'})
        
        result = annotate_source_for_web(source_code, filter_config, detailed)
        return jsonify(result)
    
    except Exception as e:
        return jsonify({
            'success': False,
            'error': f'Server error: {str(e)}',
            'lines': [],
            'tree': None
        })

if __name__ == "__main__":
    app.run(debug=True)