"""
Unit tests for AST Parser
"""

import unittest
import os
import tempfile
import json
from pathlib import Path

# Add the src directory to the Python path
import sys
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from ast_parser import ASTParser

class TestASTParser(unittest.TestCase):
    def setUp(self):
        """Set up test fixtures."""
        self.parser = ASTParser()
        
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
    
    def tearDown(self):
        """Clean up test fixtures."""
        # Remove temporary files
        if os.path.exists(self.temp_cpp.name):
            os.unlink(self.temp_cpp.name)
        
        # Remove generated AST file
        ast_file = self.parser._get_ast_filename(self.temp_cpp.name)
        if os.path.exists(ast_file):
            os.unlink(ast_file)
    
    def test_get_ast_filename(self):
        """Test AST filename generation."""
        filename = self.parser._get_ast_filename("test.cpp")
        self.assertEqual(filename, "test_ast.json")
        
        filename = self.parser._get_ast_filename("/path/to/file.cpp")
        self.assertEqual(filename, "file_ast.json")
    
    def test_generate_ast_json(self):
        """Test AST JSON generation."""
        ast_file = self.parser.generate_ast_json(self.temp_cpp.name)
        
        # Check if AST file was created
        if ast_file:  # Only test if Clang is available
            self.assertIsNotNone(ast_file)
            self.assertTrue(os.path.exists(ast_file))
            
            # Check if file contains valid JSON
            with open(ast_file, 'r') as f:
                try:
                    json.load(f)
                except json.JSONDecodeError:
                    self.fail("Generated AST file is not valid JSON")
    
    def test_parse_ast_file(self):
        """Test AST file parsing."""
        # First generate AST
        ast_file = self.parser.generate_ast_json(self.temp_cpp.name)
        
        if ast_file:  # Only test if Clang is available
            # Parse the AST file
            line_mappings = self.parser.parse_ast_file(ast_file, self.temp_cpp.name)
            
            self.assertIsNotNone(line_mappings)
            self.assertIsInstance(line_mappings, dict)
            
            # Check that we have some line mappings
            self.assertGreater(len(line_mappings), 0)
            
            # Check that line numbers are integers
            for line_num in line_mappings.keys():
                self.assertIsInstance(line_num, int)
                self.assertGreater(line_num, 0)
            
            # Check that node lists are lists of strings
            for nodes in line_mappings.values():
                self.assertIsInstance(nodes, list)
                for node in nodes:
                    self.assertIsInstance(node, str)
    
    def test_extract_line_number(self):
        """Test line number extraction."""
        # Test with loc having line
        loc = {'line': 5}
        range_info = {}
        line = self.parser._extract_line_number(loc, range_info)
        self.assertEqual(line, 5)
        
        # Test with range.begin having line
        loc = {}
        range_info = {'begin': {'line': 10}}
        line = self.parser._extract_line_number(loc, range_info)
        self.assertEqual(line, 10)
        
        # Test with no line information
        loc = {}
        range_info = {}
        line = self.parser._extract_line_number(loc, range_info)
        self.assertIsNone(line)
    
    def test_is_from_main_file(self):
        """Test main file detection."""
        current_file = "/path/to/test.cpp"
        
        # Test with matching file
        loc = {'file': '/path/to/test.cpp'}
        range_info = {}
        result = self.parser._is_from_main_file(loc, range_info, current_file)
        self.assertTrue(result)
        
        # Test with no file info (should default to True)
        loc = {}
        range_info = {}
        result = self.parser._is_from_main_file(loc, range_info, current_file)
        self.assertTrue(result)
    
    def test_get_statistics(self):
        """Test statistics generation."""
        # Test empty statistics
        stats = self.parser.get_statistics()
        self.assertEqual(stats, {})
        
        # Add some test data
        self.parser.line_to_nodes[1] = ['FunctionDecl', 'ParmVarDecl']
        self.parser.line_to_nodes[2] = ['ReturnStmt']
        
        stats = self.parser.get_statistics()
        self.assertEqual(stats['total_lines_with_ast'], 2)
        self.assertEqual(stats['total_ast_nodes'], 3)
        self.assertEqual(stats['avg_nodes_per_line'], 1.5)
        self.assertIn('node_type_counts', stats)
        self.assertIn('most_common_nodes', stats)

if __name__ == '__main__':
    unittest.main()
