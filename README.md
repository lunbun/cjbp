# cjbp: C++ Java bytecode parsing library

cjbp is a C++ library for parsing Java bytecode.

This library was originally designed for my [toy JVM](https://github.com/lunbun/cinnamon_jvm), so its APIs are geared
towards efficient code execution. Also, this library may be incomplete as it is being developed in tandem with the JVM.
Use at your own risk.

## Installation

To use the library in your own project, first clone the library:

```bash
git clone https://github.com/lunbun/cjbp.git
```

Then, build the library:

```bash
cd cjbp
mkdir build
cd build
cmake ..
make
```

To import the library into your own CMake project, set the `cjbp_DIR` CMake variable to the path of the `build`
directory. For example, if you cloned the library to
`/path/to/cjbp`, set `cjbp_DIR` to `/path/to/cjbp/build`. You can do this by setting the `Dcjbp_DIR` variable in your
CMake command when configuring your project:
```bash
cmake -Dcjbp_DIR=/path/to/cjbp/build ..
```

Finally, add the following lines to your CMakeLists.txt file to include and link the library:
```cmake
find_package(cjbp REQUIRED)

get_target_property(cjbp_INCLUDE_DIRS cjbp::cjbp INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(<target> PRIVATE ${cjbp_INCLUDE_DIRS})
target_link_libraries(<target> PRIVATE cjbp::cjbp)
```

## Usage
See the [examples](examples) directory for usage examples.
