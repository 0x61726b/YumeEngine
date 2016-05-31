[![Build Status](https://travis-ci.org/arkenthera/YumeEngine.svg?branch=master)](https://travis-ci.org/arkenthera/YumeEngine)
[![Build status](https://ci.appveyor.com/api/projects/status/w0lrpofmuuycgjso?svg=true)](https://ci.appveyor.com/project/arkenthera/yumeengine)
[![codecov.io](https://codecov.io/github/arkenthera/YumeEngine/coverage.svg?branch=master)](https://codecov.io/github/arkenthera/YumeEngine?branch=master)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Analytics](https://ga-beacon.appspot.com/UA-74178547-1/welcome-page)](https://github.com/arkenthera/YumeEngine)
[![todofy badge](https://todofy.org/b/arkenthera/YumeEngine/master)](https://todofy.org/r/arkenthera/YumeEngine/master)
[![Coverity](https://scan.coverity.com/projects/8038/badge.svg)](https://scan.coverity.com/projects/arkenthera-yumeengine)

![](http://i.imgur.com/xG2na2E.jpg)

![](http://i.imgur.com/5SyAHWa.jpg)

[CHECK OUT THE TECH DEMO HERE](https://www.youtube.com/watch?v=yKNa7dFu8iU)

# Synopsis
Yume Engine
Exploring new renderer design ideas,trying to implement as simple as possible.Heavily inspired by other open source renderers I aim to support D3D11,D3D12 and OpenGL(Vulkan in the future), this engine strives to be as platform dependent as possible.After initial planned platforms' work has been done,I'm aiming to add more platform support.

For updates from this engine development(and my other silly posts) you can check out my <a href="http://arkenthera.github.io">website</a>

**Update 2016** : Oh well, when I wrote above, it was 2 years ago. I was reading papers about game engine design,getting fired up and thought that I could create an engine supporting every platform.Well that might be doable but not if you're not working on the project full-time. Yume only supports **Direct3D 11** as of now, in the upcoming years it is for sure that I will expand to other APIs.

##Current Features

- Driving the engine through XML files,see [here](https://github.com/arkenthera/YumeEngine/blob/master/Engine/Assets/RenderCalls/ReflectiveShadowMap.xml) to see how Yume renders a Reflective Shadow Map for use in global illumination.
- ASSIMP model/material loading and importer to a common format
- Unique UI framework using Javascript bindings (Chromium Embedded Framework). See the side project [CEF3D](https://github.com/arkenthera/cef3d)
- Everything is deferred!
- [Deferred lights using depth-stencil test](http://arkenthera.github.io/Deferred-Lights-in-Yume-Engine-using-stencil-test/)
- Global Illumination through [Voxel Cone Tracing](http://research.nvidia.com/sites/default/files/publications/GIVoxels-pg2011-authors.pdf)
- [Light Propagation Volumes](http://cg.ibds.kit.edu/publications/p2010/CLPVFRII_Kaplanyan_2010/CLPVFRII_Kaplanyan_2010.pdf)
- Frustum culling,batching
- Post Processing effects such as, SSAO algorithm Scalable Ambient Obscurance, FXAA,DoF,Godrays,Auto Exposure

##Building
<b>Prerequisities</b>

- <a href="http://www.cmake.org/">CMake 2.6.2 or greater</a>
- Visual Studio 2013 (Need C++11 support)

[CHECK OUT THE TECH DEMO HERE](https://www.youtube.com/watch?v=yKNa7dFu8iU)

##Some screens

![](http://i.imgur.com/WRKOVRd.jpg)

![](http://i.imgur.com/HbOeJVu.jpg)

![](http://i.imgur.com/elObCXQ.jpg)

![](http://i.imgur.com/pKh0E8q.jpg)

![](http://i.imgur.com/00caEIY.png)

![](http://i.imgur.com/nimxfmi.jpg)

![](http://i.imgur.com/wPyh1ed.jpg) 

![](http://i.imgur.com/Faot4ol.jpg)

![](http://i.imgur.com/o5v455c.png)

# **Sources Used On This Project**

[Stateless, layered, multi-threaded rendering – Part 1](http://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/ "Stateless, layered, multi-threaded rendering – Part 1")

[Stateless Rendering](http://jendrikillner.bitbucket.org/blog/blog/stateless_rendering/ "Stateless Rendering")

[Designing a Modern Rendering Engine](https://www.cg.tuwien.ac.at/research/publications/2007/bauchinger-2007-mre/bauchinger-2007-mre-Thesis.pdf "Designing a Modern Rendering Engine")

[OGRE 3D](http://www.ogre3d.org/)
[Horde 3D](http://www.horde3d.org/)
[Hy3](https://hieroglyph3.codeplex.com/)
[Frank Luna's D3D11 Book](http://www.amazon.com/Introduction-3D-Game-Programming-DirectX/dp/1936420228)

[Tobias Franke's awesome Dirtchamber](http://www.tobias-franke.eu/?dev)

<img src = "http://i.imgur.com/mPVCTYw.png" />
