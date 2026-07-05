# codebase-cpp
My personal codebase for C++ apps. The code is designed for GCC C++23. Maybe I will add support for earlier C++ versions in the future. 

## compilation
Use make.

## network
Helpers for using sockets. Mainly for unix use. Will update for Windows if needed in the future.

## module
Code in the codebase converted to C++20 modules. I have not had a good time with modules, hopefully they become easier to use in the future.

### module compilation
```shell
# Step 1: Compile the all of the module interfaces
g++ -std=c++23 -fmodules-ts -x c++ -Wall -Werror -Wpedantic -c file.cppm -o file_ifc.o

# Step 2: Compile the module implementations (if applicable).
g++ -std=c++23 -fmodules-ts -Wall -Werror -Wpedantic -c file.cpp -o file.o
```
