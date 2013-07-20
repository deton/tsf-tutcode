@echo off

pushd %~dp0

"%WIX%bin\candle.exe" -nologo "tsftutcode-x64.wxs"
"%WIX%bin\light.exe" -nologo -ext WixUIExtension "tsftutcode-x64.wixobj"

"%WIX%bin\candle.exe" -nologo "tsftutcode-x86.wxs"
"%WIX%bin\light.exe" -nologo -ext WixUIExtension "tsftutcode-x86.wixobj"

popd
