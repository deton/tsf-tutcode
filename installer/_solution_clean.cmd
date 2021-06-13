@echo off
setlocal

pushd "%~dp0"

call _vsdev.cmd

call _version.cmd

rem x86

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Clean ^
/property:Configuration=Release,Platform=x86

rem x64

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Clean ^
/property:Configuration=Release,Platform=x64

rem ARM32

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Clean ^
/property:Configuration=Release,Platform=ARM

rem ARM64

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Clean ^
/property:Configuration=Release,Platform=ARM64

rem ARM64EC

msbuild "..\imcrvskk.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Clean ^
/property:Configuration=Release,Platform=ARM64EC

popd

endlocal
