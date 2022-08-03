cd ..
if not exist build (
	mkdir build
	echo "build directory has been created"
)
cmake -B build .
cmake --build build

if not exist %cd%\example\config (
	.\build\Debug\Alex --spp 64 -o %cd%\example\output.png -s %cd%\example\scene.xml -d %cd%\example\config -nt 16
) else (
	.\build\Debug\Alex -c %cd%\example\config
)
