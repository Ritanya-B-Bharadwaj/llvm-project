#!/usr/bin/env python3
"""
Enhanced demo script that leverages the OpenMP test suite to demonstrate
the IR mapping tool across various OpenMP constructs.
"""

import os
import subprocess
import glob
import json
from pathlib import Path

def find_openmp_tests():
    """Find interesting OpenMP test cases from the runtime test suite."""
    test_dir = "openmp/runtime/test"
    
    # Categories of tests we want to showcase
    categories = [
        "parallel",
        "worksharing",
        "tasking", 
        "critical",
        "barrier",
        "atomic"
    ]
    
    test_files = []
    for category in categories:
        pattern = f"{test_dir}/{category}/*.c"
        files = glob.glob(pattern)
        if files:
            # Take first few from each category
            test_files.extend(files[:2])
    
    return test_files

def run_ir_mapping_demo():
    """Run the IR mapping tool on various OpenMP test cases."""
    
    # Ensure output directory exists
    os.makedirs("output/test_suite", exist_ok=True)
    
    test_files = find_openmp_tests()
    results = {}
    
    print("🚀 Running OpenMP IR Mapping Demo on Test Suite")
    print("=" * 60)
    
    for i, test_file in enumerate(test_files[:5], 1):  # Limit to 5 for demo
        print(f"\n📁 Processing {i}/5: {test_file}")
        
        # Create output filenames
        base_name = Path(test_file).stem
        category = Path(test_file).parent.name
        output_prefix = f"output/test_suite/{category}_{base_name}"
        
        try:
            # Run the IR mapping tool
            cmd = [
                "python3", "ompir_map.py", test_file,
                "--annotated", f"{output_prefix}_annotated.ll",
                "--json", f"{output_prefix}_mapping.json",
                "--explain", f"{output_prefix}_explanations.md"
            ]
            
            subprocess.run(cmd, check=True, capture_output=True)
            
            # Load and summarize results
            with open(f"{output_prefix}_mapping.json", 'r') as f:
                mapping = json.load(f)
            
            results[test_file] = {
                "category": category,
                "directives_found": len(mapping.get("directives", [])),
                "ir_mappings": len(mapping.get("mappings", [])),
                "output_files": {
                    "annotated_ir": f"{output_prefix}_annotated.ll",
                    "mapping_json": f"{output_prefix}_mapping.json", 
                    "explanations": f"{output_prefix}_explanations.md"
                }
            }
            
            print(f"   ✅ Found {results[test_file]['directives_found']} OpenMP directives")
            print(f"   ✅ Generated {results[test_file]['ir_mappings']} IR mappings")
            
        except subprocess.CalledProcessError as e:
            print(f"   ❌ Failed to process {test_file}: {e}")
            results[test_file] = {"error": str(e)}
    
    # Generate summary report
    generate_summary_report(results)
    print(f"\n🎉 Demo complete! Check output/test_suite/ for results")

def generate_summary_report(results):
    """Generate an HTML summary report of all test results."""
    
    html_content = """
<!DOCTYPE html>
<html>
<head>
    <title>OpenMP IR Mapping Demo Results</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .test-result { border: 1px solid #ddd; margin: 10px 0; padding: 15px; border-radius: 5px; }
        .success { border-left: 5px solid #4CAF50; }
        .error { border-left: 5px solid #f44336; }
        .category { background-color: #e3f2fd; padding: 5px 10px; border-radius: 3px; display: inline-block; }
        .stats { background-color: #f5f5f5; padding: 10px; border-radius: 5px; margin: 10px 0; }
    </style>
</head>
<body>
    <h1>🔍 OpenMP IR Mapping Demo Results</h1>
    <div class="stats">
        <h3>📊 Summary Statistics</h3>
        <ul>
"""
    
    total_tests = len(results)
    successful_tests = len([r for r in results.values() if "error" not in r])
    total_directives = sum(r.get("directives_found", 0) for r in results.values())
    
    html_content += f"""
            <li><strong>Total Tests Processed:</strong> {total_tests}</li>
            <li><strong>Successful Analyses:</strong> {successful_tests}</li>
            <li><strong>Total OpenMP Directives Found:</strong> {total_directives}</li>
        </ul>
    </div>
    
    <h3>📋 Detailed Results</h3>
"""
    
    for test_file, result in results.items():
        test_name = Path(test_file).name
        if "error" in result:
            html_content += f"""
    <div class="test-result error">
        <h4>{test_name}</h4>
        <p><strong>❌ Error:</strong> {result['error']}</p>
    </div>
"""
        else:
            html_content += f"""
    <div class="test-result success">
        <h4>{test_name}</h4>
        <span class="category">{result['category']}</span>
        <p><strong>OpenMP Directives:</strong> {result['directives_found']}</p>
        <p><strong>IR Mappings:</strong> {result['ir_mappings']}</p>
        <p><strong>Generated Files:</strong></p>
        <ul>
            <li><a href="{result['output_files']['annotated_ir']}">Annotated IR</a></li>
            <li><a href="{result['output_files']['mapping_json']}">Mapping JSON</a></li>
            <li><a href="{result['output_files']['explanations']}">AI Explanations</a></li>
        </ul>
    </div>
"""
    
    html_content += """
</body>
</html>
"""
    
    with open("output/test_suite/summary_report.html", "w") as f:
        f.write(html_content)

if __name__ == "__main__":
    run_ir_mapping_demo()
