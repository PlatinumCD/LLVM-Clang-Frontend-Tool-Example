# LLVM-Clang-Frontend-Tool-Example

This tool inserts profiling instrumentation into C programs. 

ssh into server
```
ssh delphi.nic.uoregon.edu
```

Load modules
```
module load gcc/7.3
module load cmake/3.15
```

Clone Spack
```
git clone https://github.com/spack/spack.git
export PATH=$PATH:spack/bin
```

Get LLVM 13.0.1
```
spack install --dirty llvm@13.0.1 +shared_libs %gcc@7.3.0
```

Get Tool example
```
git clone https://github.com/PlatinumCD/LLVM-Clang-Frontend-Tool-Example.git
cd LLVM-Clang-Frontend-Tool-Example
```

Build Tool
```
mkdir build
cd build
cmake ..
make
```

Run Tool
```
./mini-instrumentor ../example.c
cat example.instr.c
```
