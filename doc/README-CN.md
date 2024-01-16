# Evlk-Shell-Library
[English](../README.md)/[chinese]()

Evolink Shell Arduino 库用于在 arduino 硬件上解释和运行自定义命令行脚本/命令。**Evolink Shell库不是异步的！不具有分配线程的能力！**<br><br>

尽管Evolink Shell library被设计用于在arduino硬件上面工作，但是截至目前(***版本0.1.0***)任然可以通过某些方法使其在通用C++平台上工作。

> 到目前为止可以通过改变`String`类型为`std::string`来解决`String`和`std::string`类型不同的问题，***但是***有一些弊端所以并不建议这样移植。
```C++
typedef std::string String;
```
<br>

Evolink Shell library需要stl容器(`vector`,`list`,`map`)支持，通常来说可以被任意C++编译器编译，但也可能需要另外安装[ArduinoSTL](https://github.com/mike-matera/ArduinoSTL)库以提供容器支持。

*请尽量将c++标准升级到***C++20***及以上，将来可能不会再对低版本的c++标准进行支持和维护*

# 如何使用
- [exsamples/shell/shell.ino](./exsamples/shell/shell.ino)