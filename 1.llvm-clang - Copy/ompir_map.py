#!/usr/bin/env python3
import argparse
import os
import subprocess
import re
import json
from typing import List, Dict, Any

try:
    from google import genai
except ImportError:  # pragma: no cover - dependency may not be installed
    genai = None


def parse_omp_directives(source_path):
    directives = []
    with open(source_path, 'r') as f:
        for lineno, line in enumerate(f, 1):
            line = line.strip()
            if line.startswith('#pragma omp'):
                # capture directive after '#pragma'
                directive = line[len('#pragma '):]
                directives.append({'line': lineno, 'directive': directive})
    return directives


def compile_to_ir(source_path, output_ll):
    clang = '/usr/bin/clang++' if os.path.exists('/usr/bin/clang++') else 'clang++'
    
    # Enhanced compilation with OpenMP runtime integration
    openmp_runtime_path = os.path.join(os.path.dirname(__file__), 'openmp', 'runtime')
    openmp_include_path = os.path.join(openmp_runtime_path, 'src')
    
    cmd = [
        clang,
        '-fopenmp',
        '-g',
        '-O0',
        '-S',
        '-emit-llvm',
        source_path,
        '-o', output_ll,
        '-I/usr/lib/llvm-18/lib/clang/18/include'
    ]
    
    # Add OpenMP runtime includes if available
    if os.path.exists(openmp_include_path):
        cmd.insert(-1, f'-I{openmp_include_path}')
        print(f"Using OpenMP runtime headers from: {openmp_include_path}")
    
    try:
        subprocess.check_call(cmd)
        print(f"Successfully compiled {source_path} to {output_ll}")
    except subprocess.CalledProcessError as e:
        print('Error invoking clang++:', e)
        raise


def map_directives_to_ir(directives, ir_path):
    ir_lines = []
    with open(ir_path, 'r') as f:
        ir_lines = f.readlines()

    # Build map of metadata id -> source line
    meta_re = re.compile(r'^!(\d+) = !DILocation\(line: (\d+),')
    metadata = {}
    for line in ir_lines:
        m = meta_re.match(line.strip())
        if m:
            metadata[m.group(1)] = int(m.group(2))

    line_to_dir = {d['line']: d['directive'] for d in directives}
    call_pat = re.compile(r'\bcall\b.*@(__kmpc|__tgt)[\w.]*\(')

    mapping = []
    for idx, line in enumerate(ir_lines):
        if line.lstrip().startswith('declare'):
            continue
        if call_pat.search(line):
            dbg = None
            m = re.search(r'!dbg !(\d+)', line)
            if m:
                dbg = metadata.get(m.group(1))
            directive = line_to_dir.get(dbg)
            mapping.append({
                'directive': directive,
                'source_line': dbg,
                'ir_line': idx + 1,
                'ir_text': line.strip()
            })

    return mapping


def annotate_ir(ir_path, directives, mapping, annotated_path):
    with open(ir_path, 'r') as f:
        lines = f.readlines()

    annotations = {}
    for m in mapping:
        ir_line = m['ir_line']
        if ir_line not in annotations:
            annotations[ir_line] = []
        if m['directive'] and m['source_line']:
            annotations[ir_line].append(f"[Line {m['source_line']}] #pragma {m['directive']}")
        else:
            annotations[ir_line].append('OpenMP runtime call')

    with open(annotated_path, 'w') as f:
        for idx, line in enumerate(lines, 1):
            if idx in annotations:
                directive_comments = ', '.join(annotations[idx])
                f.write(f"; OpenMP: {directive_comments}\n")
            f.write(line)


def structured_mapping(directives, mapping):
    result = {}
    for d in directives:
        result.setdefault(d['directive'], {'source_line': d['line'], 'ir': []})
    for m in mapping:
        key = m['directive'] if m['directive'] is not None else 'unknown'
        entry = result.setdefault(key, {'source_line': m['source_line'], 'ir': []})
        entry['ir'].append({'line': m['ir_line'], 'text': m['ir_text']})
    return result


def generate_explanations(mapping: List[Dict[str, Any]], api_key: str, model: str = 'gemini-2.0-flash') -> Dict[str, str]:
    """Use Google's Gemini API to explain each directive and its IR instruction."""
    if genai is None:
        print('google-genai package not installed. Skipping explanations.')
        return {}

    client = genai.Client(api_key=api_key)
    explanations = {}
    for m in mapping:
        directive = m['directive'] or 'OpenMP runtime call'
        prompt = (
            f"Explain the purpose of the OpenMP directive '{directive}' and how the "
            f"IR instruction '{m['ir_text']}' implements it."
        )
        try:
            resp = client.models.generate_content(model=model, contents=prompt)
            explanations[f"line {m['ir_line']}"] = resp.text
        except Exception as e:  # pragma: no cover - network issues
            explanations[f"line {m['ir_line']}"] = f'Error: {e}'
    return explanations


def main():
    parser = argparse.ArgumentParser(description='Map OpenMP directives to LLVM IR.')
    parser.add_argument('source', help='C/C++ source file')
    parser.add_argument('--annotated', help='Output annotated IR file')
    parser.add_argument('--json', help='Output mapping JSON file')
    parser.add_argument('--ir', help='Intermediate IR file (generated automatically if not set)')
    parser.add_argument(
        '--explain', nargs='?', const=True, metavar='FILE',
        help='Use Gemini to explain each directive. Optionally write to FILE')
    parser.add_argument('--api-key', help='Gemini API key (defaults to GOOGLE_API_KEY env var, then hardcoded default)')
    parser.add_argument('--model', default='gemini-2.0-flash', help='Gemini model to use')
    args = parser.parse_args()

    source_path = args.source
    ir_path = args.ir or os.path.splitext(source_path)[0] + '.ll'

    directives = parse_omp_directives(source_path)
    compile_to_ir(source_path, ir_path)
    mapping = map_directives_to_ir(directives, ir_path)

    if args.annotated:
        annotate_ir(ir_path, directives, mapping, args.annotated)
    if args.json:
        with open(args.json, 'w') as f:
            json.dump(structured_mapping(directives, mapping), f, indent=2)

    if args.explain:
        # Default hardcoded API key
        default_api_key = 'AIzaSyAlxhw27XZXOHVrufhs_KrwR40Td8dyoCw'
        api_key = args.api_key or os.getenv('GOOGLE_API_KEY') or default_api_key
        if not api_key:
            print('No API key provided for Gemini explanations.')
        else:
            explanations = generate_explanations(mapping, api_key, model=args.model)
            if isinstance(args.explain, str):
                with open(args.explain, 'w') as f:
                    for line, text in explanations.items():
                        f.write(f"{line}: {text}\n")
            else:
                print('Explanations:')
                for line, text in explanations.items():
                    print(f"{line}: {text}")

    print('Directives:', directives)
    print('Mapping:', mapping)


if __name__ == '__main__':
    main()
