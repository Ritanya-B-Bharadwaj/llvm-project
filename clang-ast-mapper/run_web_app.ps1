# PowerShell script to run the Clang AST Line Mapper Web Application

Write-Host "Installing required dependencies..." -ForegroundColor Green
pip install -r web_app/requirements.txt

Write-Host "Starting Clang AST Line Mapper Web Application..." -ForegroundColor Green
python web_app/app.py

if ($LASTEXITCODE -ne 0) {
    Write-Host "Error starting the application. Please check that all dependencies are installed." -ForegroundColor Red
    Write-Host "Run: pip install -r web_app/requirements.txt" -ForegroundColor Yellow
    Read-Host "Press Enter to continue..."
}
