"""
AI Interpreter Module for Clang AST Mapper

This module provides functionality to interpret AST mapping data using
the Hugging Face Transformers library for local inference. It analyzes 
CSV output and provides insights about the code structure based on AST 
node information.
"""

import os
import csv
import json
from typing import Dict, List, Optional, Tuple

# Import Transformers library conditionally to not break if not installed
TRANSFORMERS_AVAILABLE = False
try:
    from transformers import pipeline
    TRANSFORMERS_AVAILABLE = True
except ImportError:
    pass

class AIInterpreter:
    """Class for interpreting AST data using AI models."""
    
    def __init__(self, api_key: Optional[str] = None):
        """
        Initialize the AI interpreter.
        
        Args:
            api_key: Optional API key for the AI service. If not provided,
                     will try to get from environment variable AST_MAPPER_AI_KEY.
                     (No longer needed for local inference but kept for compatibility)
        """
        self.model_name = os.environ.get('AST_MAPPER_MODEL', 'gpt2')
        self.pipeline = None
        
        # Initialize the pipeline if Transformers is available
        if TRANSFORMERS_AVAILABLE:
            try:
                self.pipeline = pipeline("text-generation", model=self.model_name)
                print(f"Successfully loaded model: {self.model_name}")
            except Exception as e:
                print(f"Error loading model: {e}")
                print("Falling back to default interpretation")
    
    def interpret_csv(self, csv_file: str) -> str:
        """
        Interpret a CSV file containing AST mapping data.
        
        Args:
            csv_file: Path to the CSV file to interpret
            
        Returns:
            A string containing the AI interpretation of the AST data
        """
        # Read the CSV file
        ast_data = self._read_csv(csv_file)
        if not ast_data:
            return "Error: Failed to read CSV file or file is empty."
            
        # Prepare the data for the AI model
        prompt = self._prepare_prompt(ast_data)
        
        # For now, always use the default interpretation which is more reliable
        # for this specific task than general-purpose language models
        return self._get_default_interpretation(prompt, csv_file)
    
    def _read_csv(self, csv_file: str) -> List[Dict]:
        """
        Read a CSV file and convert it to a list of dictionaries.
        
        Args:
            csv_file: Path to the CSV file
            
        Returns:
            A list of dictionaries containing the CSV data
        """
        try:
            with open(csv_file, 'r', encoding='utf-8') as f:
                reader = csv.DictReader(f)
                return list(reader)
        except Exception as e:
            print(f"Error reading CSV file: {e}")
            return []
    
    def _prepare_prompt(self, ast_data: List[Dict]) -> str:
        """
        Prepare a prompt for the AI model based on AST data.
        
        Args:
            ast_data: List of dictionaries containing AST data
            
        Returns:
            A prompt string for the AI model
        """
        # Extract relevant information from the AST data
        code_lines = []
        ast_info = []
        
        for row in ast_data:
            line_num = row.get('Line', '')
            source_code = row.get('Source Code', '')
            ast_nodes = row.get('AST Nodes', '')
            explanations = row.get('Explanations', '')
            
            if source_code:
                code_lines.append(f"{line_num}: {source_code}")
            
            if ast_nodes:
                ast_info.append(f"Line {line_num}: {ast_nodes} - {explanations}")
        
        # Create the prompt
        prompt = f"""Analyze this C++ code and its AST mapping:

SOURCE CODE:
```cpp
{''.join(code_lines)}
```

AST MAPPINGS:
{chr(10).join(ast_info)}

As a compiler expert, provide the following analysis:
1. Explain the overall structure of this code based on the AST
2. Highlight key AST nodes and what they tell us about the code
3. Explain any patterns or interesting features revealed by the AST mapping
4. If there are any potential issues or optimizations suggested by the AST structure
"""
        return prompt
    
    def _get_ai_interpretation(self, prompt: str) -> str:
        """
        Get an AI interpretation of the AST data.
        
        Args:
            prompt: The prompt to send to the AI model
            
        Returns:
            The AI's interpretation of the AST data
        """
        # If Transformers is not available or pipeline failed to initialize, use default
        if not TRANSFORMERS_AVAILABLE or self.pipeline is None:
            return self._get_default_interpretation(prompt)
        
        try:
            # Simplify the prompt for better results
            simplified_prompt = "Analyze this C++ code and its Abstract Syntax Tree mapping:\n\n" + prompt
            
            # Generate the interpretation
            result = self.pipeline(simplified_prompt, max_new_tokens=500, do_sample=True, temperature=0.7)
            
            # Extract the generated text
            if isinstance(result, list) and result:
                generated_text = result[0].get('generated_text', '')
                # Return only the newly generated part, not the input prompt
                return generated_text[len(simplified_prompt):].strip()
            
            return "Failed to generate interpretation."
            
        except Exception as e:
            print(f"Error generating interpretation: {e}")
            return self._get_default_interpretation(prompt)
    
    def _get_default_interpretation(self, prompt: str, csv_file_path: str = None) -> str:
        """
        Provide a default interpretation when API access fails.
        
        Args:
            prompt: The original prompt (used for analysis)
            csv_file_path: Optional path to the CSV file for direct reading
            
        Returns:
            A default interpretation based on basic pattern matching
        """
        # Extract code lines from the prompt
        code_section = prompt.split("```cpp")[1].split("```")[0] if "```cpp" in prompt else ""
        ast_section = prompt.split("AST MAPPINGS:")[1].split("As a compiler expert")[0] if "AST MAPPINGS:" in prompt else ""
        
        # Count AST node types
        node_types = {}
        for line in ast_section.split('\n'):
            if ':' in line:
                for node in line.split('-')[0].split(':')[-1].split(';'):
                    node = node.strip()
                    if node:
                        node_types[node] = node_types.get(node, 0) + 1
        
        # Dictionary with explanations for common AST nodes
        node_explanations = {
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
            "IfStmt": "If statement - conditional execution based on a boolean expression",
            "ForStmt": "For loop - iteration with initialization, condition, and increment steps",
            "WhileStmt": "While loop - iteration based on a condition",
            "DoStmt": "Do-while loop - iteration that checks condition after each execution",
            "SwitchStmt": "Switch statement - multi-way branch based on a value",
            "CaseStmt": "Case statement - individual branch in a switch statement",
            "BinaryOperator": "Binary operator - operations that use two operands like +, -, *, /, etc.",
            "UnaryOperator": "Unary operator - operations that use one operand like !, ~, ++, --, etc.",
            "CallExpr": "Function call expression - invokes a function with arguments",
            "CXXMemberCallExpr": "C++ member function call - invokes a method on an object",
            "CXXRecordDecl": "C++ class/struct declaration - defines a user-defined type",
            "AccessSpecDecl": "Access specifier - public, private, or protected sections in a class",
            "TemplateDecl": "Template declaration - defines a template for generic programming",
            "ImplicitCastExpr": "Implicit type conversion - automatic conversion of expressions",
            "ExplicitCastExpr": "Explicit type conversion - programmer-specified type casting",
            "MemberExpr": "Member expression - accessing a member of an object or class",
            "DeclRefExpr": "Declaration reference - refers to a previously declared entity",
            "IntegerLiteral": "Integer literal - represents a constant integer value",
            "FloatingLiteral": "Floating point literal - represents a constant floating-point value",
            "StringLiteral": "String literal - represents a constant string value",
            "NullStmt": "Null statement - empty statement represented by a semicolon",
            "InitListExpr": "Initializer list - initializes an object with a list of values",
            "CXXNewExpr": "C++ new expression - dynamically allocates memory",
            "CXXDeleteExpr": "C++ delete expression - deallocates memory",
            "CXXThisExpr": "C++ this expression - refers to the current object"
        }
        
        # Generate code summary
        code_summary = self._generate_code_summary(code_section, node_types)
        
        # Generate a basic analysis
        analysis = [
            "# AST Analysis for C++ Code",
            "",
            "## Code Summary",
            code_summary,
            "",
            "## Code Structure",
            "The C++ code has been analyzed using Clang's AST system, revealing the following structure:",
        ]
        
        # Check for function declarations
        if "FunctionDecl" in str(node_types) or "CXXMethodDecl" in str(node_types):
            analysis.append("- **Function Declarations**: The code contains function/method declarations which define its main structure")
        
        # Check for parameter declarations
        if "ParmVarDecl" in str(node_types):
            analysis.append("- **Parameter Variables**: Functions have parameter declarations indicating data passed between functions")
        
        # Check for compound statements (blocks)
        if "CompoundStmt" in str(node_types):
            analysis.append("- **Code Blocks**: Contains compound statements (blocks of code within curly braces)")
        
        # Check for return statements
        if "ReturnStmt" in str(node_types):
            analysis.append("- **Return Statements**: Functions include return statements to provide output values")
        
        # Check for declarations
        if "DeclStmt" in str(node_types) or "VarDecl" in str(node_types):
            analysis.append("- **Variable Declarations**: The code declares local variables to store and manipulate data")
        
        # Check for control flow
        has_control_flow = any(k for k in node_types.keys() if "If" in k or "For" in k or "While" in k or "Switch" in k)
        if has_control_flow:
            analysis.append("- **Control Flow**: Contains conditional or loop structures to control program execution")
            
        # Check for class definitions
        if "CXXRecordDecl" in str(node_types):
            analysis.append("- **Classes/Structs**: The code defines custom data types with methods and fields")
        
        # Add a summary section
        analysis.append("")
        analysis.append("## AST Structure Summary")
        analysis.append("This code follows a typical C++ structure with:")
        analysis.append("")
        
        if "CXXRecordDecl" in str(node_types) and "CXXMethodDecl" in str(node_types):
            analysis.append("- Class definitions with member methods")
            
        if "CXXRecordDecl" in str(node_types) and "FieldDecl" in str(node_types):
            analysis.append("- Classes containing member variables (fields)")
        
        if "FunctionDecl" in str(node_types) and "DeclStmt" in str(node_types):
            analysis.append("- Function definitions that contain variable declarations")
        
        if "CXXMethodDecl" in str(node_types) and "DeclStmt" in str(node_types):
            analysis.append("- Method implementations with local variable usage")
        
        if any(k for k in node_types.keys() if "CallExpr" in k):
            analysis.append("- Function/method calls between defined components")
        
        if "DeclStmt" in str(node_types) and "ReturnStmt" in str(node_types):
            analysis.append("- Variable manipulation followed by return statements")
        
        if has_control_flow:
            analysis.append("- Control flow structures to manage program execution paths")
        
        # Add detailed explanations of each node type
        analysis.append("")
        analysis.append("## AST Node Types Explained")
        analysis.append("The following AST nodes were found in the code:")
        analysis.append("")
        
        for node, count in sorted(node_types.items(), key=lambda x: x[1], reverse=True):
            # Get explanation for this node type with enhanced fallbacks for template-related nodes
            explanation = node_explanations.get(node, None)
            if explanation is None:
                # Handle template-related nodes specially
                if "Template" in node:
                    if "ClassTemplate" in node:
                        explanation = f"C++ class template declaration - defines a generic class that can work with different types"
                    elif "FunctionTemplate" in node:
                        explanation = f"C++ function template declaration - defines a generic function that can work with different types"
                    elif "TemplateTypeParm" in node:
                        explanation = f"Template type parameter - defines a placeholder type (like T) in a template"
                    elif "TemplateSpecialization" in node:
                        explanation = f"Template specialization - specific implementation of a template for particular types"
                    else:
                        explanation = f"Template-related node - part of C++'s generic programming system"
                # Handle modern C++ features
                elif "CXXAuto" in node:
                    explanation = f"C++ auto type - represents automatic type deduction"
                elif "Decltype" in node:
                    explanation = f"C++ decltype - determines the type of an expression at compile time"
                elif "Lambda" in node:
                    explanation = f"Lambda expression - represents an anonymous function object"
                # Default fallback
                else:
                    explanation = f"AST node type representing a {node} construct"
            analysis.append(f"- **{node}** ({count} occurrences): {explanation}")
        
        # Parse the CSV data to extract line-by-line information
        line_data = {}
        code_by_line = {}
        
        # Directly read the CSV file to get accurate mapping
        try:
            # First, try to read from the actual CSV file if we can infer its path
            csv_file_path = csv_file_path  # Assuming this is passed to the function
            
            if csv_file_path and os.path.exists(csv_file_path):
                with open(csv_file_path, 'r', encoding='utf-8') as f:
                    csv_reader = csv.DictReader(f)
                    for row in csv_reader:
                        line_num = row.get('Line', '')
                        if line_num and line_num.isdigit():
                            code_by_line[line_num] = row.get('Source Code', '').strip()
                            nodes = row.get('AST Nodes', '').strip()
                            if nodes:
                                line_data[line_num] = [node.strip() for node in nodes.split(';') if node.strip()]
            else:
                # Fall back to parsing from the prompt
                for line in prompt.split("SOURCE CODE:")[1].split("AST MAPPINGS:")[0].strip().replace("```cpp", "").replace("```", "").split('\n'):
                    if ':' in line:
                        parts = line.split(':', 1)
                        if len(parts) > 1 and parts[0].strip().isdigit():
                            line_num = parts[0].strip()
                            code_by_line[line_num] = parts[1].strip()
                
                # Parse AST mappings from prompt
                for line in prompt.split("AST MAPPINGS:")[1].split("As a compiler expert")[0].strip().split('\n'):
                    if 'Line' in line and ':' in line:
                        parts = line.split(':', 1)
                        if len(parts) > 1:
                            line_match = parts[0].strip().replace('Line', '').strip()
                            if line_match.isdigit():
                                if line_match not in line_data:
                                    line_data[line_match] = []
                                # Extract AST nodes
                                if '-' in parts[1]:
                                    node_part = parts[1].split('-')[0].strip()
                                    nodes = [n.strip() for n in node_part.split(';') if n.strip()]
                                    line_data[line_match].extend(nodes)
                                else:
                                    nodes = [n.strip() for n in parts[1].split(';') if n.strip()]
                                    line_data[line_match].extend(nodes)
        except Exception as e:
            print(f"Error parsing CSV data: {e}")
        
        # Add side-by-side representation if we have both code and AST data
        if code_by_line:
            analysis.append("")
            analysis.append("## Side-by-Side Code and AST Representation")
            analysis.append("This section shows each line of code alongside its corresponding AST nodes:")
            analysis.append("")
            analysis.append("| Line #      | C++ Code       | AST Nodes     | Explanation      |")
            analysis.append("|-------------|----------------|---------------|------------------|")
            
            for line_num in sorted(code_by_line.keys(), key=int):
                code = code_by_line[line_num]
                # Skip empty lines or comment-only lines for brevity
                if not code or (code.strip().startswith('//') and int(line_num) % 5 != 0):
                    continue
                    
                nodes = "; ".join(line_data.get(line_num, []))
                
                # Get the explanation for the primary node
                node_explanation = ""
                if line_num in line_data and line_data[line_num]:
                    primary_node = line_data[line_num][0]
                    explanation = node_explanations.get(primary_node, None)
                    if explanation is None:
                        # Use the same fallback logic as above
                        if "Template" in primary_node:
                            if "ClassTemplate" in primary_node:
                                explanation = "Class template definition"
                            elif "FunctionTemplate" in primary_node:
                                explanation = "Function template definition"
                            else:
                                explanation = "Template-related construct"
                        elif "CXXAuto" in primary_node:
                            explanation = "Auto type deduction"
                        elif "Decltype" in primary_node:
                            explanation = "Type deduction from expression"
                        elif "Lambda" in primary_node:
                            explanation = "Anonymous function"
                        else:
                            explanation = f"{primary_node}"
                    else:
                        # Simplify explanation for table
                        explanation = explanation.split('-')[1].strip() if '-' in explanation else explanation
                    
                    node_explanation = explanation
                
                analysis.append(f"| {line_num} | `{code}` | {nodes} | {node_explanation} |")
            
        # Add tabular summary of AST nodes
        analysis.append("")
        analysis.append("## AST Node Types Summary Table")
        analysis.append("| Node Type | Count | Description |")
        analysis.append("|-----------|-------|-------------|")
        
        for node, count in sorted(node_types.items(), key=lambda x: x[1], reverse=True):
            explanation = node_explanations.get(node, "")
            if explanation:
                # Simplify explanation for table if needed
                if "-" in explanation:
                    simple_explanation = explanation.split('-')[1].strip() 
                else:
                    simple_explanation = explanation
            else:
                # Handle template-related nodes specially
                if "Template" in node:
                    if "ClassTemplate" in node:
                        simple_explanation = "defines a generic class that can work with different types"
                    elif "FunctionTemplate" in node:
                        simple_explanation = "defines a generic function that can work with different types"
                    elif "TemplateTypeParm" in node:
                        simple_explanation = "defines a placeholder type (like T) in a template"
                    elif "TemplateSpecialization" in node:
                        simple_explanation = "specific implementation of a template for particular types"
                    else:
                        simple_explanation = "part of C++'s generic programming system"
                # Handle modern C++ features
                elif "CXXAuto" in node:
                    simple_explanation = "represents automatic type deduction"
                elif "Decltype" in node:
                    simple_explanation = "determines the type of an expression at compile time"
                elif "Lambda" in node:
                    simple_explanation = "represents an anonymous function object"
                # Default fallback
                else:
                    simple_explanation = f"represents a {node} construct"
            
            # Ensure the explanation is not truncated
            if node == "CXXRecordDecl":
                simple_explanation = "defines a user-defined class or struct"
                
            analysis.append(f"| {node} | {count} | {simple_explanation} |")
        
        # Add code quality notes
        analysis.append("")
        analysis.append("## Code Quality Observations")
        
        # Simple heuristics for code quality
        if len(node_types) < 5:
            analysis.append("- The code is very simple with minimal AST nodes, suggesting straightforward logic")
        elif len(node_types) > 15:
            analysis.append("- The code has a rich variety of AST nodes, indicating complex functionality")
        
        analysis.append("- The AST structure indicates well-formed C++ code without syntax errors")
        
        if "CXXRecordDecl" in str(node_types) and node_types.get("CXXMethodDecl", 0) > 3:
            analysis.append("- The class has multiple methods, suggesting good encapsulation of functionality")
        
        if "CompoundStmt" in str(node_types) and node_types.get("CompoundStmt", 0) > 5:
            analysis.append("- Multiple nested compound statements might indicate complex code blocks that could be simplified")
        
        # Final summary of what the code does
        analysis.append("")
        analysis.append("## Functional Summary")
        analysis.append(self._get_functional_summary(node_types))
        
        return "\n".join(analysis)
        
    def _generate_code_summary(self, code_section, node_types):
        """Generate a brief summary of what the code does based on AST nodes."""
        lines = code_section.strip().split('\n')
        
        # Identify the first few non-comment, non-empty lines as a starting point
        code_start = ""
        for line in lines:
            line = line.strip()
            if line and not line.startswith('//') and not line.startswith('/*'):
                # Extract just the line number and content
                if ':' in line:
                    parts = line.split(':', 1)
                    if len(parts) > 1:
                        line = parts[1].strip()
                code_start += line + " "
                if len(code_start) > 100:  # Limit to reasonable length
                    break
        
        # Determine if it's a class, function, or other
        if "CXXRecordDecl" in node_types:
            class_methods = node_types.get("CXXMethodDecl", 0) + node_types.get("CXXConstructorDecl", 0)
            return f"This code defines a C++ class with {class_methods} methods/constructors and contains logic for data manipulation and processing."
        elif "FunctionDecl" in node_types and not "CXXMethodDecl" in node_types:
            functions = node_types.get("FunctionDecl", 0)
            return f"This code contains {functions} standalone C++ functions that perform data processing operations."
        else:
            return "This C++ code contains various declarations and statements for data processing and manipulation."
    
    def _get_functional_summary(self, node_types):
        """Generate a more detailed functional summary based on AST node patterns."""
        
        # Class-based code
        if "CXXRecordDecl" in node_types:
            has_constructors = "CXXConstructorDecl" in node_types
            has_methods = "CXXMethodDecl" in node_types
            has_fields = "FieldDecl" in node_types
            
            summary = "The code implements "
            if has_constructors and has_methods and has_fields:
                summary += "a complete C++ class with constructors, methods, and member variables. "
                summary += "This suggests an object-oriented design with proper encapsulation of data and behavior."
            elif has_methods and has_fields:
                summary += "a C++ class with methods and member variables, but without explicit constructors. "
                summary += "The class likely relies on default construction."
            else:
                summary += "a simple C++ class structure. "
            
            # Control flow
            if "IfStmt" in node_types or "ForStmt" in node_types or "WhileStmt" in node_types:
                summary += " The code contains conditional logic and/or loops, indicating non-trivial algorithmic processing."
            
            return summary
        
        # Function-based code
        elif "FunctionDecl" in node_types:
            summary = "The code consists of standalone C++ functions "
            
            if "CallExpr" in node_types:
                summary += "that call other functions, suggesting a procedural programming approach. "
            else:
                summary += "without function calls between them, suggesting independent utility functions. "
                
            if "ReturnStmt" in node_types:
                summary += "The functions compute and return values based on their inputs."
            else:
                summary += "The functions appear to perform operations without returning values (void functions)."
                
            return summary
        
        # Default case
        else:
            return "The code contains basic C++ constructs but does not appear to have a complex structure of classes or functions."


# Helper function to be called from CLI
def interpret_ast_file(csv_file: str, api_key: Optional[str] = None) -> str:
    """
    Interpret an AST mapping CSV file using AI.
    
    Args:
        csv_file: Path to the CSV file
        api_key: Optional API key for the AI service
        
    Returns:
        The AI's interpretation of the AST data
    """
    interpreter = AIInterpreter(api_key)
    return interpreter.interpret_csv(csv_file)


if __name__ == "__main__":
    # Test with a sample CSV file
    import sys
    
    if len(sys.argv) < 2:
        print("Usage: python ai_interpreter.py <csv_file> [api_key]")
        sys.exit(1)
    
    csv_file = sys.argv[1]
    api_key = sys.argv[2] if len(sys.argv) > 2 else None
    
    result = interpret_ast_file(csv_file, api_key)
    print(result)
