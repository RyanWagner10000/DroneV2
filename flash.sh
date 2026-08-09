#!/bin/bash
# Create a new build folder and make the project

CURR_DIR=$(pwd)
INTERFACE_CFG="$CURR_DIR/scripts/stlink.cfg"
TARGET_CFG="$CURR_DIR/scripts/stm32f4x.cfg"

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
    # Try to cd into build folder if it exists
    cd build/
)
catch || {
    echo "Build directory does not exist. RUnning build script..."
    /bin/bash make.sh
}

cd build/
OS_NAME=$(uname)

if [ "$OS_NAME" == "Linux" ]; then

    try
    (
        echo ""
        echo " -- Launching openOCD -- "
        echo ""
        openocd -f "$INTERFACE_CFG" -f "$TARGET_CFG" -c 'program firmware.elf verify reset exit'
    )
    catch || {
        echo "Launching openOCD was unsuccessful."
        exit 1
    }

elif [[ "$OS_NAME" == CYGWIN* || "$OS_NAME" == MINGW* ]]; then
    
    try
    (
        echo ""
        echo " -- Launching openOCD -- "
        echo ""
        OPENOCD_COMMAND="openocd -f $INTERFACE_CFG -f $TARGET_CFG -c 'program firmware.elf verify reset exit'"
        powershell.exe -Command "Start-Process powershell -ArgumentList '-NoExit', \"& { $OPENOCD_COMMAND }\""
    )
    catch || {
        echo "Launching openOCD was unsuccessful."
        exit 1
    }

else
    echo "Unknown OS: $OS_NAME"
    exit 1
fi

echo " -- DONE! -- "

# Sources
# https://www.xmodulo.com/catch-handle-errors-bash.html