# Clang AST Line Mapper Web Application

This is a Flask web application that provides a user interface for the Clang AST Line Mapper tool. The application allows users to input C++ code and visualize the AST (Abstract Syntax Tree) nodes alongside the source code with AI-powered interpretations.

## Features

- Input C++ code via a syntax-highlighted editor
- Visualize source code with corresponding AST nodes in a side-by-side view
- Get AI-powered interpretation of code structure with detailed explanations
- Load example code snippets for quick exploration
- Responsive web interface with Bootstrap styling

## Installation

1. Make sure you have Python 3.8+ installed
2. Clone the repository
3. Install dependencies:

```bash
cd clang-ast-mapper/web_app
pip install -r requirements.txt
```

## Running the Application

From the `web_app` directory, run:

```bash
python app.py
```

The application will be available at http://localhost:5000

## Directory Structure

```
web_app/
├── app.py                  # Main Flask application
├── requirements.txt        # Python dependencies
├── static/                 # Static assets
│   ├── css/                # CSS stylesheets
│   └── js/                 # JavaScript files
├── templates/              # HTML templates
│   ├── base.html           # Base template with common structure
│   └── index.html          # Main page template
└── temp/                   # Temporary files (created at runtime)
```

## Using the Application

1. Enter or paste C++ code in the editor
2. Click "Analyze Code" to process the input
3. View the side-by-side representation of code and AST nodes
4. Check the AI interpretation tab for detailed analysis

## Development

To contribute to the development:

1. Fork the repository
2. Create a new branch for your feature
3. Make your changes
4. Submit a pull request

## License

This project is licensed under the same terms as the main Clang AST Line Mapper project.
