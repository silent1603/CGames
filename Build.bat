@echo off

mkdir .\Build
pushd .\Build
cl -FC -Zi ..\src/main.cpp /link user32.lib Gdi32.lib
popd 