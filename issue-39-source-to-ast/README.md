# Source-to-AST Annotator

This project takes a C++ source file and displays its **Abstract Syntax Tree (AST)** annotated with Clang AST node types. The tool offers a web interface for users to upload code, visualize the AST structure, and understand compilation processes better.

---

## Key Features and Workflow

1. **Upload C++ Source File**: Users can upload any `.cpp` file via the interface.
2. **AST Generation**: The backend invokes Clang to generate the AST in JSON format.
3. **Annotation**: The AST nodes are processed and labeled with their node types.
4. **Visualization**: Annotated AST is displayed cleanly on the frontend.

---

## Interface Screenshots

### Upload and Parse Interface
![Interface - Upload and Parse](screenshots/annotator.png)

### AST View Panel
![Interface - AST View](screenshots/ast%20tree.png)

### Node Annotations
![Interface - AST](screenshots/ast.png)

### Compiler Guide Panel
![Interface - Compiler Overview](screenshots/compiler%20guide.png)

---

## Directory Structure

```

issue-39-source-to-ast/
├── annotate.py
├── annotate\_ast.py
├── templates/
│   └── index.html
├── static/
│   └── styles.css
├── screenshots/
│   ├── annotator.png
│   ├── ast tree.png
│   ├── ast.png
│   └── compiler guide.png
└── README.md

````

---

## 🧑Running on VS Code with WSL (Ubuntu)

1. **Open VS Code in WSL mode**  
   - Install **WSL** and a Linux distro (Ubuntu recommended)
   - Open VS Code → Use `Remote - WSL` extension to connect

2. **Clone the Repository:**

```bash
git clone https://github.com/anjaliheda/Source-to-AST-Annotator.git
cd Source-to-AST-Annotator
````

3. **Set Up Virtual Environment (Optional):**

```bash
sudo apt update
sudo apt install python3-venv
python3 -m venv venv
source venv/bin/activate
```

4. **Install Dependencies:**

```bash
pip install flask
```

5. **Run the App:**

```bash
python3 annotate.py
```

6. **View in Browser:**

```
http://localhost:5000
```

---

## 📌 Notes

* Ensure Clang is installed (`clang -Xclang -ast-dump=json -fsyntax-only <file.cpp>`)
* If files like `ast.json` exceed GitHub limits, use `.gitignore` or Git LFS
* This project is part of **Issue #39** for the HPE Compiler repo

---

### 🔄 Next Steps

1. Replace your `README.md` in `issue-39-source-to-ast/` with the content above.
2. Commit the update:

```bash
git add README.md
git commit -m "Final README with screenshots, setup and guide"
git push origin issue-39
````

3. Comment on your pull request that the README has been finalized.


