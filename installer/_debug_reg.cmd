@echo off
setlocal
pushd "%~dp0"

openfiles > nul 2>&1
if %errorlevel% equ 1 (
  powershell start-process "%~nx0" -verb runas
) else (
  regsvr32 /s "..\build\Win32\Debug\imtuttip.dll"
  regsvr32 /s "..\build\x64\Debug\imtuttip.dll"
)

popd
endlocal
