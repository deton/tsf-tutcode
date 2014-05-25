@rem Usage: build.bat [Win32|x64] [/t:Clean]
set PF=%1
if "%PF%"=="" (set PF=Win32) else shift
msbuild tsf-tutcode.sln /p:Configuration=Release /p:Platform=%PF% %1
