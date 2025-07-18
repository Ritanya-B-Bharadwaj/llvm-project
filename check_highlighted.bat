@echo off
setlocal enableDelayedExpansion

rem — Find the attribute group number for IsHighlighted
set "attrGroup="
for /f "tokens=2 delims=# " %%G in ('findstr /C:"attributes #.*IsHighlighted" highlighted.ll') do (
  set "attrGroup=%%G"
  goto :found
)
:found

if not defined attrGroup (
  echo No IsHighlighted attribute group found in highlighted.ll
  goto :end
)

echo Found IsHighlighted attribute group: #%attrGroup%
echo Highlighted functions:

rem — Now list all function definitions that end in that group
for /f "tokens=2 delims=@ " %%F in ('findstr /R "^define.*#%attrGroup%" highlighted.ll') do (
  echo  - %%F
)

:end
