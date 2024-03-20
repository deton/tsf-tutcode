@echo off
setlocal

pushd "%~dp0"

call _vsdev.cmd

call _version.cmd

echo build bundle

wix build -arch x86 ^
-ext WixToolset.Bal.wixext -ext WixToolset.Util.wixext ^
installer-bundle.wxs -out "%TARGETDIR%\tsftutcode-%VERSION%.exe"

popd

endlocal
