# YumeEngine
Modern Cross Platform Renderer

#Building the code
##Linux
<b>Prerequisities</b>

- <a href="http://www.cmake.org/">CMake 2.6.2 or greater</a>
On Ubuntu/Debian
`sudo apt-get install cmake`
- OpenGL
`sudo apt-get install freeglut3-dev`
This should install all required dependencies
- IDE of your choice (not required)
- GNUXX `sudo apt-get install g++`

<b>Building</b>

After forking the repository go to the directory 
- `mkdir _build && cd _build`
- `sudo cmake ..`
If all dependencies are satisfied,it should say Configuring Done Generating Done.
- Then on the _build directory just type `make`

If there is no error then the project has been built.You can check out the bin folder for sample projects.

##Windows
<b>Prerequisities</b>

- <a href="http://www.cmake.org/">CMake 2.6.2 or greater</a>
- Visual Studio
- <a href="http://www.microsoft.com/en-us/download/details.aspx?id=6812">DirectX SDK June 2010</a>
- <a href="https://dev.windows.com/en-us/downloads/windows-10-sdk">Windows 10 SDK</a> if you want to build Direct3D 12 library

Note:If you have VS2015 you may already have required files.

<b>Building</b>

If you have CMake GUI go to the associated directory and create a _build dir.Then hit Configure.Tune in the options as you desire.Then hit configure again(no red bars should remain). Then hit Generate.Go to _build directory and launch YumeEngine.sln. Build the ALL_BUILD project.

# **Sources Used On This Project**

[Stateless, layered, multi-threaded rendering – Part 1](http://blog.molecular-matters.com/2014/11/06/stateless-layered-multi-threaded-rendering-part-1/ "Stateless, layered, multi-threaded rendering – Part 1")

[Stateless Rendering](http://jendrikillner.bitbucket.org/blog/blog/stateless_rendering/ "Stateless Rendering")

[Designing a Modern Rendering Engine](https://www.cg.tuwien.ac.at/research/publications/2007/bauchinger-2007-mre/bauchinger-2007-mre-Thesis.pdf "Designing a Modern Rendering Engine")

[OGRE 3D](http://www.ogre3d.org/)

[Horde 3D](http://www.horde3d.org/)

[Hy3](https://hieroglyph3.codeplex.com/)

[Frank Luna's D3D11 Book](http://www.amazon.com/Introduction-3D-Game-Programming-DirectX/dp/1936420228)

<img src = "http://i.imgur.com/mPVCTYw.png" />

Logo credits go to julesawdupin
