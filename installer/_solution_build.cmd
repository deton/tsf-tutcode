@echo off
setlocal
pushd "%~dp0"

call _vsdev.cmd

call _env.cmd

set BUILDCOMMAND=msbuild "..\tsf-tutcode.sln" -nologo -maxcpucount -verbosity:normal
set BUILDCOMMAND=%BUILDCOMMAND% -target:Build -property:Configuration=Release

rem x86
%BUILDCOMMAND% -property:Platform=x86

rem x64
%BUILDCOMMAND% -property:Platform=x64

if "%ENABLE_PLATFORM_ARM%" neq "0" (

rem ARM32
%BUILDCOMMAND% -property:Platform=ARM -property:WindowsTargetPlatformVersion=10.0.22621.0

rem ARM64
%BUILDCOMMAND% -property:Platform=ARM64

rem ARM64EC
%BUILDCOMMAND% -property:Platform=ARM64EC

)

popd
endlocal
