@echo off
setlocal

pushd %~dp0

call _version.cmd
set TARGETDIR=build

"%WIX%bin\candle.exe" tsftutcode-x64.wxs -nologo -out "%TARGETDIR%\tsftutcode-%VERSION%-x64.wixobj"
"%WIX%bin\light.exe" "%TARGETDIR%\tsftutcode-%VERSION%-x64.wixobj" -nologo -out "%TARGETDIR%\tsftutcode-%VERSION%-x64.msi" -ext WixUIExtension -sw1056

popd

endlocal
