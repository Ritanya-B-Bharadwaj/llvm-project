import streamlit as st
import subprocess
import tempfile
import os

# Title
st.title("Regular Memory Access Analyzer")

# Input method
code = st.text_area("Enter C Code:", height=300)

if st.button("Analyze"):
    with tempfile.TemporaryDirectory() as temp_dir:
        file_path = os.path.join(temp_dir, "input.c")
        
        # Write input code to a temp file
        with open(file_path, "w") as f:
            f.write(code)

        # Correct path to your compiled binary inside build/
        tool_path = "./build/analyze_regular_memory_access"

        # Construct the command
        command = [
            tool_path,
            "-analyze-regular-memory-access",
            file_path,
            "--",
            "-x", "c",
            "--sysroot=/Library/Developer/CommandLineTools/SDKs/MacOSX14.5.sdk"
        ]

        try:
            # Run the analyzer
            result = subprocess.run(command, capture_output=True, text=True)
            output = result.stdout.strip()
            error_output = result.stderr.strip()

            # Display results
            st.subheader("Analysis Output:")
            st.code(output if output else "(No output)", language="text")

            if error_output:
                st.subheader("Errors/Warnings:")
                st.code(error_output, language="text")

        except Exception as e:
            st.error(f"Error running analyzer: {e}")
