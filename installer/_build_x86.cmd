@echo off
setlocal
set TARGETDIR=build

pushd %~dp0

"%WIX%bin\candle.exe" tsftutcode-x86.wxs -nologo -out "%TARGETDIR%\tsftutcode-x86.wixobj"
"%WIX%bin\light.exe" "%TARGETDIR%\tsftutcode-x86.wixobj" -nologo -out "%TARGETDIR%\tsftutcode-x86.msi" -ext WixUIExtension -sw1056

popd

endlocal
