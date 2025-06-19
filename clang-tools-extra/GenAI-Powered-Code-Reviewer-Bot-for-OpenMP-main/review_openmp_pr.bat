@echo off
setlocal

:: Get the directory where this BAT file is located
set SCRIPT_DIR=%~dp0

if "%1"=="" (
    echo Usage: review_openmp_pr.bat PR_NUMBER
    exit /b
)

echo 🔎 Running code reviewer for PR #%1
python "%SCRIPT_DIR%src\suggest_code_review.py" %1
