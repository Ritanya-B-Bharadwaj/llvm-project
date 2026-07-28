"""
Integration tests for the complete AST Line Mapper tool
"""

import unittest
import os
import tempfile
import subprocess
import sys
from pathlib import Path

class TestIntegration(unittest.TestCase):
    def setUp(self):
        """Set up test fixtures."""
        # Create a temporary C++ file for testing
        self.temp_cpp = tempfile.NamedTemporaryFile(mode='w', suffix='.cpp', delete=False)
        self.temp_cpp.write("""
int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 3);
    return 0;
}
""")
        self.temp_cpp.close()
        
        # Get the path to the main script
        self.script_path = os.path.join(
            os.path.dirname(__file__), 
            '..', 
            'ast_line_mapper.py'
        )
    
    def tearDown(self):
        """Clean up test fixtures."""
        if os.path.exists(self.temp_cpp.name):
            os.unlink(self.temp_cpp.name)
        
        # Clean up generated AST files
        base_name = Path(self.temp_cpp.name).stem
        ast_file = f"{base_name}_ast.json"
        if os.path.exists(ast_file):
            os.unlink(ast_file)
    
    def test_basic_usage(self):
        """Test basic usage of the tool."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                self.temp_cpp.name
            ], capture_output=True, text=True, timeout=30)
            
            # Check if the command ran successfully or failed due to missing Clang
            if result.returncode == 0:
                self.assertIn("AST-ANNOTATED SOURCE", result.stdout)
                self.assertIn("FunctionDecl", result.stdout)
            else:
                # If Clang is not available, check for appropriate error message
                self.assertIn("Failed to generate AST JSON", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")
    
    def test_explanations_flag(self):
        """Test the --explanations flag."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                self.temp_cpp.name,
                '--explanations'
            ], capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                self.assertIn("AST-ANNOTATED SOURCE", result.stdout)
                self.assertIn("function declaration", result.stdout)
                self.assertIn("→", result.stdout)
            else:
                self.assertIn("Failed to generate AST JSON", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")
    
    def test_side_by_side_flag(self):
        """Test the --side-by-side flag."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                self.temp_cpp.name,
                '--side-by-side'
            ], capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                self.assertIn("SIDE-BY-SIDE VIEW", result.stdout)
                self.assertIn("SOURCE CODE", result.stdout)
                self.assertIn("AST NODES", result.stdout)
            else:
                self.assertIn("Failed to generate AST JSON", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")
    
    def test_generate_ast_flag(self):
        """Test the --generate-ast flag."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                self.temp_cpp.name,
                '--generate-ast'
            ], capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                self.assertIn("AST JSON generated", result.stdout)
                # Should not contain annotation output
                self.assertNotIn("AST-ANNOTATED SOURCE", result.stdout)
            else:
                self.assertIn("Failed to generate AST JSON", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")
    
    def test_json_format(self):
        """Test JSON format output."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                self.temp_cpp.name,
                '--format', 'json'
            ], capture_output=True, text=True, timeout=30)
            
            if result.returncode == 0:
                # Should contain JSON output
                self.assertIn('"line_mappings"', result.stdout)
                self.assertIn('"explanations"', result.stdout)
            else:
                self.assertIn("Failed to generate AST JSON", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")
    
    def test_help_flag(self):
        """Test the --help flag."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                '--help'
            ], capture_output=True, text=True, timeout=10)
            
            self.assertEqual(result.returncode, 0)
            self.assertIn("Map C++ source lines to AST nodes", result.stdout)
            self.assertIn("--explanations", result.stdout)
            self.assertIn("--side-by-side", result.stdout)
            self.assertIn("--generate-ast", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")
    
    def test_version_flag(self):
        """Test the --version flag."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                '--version'
            ], capture_output=True, text=True, timeout=10)
            
            self.assertEqual(result.returncode, 0)
            self.assertIn("Clang AST Line Mapper", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")
    
    def test_nonexistent_file(self):
        """Test handling of non-existent files."""
        try:
            result = subprocess.run([
                sys.executable, 
                self.script_path, 
                'nonexistent_file.cpp'
            ], capture_output=True, text=True, timeout=10)
            
            self.assertNotEqual(result.returncode, 0)
            self.assertIn("File not found", result.stdout)
                
        except subprocess.TimeoutExpired:
            self.fail("Tool execution timed out")
        except FileNotFoundError:
            self.skipTest("Python executable not found")

if __name__ == '__main__':
    unittest.main()
