#!/usr/bin/env python3
"""
Clang AST Line Mapper Tool
Maps source lines to AST nodes using Clang's JSON AST dump

Main CLI interface for the AST line mapping tool.
"""

import argparse
import sys
import os
from pathlib import Path

# Add the src directory to the Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

from ast_parser import ASTParser
from source_annotator import SourceAnnotator
from node_explanations import NodeExplanations
from ai_interpreter import interpret_ast_file

class ASTLineMapper:
    """Main class for the AST line mapping tool."""
    
    def __init__(self):
        self.parser = ASTParser()
        self.annotator = SourceAnnotator()
        self.explanations = NodeExplanations()
    
    def process_file(self, cpp_file, options):
        """Process a C++ file and generate AST annotations."""
        if not os.path.exists(cpp_file):
            print(f"Error: File not found: {cpp_file}")
            return False
        
        print(f"Processing: {cpp_file}")
        
        # Generate AST JSON
        ast_file = self.parser.generate_ast_json(cpp_file)
        if not ast_file:
            return False
        
        # If only generating AST, stop here
        if options.generate_ast:
            print(f"AST JSON generated: {ast_file}")
            return True
        
        # Parse AST and create line mappings
        line_mappings = self.parser.parse_ast_file(ast_file, cpp_file)
        if not line_mappings:
            print("Error: Failed to parse AST file")
            return False
        
        # Generate output based on format
        if options.format == 'json':
            self._output_json(line_mappings, options.output)
        elif options.format == 'side-by-side':
            self._output_side_by_side(cpp_file, line_mappings, options)
        elif options.format == 'table':
            self._output_table(cpp_file, line_mappings, options)
        elif options.format == 'csv':
            self._output_csv(cpp_file, line_mappings, options)
        else:  # annotated format (default)
            self._output_annotated(cpp_file, line_mappings, options)
        
        return True
    
    def _output_annotated(self, cpp_file, line_mappings, options):
        """Output annotated source format."""
        output = self.annotator.annotate_source(
            cpp_file, 
            line_mappings, 
            include_explanations=options.explanations,
            explanations_dict=self.explanations.get_all_explanations()
        )
        
        if options.output:
            with open(options.output, 'w', encoding='utf-8') as f:
                f.write(output)
            print(f"Output saved to: {options.output}")
        else:
            print(output)
    
    def _output_side_by_side(self, cpp_file, line_mappings, options):
        """Output side-by-side format."""
        output = self.annotator.side_by_side_view(
            cpp_file, 
            line_mappings,
            explanations_dict=self.explanations.get_all_explanations() if options.explanations else None
        )
        
        if options.output:
            with open(options.output, 'w', encoding='utf-8') as f:
                f.write(output)
            print(f"Output saved to: {options.output}")
        else:
            print(output)
    
    def _output_json(self, line_mappings, output_file):
        """Output JSON format."""
        import json
        
        json_output = {
            "line_mappings": {str(k): v for k, v in line_mappings.items()},
            "explanations": self.explanations.get_all_explanations()
        }
        
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(json_output, f, indent=2)
            print(f"JSON output saved to: {output_file}")
        else:
            print(json.dumps(json_output, indent=2))
    
    def _output_table(self, cpp_file, line_mappings, options):
        """Output table format."""
        output = self.annotator.generate_table_output(
            cpp_file, 
            line_mappings,
            explanations_dict=self.explanations.get_all_explanations() if options.explanations else None
        )
        
        if options.output:
            with open(options.output, 'w', encoding='utf-8') as f:
                f.write(output)
            print(f"Output saved to: {options.output}")
        else:
            print(output)
    
    def _output_csv(self, cpp_file, line_mappings, options):
        """Output CSV format."""
        output = self.annotator.generate_csv_output(
            cpp_file, 
            line_mappings,
            explanations_dict=self.explanations.get_all_explanations() if options.explanations else None
        )
        
        if options.output:
            with open(options.output, 'w', encoding='utf-8') as f:
                f.write(output)
            print(f"Output saved to: {options.output}")
        else:
            print(output)


def main():
    """Main entry point for the CLI tool."""
    parser = argparse.ArgumentParser(
        description="Map C++ source lines to AST nodes using Clang",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python ast_line_mapper.py example.cpp
  python ast_line_mapper.py example.cpp --explanations
  python ast_line_mapper.py example.cpp --side-by-side
  python ast_line_mapper.py example.cpp --format table --explanations
  python ast_line_mapper.py example.cpp --format csv --output result.csv
  python ast_line_mapper.py example.cpp --format json --output result.json
  python ast_line_mapper.py example.cpp --format csv --output result.csv --interpret
  python ast_line_mapper.py example.cpp --format csv --output result.csv --interpret --api-key YOUR_API_KEY
        """
    )
    
    parser.add_argument(
        'cpp_file',
        help='C++ source file to process'
    )
    
    parser.add_argument(
        '--explanations',
        action='store_true',
        help='Include human-readable explanations for AST nodes'
    )
    
    parser.add_argument(
        '--side-by-side',
        action='store_true',
        help='Display source and AST information side by side (deprecated: use --format side-by-side)'
    )
    
    parser.add_argument(
        '--generate-ast',
        action='store_true',
        help='Generate AST JSON file only (don\'t annotate)'
    )
    
    parser.add_argument(
        '--format',
        choices=['annotated', 'side-by-side', 'table', 'csv', 'json'],
        default='annotated',
        help='Output format (default: annotated)'
    )
    
    parser.add_argument(
        '--output',
        help='Save output to specified file'
    )
    
    parser.add_argument(
        '--interpret',
        action='store_true',
        help='Use AI to interpret the AST output (only works with CSV format)'
    )
    
    parser.add_argument(
        '--api-key',
        help='API key for AI interpretation (or set AST_MAPPER_AI_KEY environment variable)'
    )
    
    parser.add_argument(
        '--version',
        action='version',
        version='Clang AST Line Mapper 1.0.0'
    )
    
    args = parser.parse_args()
    
    # Handle deprecated --side-by-side option
    if args.side_by_side:
        args.format = 'side-by-side'
    
    # Create and run the mapper
    mapper = ASTLineMapper()
    
    try:
        success = mapper.process_file(args.cpp_file, args)
        
        # Handle AI interpretation if requested
        if success and args.interpret:
            if args.format != 'csv':
                print("Warning: AI interpretation only works with CSV format. Please use --format csv")
            elif not args.output:
                print("Warning: AI interpretation requires saving output to a file. Please use --output")
            else:
                print("\nGenerating AI interpretation of the AST mapping...")
                try:
                    interpretation = interpret_ast_file(args.output, args.api_key)
                    interpretation_file = f"{os.path.splitext(args.output)[0]}_interpretation.md"
                    with open(interpretation_file, 'w', encoding='utf-8') as f:
                        f.write(interpretation)
                    print(f"AI interpretation saved to: {interpretation_file}")
                except Exception as e:
                    print(f"Error generating AI interpretation: {e}")
        
        sys.exit(0 if success else 1)
    except KeyboardInterrupt:
        print("\nProcess interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
