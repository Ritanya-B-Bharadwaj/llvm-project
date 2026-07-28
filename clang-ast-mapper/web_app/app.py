"""
Flask web application for Clang AST Line Mapper
Provides a user interface for analyzing C++ code with AST visualization
"""

import os
import sys
import tempfile
import uuid
import markdown
import bleach
from flask import Flask, render_template, request, jsonify, session
from flask_session import Session

# Add the parent directory to sys.path to import the AST mapper modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))
from src.ast_parser import ASTParser
from src.source_annotator import SourceAnnotator
from src.ai_interpreter import AIInterpreter
from src.utils import check_environment

app = Flask(__name__)
app.config['SECRET_KEY'] = os.environ.get('SECRET_KEY', str(uuid.uuid4()))
app.config['SESSION_TYPE'] = 'filesystem'
app.config['SESSION_FILE_DIR'] = os.path.join(os.path.dirname(__file__), 'flask_session')
Session(app)

# Create necessary directories
os.makedirs(app.config['SESSION_FILE_DIR'], exist_ok=True)
os.makedirs(os.path.join(os.path.dirname(__file__), 'temp'), exist_ok=True)

@app.route('/')
def index():
    """Render the main page."""
    return render_template('index.html')

@app.route('/analyze', methods=['POST'])
def analyze():
    """Analyze C++ code and return the results."""
    try:
        # Get code from the request
        code = request.form.get('code', '')
        include_ai = request.form.get('include_ai', 'true') == 'true'
        custom_prompt = request.form.get('custom_prompt', '')
        
        # Save code to a temporary file
        temp_dir = os.path.join(os.path.dirname(__file__), 'temp')
        os.makedirs(temp_dir, exist_ok=True)
        
        temp_file = tempfile.NamedTemporaryFile(
            mode='w', 
            suffix='.cpp', 
            delete=False,
            dir=temp_dir
        )
        temp_file.write(code)
        temp_file.close()
        
        # Parse AST
        parser = ASTParser()
        ast_file = parser.generate_ast_json(temp_file.name)
        
        if not ast_file or not os.path.exists(ast_file):
            return jsonify({
                'success': False,
                'error': "Failed to generate AST JSON. Make sure Clang is installed and properly configured."
            })
            
        # Validate the AST file is valid JSON
        try:
            with open(ast_file, 'r', encoding='utf-8') as f:
                import json
                json.load(f)
        except json.JSONDecodeError:
            return jsonify({
                'success': False,
                'error': "Generated AST file is not valid JSON. Clang may not have generated a complete AST dump."
            })
        except Exception as e:
            return jsonify({
                'success': False,
                'error': f"Error reading AST file: {str(e)}"
            })
            
        line_mappings = parser.parse_ast_file(ast_file, temp_file.name)
        
        if not line_mappings:
            return jsonify({
                'success': False,
                'error': "Failed to parse AST file. The generated AST might be incomplete or invalid."
            })
        
        # Generate CSV output for AI interpretation
        annotator = SourceAnnotator()
        csv_output = annotator.generate_csv_output(
            temp_file.name, 
            line_mappings
        )
        
        # Validate CSV output
        if not csv_output or csv_output.strip() == "":
            return jsonify({
                'success': False,
                'error': "Failed to generate CSV output. The AST data might be empty or invalid."
            })
        
        # Save CSV to a temporary file
        csv_file = os.path.join(temp_dir, f"{os.path.basename(temp_file.name)}.csv")
        with open(csv_file, 'w') as f:
            f.write(csv_output)
        
        # Generate the side-by-side view
        side_by_side = []
        with open(temp_file.name, 'r') as f:
            lines = f.readlines()
            
        for i, line in enumerate(lines, 1):
            line_num = i
            code_line = line.rstrip()
            nodes = line_mappings.get(line_num, [])
            node_str = "; ".join(nodes) if nodes else ""
            
            # Get explanation for the primary node
            explanation = ""
            if nodes:
                primary_node = nodes[0]
                explanation = get_node_explanation(primary_node)
                
            side_by_side.append({
                'line_num': line_num,
                'code': code_line,
                'nodes': node_str,
                'explanation': explanation
            })
        
        # Generate AI interpretation if requested
        ai_interpretation = ""
        if include_ai:
            interpreter = AIInterpreter()
            ai_interpretation = interpreter.interpret_csv(csv_file)
            
            # Convert markdown to HTML with sanitization
            ai_interpretation = markdown.markdown(ai_interpretation)
            ai_interpretation = bleach.clean(
                ai_interpretation,
                tags=['h1', 'h2', 'h3', 'h4', 'p', 'a', 'ul', 'ol', 'li', 
                      'strong', 'em', 'code', 'pre', 'br', 'table', 'thead', 
                      'tbody', 'tr', 'th', 'td'],
                attributes={'a': ['href', 'title'], 'th': ['colspan'], 'td': ['colspan']},
                strip=True
            )
        
        # Clean up temporary files
        if os.path.exists(temp_file.name):
            os.unlink(temp_file.name)
        if os.path.exists(csv_file):
            os.unlink(csv_file)
        
        # Return the results
        return jsonify({
            'success': True,
            'side_by_side': side_by_side,
            'ai_interpretation': ai_interpretation
        })
        
    except Exception as e:
        import traceback
        error_details = traceback.format_exc()
        print(f"Error analyzing code: {str(e)}")
        print(f"Error details:\n{error_details}")
        return jsonify({
            'success': False,
            'error': str(e),
            'details': error_details
        })

def get_node_explanation(node_type):
    """Get a simple explanation for an AST node type."""
    # Use a comprehensive dictionary of node explanations
    explanations = {
        "FunctionDecl": "Function declaration - defines a function with its return type, name, and parameters",
        "CXXMethodDecl": "C++ method declaration - a function that belongs to a class or struct",
        "CXXConstructorDecl": "C++ constructor declaration - special method that initializes objects of a class",
        "CXXDestructorDecl": "C++ destructor declaration - special method that cleans up resources when object is destroyed",
        "ParmVarDecl": "Parameter variable declaration - variables that receive values passed to functions",
        "VarDecl": "Variable declaration - defines a variable with its type and name",
        "FieldDecl": "Field declaration - member variables of a class or struct",
        "DeclStmt": "Declaration statement - contains one or more variable declarations",
        "ReturnStmt": "Return statement - specifies the value to be returned from a function",
        "CompoundStmt": "Compound statement - a block of code enclosed in braces {}",
        "CallExpr": "Function call expression - invokes a function with arguments",
        "CXXRecordDecl": "C++ class/struct definition - declares a user-defined type",
        "IfStmt": "If statement - conditional execution of code",
        "ForStmt": "For loop - executes code repeatedly with a loop counter",
        "WhileStmt": "While loop - executes code as long as a condition is true",
        "SwitchStmt": "Switch statement - selects code to execute based on a value",
        "CaseStmt": "Case statement - a branch in a switch statement",
        "DefaultStmt": "Default statement - default branch in a switch statement",
        "BinaryOperator": "Binary operator expression - combines two values (e.g., +, -, *, /)",
        "UnaryOperator": "Unary operator expression - applies to a single value (e.g., -, !, ~)",
        "MemberExpr": "Member expression - accesses a member of a class/struct",
        "IntegerLiteral": "Integer literal - a numeric constant (e.g., 42)",
        "FloatingLiteral": "Floating-point literal - a decimal constant (e.g., 3.14)",
        "StringLiteral": "String literal - a text constant (e.g., \"hello\")",
        "CharacterLiteral": "Character literal - a single character constant (e.g., 'a')",
        "BoolLiteral": "Boolean literal - true or false",
        "NullStmt": "Null statement - an empty statement (semicolon with no effect)",
        "BreakStmt": "Break statement - exits from a loop or switch",
        "ContinueStmt": "Continue statement - skips to the next iteration of a loop",
        "AccessSpecDecl": "Access specifier - public, protected, or private in a class",
        "CXXThisExpr": "C++ 'this' expression - refers to the current object",
        "LambdaExpr": "Lambda expression - defines an anonymous function",
        "CXXNewExpr": "C++ 'new' expression - dynamically allocates memory",
        "CXXDeleteExpr": "C++ 'delete' expression - frees dynamically allocated memory",
        "TypedefDecl": "Typedef declaration - creates an alias for a type",
        "NamespaceDecl": "Namespace declaration - defines a scope for identifiers",
        "UsingDirective": "Using directive - imports names from a namespace",
        "UsingDecl": "Using declaration - imports a specific name",
        "TemplateDecl": "Template declaration - defines a generic class or function",
        "ClassTemplateDecl": "Class template declaration - defines a generic class",
        "FunctionTemplateDecl": "Function template declaration - defines a generic function",
        "DeclRefExpr": "Declaration reference expression - refers to a declared variable or function"
    }
    
    return explanations.get(node_type, node_type)

@app.route('/examples')
def examples():
    """Return a list of example C++ codes."""
    examples = [
        {
            'name': 'Simple Function',
            'code': '''int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 3);
    return 0;
}'''
        },
        {
            'name': 'Simple Class',
            'code': '''class Calculator {
private:
    int value;
    
public:
    Calculator(int initial) : value(initial) {}
    
    int add(int x) {
        value += x;
        return value;
    }
    
    int getValue() const {
        return value;
    }
};

int main() {
    Calculator calc(10);
    calc.add(5);
    return calc.getValue();
}'''
        },
        {
            'name': 'Modern C++ Features',
            'code': '''#include <vector>
#include <string>

// Auto type deduction
auto getValue() {
    return 42;
}

// Function with decltype
template <typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

int main() {
    // Auto variable declarations
    auto x = getValue();
    auto y = 3.14;
    
    // decltype usage
    decltype(x) z = x + 10;
    
    // Auto with function call
    auto result = add(x, y);
    
    return 0;
}'''
        }
    ]
    return jsonify(examples)

@app.route('/check-environment')
def check_env():
    """Check the environment for all required dependencies."""
    try:
        env_info = check_environment()
        return jsonify({
            'success': True,
            'environment': env_info
        })
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        })

if __name__ == '__main__':
    app.run(debug=True)
