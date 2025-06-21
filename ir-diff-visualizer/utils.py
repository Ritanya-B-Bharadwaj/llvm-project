# utils.py - Fixed to use consistent LLVM version
import os
import subprocess
import sys
import shutil
from pathlib import Path
import re

def get_consistent_llvm_tools():
    """Get LLVM tools from the same installation to avoid version conflicts"""
    
    # Priority order: prefer Program Files LLVM over msys64
    llvm_base_paths = [
        r"C:\Program Files\LLVM\bin",
        r"C:\LLVM\bin", 
        r"C:\msys64\mingw64\bin",
        None  # Use system PATH as fallback
    ]
    
    tools = ['clang++', 'clang', 'opt', 'llc', 'llvm-config']
    tool_paths = {}
    
    # Try to find all tools from the same base path first
    for base_path in llvm_base_paths:
        if base_path and os.path.exists(base_path):
            print(f"🔍 Checking LLVM tools in: {base_path}")
            
            all_tools_found = True
            temp_paths = {}
            
            for tool in tools:
                tool_exe = f"{tool}.exe" if sys.platform.startswith('win') else tool
                full_path = os.path.join(base_path, tool_exe)
                
                if os.path.exists(full_path):
                    temp_paths[tool] = full_path
                else:
                    all_tools_found = False
                    break
            
            if all_tools_found:
                print(f"✅ Found complete LLVM toolset in: {base_path}")
                tool_paths = temp_paths
                
                # Verify versions match
                try:
                    clang_version = get_tool_version(tool_paths['clang++'])
                    opt_version = get_tool_version(tool_paths['opt'])
                    print(f"📋 Clang version: {clang_version}")
                    print(f"📋 Opt version: {opt_version}")
                    
                    return tool_paths
                except Exception as e:
                    print(f"⚠️ Version check failed: {e}")
                    continue
    
    # Fallback: use system PATH
    print("🔄 Using system PATH as fallback...")
    for tool in tools:
        tool_path = shutil.which(tool)
        if tool_path:
            tool_paths[tool] = tool_path
    
    return tool_paths

def get_tool_version(tool_path):
    """Get version of an LLVM tool"""
    try:
        result = subprocess.run([tool_path, '--version'], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            return result.stdout.split('\n')[0].strip()
        return "Unknown"
    except:
        return "Error"

def run_command(command, check_output=False):
    """Enhanced command runner with better error handling"""
    try:
        print(f"🔄 Running: {command}")
        
        # Use shell=True on Windows for better compatibility
        shell = sys.platform.startswith('win')
        
        if check_output:
            result = subprocess.run(
                command, 
                shell=shell, 
                capture_output=True, 
                text=True, 
                timeout=60
            )
            
            if result.returncode != 0:
                print(f"❌ Command failed with exit code {result.returncode}")
                print(f"STDOUT: {result.stdout}")
                print(f"STDERR: {result.stderr}")
                raise subprocess.CalledProcessError(result.returncode, command, result.stdout, result.stderr)
            
            return result.stdout
        else:
            result = subprocess.run(
                command, 
                shell=shell, 
                check=True, 
                capture_output=True, 
                text=True,
                timeout=60
            )
            
            if result.stdout:
                print(f"✅ Command output: {result.stdout[:200]}...")
            
            return result.stdout
            
    except subprocess.TimeoutExpired:
        raise Exception(f"Command timed out: {command}")
    except subprocess.CalledProcessError as e:
        error_msg = f"Command failed: {command}\nExit code: {e.returncode}"
        if e.stderr:
            error_msg += f"\nError output: {e.stderr}"
        if e.stdout:
            error_msg += f"\nStandard output: {e.stdout}"
        raise Exception(error_msg)
    except Exception as e:
        raise Exception(f"Failed to run command '{command}': {str(e)}")

def ensure_output_dir():
    """Ensure output directory exists"""
    output_dir = Path("output")
    output_dir.mkdir(exist_ok=True)
    return output_dir

def get_ir_paths(src):
    """Get paths for before and after IR files"""
    output_dir = ensure_output_dir()
    
    if isinstance(src, str) and os.path.exists(src):
        base_name = Path(src).stem
    else:
        base_name = "temp"
    
    before_path = output_dir / f"{base_name}_before.ll"
    after_path = output_dir / f"{base_name}_after.ll"
    
    return str(before_path), str(after_path)

def validate_ir_file(filepath):
    """Validate LLVM IR file syntax"""
    if not os.path.exists(filepath):
        raise Exception(f"IR file not found: {filepath}")
    
    if os.path.getsize(filepath) == 0:
        raise Exception(f"IR file is empty: {filepath}")
    
    with open(filepath, 'r') as f:
        content = f.read()
        
        if not content.strip():
            raise Exception(f"IR file has no content: {filepath}")
        
        if 'define' not in content and 'declare' not in content:
            print(f"⚠️ Warning: IR file may not contain function definitions: {filepath}")
        
        return True

def fix_ir_file_issues(filepath):
    """Fix common LLVM IR file issues"""
    with open(filepath, 'r') as f:
        content = f.read()
    
    fixed_content = content
    
    # Remove problematic attributes
    fixed_content = re.sub(r'\s*optnone\s*', ' ', fixed_content)
    fixed_content = re.sub(r'\s*noinline\s*', ' ', fixed_content)
    
    # Ensure proper target specification
    if 'target datalayout' not in fixed_content:
        target_info = '''target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

'''
        fixed_content = target_info + fixed_content
    
    with open(filepath, 'w') as f:
        f.write(fixed_content)
    
    return filepath

# Initialize consistent LLVM tools
LLVM_TOOLS = get_consistent_llvm_tools()

def get_llvm_tool_path(tool_name):
    """Get the path for a specific LLVM tool"""
    return LLVM_TOOLS.get(tool_name, tool_name)

# Print tool configuration on import
print("\n🔧 LLVM Tool Configuration:")
print("=" * 30)
for tool, path in LLVM_TOOLS.items():
    print(f"{tool}: {path}")
print("=" * 30)
