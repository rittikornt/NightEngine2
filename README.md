# [NightEngine2](https://github.com/rittikornt/NightEngine2)
![Screenshot](https://user-images.githubusercontent.com/12661089/91659778-e2cfae00-ea86-11ea-9d29-3f489c54565b.jpg)

## Summary
NightEngine2 is a open-source 3D component-based game engine written in C++. This is a project migrated from my old repo [NightEngine](https://bitbucket.org/BellyLand/nightengine/overview). This project is mostly for education purpose and is by no mean production ready.

## Getting Started
NightEngine2 use cmake as build generator: [cmake](http://www.cmake.org/download/), which is used to generate platform-specific makefiles or project files. Start by cloning this repository, making sure to pass the `--recursive` flag to grab all the dependencies. If you forgot, then you can `git submodule update --init` instead.

```bash
git clone --recursive https://github.com/rittikornt/NightEngine2.git
cd nightengine
cd Build
```

Now generate a project file or makefile for your platform. If you want to use a particular IDE, make sure it is installed; don't forget to set the Start-Up Project in Visual Studio or the Target in Xcode.

```bash
# POSIX Makefile
cmake ..

# Mac OSX
cmake -G "Xcode" ..

# Microsoft Windows Visual Studio
cmake -G "Visual Studio 14" ..
cmake -G "Visual Studio 14 Win64" ..
cmake -G "Visual Studio 15 2017 Win64" ..
...
```

## Dependency
Functionality           | Library
----------------------- | ------------------------------------------
Mesh Loading            | [assimp](https://github.com/assimp/assimp)
Physics                 | [bullet](https://github.com/bulletphysics/bullet3)
OpenGL Function Loader  | [glad](https://github.com/Dav1dde/glad)
Windowing and Input     | [glfw](https://github.com/glfw/glfw)
OpenGL Mathematics      | [glm](https://github.com/g-truc/glm)
Texture Loading         | [stb](https://github.com/nothings/stb)
JSON Serialization      | [json](https://github.com/taocpp/json)
UnitTest                | [catch2](https://github.com/catchorg/Catch2)

## Screenshot
![nightengine_crytek_sponza](https://user-images.githubusercontent.com/12661089/91659795-fed34f80-ea86-11ea-8828-80c20371df4a.jpg)

![nightengine_manylights_scene](https://user-images.githubusercontent.com/12661089/91659816-21fdff00-ea87-11ea-9f8b-a9929abb449f.jpg)

![nightengine_defaultscene](https://user-images.githubusercontent.com/12661089/91659823-2e825780-ea87-11ea-8725-6b411ddf5ad5.jpg)

![nightengine_tools](https://user-images.githubusercontent.com/12661089/91659809-14e11000-ea87-11ea-8507-598de7f27235.jpg)

## License
>The MIT License (MIT)

>Copyright (c) 2019 Rittikorn Tangtrongchit

>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

>The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
