#!/usr/bin/env python3
"""
OpenMP Construct Reference Generator

This script generates comprehensive documentation about OpenMP constructs
found in the test suite, creating a knowledge base for the IR mapping tool.
"""

import os
import json
from pathlib import Path
from collections import defaultdict

def extract_openmp_knowledge():
    """Extract OpenMP construct knowledge from test files and documentation."""
    
    # Read test discovery results
    discovery_file = "output/test_discovery.json"
    if not os.path.exists(discovery_file):
        print("❌ Run discover_tests.py first to generate test discovery data")
        return None
    
    with open(discovery_file, 'r') as f:
        discovery_data = json.load(f)
    
    # Build knowledge base
    knowledge_base = {
        'constructs': defaultdict(lambda: {
            'description': '',
            'usage_examples': [],
            'test_files': [],
            'common_patterns': [],
            'ir_characteristics': []
        }),
        'categories': defaultdict(list)
    }
    
    # Process each test file
    for test in discovery_data['tests']:
        for directive in test['directives']:
            construct_name = directive['directive'].split()[0]  # First word is usually the construct
            construct_info = knowledge_base['constructs'][construct_name]
            
            # Add test file reference
            construct_info['test_files'].append({
                'file': test['path'],
                'line': directive['line_number'],
                'full_directive': directive['directive']
            })
            
            # Categorize
            knowledge_base['categories'][directive['type']].append(construct_name)
    
    # Add descriptions for common constructs
    construct_descriptions = {
        'parallel': 'Creates a team of threads to execute a parallel region',
        'for': 'Distributes loop iterations among threads in a parallel region',
        'critical': 'Ensures that only one thread executes the enclosed code at a time',
        'barrier': 'Synchronizes all threads in a team',
        'atomic': 'Ensures atomic access to a memory location',
        'task': 'Creates an explicit task that can be executed by any thread',
        'single': 'Specifies that only one thread executes the enclosed code',
        'master': 'Specifies that only the master thread executes the enclosed code',
        'sections': 'Divides work among threads using independent sections',
        'reduction': 'Performs a reduction operation on variables',
        'private': 'Creates private copies of variables for each thread',
        'shared': 'Shares variables among all threads',
        'firstprivate': 'Initializes private variables with the value from the master thread',
        'target': 'Offloads computation to a target device',
        'teams': 'Creates a league of thread teams for target regions'
    }
    
    # Add descriptions
    for construct, desc in construct_descriptions.items():
        if construct in knowledge_base['constructs']:
            knowledge_base['constructs'][construct]['description'] = desc
    
    return dict(knowledge_base)

def generate_reference_documentation(knowledge_base):
    """Generate comprehensive reference documentation."""
    
    if not knowledge_base:
        return
    
    # Generate HTML documentation
    html_content = """
<!DOCTYPE html>
<html>
<head>
    <title>OpenMP Constructs Reference</title>
    <style>
        body { font-family: 'Segoe UI', Arial, sans-serif; margin: 40px; line-height: 1.6; }
        h1 { color: #2c3e50; border-bottom: 3px solid #3498db; padding-bottom: 10px; }
        h2 { color: #34495e; border-left: 4px solid #3498db; padding-left: 15px; }
        h3 { color: #7f8c8d; }
        .construct { border: 1px solid #ecf0f1; margin: 20px 0; padding: 20px; border-radius: 8px; background: #fdfdfd; }
        .category { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 8px 15px; border-radius: 20px; display: inline-block; margin: 5px; }
        .example { background: #f8f9fa; padding: 15px; border-left: 4px solid #28a745; margin: 10px 0; font-family: 'Courier New', monospace; }
        .test-ref { background: #e8f4f8; padding: 10px; margin: 5px 0; border-radius: 5px; }
        .stats { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 20px; border-radius: 10px; margin: 20px 0; }
        pre { background: #2c3e50; color: #ecf0f1; padding: 15px; border-radius: 5px; overflow-x: auto; }
        .toc { background: #f8f9fa; padding: 20px; border-radius: 8px; margin: 20px 0; }
        .toc ul { list-style-type: none; padding-left: 20px; }
        .toc a { text-decoration: none; color: #3498db; }
        .toc a:hover { color: #2980b9; }
    </style>
</head>
<body>
    <h1>🔧 OpenMP Constructs Reference Guide</h1>
    <p><em>Comprehensive reference for OpenMP constructs found in the LLVM test suite</em></p>
"""
    
    # Table of Contents
    html_content += """
    <div class="toc">
        <h2>📋 Table of Contents</h2>
        <ul>
"""
    
    # Statistics
    total_constructs = len(knowledge_base['constructs'])
    total_examples = sum(len(info['test_files']) for info in knowledge_base['constructs'].values())
    
    html_content += f"""
        </ul>
    </div>
    
    <div class="stats">
        <h2>📊 Knowledge Base Statistics</h2>
        <ul>
            <li><strong>Total Constructs Documented:</strong> {total_constructs}</li>
            <li><strong>Total Usage Examples:</strong> {total_examples}</li>
            <li><strong>Categories Covered:</strong> {len(knowledge_base['categories'])}</li>
        </ul>
    </div>
"""
    
    # Generate content for each category
    for category, constructs in knowledge_base['categories'].items():
        unique_constructs = list(set(constructs))
        html_content += f"""
    <h2 id="{category}">
        <span class="category">{category.replace('_', ' ').title()}</span>
    </h2>
"""
        
        for construct in sorted(unique_constructs):
            if construct in knowledge_base['constructs']:
                info = knowledge_base['constructs'][construct]
                
                html_content += f"""
    <div class="construct">
        <h3 id="{construct}">#{construct}</h3>
        <p><strong>Description:</strong> {info['description'] or 'OpenMP construct for parallel programming'}</p>
"""
                
                if info['test_files']:
                    html_content += f"""
        <h4>📝 Usage Examples ({len(info['test_files'])} found)</h4>
"""
                    # Show first few examples
                    for example in info['test_files'][:3]:
                        file_name = Path(example['file']).name
                        html_content += f"""
        <div class="test-ref">
            <strong>{file_name}</strong> (line {example['line']})<br>
            <code>#{example['full_directive']}</code>
        </div>
"""
                
                html_content += """
    </div>
"""
    
    html_content += """
    <div style="margin-top: 50px; padding: 20px; background: #ecf0f1; border-radius: 8px;">
        <h3>🛠️ How to Use This Reference</h3>
        <ol>
            <li><strong>Explore Constructs:</strong> Browse through different categories to understand OpenMP constructs</li>
            <li><strong>Study Examples:</strong> Look at real test cases to see how constructs are used</li>
            <li><strong>Analyze IR Mappings:</strong> Use the ompir_map.py tool on the referenced test files</li>
            <li><strong>Compare Patterns:</strong> Study multiple examples of the same construct to understand variations</li>
        </ol>
    </div>
</body>
</html>
"""
    
    # Save documentation
    os.makedirs("output", exist_ok=True)
    with open("output/openmp_reference.html", "w") as f:
        f.write(html_content)
    
    # Also save as JSON for programmatic use
    with open("output/openmp_knowledge_base.json", "w") as f:
        json.dump(knowledge_base, f, indent=2)

def main():
    print("📚 Generating OpenMP Constructs Reference...")
    
    knowledge_base = extract_openmp_knowledge()
    if knowledge_base:
        generate_reference_documentation(knowledge_base)
        print("✅ Reference documentation generated:")
        print("   • output/openmp_reference.html")
        print("   • output/openmp_knowledge_base.json")
    else:
        print("❌ Failed to generate reference documentation")

if __name__ == "__main__":
    main()
