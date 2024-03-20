@echo off
setlocal

pushd "%~dp0"

call _vsdev.cmd

call _version.cmd

call _build_sub.cmd

echo build x86.msi

wix build -arch x86 ^
-ext WixToolset.UI.wixext ^
-src installer-x86.wxs -out "%TARGETDIR%\x86.msi"

echo build x64.msi

wix build -arch x64 ^
-ext WixToolset.UI.wixext ^
-src installer-x64.wxs -out "%TARGETDIR%\x64.msi"

echo build arm.msi

if "%ENABLE_PLATFORM_ARM%" neq "0" (
wix build -arch arm64 ^
-ext WixToolset.UI.wixext ^
-src installer-arm.wxs -out "%TARGETDIR%\arm.msi"
)

popd

endlocal
