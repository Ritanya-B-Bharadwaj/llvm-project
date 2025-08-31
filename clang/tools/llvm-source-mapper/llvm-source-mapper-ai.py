#!/usr/bin/env python3
"""
LLVM Source Mapper - AI Summary Generator

This script adds AI-generated summaries to the LLVM IR instructions in the output
from llvm-source-mapper using the Groq Cloud API.

This is an optional post-processing step that enhances the basic source-to-IR 
mapping with intelligent explanations of what each IR instruction does.
"""

import os
import sys
import re
import json
import argparse
from typing import List, Dict, Any, Optional

# Check for required packages
try:
    import requests
except ImportError:
    print("Error: 'requests' package is required. Install with: pip install requests")
    sys.exit(1)

try:
    from dotenv import load_dotenv
    load_dotenv()
except ImportError:
    print("Warning: python-dotenv is not installed. .env file will not be loaded.")

# Configure Groq API credentials
GROQ_API_KEY = os.environ.get('GROQ_API_KEY', '')
GROQ_API_ENDPOINT = "https://api.groq.com/openai/v1/chat/completions"
MODEL = os.environ.get('GROQ_MODEL', 'llama-3.3-70b-versatile')

def setup_args() -> argparse.Namespace:
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description='Add AI-generated summaries to LLVM IR mappings')
    parser.add_argument('input_file', help='Input markdown file with LLVM IR mappings')
    parser.add_argument('output_file', help='Output file with summaries')
    parser.add_argument('--api-key', help='Groq API Key (can also be set via GROQ_API_KEY env var)')
    parser.add_argument('--model', help='Groq model to use (can also be set via GROQ_MODEL env var)')
    parser.add_argument('--html', action='store_true', help='Output HTML format instead of Markdown')
    return parser.parse_args()

def create_llvm_summary_prompt(source_code: str, llvm_ir: str) -> str:
    """Create a prompt for generating a concise LLVM IR summary."""
    return f"""Analyze this C++ code and its corresponding LLVM IR:

C++ Source:
```cpp
{source_code}
```

LLVM IR:
```llvm
{llvm_ir}
```

Provide a concise 1-2 sentence technical summary focusing on:
1. What LLVM IR instructions are generated
2. Key optimizations or patterns
3. Memory operations (loads, stores, allocations)

Be specific about LLVM concepts like SSA form, basic blocks, or instruction types. Keep it under 100 words and avoid starting with phrases like "This code" or "The LLVM IR"."""

def query_groq_api(prompt: str, api_key: str, model: str) -> Optional[str]:
    """Query the Groq API for a summary."""
    headers = {
        'Authorization': f'Bearer {api_key}',
        'Content-Type': 'application/json'
    }

    data = {
        'model': model,
        'messages': [
            {
                'role': 'user',
                'content': prompt
            }
        ],
        'max_tokens': 150,
        'temperature': 0.3
    }

    try:
        response = requests.post(GROQ_API_ENDPOINT, headers=headers, json=data, timeout=30)
        response.raise_for_status()
        result = response.json()
        return result['choices'][0]['message']['content'].strip()
    except Exception as e:
        print(f"Error querying Groq API: {e}")
        return None

def extract_llvm_sections(file_path: str) -> Dict[int, Dict[str, Any]]:
    """Extract source line, source code and LLVM IR instructions from the markdown file."""
    with open(file_path, 'r') as f:
        content = f.read()

    # Pattern to extract table rows with source line, code, and LLVM IR instructions
    pattern = r'\|\s*(\d+)\s*\|\s*`([^`]*)`\s*\|\s*<pre>(.*?)</pre>\s*\|\s*(.*?)\s*\|'
    matches = re.findall(pattern, content, re.DOTALL)

    sections = {}
    for match in matches:
        line_num = int(match[0])
        source_code = match[1].strip()
        llvm_ir = match[2].strip()
        sections[line_num] = {
            'source_code': source_code,
            'llvm_ir': llvm_ir,
            'summary': ''
        }
    
    return sections

def add_summaries_to_sections(sections: Dict[int, Dict[str, Any]], api_key: str, model: str) -> None:
    """Add AI-generated summaries to each section."""
    if not api_key:
        print("Warning: No API key provided. Summaries will be empty.")
        return

    total = len(sections)
    for i, (line_num, section) in enumerate(sections.items(), 1):
        print(f"Generating summary for line {line_num} ({i}/{total})...")
        
        prompt = create_llvm_summary_prompt(section['source_code'], section['llvm_ir'])
        summary = query_groq_api(prompt, api_key, model)
        
        if summary:
            # Clean up common repetitive phrases
            summary = re.sub(r'^(This code|The LLVM IR|The code|This LLVM IR)\s*', '', summary, flags=re.IGNORECASE)
            section['summary'] = summary.strip()
        else:
            section['summary'] = "Summary generation failed"

def write_markdown_output(sections: Dict[int, Dict[str, Any]], output_file: str, original_content: str) -> None:
    """Write the sections back to markdown format with summaries."""
    for line_num, section in sections.items():
        if section['summary']:
            # Pattern to replace the empty summary in the markdown table
            pattern = r'(\|\s*{}\s*\|\s*`[^`]*`\s*\|\s*<pre>.*?</pre>\s*\|\s*)(.*?)(\s*\|)'.format(line_num)
            replacement = r'\1{}\3'.format(section['summary'])
            original_content = re.sub(pattern, replacement, original_content, flags=re.DOTALL)
    
    with open(output_file, 'w') as f:
        f.write(original_content)

def write_html_output(sections: Dict[int, Dict[str, Any]], output_file: str) -> None:
    """Write the mapping and summaries as a styled HTML table matching the LLVM style."""
    html_content = '''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LLVM Source to IR Mapping with AI Summaries</title>
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Noto Sans', Helvetica, Arial, sans-serif;
            line-height: 1.5;
            color: #1f2328;
            background-color: #ffffff;
            margin: 0;
            padding: 24px;
        }

        .container {
            max-width: 1200px;
            margin: 0 auto;
        }

        h1 {
            font-size: 32px;
            font-weight: 600;
            margin-bottom: 24px;
            border-bottom: 1px solid #d1d9e0;
            padding-bottom: 10px;
        }

        table {
            width: 100%;
            border-collapse: collapse;
            border-spacing: 0;
            margin-top: 16px;
            font-size: 14px;
        }

        th, td {
            padding: 6px 13px;
            border: 1px solid #d1d9e0;
            text-align: left;
            vertical-align: top;
        }

        th {
            font-weight: 600;
            background-color: #f6f8fa;
        }

        tr:nth-child(2n) {
            background-color: #f6f8fa;
        }

        code {
            padding: 0.2em 0.4em;
            margin: 0;
            font-size: 85%;
            background-color: rgba(175,184,193,0.2);
            border-radius: 6px;
            font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, "Liberation Mono", monospace;
        }

        pre {
            padding: 16px;
            overflow: auto;
            font-size: 85%;
            line-height: 1.45;
            background-color: #f6f8fa;
            border-radius: 6px;
            margin: 0;
            font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, "Liberation Mono", monospace;
            white-space: pre;
        }

        .line-number {
            width: 80px;
            text-align: center;
            font-weight: 600;
        }

        .source-code {
            width: 25%;
            min-width: 200px;
        }

        .llvm-ir {
            width: 40%;
            min-width: 300px;
        }

        .summary {
            width: 25%;
            min-width: 200px;
            color: #656d76;
            font-style: italic;
        }

        @media (max-width: 768px) {
            body {
                padding: 16px;
            }
            
            table {
                font-size: 12px;
            }
            
            th, td {
                padding: 4px 8px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>LLVM Source to IR Mapping with AI Summaries</h1>
        <table>
            <thead>
                <tr>
                    <th class="line-number">Line</th>
                    <th class="source-code">Source Code</th>
                    <th class="llvm-ir">LLVM IR</th>
                    <th class="summary">AI Summary</th>
                </tr>
            </thead>
            <tbody>
'''

    for line_num in sorted(sections.keys()):
        section = sections[line_num]
        
        # Escape HTML characters
        source_code = section["source_code"].replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
        llvm_ir = section["llvm_ir"].replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
        summary = section["summary"].replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")
        
        html_content += f'''                <tr>
                    <td class="line-number">{line_num}</td>
                    <td class="source-code"><code>{source_code}</code></td>
                    <td class="llvm-ir"><pre>{llvm_ir}</pre></td>
                    <td class="summary">{summary}</td>
                </tr>
'''

    html_content += '''            </tbody>
        </table>
    </div>
</body>
</html>
'''

    with open(output_file, 'w') as f:
        f.write(html_content)

def main() -> None:
    """Main function to add summaries to LLVM IR instructions"""
    args = setup_args()
    
    # Set API key and model from command line if provided
    api_key = args.api_key or GROQ_API_KEY
    model = args.model or MODEL
    
    print(f"Using AI model: {model}")
    
    if not api_key:
        print("Warning: No API key provided. Summaries will be empty.")
        print("Set GROQ_API_KEY environment variable or use --api-key option.")
    
    print(f"Processing {args.input_file}...")
    
    # Read original content for markdown output
    with open(args.input_file, 'r') as f:
        original_content = f.read()
    
    # Extract LLVM IR sections
    sections = extract_llvm_sections(args.input_file)
    print(f"Found {len(sections)} source lines with LLVM IR instructions.")
    
    # Generate summaries
    add_summaries_to_sections(sections, api_key, model)
    
    # Choose output format
    if args.html or args.output_file.endswith('.html'):
        write_html_output(sections, args.output_file)
        print(f"HTML output written to {args.output_file}")
    else:
        write_markdown_output(sections, args.output_file, original_content)
        print(f"Markdown output with summaries written to {args.output_file}")

if __name__ == "__main__":
    main()
