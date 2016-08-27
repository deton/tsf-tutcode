@echo off
setlocal

pushd "%~dp0"

call "%VS140COMNTOOLS%VsDevCmd.bat"

msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Release,Platform=Win32
if %errorlevel% == 0 (
msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Release,Platform=x64
)

popd

endlocal
