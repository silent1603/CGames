@echo off

mkdir .\Build
pushd .\Build
cl -Zi ..\src/main.cpp /link user32.lib Gdi32.lib
popd 