@echo off

pushd %~dp0

7z.exe a tsftutcode.7z tsftutcode-x64.msi tsftutcode-x86.msi ..\README.TXT ..\LICENSE.TXT tutcode.tbl tcode.tbl maze.dic

popd
