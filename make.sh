#!/bin/bash
# Create a new build folder and make the project

CURR_DIR=$(pwd)

function try()
{
    [[ $- = *e* ]]; SAVED_OPT_E=$?
    set +e
}

function throw()
{
    exit $1
}

function catch()
{
    export exception_code=$?
    (( $SAVED_OPT_E )) && set +e
    return $exception_code
}

try
(
    # Clear terminal
    clear

    # Remove build folder if it exists
    rm -rdf build/

    # Make new build folder, but don't error if it already exists
    mkdir -p build

    echo ""
    echo "Cleared and remade /build directory."
    echo ""
)
catch || {
    echo "Clearing and re-making build dir was unsuccessful."
}

# cd into new build folder
cd build/

# Try to Make CMake project in build folder
try
(
    echo ""
    echo " -- Making CMake Project -- "
    echo ""
    cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi-gcc.cmake ..
)
catch || {
    echo "Project Making operation was unsuccessful."
    exit 1
}

# Try to Make the project
try
(
    echo ""
    echo " -- Building CMake Project -- "
    echo ""
    cmake --build . -j2
)
catch || {
    echo "Project Building operation was unsuccessful."
    exit 1
}

# Print the size of the compiled elf file
echo ""
echo "firmware.elf File Dimensions"
arm-none-eabi-size firmware.elf
echo ""
arm-none-eabi-objdump -h firmware.elf

echo " -- DONE! -- "

# Sources
# https://www.xmodulo.com/catch-handle-errors-bash.html