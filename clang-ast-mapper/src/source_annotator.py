"""
Source Annotator Module
Handles annotation of source code with AST information
"""

import os
from pathlib import Path

class SourceAnnotator:
    """Annotates source code with AST node information."""
    
    def __init__(self):
        pass
    
    def annotate_source(self, cpp_file, line_mappings, include_explanations=False, explanations_dict=None):
        """Annotate source file with AST node information."""
        try:
            with open(cpp_file, 'r', encoding='utf-8') as f:
                source_lines = f.readlines()
        except Exception as e:
            return f"❌ Error reading source file: {e}"
        
        output_lines = []
        output_lines.append(f"{'='*60}")
        output_lines.append(f"AST-ANNOTATED SOURCE: {cpp_file}")
        output_lines.append(f"{'='*60}")
        output_lines.append("")
        
        for line_num, line in enumerate(source_lines, 1):
            # Add the source line
            output_lines.append(f"{line_num:3d}: {line.rstrip()}")
            
            # Add AST nodes for this line
            if line_num in line_mappings:
                nodes = line_mappings[line_num]
                if nodes:
                    output_lines.append(f"     AST: {', '.join(nodes)}")
                    
                    # Add explanations if requested
                    if include_explanations and explanations_dict:
                        for node in nodes:
                            explanation = explanations_dict.get(node, f"AST node of type {node}")
                            output_lines.append(f"          → {node}: {explanation}")
            
            output_lines.append("")  # Empty line for readability
        
        return "\n".join(output_lines)
    
    def side_by_side_view(self, cpp_file, line_mappings, explanations_dict=None):
        """Generate side-by-side view of source and AST nodes."""
        try:
            with open(cpp_file, 'r', encoding='utf-8') as f:
                source_lines = f.readlines()
        except Exception as e:
            return f"❌ Error reading source file: {e}"
        
        output_lines = []
        output_lines.append(f"{'='*100}")
        output_lines.append(f"SIDE-BY-SIDE VIEW: {cpp_file}")
        output_lines.append(f"{'='*100}")
        output_lines.append(f"{'SOURCE CODE':<60} {'AST NODES':<40}")
        output_lines.append(f"{'-'*60} {'-'*40}")
        
        for line_num, line in enumerate(source_lines, 1):
            source_part = f"{line_num:3d}: {line.rstrip()}"
            
            # Get AST nodes for this line
            if line_num in line_mappings and line_mappings[line_num]:
                nodes = line_mappings[line_num]
                ast_part = f"AST: {', '.join(nodes)}"
            else:
                ast_part = ""
            
            output_lines.append(f"{source_part:<60} {ast_part:<40}")
            
            # Add explanations if available
            if explanations_dict and line_num in line_mappings:
                for node in line_mappings[line_num]:
                    explanation = explanations_dict.get(node, f"AST node of type {node}")
                    if len(explanation) > 35:
                        explanation = explanation[:32] + "..."
                    output_lines.append(f"{'':60} → {node}: {explanation}")
        
        return "\n".join(output_lines)
    
    def generate_html_output(self, cpp_file, line_mappings, explanations_dict=None):
        """Generate HTML output with syntax highlighting and tooltips."""
        try:
            with open(cpp_file, 'r', encoding='utf-8') as f:
                source_lines = f.readlines()
        except Exception as e:
            return f"<p>Error reading source file: {e}</p>"
        
        html_lines = []
        html_lines.append("<!DOCTYPE html>")
        html_lines.append("<html>")
        html_lines.append("<head>")
        html_lines.append("<title>AST Annotated Source</title>")
        html_lines.append("<style>")
        html_lines.append(self._get_css_styles())
        html_lines.append("</style>")
        html_lines.append("</head>")
        html_lines.append("<body>")
        html_lines.append(f"<h1>AST-Annotated Source: {os.path.basename(cpp_file)}</h1>")
        
        # Add table format
        html_lines.append('<table class="ast-table">')
        html_lines.append('<thead>')
        html_lines.append('<tr>')
        html_lines.append('<th>Line</th>')
        html_lines.append('<th>Source Code</th>')
        html_lines.append('<th>AST Nodes</th>')
        if explanations_dict:
            html_lines.append('<th>Explanations</th>')
        html_lines.append('</tr>')
        html_lines.append('</thead>')
        html_lines.append('<tbody>')
        
        for line_num, line in enumerate(source_lines, 1):
            # Create line with AST information
            line_class = "source-line"
            if line_num in line_mappings and line_mappings[line_num]:
                line_class += " has-ast"
            
            html_lines.append(f'<tr class="{line_class}">')
            html_lines.append(f'<td class="line-number">{line_num}</td>')
            html_lines.append(f'<td class="source-code">{self._escape_html(line.rstrip())}</td>')
            
            # Add AST information
            if line_num in line_mappings and line_mappings[line_num]:
                nodes = line_mappings[line_num]
                html_lines.append(f'<td class="ast-nodes">{", ".join(nodes)}</td>')
                
                # Add explanations
                if explanations_dict:
                    explanations_html = []
                    for node in nodes:
                        explanation = explanations_dict.get(node, f"AST node of type {node}")
                        explanations_html.append(f'<div class="explanation"><strong>{node}:</strong> {explanation}</div>')
                    html_lines.append(f'<td class="explanations">{"".join(explanations_html)}</td>')
            else:
                html_lines.append('<td class="ast-nodes"></td>')
                if explanations_dict:
                    html_lines.append('<td class="explanations"></td>')
            
            html_lines.append('</tr>')
        
        html_lines.append('</tbody>')
        html_lines.append('</table>')
        html_lines.append("</body>")
        html_lines.append("</html>")
        
        return "\n".join(html_lines)
    
    def _escape_html(self, text):
        """Escape HTML special characters."""
        return (text
                .replace('&', '&amp;')
                .replace('<', '&lt;')
                .replace('>', '&gt;')
                .replace('"', '&quot;')
                .replace("'", '&#39;'))
    
    def _get_css_styles(self):
        """Get CSS styles for HTML output."""
        return """
        body {
            font-family: 'Courier New', monospace;
            margin: 20px;
            background-color: #f5f5f5;
        }
        
        h1 {
            color: #333;
            border-bottom: 2px solid #007acc;
            padding-bottom: 10px;
            text-align: center;
        }
        
        .ast-table {
            width: 100%;
            border-collapse: collapse;
            background-color: white;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            border-radius: 5px;
            overflow: hidden;
        }
        
        .ast-table th {
            background-color: #007acc;
            color: white;
            padding: 12px;
            text-align: left;
            font-weight: bold;
        }
        
        .ast-table td {
            padding: 8px 12px;
            border-bottom: 1px solid #ddd;
            vertical-align: top;
        }
        
        .ast-table tr:nth-child(even) {
            background-color: #f9f9f9;
        }
        
        .ast-table tr.has-ast {
            background-color: #e6f3ff;
        }
        
        .ast-table tr.has-ast:nth-child(even) {
            background-color: #d9ecff;
        }
        
        .line-number {
            color: #666;
            font-weight: bold;
            text-align: right;
            width: 50px;
        }
        
        .source-code {
            color: #333;
            font-family: 'Courier New', monospace;
            white-space: pre;
            min-width: 400px;
        }
        
        .ast-nodes {
            color: #007acc;
            font-weight: bold;
            min-width: 200px;
        }
        
        .explanations {
            color: #666;
            font-size: 0.9em;
            max-width: 300px;
        }
        
        .explanation {
            margin-bottom: 4px;
            padding: 2px 4px;
            background-color: #f0f0f0;
            border-radius: 3px;
        }
        
        .explanation strong {
            color: #007acc;
        }
        
        /* Hover effects */
        .ast-table tr:hover {
            background-color: #fff3cd;
        }
        
        .ast-table tr.has-ast:hover {
            background-color: #cce7ff;
        }
        
        /* Responsive design */
        @media (max-width: 768px) {
            .ast-table {
                font-size: 0.8em;
            }
            
            .ast-table th,
            .ast-table td {
                padding: 6px 8px;
            }
            
            .source-code {
                min-width: 250px;
            }
            
            .explanations {
                max-width: 200px;
            }
        }
        """
    
    def generate_markdown_output(self, cpp_file, line_mappings, explanations_dict=None):
        """Generate Markdown output."""
        try:
            with open(cpp_file, 'r', encoding='utf-8') as f:
                source_lines = f.readlines()
        except Exception as e:
            return f"Error reading source file: {e}"
        
        output_lines = []
        output_lines.append(f"# AST-Annotated Source: {os.path.basename(cpp_file)}")
        output_lines.append("")
        output_lines.append("```cpp")
        
        for line_num, line in enumerate(source_lines, 1):
            # Add the source line
            output_lines.append(f"{line_num:3d}: {line.rstrip()}")
            
            # Add AST nodes for this line
            if line_num in line_mappings and line_mappings[line_num]:
                nodes = line_mappings[line_num]
                ast_comment = f"// AST: {', '.join(nodes)}"
                output_lines.append(f"     {ast_comment}")
                
                # Add explanations if requested
                if explanations_dict:
                    for node in nodes:
                        explanation = explanations_dict.get(node, f"AST node of type {node}")
                        output_lines.append(f"     // → {node}: {explanation}")
            
            output_lines.append("")  # Empty line for readability
        
        output_lines.append("```")
        return "\n".join(output_lines)
    
    def generate_table_output(self, cpp_file, line_mappings, explanations_dict=None):
        """Generate table format output."""
        try:
            with open(cpp_file, 'r', encoding='utf-8') as f:
                source_lines = f.readlines()
        except Exception as e:
            return f"Error reading source file: {e}"
        
        output_lines = []
        output_lines.append(f"AST Table for: {os.path.basename(cpp_file)}")
        output_lines.append("=" * 120)
        
        # Table header
        output_lines.append(f"{'Line':<4} | {'Source Code':<60} | {'AST Nodes':<50}")
        output_lines.append("-" * 4 + "-+-" + "-" * 60 + "-+-" + "-" * 50)
        
        for line_num, line in enumerate(source_lines, 1):
            source_code = line.rstrip()
            if len(source_code) > 57:
                source_code = source_code[:54] + "..."
            
            # Get AST nodes for this line
            if line_num in line_mappings and line_mappings[line_num]:
                nodes = line_mappings[line_num]
                ast_nodes = ", ".join(nodes)
                if len(ast_nodes) > 47:
                    ast_nodes = ast_nodes[:44] + "..."
            else:
                ast_nodes = ""
            
            output_lines.append(f"{line_num:<4} | {source_code:<60} | {ast_nodes:<50}")
            
            # Add explanations if requested
            if explanations_dict and line_num in line_mappings and line_mappings[line_num]:
                for node in line_mappings[line_num]:
                    explanation = explanations_dict.get(node, f"AST node of type {node}")
                    if len(explanation) > 47:
                        explanation = explanation[:44] + "..."
                    output_lines.append(f"{'':4} | {'':60} | → {node}: {explanation}")
        
        return "\n".join(output_lines)
    
    def generate_csv_output(self, cpp_file, line_mappings, explanations_dict=None):
        """Generate CSV format output."""
        import csv
        import io
        
        try:
            with open(cpp_file, 'r', encoding='utf-8') as f:
                source_lines = f.readlines()
        except Exception as e:
            return f"Error reading source file: {e}"
        
        # Use StringIO to build CSV in memory
        output_buffer = io.StringIO()
        csv_writer = csv.writer(output_buffer, quoting=csv.QUOTE_MINIMAL, lineterminator='\n')
        
        # Write header
        csv_writer.writerow(["Line", "Source Code", "AST Nodes", "Explanations"])
        
        for line_num, line in enumerate(source_lines, 1):
            source_code = line.rstrip()
            
            # Get AST nodes for this line
            if line_num in line_mappings and line_mappings[line_num]:
                nodes = line_mappings[line_num]
                ast_nodes = "; ".join(nodes)
                
                # Get explanations if requested
                if explanations_dict:
                    explanations = []
                    for node in nodes:
                        explanation = explanations_dict.get(node, f"AST node of type {node}")
                        explanations.append(f"{node}: {explanation}")
                    explanations_str = "; ".join(explanations)
                else:
                    explanations_str = ""
            else:
                ast_nodes = ""
                explanations_str = ""
            
            csv_writer.writerow([line_num, source_code, ast_nodes, explanations_str])
        
        return output_buffer.getvalue()
