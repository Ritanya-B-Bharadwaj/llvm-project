"""
Unit tests for Source Annotator
"""

import unittest
import os
import tempfile
from pathlib import Path

# Add the src directory to the Python path
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from source_annotator import SourceAnnotator

class TestSourceAnnotator(unittest.TestCase):
    def setUp(self):
        """Set up test fixtures."""
        self.annotator = SourceAnnotator()
        
        # Create a temporary C++ file for testing
        self.temp_cpp = tempfile.NamedTemporaryFile(mode='w', suffix='.cpp', delete=False)
        self.temp_cpp.write("""int add(int a, int b) {
    return a + b;
}

int main() {
    int result = add(5, 3);
    return 0;
}""")
        self.temp_cpp.close()
        
        # Sample line mappings for testing
        self.line_mappings = {
            1: ['FunctionDecl', 'ParmVarDecl'],
            2: ['ReturnStmt'],
            5: ['FunctionDecl'],
            6: ['DeclStmt', 'CallExpr'],
            7: ['ReturnStmt']
        }
        
        # Sample explanations
        self.explanations = {
            'FunctionDecl': 'function declaration',
            'ParmVarDecl': 'parameter variable declaration',
            'ReturnStmt': 'return statement',
            'DeclStmt': 'declaration statement',
            'CallExpr': 'function call expression'
        }
    
    def tearDown(self):
        """Clean up test fixtures."""
        if os.path.exists(self.temp_cpp.name):
            os.unlink(self.temp_cpp.name)
    
    def test_annotate_source_basic(self):
        """Test basic source annotation."""
        result = self.annotator.annotate_source(
            self.temp_cpp.name,
            self.line_mappings,
            include_explanations=False
        )
        
        self.assertIsInstance(result, str)
        self.assertIn("AST-ANNOTATED SOURCE", result)
        self.assertIn("FunctionDecl", result)
        self.assertIn("ReturnStmt", result)
    
    def test_annotate_source_with_explanations(self):
        """Test source annotation with explanations."""
        result = self.annotator.annotate_source(
            self.temp_cpp.name,
            self.line_mappings,
            include_explanations=True,
            explanations_dict=self.explanations
        )
        
        self.assertIsInstance(result, str)
        self.assertIn("AST-ANNOTATED SOURCE", result)
        self.assertIn("FunctionDecl", result)
        self.assertIn("function declaration", result)
        self.assertIn("→", result)
    
    def test_side_by_side_view(self):
        """Test side-by-side view generation."""
        result = self.annotator.side_by_side_view(
            self.temp_cpp.name,
            self.line_mappings
        )
        
        self.assertIsInstance(result, str)
        self.assertIn("SIDE-BY-SIDE VIEW", result)
        self.assertIn("SOURCE CODE", result)
        self.assertIn("AST NODES", result)
        self.assertIn("FunctionDecl", result)
    
    def test_side_by_side_view_with_explanations(self):
        """Test side-by-side view with explanations."""
        result = self.annotator.side_by_side_view(
            self.temp_cpp.name,
            self.line_mappings,
            explanations_dict=self.explanations
        )
        
        self.assertIsInstance(result, str)
        self.assertIn("SIDE-BY-SIDE VIEW", result)
        self.assertIn("function declaration", result)
        self.assertIn("→", result)
    
    def test_generate_html_output(self):
        """Test HTML output generation."""
        result = self.annotator.generate_html_output(
            self.temp_cpp.name,
            self.line_mappings,
            explanations_dict=self.explanations
        )
        
        self.assertIsInstance(result, str)
        self.assertIn("<!DOCTYPE html>", result)
        self.assertIn("<html>", result)
        self.assertIn("AST-Annotated Source", result)
        self.assertIn("FunctionDecl", result)
        self.assertIn("function declaration", result)
    
    def test_generate_markdown_output(self):
        """Test Markdown output generation."""
        result = self.annotator.generate_markdown_output(
            self.temp_cpp.name,
            self.line_mappings,
            explanations_dict=self.explanations
        )
        
        self.assertIsInstance(result, str)
        self.assertIn("# AST-Annotated Source", result)
        self.assertIn("```cpp", result)
        self.assertIn("// AST:", result)
        self.assertIn("FunctionDecl", result)
        self.assertIn("function declaration", result)
    
    def test_escape_html(self):
        """Test HTML escaping."""
        test_string = '<div class="test">Hello & "world"</div>'
        escaped = self.annotator._escape_html(test_string)
        
        self.assertNotIn('<', escaped)
        self.assertNotIn('>', escaped)
        self.assertNotIn('&', escaped)
        self.assertNotIn('"', escaped)
        self.assertIn('&lt;', escaped)
        self.assertIn('&gt;', escaped)
        self.assertIn('&amp;', escaped)
        self.assertIn('&quot;', escaped)
    
    def test_file_not_found(self):
        """Test handling of non-existent files."""
        result = self.annotator.annotate_source(
            "non_existent_file.cpp",
            self.line_mappings
        )
        
        self.assertIn("Error reading source file", result)

if __name__ == '__main__':
    unittest.main()
