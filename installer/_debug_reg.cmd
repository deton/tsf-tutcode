pushd %~dp0
regsvr32 /s ..\Win32\Debug\imtuttip.dll
regsvr32 /s ..\x64\Debug\imtuttip.dll
popd
