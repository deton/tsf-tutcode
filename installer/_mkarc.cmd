@echo off
setlocal
set TARGETDIR=build

pushd %~dp0

del %TARGETDIR%\config-lua.zip

pushd config-lua
7z.exe a -tzip "..\%TARGETDIR%\config-lua.zip" *.lua
popd

pushd %TARGETDIR%
7z.exe a -tzip tsf-tutcode.zip tsftutcode-x64.msi tsftutcode-x86.msi ..\..\README.TXT ..\..\LICENSE.TXT ..\tutcode.tbl ..\tcode.tbl ..\maze.dic config-lua.zip
popd

popd

endlocal
