# cppevent

Obsolete!
A few years later, I looked back. This library is very poorly written, inefficient, and relies on libevent and muggCC. I have now added network module in muggCC directly, so this project will not be maintained!

****

## other language readme ##
[中文](readme_cn.md)

Cppevent is a cross-platform lightweight network library for c++, the target of this library is easy to use, let me start up a network program quickly and simply. now only support TCP. 

## How to build ##
To find a compiler that support c++11, and then use cmake command:
```
git submodule update --init --recursive
mkdir build
cd build
cmake ..
```