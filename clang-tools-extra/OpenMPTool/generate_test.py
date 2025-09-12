import sys
import os
import re # Import regex module
from dotenv import load_dotenv
import google.generativeai as genai
from google.api_core import exceptions # Import exceptions for better error handling

# Load API key from .env file
# The .env file should be in the same directory as this script.
load_dotenv()
api_key = os.getenv("GEMINI_API_KEY")

if not api_key:
    print("❌ ERROR: GEMINI_API_KEY not set. Please create a .env file in the script directory")
    print("        with content like: GEMINI_API_KEY='YOUR_API_KEY_HERE'")
    sys.exit(1)

# Configure the generative AI library with the API key
genai.configure(api_key=api_key)

# Check if prompt file path is provided as a command-line argument
if len(sys.argv) < 2:
    print("❌ Usage: python3 generate_test.py <prompt_file>")
    sys.exit(1)

prompt_path = sys.argv[1]
output_path = "/tmp/generated_test.c" # Keep .c extension for simplicity with tool's mv command

# Read the prompt from the specified file
try:
    with open(prompt_path, 'r') as f:
        prompt = f.read()
except FileNotFoundError:
    print(f"❌ ERROR: Prompt file not found at {prompt_path}")
    sys.exit(1)
except Exception as e:
    print(f"❌ ERROR: Could not read prompt file {prompt_path}: {e}")
    sys.exit(1)

print(f"💡 Sending prompt to Gemini API (Model: gemini-2.0-flash)...")
print("-" * 30)
print(prompt) # Print the prompt being sent for debugging
print("-" * 30)

# Send the prompt to the Gemini model
try:
    model = genai.GenerativeModel('gemini-2.0-flash')
    response = model.generate_content(prompt) 

    # Check for empty response
    if not response or not response.text:
        print("❌ ERROR: Gemini API returned an empty or unparseable response.")
        if response and response.prompt_feedback:
            print(f"Prompt Feedback: {response.prompt_feedback}")
        sys.exit(1)

    # --- START: HIGHLY ROBUST LOGIC to extract ONLY the C/C++ code block ---
    generated_raw_text = response.text.strip()
    
    # Define a regex that looks for a markdown code block starting with ```
    # and optionally followed by c, cpp, c++, or no language specifier,
    # then captures everything until the closing ```.
    # Using re.DOTALL ensures '.' matches newlines.
    # Using re.MULTILINE helps with ^ and $ if needed, but not strictly for this.
    code_block_pattern = re.compile(r"```(?:c\+\+|c|cpp)?\n(.*?)\n```", re.DOTALL)
    
    match = code_block_pattern.search(generated_raw_text)
    
    if match:
        generated_code = match.group(1).strip() # Extract the captured group (the code itself)
    else:
        # Fallback: If no markdown code block is found, assume the entire response is code.
        # This is less safe but necessary if the LLM sometimes doesn't use fences.
        print("⚠️ Warning: No markdown code block found. Attempting to use full response as code.")
        generated_code = generated_raw_text.strip()

    # Final check for emptiness after extraction
    if not generated_code:
        print("❌ ERROR: Extracted code is empty after processing.")
        sys.exit(1)
    # --- END: HIGHLY ROBUST LOGIC ---

    # Save the processed generated code to the output file
    with open(output_path, "w") as f:
        f.write(generated_code)
    print(f"✅ Test case saved to {output_path}")

    # --- Display the generated test case content ---
    print("\n--- Generated Test Case Content (Cleaned) ---")
    print(generated_code)
    print("-------------------------------------------\n")
    # --- END Display ---

except exceptions.GoogleAPIError as e:
    print(f"❌ ERROR: Google Gemini API error: {e}")
    print("Please check your API key, network connection, and API quota.")
    sys.exit(1)
except Exception as e:
    print(f"❌ An unexpected error occurred during Gemini API call: {e}")
    sys.exit(1)

# Compile and run the generated C code using Clang
print("⚙️ Attempting to compile and execute the generated test case...")

# Define the absolute path to your clang executable.
CLANG_PATH = os.path.expanduser("~/llvm-build/bin/clang") 

# Direct path to the directory containing omp.h, as found by the user
OPENMP_HEADERS_PATH = os.path.expanduser("/home/ananya/llvm-build/include/clang-default-headers")

# Directory containing libomp.so, as found by the user
OPENMP_LIB_PATH = os.path.expanduser("/usr/lib/llvm-14/lib")


# IMPORTANT FIX: Removed '-x c++' because the generated code is now C, not C++.
# When compiling a .c file, clang defaults to C mode. If the AI sometimes sends C++,
# this might cause issues again. A more sophisticated solution would inspect the
# generated code for C++ specific headers (like <iostream>) and dynamically add -x c++.
# For now, let's keep it in C mode as the AI seems to be favoring C for these minimal tests.
compile_command = f"{CLANG_PATH} -fopenmp -I{OPENMP_HEADERS_PATH} -L{OPENMP_LIB_PATH} {output_path} -o /tmp/gen_test"
execute_command = f"/tmp/gen_test"

# Run compilation
ret_compile = os.system(compile_command)
if ret_compile != 0:
    print(f"❌ Compilation failed. Command: {compile_command}")
    print(f"Please ensure '{CLANG_PATH}' is the correct path to your Clang executable.")
    print(f"Also, verify that the OpenMP headers path '{OPENMP_HEADERS_PATH}' is correct.")
    print(f"And the OpenMP library path '{OPENMP_LIB_PATH}' is correct.")
    # For debugging, also print the content of the generated file
    print("\n--- Content of generated_test.c (for failed compilation debug) ---")
    try:
        with open(output_path, 'r') as f:
            print(f.read())
    except Exception as e:
        print(f"Could not read {output_path}: {e}")
    print("------------------------------------------------------------------\n")
    sys.exit(1)
else:
    print("✅ Compilation successful.")
    # If compilation is successful, run the executable
    ret_exec = os.system(execute_command)
    if ret_exec != 0:
        print(f"❌ Execution failed. Command: {execute_command}")
        # When execution fails, it's useful to see if the generated binary ran at all
        print("This might indicate an issue with the generated test case's logic or runtime environment.")
        sys.exit(1)
    else:
        print("✅ Test case executed successfully.")

