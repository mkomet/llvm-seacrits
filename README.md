# llvm-seacrits

### Pre-requisites

* llvm-12
* llvm-12-dev
* clang-12


### Building the IR Pass
```shell
clang++-12 -I /usr/include/llvm-c-12/ -I /usr/include/llvm-12/ -shared -fPIC TypeInfoHashPass.cpp -o TypeInfoHashPass.so -fno-exceptions -std=c++17 -lLLVM-12
```

### Using the IR Pass

For example, on file `example.cpp`

```shell
clang++-12  example.cpp -std=c++17 -fno-rtti -c -flto -o example.bc
opt-12 -load ./TypeInfoHashPass.so --modify-type-info-pass  -O0  example.bc -o example.bc
clang++-12 example.bc  -static-libstdc++ -fno-rtti -o example -g0 -O3 -s
```

Now check the product binary, where none of the original type_info hashes will reside.

> **_NOTE:_** This will not include the type_info names from the statically compiled libstdc++.a (or libc++.a)
