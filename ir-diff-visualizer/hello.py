# test_llvm_fix.py - Test the LLVM version fix
import subprocess
import tempfile
import os

def test_llvm_consistency():
    """Test if LLVM tools work together consistently"""
    
    print("🧪 Testing LLVM Tool Consistency")
    print("=" * 40)
    
    # Create test C++ file
    test_cpp = '''
int add_numbers(int a, int b) {
    int unused = 42;  // Dead code for testing
    int temp = a + 0; // Identity for instcombine
    return temp + b;
}

int main() {
    return add_numbers(10, 20);
}
'''
    
    try:
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cpp', delete=False) as f:
            f.write(test_cpp)
            cpp_file = f.name
        
        ir_file = cpp_file.replace('.cpp', '.ll')
        opt_file = cpp_file.replace('.cpp', '_opt.ll')
        
        # Test 1: Clang compilation
        print("1️⃣ Testing clang++ compilation...")
        clang_cmd = f'"C:\\Program Files\\LLVM\\bin\\clang++.exe" -S -emit-llvm -O0 "{cpp_file}" -o "{ir_file}"'
        result1 = subprocess.run(clang_cmd, shell=True, capture_output=True, text=True)
        
        if result1.returncode == 0:
            print("✅ Clang compilation successful")
        else:
            print(f"❌ Clang failed: {result1.stderr}")
            return False
        
        # Test 2: Opt optimization  
        print("2️⃣ Testing opt optimization...")
        opt_cmd = f'"C:\\Program Files\\LLVM\\bin\\opt.exe" -passes="instcombine" "{ir_file}" -S -o "{opt_file}"'
        result2 = subprocess.run(opt_cmd, shell=True, capture_output=True, text=True)
        
        if result2.returncode == 0:
            print("✅ Opt optimization successful")
            
            # Check if optimization actually worked
            with open(ir_file, 'r') as f:
                before_content = f.read()
            with open(opt_file, 'r') as f:
                after_content = f.read()
            
            print(f"📊 Before: {len(before_content)} chars")
            print(f"📊 After: {len(after_content)} chars")
            
            if len(before_content) != len(after_content):
                print("✅ Optimization appears to have made changes")
            else:
                print("⚠️ No changes detected (this might be normal)")
            
            return True
        else:
            print(f"❌ Opt failed: {result2.stderr}")
            
            # Try with Program Files opt
            if "msys64" in result2.stderr or "mingw64" in result2.stderr:
                print("🔄 Trying with Program Files opt...")
                
                # Check if Program Files has opt
                program_files_opt = r"C:\Program Files\LLVM\bin\opt.exe"
                if os.path.exists(program_files_opt):
                    opt_cmd2 = f'"{program_files_opt}" -passes="instcombine" "{ir_file}" -S -o "{opt_file}"'
                    result3 = subprocess.run(opt_cmd2, shell=True, capture_output=True, text=True)
                    
                    if result3.returncode == 0:
                        print("✅ Program Files opt worked!")
                        return True
                    else:
                        print(f"❌ Program Files opt also failed: {result3.stderr}")
                else:
                    print("❌ Program Files opt not found")
            
            return False
            
    except Exception as e:
        print(f"❌ Test failed: {e}")
        return False
    finally:
        # Cleanup
        for file_path in [cpp_file, ir_file, opt_file]:
            if os.path.exists(file_path):
                os.remove(file_path)

if __name__ == "__main__":
    success = test_llvm_consistency()
    
    if success:
        print("\n🎉 LLVM tools are working correctly!")
        print("You should be able to run your IR diff analyzer now.")
    else:
        print("\n❌ LLVM tools have compatibility issues.")
        print("Recommended fix: Reinstall LLVM from the official release.")
        print("Download from: https://github.com/llvm/llvm-project/releases")