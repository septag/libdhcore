## libdhcore: darkHAMMER Core 'C' library

Version 0.5.1-dev  
[http://www.hmrengine.com](http://www.hmrengine.com)  
[https://github.com/septag/libdhcore](https://github.com/septag/libdhcore)  

### Description
*libdhcore* is a lightweight and cross-platform C (C99) library, mainly targeted to game developers. It is used in *darkHAMMER* game engine, I don't know how mature it is, but it is being used and tested for 2 years in various projects.  
It can be built with GCC, MSVC and Clang compilers, on x86, x86-64 and ARM platforms, including windows, linux, MacOS and RaspberryPI.  
Source code License is BSD.  

Here's a list of library modules and their descriptions:  

- **Memory manager**: Automatic leak detection, Memory ID tracing and stats (allocate by userId), with allocator and alignment support.
- **Dynamic Array**: Fast C-friendly dynamic array container
- **Color**: RGBA color type with common math functions
- **Commander**: Command line argument parser and setup
- **Crash manager**: Multi-platform crash handler and callstack report, with user definable callbacks on application crash
- **Error manager**: Error handling functions, for keeping Error stack with reports
- **File-IO**: File IO abstraction (memory and disk), with Virtual directory support, PAK files and automatic directory monitoring
- **FreeList allocator**: Fixed size, dynamic malloc, freelist allocator for custom allocations
- **Pool allocator**: Fast dynamic size, fixed malloc, pool allocator for custom memory allocations
- **Stack allocator**: Very fast Stack based allocator for temperory custom memory allocations
- **Hash tables**: Optimized Open, Closed and Chained Hash table implementations with Linear probing and support for custom allocators.
- **Hash**: Murmur (32bit, 128bit, incremental) hashing functions
- **Hardware Info**: Multi-platform CPU and memory information queries
- **JSON**: JSON parser and writer, which is a thin wrapper over cJSON library with custom allocators
- **Linked List**: Common C-style linked list
- **Logging**: Formatted logging system, to different targets like console terminal, files and custom user functions.
- **MT**: Cross-platform threading and Atomic functions, using Posix on unix derivatives and Win32 API on Windows.
- **Sockets**: Sockets (UDP/TCP) wrapper functions
- **Numeric**: Random numbers, and common numeric helper functions
- **Pak files**: Compressed (or non-compressed) PAK file reading and writing.
- **Primitives**: Rectangle, Sphere and AABB types with common mathematic functions
- **Queue**: Common C-Style queue structure
- **Stack**: Common C-Style stack structure
- **JSON-RPC**: Flexible JSON-RPC parsing and responding, API signature control and help generation
- **Standard math**: Common helper math functions
- **Vector math**: Common SIMD optimized Vector math, Vector, Matrix and Quaternions (left-handed system)
- **String helpers**: *libdhcore* doesn't provide string objects, but provides helper functions for common String and Path manipulations
- **Task Manager**: Multi-threaded task dispatcher
- **Timer**: High-Res OS timers
- **Utility functions**: Common OS specific functions, copy, move, delete files, etc.
- **Compresssion**: Thin layer over *miniz* library for data compression

### Installation

#### Linux
Install the packages listed below before proceeding to build and install the library :   

- **git**: source control client (not required, as you can download the source too)
- **python**: version 3 or higher, required in order to run the *waf* build system
- **waf**: It also can be found in `libdhcore/var/waf` directory
- **gcc**: Tested on version 4.8 

After checking for above packages, run these commands, and change `/install/path` to your preferred path on disk (for example: `--prefix=/usr`) :

```
git clone git@github.com:septag/libdhcore.git
cd libdhcore
waf configure --prefix=/install/path
waf build
sudo waf install
```

To build debug libraries, which will have `-dbg` suffix after their names, use the commands below instead of last two lines:

```
waf build_debug
sudo waf install_debug
```

To build test applications (*$(prefix)/bin/dhcore-test*) , use `--build-tests` argument in `waf configure`.   

To use a cross compiler, use `--cross-compile` argument in *configure* and provide your compiler binary. For example, I have a ARMv6 cross compiler named *armv6-rpi-linux-gnueabi-gcc*, I use below command to configure the build for that platform:

```
waf configure --cross-compile=armv6-rpi-linux-gnueabi-gcc
```

For more *configure* options, run `waf --help` when you are in *libdhcore* directory to see more command line arguments.

#### Windows
The code should compile on windows using the same commands with `waf`, in the following days, I'll test it on that platform and write a more useful documentation. Also I'll add Visual Studio 2012 project files and thus easier build for MSVC folks.


#### MacOS
The code was also built on MacOS (Clang/GCC) last year, but unfortunately I don't have MacOS device to test it myself,  so contributors are welcomed.


### Usage
Make sure the you have setup *include* and *library* paths of *libdhcore* in your compiler. Then include any header file from `dhcore` directory into your source. And link your program to `dhcore`.  

Main include file is `dhcore/core.h`, and before everything else you must *Initialize* core lib at the beginning of your program and *Release* it after everything else is done:  

```
#include <stdio.h>
#include "dhcore/core.h"

int main(int argc, char** argv)
{
    result_t r = core_init(CORE_INIT_ALL);
    if (IS_FAIL(r))
        exit(-1);

    puts("Hello world");

    core_release(TRUE); // pass TRUE for leak report
    return 0;
}
```

### Documentation
For API documentation (which is at it's early stage), check out this [API Documentation](http://www.hmrengine.com/docs/core/html).  
You can also build the documentation by installing **doxygen** and *cd* to `/path/to/libdhcore/doc` directory and run:  

```
doxygen doxygen-core.conf
```

After that, HTML documentation will be ready at `doc/html` directory

### Credits

- **libdhcore**: Sepehr Taghdisian *(septag@github, sep.tagh@gmail)*
- **cJSON**: [http://sourceforge.net/projects/cjson](http://sourceforge.net/projects/cjson)
- **miniz**: [https://code.google.com/p/miniz](https://code.google.com/p/miniz)
- **waf**: [https://code.google.com/p/waf](https://code.google.com/p/waf)
