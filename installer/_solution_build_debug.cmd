@echo off
setlocal

pushd "%~dp0"

call _vsdev.cmd

call _version.cmd

msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Debug,Platform=x86
msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Debug,Platform=x64

if "%ENABLE_PLATFORM_ARM%" neq "0" (
  msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Debug,Platform=ARM
  msbuild "..\tsf-tutcode.sln" /nologo /maxcpucount /verbosity:normal /target:Build /property:Configuration=Debug,Platform=ARM64
)

popd

endlocal
