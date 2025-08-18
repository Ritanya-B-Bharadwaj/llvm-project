"""
AST Parser Module
Handles parsing of Clang AST JSON output and line mapping
"""

import json
import subprocess
import os
from collections import defaultdict
from pathlib import Path

class ASTParser:
    """Parses Clang AST JSON and creates line mappings."""
    
    def __init__(self):
        self.line_to_nodes = defaultdict(list)
    
    def generate_ast_json(self, cpp_file):
        """Generate AST JSON using Clang."""
        ast_file = self._get_ast_filename(cpp_file)
        
        # Try different clang commands
        clang_commands = [
            f'clang++ -Xclang -ast-dump=json -fsyntax-only "{cpp_file}"',
            f'clang -Xclang -ast-dump=json -fsyntax-only "{cpp_file}"',
        ]
        
        # On Windows, try with Visual Studio includes
        if os.name == 'nt':
            vs_includes = [
                r'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.35.32215\include',
                r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC\14.35.32215\include',
                r'C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\include',
            ]
            
            for include_path in vs_includes:
                if os.path.exists(include_path):
                    clang_commands.append(
                        f'clang++ -Xclang -ast-dump=json -fsyntax-only -I"{include_path}" "{cpp_file}"'
                    )
                    break
        
        for cmd in clang_commands:
            try:
                print(f"  Trying: {cmd}")
                result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
                if result.returncode == 0:
                    with open(ast_file, 'w', encoding='utf-8') as f:
                        f.write(result.stdout)
                    print(f"  Success: AST JSON generated: {ast_file}")
                    return ast_file
                else:
                    print(f"  Failed: {result.stderr.strip()}")
            except Exception as e:
                print(f"  Exception: {e}")
        
        print("Failed to generate AST JSON with any Clang command")
        self._print_setup_help()
        return None
    
    def _get_ast_filename(self, cpp_file):
        """Get the AST filename for a given C++ file."""
        base_name = Path(cpp_file).stem
        return f"{base_name}_ast.json"
    
    def _print_setup_help(self):
        """Print help for setting up Clang."""
        print("\nSetup Help:")
        print("1. Ensure Clang is installed and in PATH")
        print("2. For Windows: Run from Developer Command Prompt or Visual Studio Developer PowerShell")
        print("3. For standard library headers, make sure your development environment is properly configured")
        print("4. Try compiling a simple file manually first: clang++ -fsyntax-only yourfile.cpp")
    
    def parse_ast_file(self, ast_file, cpp_file):
        """Parse AST JSON file and create line mappings."""
        try:
            with open(ast_file, 'r', encoding='utf-8') as f:
                ast_data = json.load(f)
        except Exception as e:
            print(f"Error reading AST JSON: {e}")
            return None
        
        # Clear previous mappings
        self.line_to_nodes.clear()
        
        # Collect nodes by line
        self._collect_nodes_by_line(ast_data, cpp_file)
        
        # Convert to regular dict and remove duplicates
        line_mappings = {}
        for line_num, nodes in self.line_to_nodes.items():
            line_mappings[line_num] = list(set(nodes))  # Remove duplicates
        
        return line_mappings
    
    def _collect_nodes_by_line(self, node, current_file):
        """Recursively collect AST nodes by line number."""
        if isinstance(node, dict):
            # Check if this node has location information
            loc = node.get('loc', {})
            range_info = node.get('range', {})
            
            # Try to get line number from 'loc' or 'range.begin'
            line = self._extract_line_number(loc, range_info)
            
            # If we have a line number and node kind, add to mapping
            if line and 'kind' in node:
                kind = node['kind']
                # Only add if it's from the main file (not includes)
                if self._is_from_main_file(loc, range_info, current_file):
                    self.line_to_nodes[line].append(kind)
            
            # Recursively process all values
            for key, value in node.items():
                if key == 'inner':  # 'inner' contains child nodes
                    self._collect_nodes_by_line(value, current_file)
                elif isinstance(value, (dict, list)):
                    self._collect_nodes_by_line(value, current_file)
        
        elif isinstance(node, list):
            for item in node:
                self._collect_nodes_by_line(item, current_file)
    
    def _extract_line_number(self, loc, range_info):
        """Extract line number from location information."""
        # Try to get line number from 'loc'
        if 'line' in loc:
            return loc['line']
        
        # Try to get line number from 'range.begin'
        if 'begin' in range_info and isinstance(range_info['begin'], dict):
            if 'line' in range_info['begin']:
                return range_info['begin']['line']
        
        return None
    
    def _is_from_main_file(self, loc, range_info, current_file):
        """Check if the AST node is from the main file (not includes)."""
        # Get file information
        file_info = loc.get('file')
        if not file_info and 'begin' in range_info:
            file_info = range_info.get('begin', {}).get('file')
        
        # If no file info, assume it's from main file
        if not file_info:
            return True
        
        # Check if file path matches current file
        if isinstance(file_info, str):
            # Normalize paths for comparison
            file_path = os.path.normpath(file_info)
            current_path = os.path.normpath(os.path.abspath(current_file))
            return file_path == current_path
        
        return True  # Default to including if we can't determine
    
    def get_statistics(self):
        """Get statistics about the parsed AST."""
        if not self.line_to_nodes:
            return {}
        
        total_lines = len(self.line_to_nodes)
        total_nodes = sum(len(nodes) for nodes in self.line_to_nodes.values())
        
        # Count node types
        node_counts = defaultdict(int)
        for nodes in self.line_to_nodes.values():
            for node in nodes:
                node_counts[node] += 1
        
        return {
            'total_lines_with_ast': total_lines,
            'total_ast_nodes': total_nodes,
            'avg_nodes_per_line': total_nodes / total_lines if total_lines > 0 else 0,
            'node_type_counts': dict(node_counts),
            'most_common_nodes': sorted(node_counts.items(), key=lambda x: x[1], reverse=True)[:10]
        }
