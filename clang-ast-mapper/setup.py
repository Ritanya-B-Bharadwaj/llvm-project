"""
Setup script for Clang AST Line Mapper
"""

from setuptools import setup, find_packages
import os

# Read the README file
with open("README.md", "r", encoding="utf-8") as fh:
    long_description = fh.read()

# Read the requirements file
with open("requirements.txt", "r", encoding="utf-8") as fh:
    requirements = [line.strip() for line in fh if line.strip() and not line.startswith("#")]

setup(
    name="clang-ast-mapper",
    version="1.0.0",
    author="AST Mapper Development Team",
    author_email="dev@astmapper.com",
    description="A CLI tool that maps C++ source lines to AST nodes using Clang",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/yourusername/clang-ast-mapper",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Software Development :: Code Generators",
        "Topic :: Software Development :: Compilers",
        "Topic :: Software Development :: Documentation",
        "Topic :: Utilities",
    ],
    python_requires=">=3.6",
    install_requires=requirements,
    extras_require={
        "dev": [
            "pytest>=6.0.0",
            "pytest-cov>=2.0.0",
            "flake8>=3.8.0",
            "black>=21.0.0",
            "isort>=5.0.0",
            "mypy>=0.910",
        ],
        "docs": [
            "sphinx>=4.0.0",
            "sphinx-rtd-theme>=1.0.0",
        ],
    },
    entry_points={
        "console_scripts": [
            "clang-ast-mapper=ast_line_mapper:main",
        ],
    },
    project_urls={
        "Bug Reports": "https://github.com/yourusername/clang-ast-mapper/issues",
        "Source": "https://github.com/yourusername/clang-ast-mapper",
        "Documentation": "https://clang-ast-mapper.readthedocs.io/",
    },
    keywords="clang ast parser c++ source code analysis",
    package_data={
        "": ["*.md", "*.txt", "*.json"],
    },
    include_package_data=True,
    zip_safe=False,
)
