@echo off
setlocal

pushd "%~dp0"

call _vsdev.cmd

call _version.cmd

msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Release,Platform=x86
if %errorlevel% == 0 (
msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Release,Platform=x64
)

if "%ENABLE_PLATFORM_ARM%" neq "0" (
  msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Release,Platform=ARM
  msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Release,Platform=ARM64
)

popd

endlocal
