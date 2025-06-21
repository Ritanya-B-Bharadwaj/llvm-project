import os
import click
from ir_diff import process_cpp_and_diff
from genai import generate_summary
from html_report import generate_enhanced_html_report

def run_ir_diff(source_file, passes):
    """
    Main function to run IR diff analysis with enhanced features
    
    Args:
        source_file: Path to C++ source file
        passes: LLVM optimization passes to apply
        
    Returns:
        Path to generated HTML report
    """
    try:
        # Process with enhanced backend
        html_report_path = process_cpp_and_diff(source_file, passes)
        return html_report_path
    except Exception as e:
        print(f"❌ Error generating report: {e}")
        raise

@click.command()
@click.option("--source", required=True, help="Path to C++ source file.")
@click.option("--passes", default="mem2reg", help="LLVM passes to apply.")
def main(source, passes):
    """LLVM IR Diff Analyzer - Professional optimization analysis tool"""
    print("🚀 LLVM IR Diff Analyzer")
    print("=" * 50)
    
    try:
        report_path = run_ir_diff(source, passes)
        print(f"\n✅ Analysis complete!")
        print(f"📋 Professional report: {report_path}")
        
    except Exception as e:
        print(f"\n❌ Analysis failed: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    main()
