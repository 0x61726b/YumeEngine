[![Build Status](https://travis-ci.org/arkenthera/YumeEngine.svg?branch=master)](https://travis-ci.org/arkenthera/YumeEngine)
[![Build status](https://ci.appveyor.com/api/projects/status/w0lrpofmuuycgjso?svg=true)](https://ci.appveyor.com/project/arkenthera/yumeengine)
[![codecov.io](https://codecov.io/github/arkenthera/YumeEngine/coverage.svg?branch=master)](https://codecov.io/github/arkenthera/YumeEngine?branch=master)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Analytics](https://ga-beacon.appspot.com/UA-74178547-1/welcome-page)](https://github.com/arkenthera/YumeEngine)
[![todofy badge](https://todofy.org/b/arkenthera/YumeEngine/master)](https://todofy.org/r/arkenthera/YumeEngine/master)
[![Coverity](https://scan.coverity.com/projects/8038/badge.svg)](https://scan.coverity.com/projects/arkenthera-yumeengine)



![](http://i.imgur.com/61mRXuH.jpg)


![](http://i.imgur.com/jhi44QL.jpg)

# Synopsis
Yume Engine
Exploring new renderer design ideas,trying to implement as simple as possible.Heavily inspired by other open source renderers I aim to support D3D11,D3D12 and OpenGL(Vulkan in the future), this engine strives to be as platform dependent as possible.After initial planned platforms' work has been done,I'm aiming to add more platform support.

For updates from this engine development(and my other silly posts) you can check out my <a href="http://arkenthera.github.io">website</a>

#Building

```
git clone https://github.com/arkenthera/YumeEngine
cd YumeEngine
git submodule update --init --recursive
```

##Mac OS X
<b>Prerequisities</b>
- [CMake](http://www.cmake.org)
- OpenGL (probably already installed)
- XCode or Command line tools


Tested on Yosemite 10.10

##Debian/Ubuntu
<b>Prerequisities</b>
- [CMake](http://www.cmake.org)
- ```sudo apt-get install build-essential libgl1-mesa-dev```
- GCC 4.8

Tested on Ubuntu 14.04

```
mkdir build && cd build
cmake ..
make
```

##Windows
<b>Prerequisities</b>

- <a href="http://www.cmake.org/">CMake 2.6.2 or greater</a>
- Visual Studio 2013 (Need C++11 support)
- <a href="http://www.microsoft.com/en-us/download/details.aspx?id=6812">DirectX SDK June 2010</a>

You can use CMake GUI to create a Vs2013 solution then build it.

Tested on Win10 x64 Vs2013

# **Sources Used On This Project**

[Stateless, layered, multi-threaded rendering – Part 1](http://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/ "Stateless, layered, multi-threaded rendering – Part 1")

[Stateless Rendering](http://jendrikillner.bitbucket.org/blog/blog/stateless_rendering/ "Stateless Rendering")

[Designing a Modern Rendering Engine](https://www.cg.tuwien.ac.at/research/publications/2007/bauchinger-2007-mre/bauchinger-2007-mre-Thesis.pdf "Designing a Modern Rendering Engine")

[OGRE 3D](http://www.ogre3d.org/)
[Horde 3D](http://www.horde3d.org/)
[Hy3](https://hieroglyph3.codeplex.com/)
[Frank Luna's D3D11 Book](http://www.amazon.com/Introduction-3D-Game-Programming-DirectX/dp/1936420228)

<img src = "http://i.imgur.com/mPVCTYw.png" />
