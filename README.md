# Evlk-Shell-Library
[English]()/[chinese](doc/README-CN.md)

Evolink Shell Arduino library is used to interpret and run custom command-line scripts/commands on arduino hardware.**The Evolink Shell library is not asynchronous! No ability to allocate threads!**<br><br>

Although the Evolink Shell library is designed to work on Arduino hardware, as of now (***version 0.1.0***) there are still ways to make it work on the general C++ platform.

>So far, the problem of different types of `String` and `std::string` can be solved by changing the `String` type to `std::string`, ***but*** there are some disadvantages, so it is not recommended to transplant in this way.
```C++
typedef std::string String;
```

Evolink Shell library requires stl container (`vector`, `list`, `map`) support. Generally speaking, it can be compiled by any C++ compiler, but it may also need to be installed separately [ArduinoSTL](https://github.com/mike-matera/ArduinoSTL) library to provide container support.

*Please try to upgrade the C++ standard to ***C++20*** and above, and the lower version of the C++ standard may no longer be supported and maintained in the future*

# How To Use
- [exsamples/shell/shell.ino](./exsamples/shell/shell.ino)