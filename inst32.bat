@echo off
taskkill /IM imtutmgr.exe
copy /Y win32\Release\*.dll \Windows\System32\IME\IMTSFTUTCODE
if errorlevel 1 exit
copy /Y win32\Release\*.exe \Windows\System32\IME\IMTSFTUTCODE
copy /Y installer\config-lua\init.lua \Windows\IME\IMTSFTUTCODE
copy /Y installer\config-share\config.xml \Windows\IME\IMTSFTUTCODE
copy /Y installer\config-share\mazedict.txt \Windows\IME\IMTSFTUTCODE\skkdict.txt
