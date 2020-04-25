@echo off
setlocal

pushd "%~dp0"

call _vsdev.cmd

call _version.cmd

rem x86

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Build ^
/property:Configuration=Release,Platform=x86

rem x64

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Build ^
/property:Configuration=Release,Platform=x64

if "%ENABLE_PLATFORM_ARM%" neq "0" (

rem ARM32

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Build ^
/property:Configuration=Release,Platform=ARM

rem ARM64

msbuild "..\tsf-tutcode.sln" ^
/nologo /maxcpucount /verbosity:normal /target:Build ^
/property:Configuration=Release,Platform=ARM64

)

popd

endlocal
