@echo off
setlocal

pushd %~dp0

call _version.cmd
set TARGETDIR=build

del "%TARGETDIR%\config-lua.zip"

pushd config-lua
7z.exe a -tzip -mtc=off "..\%TARGETDIR%\config-lua.zip" *.lua
popd

del "%TARGETDIR%\tsf-tutcode.zip"

pushd "%TARGETDIR%"
7z.exe a -tzip -mtc=off tsf-tutcode-%VERSION%.zip tsftutcode-x64.msi tsftutcode-x86.msi ..\..\README.TXT ..\..\LICENSE.TXT ..\kanatable-tutcode.txt ..\kanatable-tcode.txt ..\maze.dic config-lua.zip
popd

popd

endlocal
