The only major caveat with my code is that when you run the software with arguments such that all data should be 
imported, at the end of the output the program will complain that a vector has reached maximum size. I am not sure why 
this is happening or if it can be fixed.

I compiled this CW with GCC and ran it on Windows 10 with a WSL Ubuntu installation. Here is the output of the gcc -v 
command on my laptop:

Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/9/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:hsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.3.0-17ubuntu1~20.04' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs 
--enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 
--program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext 
--enable-threads=posix --libdir=/usr/lib --enable-nls --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes 
--with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie 
--with-system-zlib --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --with-arch-32=i686 
--with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic 
--enable-offload-targets=nvptx-none=/build/gcc-9-HskZEa/gcc-9-9.3.0/debian/tmp-nvptx/usr,hsa --without-cuda-driver 
--enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)

For some reason, my code fails test 12 on CS Autograder, but it passes on my laptop. Moreover, 
tests for outputs 4,5,6 and 7 also fail on Autograder, but when ran on my laptop the output is the correct one, and I 
have used a string comparison web site to conform my program gives the correct output. For some reason, on Autograder 
the code puts the authority code instead of the measure code, when on my laptop it works correctly.

The following is a list of pieces of code which I think you should pay attention to, as I believe the solution 
I found is clever and would give me more marks:

- The code in bethyw.cpp for parsing command line arguments, in the methods you specified and the private methods I added.
  
- The code in areas.cpp in the populateFromWelshStatsJSON and populateFromAuthorityByYearCSV methods and the private 
  static methods I added which are used in those two methods.
    
- The way I implemented object to json conversion, using the to_json method in areas.cpp, area.cpp and measure.cpp .

- The way I implemented the << operators in areas.cpp, area.cpp and measure.cpp, including the private static methods 
  used for that in measure.cpp .
  
- The way I implemented the "insert or merge" behaviour you described for setArea and setMeasure, using the overloaded 
  copy assigment operators in the Area and Measure classes.