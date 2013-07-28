@echo off

pushd %~dp0

pushd config-sample
7za.exe a config-sample.7z *
move config-sample.7z ..\
popd

7za.exe a tsftutcode.7z tsftutcode-x64.msi tsftutcode-x86.msi ..\README.TXT ..\LICENSE.TXT tuttable.txt maze.dic config-sample.7z

popd
