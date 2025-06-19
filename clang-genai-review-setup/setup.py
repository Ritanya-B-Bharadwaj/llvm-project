from setuptools import setup, find_packages

setup(
    name='clang-genai-review',
    version='0.1',
    packages=find_packages(),
    install_requires=[
        'requests',
        'groq',
        'python-dotenv',
    ],
    entry_points={
        'console_scripts': [
            'clang-genai-review = clang_genai_review.cli:main',
        ],
    },
)
