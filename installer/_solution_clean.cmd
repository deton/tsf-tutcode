@echo off
setlocal

pushd "%~dp0"

call "%VS140COMNTOOLS%VsDevCmd.bat"

msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Clean /property:Configuration=Release,Platform=x86
msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Clean /property:Configuration=Release,Platform=x64

popd

endlocal
