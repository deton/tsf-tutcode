pushd %~dp0
regsvr32 /s /u ..\Win32\Debug\imtuttip.dll
regsvr32 /s /u ..\x64\Debug\imtuttip.dll
popd
