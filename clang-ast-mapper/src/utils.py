"""
Utility module for checking environment configuration
"""

import subprocess
import os
import platform
import sys
from pathlib import Path

def check_clang_installation():
    """Check if Clang is installed and available in the PATH."""
    try:
        # Try to run clang --version
        result = subprocess.run(['clang', '--version'], 
                                capture_output=True, 
                                text=True,
                                timeout=5)
        if result.returncode == 0:
            return {
                'installed': True,
                'version': result.stdout.split('\n')[0],
                'path': get_clang_path()
            }
        return {
            'installed': False,
            'error': 'Clang is not responding correctly'
        }
    except FileNotFoundError:
        return {
            'installed': False,
            'error': 'Clang is not installed or not in PATH'
        }
    except Exception as e:
        return {
            'installed': False,
            'error': str(e)
        }

def get_clang_path():
    """Get the path to the clang executable."""
    try:
        if platform.system() == 'Windows':
            result = subprocess.run(['where', 'clang'], 
                                    capture_output=True, 
                                    text=True)
        else:
            result = subprocess.run(['which', 'clang'], 
                                    capture_output=True, 
                                    text=True)
        
        if result.returncode == 0:
            return result.stdout.strip()
        return None
    except Exception:
        return None

def get_system_info():
    """Get system information for diagnostics."""
    return {
        'os': platform.system(),
        'os_version': platform.version(),
        'python_version': platform.python_version(),
        'platform': platform.platform(),
        'path': os.environ.get('PATH', '')
    }

def check_environment():
    """Check the environment for all required dependencies."""
    return {
        'clang': check_clang_installation(),
        'system': get_system_info(),
        'python_path': sys.executable,
        'current_dir': os.getcwd(),
        'script_dir': os.path.dirname(os.path.abspath(__file__)),
    }

if __name__ == '__main__':
    print(check_environment())
