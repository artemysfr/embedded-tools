# embedded-tools
Miscellaneous tools to bring up/test embedded Linux systems

## modbus\_xy-md02
Small prog to read temperature and humidity sensor connected on RS485 serial port through modbus RTU protocol.
To build (tested with Buildroot cross-toolchain):

    mkdir build
    cd build/
    cmake -DCMAKE_TOOLCHAIN_FILE=/path_to_buildroot/output/host/share/buildroot/toolchainfile.cmake ../
    cmake --build .

You will get an executable named *xy-mt02* which you can copy and run on your embedded system.
