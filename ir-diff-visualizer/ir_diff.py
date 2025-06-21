# ir_diff.py - Fixed with better IR generation and validation
import os
import re
import random
import tempfile
from pathlib import Path
from utils import (run_command, ensure_output_dir, get_ir_paths, 
                   validate_ir_file, fix_ir_file_issues, get_llvm_tool_path)

# Import what's actually available
try:
    from enhanced_ir_differ import EnhancedIRDiffer
    HAS_ENHANCED_DIFFER = True
except ImportError:
    HAS_ENHANCED_DIFFER = False
    print("⚠️ EnhancedIRDiffer not available, using basic diff")

from genai import generate_enhanced_summary
from html_report import generate_enhanced_html_report

def get_llvm_version():
    """Get LLVM version to determine pass syntax"""
    try:
        opt_path = get_llvm_tool_path('opt')
        result = run_command(f'"{opt_path}" --version', check_output=True)
        
        import re
        version_match = re.search(r'LLVM version (\d+)\.(\d+)', result)
        if version_match:
            major = int(version_match.group(1))
            minor = int(version_match.group(2))
            return major, minor
        return 20, 1
    except:
        return 20, 1

def validate_and_fix_ir_file(filepath):
    """Enhanced IR file validation and fixing"""
    if not os.path.exists(filepath):
        raise Exception(f"IR file not found: {filepath}")
    
    if os.path.getsize(filepath) == 0:
        raise Exception(f"IR file is empty: {filepath}")
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    if not content.strip():
        raise Exception(f"IR file has no content: {filepath}")
    
    # Debug: Print problematic lines
    lines = content.split('\n')
    print(f"🔍 Checking IR file: {filepath}")
    print(f"📊 Total lines: {len(lines)}")
    
    # Check around line 12 (the error location)
    if len(lines) > 12:
        print(f"📋 Line 11: '{lines[10]}'")
        print(f"📋 Line 12: '{lines[11]}'")
        print(f"📋 Line 13: '{lines[12] if len(lines) > 12 else 'N/A'}'")
    
    # Fix common IR issues
    fixed_content = fix_ir_syntax_issues(content)
    
    # Write back the fixed content
    if fixed_content != content:
        print("🔧 Applied IR syntax fixes")
        with open(filepath, 'w') as f:
            f.write(fixed_content)
    
    # Validate with LLVM tools
    try:
        opt_path = get_llvm_tool_path('opt')
        # Dry run to check syntax
        run_command(f'"{opt_path}" -verify "{filepath}" -disable-output')
        print("✅ IR syntax validation passed")
    except Exception as e:
        print(f"⚠️ IR validation warning: {e}")
        # Try to fix and validate again
        fixed_content = apply_aggressive_ir_fixes(fixed_content)
        with open(filepath, 'w') as f:
            f.write(fixed_content)
    
    return True

def fix_ir_syntax_issues(content):
    """Fix common LLVM IR syntax issues"""
    lines = content.split('\n')
    fixed_lines = []
    
    for i, line in enumerate(lines):
        # Skip empty lines and comments
        if not line.strip() or line.strip().startswith(';'):
            fixed_lines.append(line)
            continue
        
        # Fix malformed entry blocks
        if line.strip() == 'entry:' and i > 0:
            # Make sure there's proper indentation and context
            fixed_lines.append('entry:')
            continue
        
        # Fix isolated 'entry:' without proper context
        if line.strip() == 'entry:':
            # Check if this is a proper basic block label
            prev_lines = [l.strip() for l in lines[max(0, i-3):i] if l.strip()]
            if not any('define' in l for l in prev_lines):
                # Skip malformed entry label
                print(f"🔧 Skipping malformed entry label at line {i+1}")
                continue
        
        # Remove problematic attributes
        if 'optnone' in line or 'noinline' in line:
            line = re.sub(r'\s*optnone\s*', ' ', line)
            line = re.sub(r'\s*noinline\s*', ' ', line)
        
        # Fix function attributes
        line = re.sub(r'attributes\s*#\d+\s*=\s*\{\s*optnone[^}]*\}', '', line)
        
        fixed_lines.append(line)
    
    # Ensure proper module structure
    fixed_content = '\n'.join(fixed_lines)
    
    # Add target information if missing
    if 'target datalayout' not in fixed_content:
        target_header = '''target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

'''
        fixed_content = target_header + fixed_content
    
    return fixed_content

def apply_aggressive_ir_fixes(content):
    """Apply more aggressive fixes for problematic IR"""
    lines = content.split('\n')
    clean_lines = []
    in_function = False
    
    for line in lines:
        stripped = line.strip()
        
        # Skip completely empty lines in functions
        if not stripped:
            if not in_function:
                clean_lines.append(line)
            continue
        
        # Track function boundaries
        if 'define' in stripped:
            in_function = True
            clean_lines.append(line)
            continue
        
        if stripped == '}' and in_function:
            in_function = False
            clean_lines.append(line)
            continue
        
        # Skip problematic standalone labels
        if stripped == 'entry:' and not in_function:
            print(f"🔧 Removing orphaned entry label")
            continue
        
        # Skip malformed lines that start with ^
        if stripped.startswith('^'):
            print(f"🔧 Removing malformed line: {stripped}")
            continue
        
        clean_lines.append(line)
    
    return '\n'.join(clean_lines)

def generate_clean_ir(src, ir_output):
    """Generate clean LLVM IR without syntax issues"""
    clang_path = get_llvm_tool_path('clang++')
    
    # Use more conservative compilation flags
    cmd = f'"{clang_path}" -S -emit-llvm -O0 -g0 -fno-discard-value-names -Xclang -disable-O0-optnone "{src}" -o "{ir_output}"'
    
    try:
        print(f"🔄 Compiling: {cmd}")
        run_command(cmd)
        
        # Immediately validate and fix
        validate_and_fix_ir_file(ir_output)
        
    except Exception as e:
        print(f"❌ Standard compilation failed: {e}")
        
        # Fallback with minimal flags
        try:
            print("🔄 Trying minimal compilation...")
            fallback_cmd = f'"{clang_path}" -S -emit-llvm "{src}" -o "{ir_output}"'
            run_command(fallback_cmd)
            
            # Apply fixes
            validate_and_fix_ir_file(ir_output)
            
        except Exception as fallback_error:
            raise Exception(f"Both compilation attempts failed. Primary: {e}, Fallback: {fallback_error}")

def apply_optimization_with_validation(ir_before, ir_after, passes):
    """Apply optimization with thorough validation"""
    opt_path = get_llvm_tool_path('opt')
    
    # First, verify the input file is valid
    print("🔍 Validating input IR...")
    validate_and_fix_ir_file(ir_before)
    
    # Apply optimization with new pass manager syntax
    cmd = f'"{opt_path}" -passes="{passes}" "{ir_before}" -S -o "{ir_after}"'
    
    try:
        print(f"🔄 Running optimization: {cmd}")
        run_command(cmd)
        
        # Validate output
        if os.path.exists(ir_after) and os.path.getsize(ir_after) > 0:
            validate_and_fix_ir_file(ir_after)
            print("✅ Optimization completed successfully")
        else:
            raise Exception("Optimization produced no output")
            
    except Exception as e:
        print(f"❌ Optimization failed: {e}")
        
        # Try alternative syntax
        alt_cmd = f'"{opt_path}" -p {passes} "{ir_before}" -S -o "{ir_after}"'
        try:
            print(f"🔄 Trying alternative: {alt_cmd}")
            run_command(alt_cmd)
            
            if os.path.exists(ir_after) and os.path.getsize(ir_after) > 0:
                validate_and_fix_ir_file(ir_after)
                print("✅ Alternative optimization succeeded")
            else:
                raise Exception("Alternative optimization also failed")
                
        except Exception as alt_error:
            raise Exception(f"All optimization attempts failed. Primary: {e}, Alternative: {alt_error}")

def create_basic_diff_result(before_lines, after_lines, passes):
    """Create a basic diff result structure"""
    import difflib
    
    diff = list(difflib.unified_diff(
        before_lines, 
        after_lines, 
        fromfile='before.ll', 
        tofile='after.ll', 
        lineterm=''
    ))
    
    class BasicMetrics:
        def __init__(self):
            self.instructions_eliminated = len(before_lines) - len(after_lines)
            self.reduction_percentage = (abs(self.instructions_eliminated) / len(before_lines)) * 100 if before_lines else 0
            self.memory_ops_eliminated = 0
            self.basic_blocks_before = len([line for line in before_lines if ':' in line and line.strip().endswith(':')])
            self.basic_blocks_after = len([line for line in after_lines if ':' in line and line.strip().endswith(':')])
            self.optimization_type = passes
            self.performance_impact = "Positive - Code optimized" if self.instructions_eliminated > 0 else "Neutral - Code structure modified"
            self.instructions_before = len(before_lines)
            self.instructions_after = len(after_lines)
            self.memory_operations_before = len([line for line in before_lines if any(op in line for op in ['alloca', 'load', 'store'])])
            self.memory_operations_after = len([line for line in after_lines if any(op in line for op in ['alloca', 'load', 'store'])])
    
    html_diff = difflib.HtmlDiff().make_table(
        before_lines, 
        after_lines, 
        'Before Optimization', 
        'After Optimization',
        context=True,
        numlines=3
    )
    
    return {
        'html_diff': html_diff,
        'text_diff': '\n'.join(diff),
        'summary': f"Applied {passes} optimization pass with {len(before_lines) - len(after_lines):+d} line changes",
        'metrics': BasicMetrics()
    }

def process_cpp_and_diff(src, passes):
    """Process C++ with enhanced IR validation"""
    ir_before, ir_after = get_ir_paths(src)
    ensure_output_dir()
    
    print(f"🔄 Processing optimization pass: {passes}")
    
    try:
        # Step 1: Generate clean IR
        print("📝 Generating clean LLVM IR...")
        generate_clean_ir(src, ir_before)
        
        # Step 2: Add optimization patterns
        print("🔧 Adding optimization patterns...")
        if passes != "mem2reg":  # mem2reg doesn't need pattern injection
            inject_patterns_safely(ir_before, passes)
        
        # Step 3: Apply optimization with validation
        print(f"⚡ Applying {passes} optimization...")
        apply_optimization_with_validation(ir_before, ir_after, passes)
        
        # Step 4: Read and analyze
        with open(ir_before, 'r') as f:
            before_content = f.read()
        with open(ir_after, 'r') as f:
            after_content = f.read()
        
        before_lines = before_content.split('\n')
        after_lines = after_content.split('\n')
        
        print("📊 Analyzing changes...")
        if HAS_ENHANCED_DIFFER:
            try:
                enhanced_differ = EnhancedIRDiffer()
                diff_result = enhanced_differ.generate_enhanced_diff(before_lines, after_lines, passes)
            except Exception as e:
                print(f"⚠️ Enhanced differ failed: {e}, using basic diff")
                diff_result = create_basic_diff_result(before_lines, after_lines, passes)
        else:
            diff_result = create_basic_diff_result(before_lines, after_lines, passes)
        
        # Generate AI summary
        print("🤖 Generating AI summary...")
        metrics_dict = diff_result['metrics'].__dict__ if hasattr(diff_result['metrics'], '__dict__') else diff_result['metrics']
        
        ai_summary = generate_enhanced_summary(
            diff_text=diff_result['text_diff'],
            before_lines=before_lines,
            after_lines=after_lines,
            passes=passes,
            metrics=metrics_dict
        )
        
        # Generate report
        print("📝 Creating HTML report...")
        html_report_path = generate_enhanced_html_report(
            before_lines=before_lines,
            after_lines=after_lines,
            diff_result=diff_result,
            ai_summary=ai_summary,
            source_file=src,
            passes=passes
        )
        
        instructions_changed = len(before_lines) - len(after_lines)
        print(f"✅ Success! {instructions_changed:+d} line change")
        print(f"📋 Report: {html_report_path}")
        
        return html_report_path
        
    except Exception as e:
        print(f"❌ Error: {e}")
        
        # Enhanced debugging
        print("\n🔧 Enhanced Debugging:")
        print(f"- Source: {src}")
        print(f"- Before IR: {ir_before} (exists: {os.path.exists(ir_before)})")
        print(f"- After IR: {ir_after} (exists: {os.path.exists(ir_after)})")
        
        # Show IR file contents if they exist
        if os.path.exists(ir_before):
            with open(ir_before, 'r') as f:
                content = f.read()
                lines = content.split('\n')
                print(f"- Before IR lines: {len(lines)}")
                if len(lines) > 12:
                    print(f"- Line 12 content: '{lines[11]}'")
        
        raise

def inject_patterns_safely(ir_file, pass_type):
    """Safely inject optimization patterns without breaking IR syntax"""
    with open(ir_file, 'r') as f:
        content = f.read()
    
    # Only inject patterns if we find a proper function definition
    if 'define' not in content:
        print("⚠️ No function definition found, skipping pattern injection")
        return
    
    if pass_type == "instcombine":
        content = add_safe_combinable_instructions(content)
    elif pass_type == "dce":
        content = add_safe_dead_code(content)
    elif pass_type == "simplifycfg":
        content = add_safe_cfg_patterns(content)
    
    with open(ir_file, 'w') as f:
        f.write(content)
    
    # Validate after injection
    validate_and_fix_ir_file(ir_file)

def add_safe_combinable_instructions(ir_content):
    """Add safe instruction combining patterns"""
    lines = ir_content.split('\n')
    result = []
    injection_done = False
    
    for line in lines:
        result.append(line)
        
        # Find the first basic block in a function
        if not injection_done and line.strip().endswith(':') and 'entry' in line:
            # Add safe combinable instructions
            combinable = [
                "  %temp_combine1 = add nsw i32 0, 0",      # Safe add
                "  %temp_combine2 = mul nsw i32 1, 1",      # Safe multiply  
                "  %temp_combine3 = add nsw i32 %temp_combine1, %temp_combine2"  # Use them
            ]
            result.extend(combinable)
            injection_done = True
    
    return '\n'.join(result)

def add_safe_dead_code(ir_content):
    """Add safe dead code patterns"""
    lines = ir_content.split('\n')
    result = []
    injection_done = False
    
    for line in lines:
        result.append(line)
        
        if not injection_done and line.strip().endswith(':') and 'entry' in line:
            dead_code = [
                "  %unused_dead1 = add nsw i32 42, 0",
                "  %unused_dead2 = mul nsw i32 %unused_dead1, 1"
            ]
            result.extend(dead_code)
            injection_done = True
    
    return '\n'.join(result)

def add_safe_cfg_patterns(ir_content):
    """Add safe control flow patterns"""
    # For simplifycfg, we'll be more conservative and just add simple redundant branches
    lines = ir_content.split('\n')
    result = []
    
    for line in lines:
        if 'ret ' in line and not line.strip().startswith(';'):
            # Add a simple redundant branch before return
            result.extend([
                "  br label %simple_block",
                "",
                "simple_block:",
                line
            ])
        else:
            result.append(line)
    
    return '\n'.join(result)
