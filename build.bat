if not exist build (
	mkdir build
	echo "build directory has been created"
)
cmake -B build . 