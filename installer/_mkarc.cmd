@echo off

pushd %~dp0

7z.exe a -tzip tsftutcode.zip tsftutcode-x64.msi tsftutcode-x86.msi ..\README.TXT ..\LICENSE.TXT tutcode.tbl tcode.tbl maze.dic

popd
