#!/usr/bin/env python3
"""
OpenMP Test Discovery and Analysis Script

This script analyzes the OpenMP test suite to find interesting test cases
for demonstrating different OpenMP constructs and their IR mappings.
"""

import os
import re
import json
from pathlib import Path
from collections import defaultdict

def analyze_openmp_directive(line):
    """Extract and categorize OpenMP directive information."""
    # Remove comments and extra whitespace
    clean_line = re.sub(r'//.*', '', line).strip()
    
    if not clean_line.startswith('#pragma omp'):
        return None
    
    directive = clean_line[len('#pragma omp'):].strip()
    
    # Categorize directive types
    categories = {
        'parallel': ['parallel'],
        'worksharing': ['for', 'sections', 'section', 'single'],
        'tasking': ['task', 'taskwait', 'taskgroup', 'taskyield'],
        'synchronization': ['critical', 'barrier', 'atomic', 'flush', 'master'],
        'data_environment': ['private', 'shared', 'firstprivate', 'lastprivate', 'reduction'],
        'device': ['target', 'teams', 'distribute'],
        'misc': ['threadprivate', 'declare']
    }
    
    directive_type = 'unknown'
    for category, keywords in categories.items():
        if any(keyword in directive.lower() for keyword in keywords):
            directive_type = category
            break
    
    return {
        'directive': directive,
        'type': directive_type,
        'raw_line': line.strip()
    }

def analyze_test_file(filepath):
    """Analyze a single test file for OpenMP constructs."""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
    except Exception as e:
        return None
    
    file_info = {
        'path': str(filepath),
        'name': filepath.name,
        'category': filepath.parent.name,
        'directives': [],
        'line_count': len(lines),
        'complexity_score': 0
    }
    
    for line_no, line in enumerate(lines, 1):
        directive_info = analyze_openmp_directive(line)
        if directive_info:
            directive_info['line_number'] = line_no
            file_info['directives'].append(directive_info)
    
    # Calculate complexity score based on number and variety of directives
    directive_types = set(d['type'] for d in file_info['directives'])
    file_info['complexity_score'] = len(file_info['directives']) + len(directive_types) * 2
    
    return file_info

def discover_openmp_tests():
    """Discover and analyze all OpenMP test files."""
    test_root = Path("openmp/runtime/test")
    
    if not test_root.exists():
        print(f"❌ OpenMP test directory not found: {test_root}")
        return None
    
    print(f"🔍 Discovering OpenMP tests in: {test_root}")
    
    # Find all C/C++ test files
    test_files = []
    for pattern in ['**/*.c', '**/*.cpp', '**/*.cc']:
        test_files.extend(test_root.glob(pattern))
    
    print(f"📁 Found {len(test_files)} test files")
    
    # Analyze each test file
    analyzed_tests = []
    directive_stats = defaultdict(int)
    category_stats = defaultdict(int)
    
    for test_file in test_files:
        analysis = analyze_test_file(test_file)
        if analysis and analysis['directives']:  # Only include files with OpenMP directives
            analyzed_tests.append(analysis)
            
            # Update statistics
            category_stats[analysis['category']] += 1
            for directive in analysis['directives']:
                directive_stats[directive['type']] += 1
    
    # Sort by complexity score (most interesting first)
    analyzed_tests.sort(key=lambda x: x['complexity_score'], reverse=True)
    
    results = {
        'summary': {
            'total_files_scanned': len(test_files),
            'files_with_openmp': len(analyzed_tests),
            'categories_found': dict(category_stats),
            'directive_types_found': dict(directive_stats)
        },
        'tests': analyzed_tests
    }
    
    return results

def generate_test_recommendations(analysis_results):
    """Generate recommendations for the most interesting test cases."""
    if not analysis_results:
        return []
    
    tests = analysis_results['tests']
    
    # Recommend diverse and complex test cases
    recommendations = []
    
    # Top 5 most complex tests
    complex_tests = tests[:5]
    
    # One representative from each category
    categories_seen = set()
    category_representatives = []
    for test in tests:
        if test['category'] not in categories_seen:
            category_representatives.append(test)
            categories_seen.add(test['category'])
            if len(category_representatives) >= 8:  # Limit recommendations
                break
    
    # Combine and deduplicate
    all_recommended = complex_tests + category_representatives
    seen_paths = set()
    for test in all_recommended:
        if test['path'] not in seen_paths:
            recommendations.append(test)
            seen_paths.add(test['path'])
    
    return recommendations[:10]  # Top 10 recommendations

def main():
    print("🚀 OpenMP Test Discovery and Analysis")
    print("=" * 50)
    
    # Discover and analyze tests
    results = discover_openmp_tests()
    
    if not results:
        print("❌ No OpenMP tests found or analyzed")
        return
    
    # Print summary statistics
    summary = results['summary']
    print(f"\n📊 Discovery Summary:")
    print(f"   • Files scanned: {summary['total_files_scanned']}")
    print(f"   • Files with OpenMP: {summary['files_with_openmp']}")
    print(f"   • Categories: {', '.join(summary['categories_found'].keys())}")
    print(f"   • Directive types: {', '.join(summary['directive_types_found'].keys())}")
    
    # Generate recommendations
    recommendations = generate_test_recommendations(results)
    
    print(f"\n🎯 Top {len(recommendations)} Recommended Test Cases:")
    print("-" * 50)
    
    for i, test in enumerate(recommendations, 1):
        print(f"{i:2d}. {test['name']} ({test['category']})")
        print(f"    📍 {len(test['directives'])} directives, complexity: {test['complexity_score']}")
        directive_types = set(d['type'] for d in test['directives'])
        print(f"    🏷️  Types: {', '.join(sorted(directive_types))}")
        print()
    
    # Save detailed results
    os.makedirs("output", exist_ok=True)
    with open("output/test_discovery.json", "w") as f:
        json.dump(results, f, indent=2)
    
    # Save recommendations as a simple list for easy use
    recommended_paths = [test['path'] for test in recommendations]
    with open("output/recommended_tests.txt", "w") as f:
        for path in recommended_paths:
            f.write(f"{path}\n")
    
    print(f"💾 Detailed results saved to:")
    print(f"   • output/test_discovery.json")
    print(f"   • output/recommended_tests.txt")

if __name__ == "__main__":
    main()
