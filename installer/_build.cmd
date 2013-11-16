@echo off

pushd %~dp0

"%WIX%bin\candle.exe" tsftutcode-x64.wxs -nologo
"%WIX%bin\light.exe" tsftutcode-x64.wixobj -nologo -ext WixUIExtension -sw1056

"%WIX%bin\candle.exe" tsftutcode-x86.wxs -nologo
"%WIX%bin\light.exe" tsftutcode-x86.wixobj -nologo -ext WixUIExtension -sw1056

popd
