# Toolchain file for MinGW-w64 cross-compilation to 64-bit Windows
set(CMAKE_SYSTEM_NAME Windows) # Target system is Windows
set(CMAKE_SYSTEM_PROCESSOR x86_64) # Target architecture

# Specify the cross-compilers
# The exact names might vary slightly based on your MinGW-w64 installation.
# Use 'which x86_64-w64-mingw32-gcc' in your WSL terminal to confirm the names/paths if needed.
set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc-posix)       # C compiler
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-posix)     # C++ compiler
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)  # Resource compiler (for .rc files, if any)

# Set the target environment for find_XXX commands
# This helps CMake find Windows-specific libraries/headers if needed, though for std::thread
# the compiler and FindThreads module should handle it.
set(CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX}) #

# Modify default behavior of FIND_XXX() commands to search only in the cross-compiler paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY) #
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY) #
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)