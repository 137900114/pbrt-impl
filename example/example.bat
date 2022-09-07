cd ..

if not exist build (
	mkdir build
	echo "build directory has been created"
)

if not exist %cd%\build\Release\Alex.exe goto rebuild
if "%0" == "rebuild" goto rebuild
goto rbend

:rebuild 
cmake -B build .
cmake --build build --config Release
:rbend

if not exist %cd%\example\config (
	%cd%\build\Release\Alex --spp 64 -o %cd%\example\output.png -s %cd%\example\scene.xml -d %cd%\example\config -nt 16
) else (
	%cd%\build\Release\Alex -c %cd%\example\config
)
