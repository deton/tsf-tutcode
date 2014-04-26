@echo off
setlocal
set TARGETDIR=build

pushd %~dp0

"%WIX%bin\candle.exe" tsftutcode-x64.wxs -nologo -out %TARGETDIR%\tsftutcode-x64.wixobj
"%WIX%bin\light.exe" %TARGETDIR%\tsftutcode-x64.wixobj -nologo -out %TARGETDIR%\tsftutcode-x64.msi -ext WixUIExtension -sw1056

popd

endlocal
