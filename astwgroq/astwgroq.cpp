#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.cpp>\n";
        return 1;
    }

    const char* sourceFile = argv[1];
    const char* astFile = "ast.json";

    // Run clang to dump AST to JSON
    std::string cmd = "clang++ -Xclang -ast-dump=json -fsyntax-only ";
    cmd += sourceFile;
    cmd += " > ";
    cmd += astFile;
    std::system(cmd.c_str());

    // Run Python script to print annotated output
    std::string pythonCmd = "python3 main.py ";
    pythonCmd += sourceFile;
    pythonCmd += " ";
    pythonCmd += astFile;
    std::system(pythonCmd.c_str());

    return 0;
}
