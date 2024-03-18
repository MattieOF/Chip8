@echo off
echo Generating projects for vs2022...
cd ..
"Scripts/Bin/premake5.exe" vs2022
pause
