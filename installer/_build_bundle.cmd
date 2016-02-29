@echo off
setlocal

pushd "%~dp0"

call _version.cmd

"%WIX%bin\candle.exe" installer-bundle.wxs -nologo -out "%TARGETDIR%\tsftutcode-%VERSION%.wixobj" -ext WixBalExtension -ext WixUtilExtension
"%WIX%bin\light.exe" "%TARGETDIR%\tsftutcode-%VERSION%.wixobj" -nologo -out "%TARGETDIR%\tsftutcode-%VERSION%.exe" -ext WixBalExtension -ext WixUtilExtension

popd

endlocal
