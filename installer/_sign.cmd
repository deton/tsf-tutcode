@echo off
setlocal

pushd "%~dp0"

call _vsdev.cmd

call _version.cmd



rem > _sign.cmd  <SHA-1> <URL>
rem     * <SHA-1> : SHA-1 thumbprint of certificate
rem     * <URL> : RFC-3161 timestamp server

set DESCRIPTION="tsf-tutcode"

set SHA1HASH=%1
set TIMESTAMPSERVER=%2

rem x86
set BINFILES="..\Win32\Release\*.dll" "..\Win32\Release\*.exe"
rem x64
set BINFILES=%BINFILES% "..\x64\Release\*.dll" "..\x64\Release\*.exe"
rem ARM32   TIP only
set BINFILES=%BINFILES% "..\ARM\Release\*.dll"
rem ARM64
set BINFILES=%BINFILES% "..\ARM64\Release\*.dll" "..\ARM64\Release\*.exe"
rem ARM64EC   TIP only
set BINFILES=%BINFILES% "..\ARM64EC\Release\*.dll"

rem x86
set MSIFILES="%TARGETDIR%\x86.msi"
rem x64
set MSIFILES=%MSIFILES% "%TARGETDIR%\x64.msi"
rem ARM
set MSIFILES=%MSIFILES% "%TARGETDIR%\arm.msi"

rem bundle
set BEFILE="%TARGETDIR%\engine.exe"
set BOFILE="%TARGETDIR%\original.exe"
set BSFILE="%TARGETDIR%\tsftutcode-%VERSION%.exe"

set SIGNCOMMAND=signtool sign /v /d %DESCRIPTION% /sha1 %SHA1HASH% /fd sha256 /tr %TIMESTAMPSERVER% /td sha256



call _clean.cmd

echo sign binary files
%SIGNCOMMAND% %BINFILES%

call _build_msi.cmd

echo sign msi files
%SIGNCOMMAND% %MSIFILES%

call _build_bundle.cmd

move %BSFILE% %BOFILE%

echo detach engine
wix burn detach %BOFILE% -engine %BEFILE%

echo sign engine
%SIGNCOMMAND% %BEFILE%

echo reattach engine
wix burn reattach %BOFILE% -engine %BEFILE% -out %BSFILE%

echo sign bundle
%SIGNCOMMAND% %BSFILE%



popd

endlocal
