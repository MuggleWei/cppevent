# cppevent

已废弃!
几年之后回头在来看, 这个库代写的很不好, 效率不高, 而且依赖libevent和muggleCC, 我现在已经直接在muggleCC中加入了网络模块的功能, 所以此项目将不在维护!

****

## 其他语言 ##
[English](readme.md)

Cppevent是一个跨平台的轻量级c++网络库，主要目标是开发一个简单易用的网络库，让我能方便快速的搭建起一个网络程序。当前暂时只支持TCP.

## 编译 ##
你需要一个支持c++11语法的编译器，接着就像大多数的cmake工程一样，运行命令:
```
git submodule update --init --recursive
mkdir build
cd build
cmake ..
```