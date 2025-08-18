"""
Entry point for the Clang AST Line Mapper web application
"""

from web_app.app import app

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
