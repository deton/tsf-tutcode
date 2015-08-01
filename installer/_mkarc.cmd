@echo off
setlocal

pushd %~dp0

call _version.cmd
set TARGETDIR=build

del "%TARGETDIR%\tsf-tutcode-%VERSION%.zip"

pushd "%TARGETDIR%"
7z.exe a -tzip -mtc=off tsf-tutcode-%VERSION%.zip tsftutcode-%VERSION%-x64.msi tsftutcode-%VERSION%-x86.msi ..\..\README.TXT ..\..\LICENSE.TXT ..\kanatable-tutcode.txt ..\kanatable-tcode.txt ..\maze.dic
popd

popd

endlocal
