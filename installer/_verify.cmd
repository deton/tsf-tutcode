@echo off
setlocal
pushd "%~dp0"

call _vsdev.cmd

call _env.cmd



set BINFILES=
rem x86
set BINFILES=%BINFILES% "..\build\Win32\Release\*.dll" "..\build\Win32\Release\*.exe"
rem x64
set BINFILES=%BINFILES% "..\build\x64\Release\*.dll" "..\build\x64\Release\*.exe"
rem ARM32   TIP only
set BINFILES=%BINFILES% "..\build\ARM\Release\*.dll"
rem ARM64
set BINFILES=%BINFILES% "..\build\ARM64\Release\*.dll" "..\build\ARM64\Release\*.exe"
rem ARM64EC   TIP only
set BINFILES=%BINFILES% "..\build\ARM64EC\Release\*.dll"

set MSIFILES=
rem x86
set MSIFILES=%MSIFILES% "%OutDir%\x86.msi"
rem x64
set MSIFILES=%MSIFILES% "%OutDir%\x64.msi"
rem ARM32
set MSIFILES=%MSIFILES% "%OutDir%\arm32.msi"
rem ARM64
set MSIFILES=%MSIFILES% "%OutDir%\arm64.msi"

rem bundle
set BEFILE="%OutDir%\tsftutcode-%VERSION%-engine.exe"
set BSFILE="%OutDir%\tsftutcode-%VERSION%.exe"



signtool verify /all /v /d /pa /tw %BINFILES% %MSIFILES% %BEFILE% %BSFILE%

set SIGNCOUNT=0
for %%i in (%BINFILES% %MSIFILES% %BEFILE% %BSFILE%) do set /a "SIGNCOUNT = SIGNCOUNT + 1"

echo;
echo     %SIGNCOUNT% signatures in all.



popd
endlocal
