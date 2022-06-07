# Raytracing in OpenGL

This is what it says. Raytracing in a Weekend, implemented in OpenGL.

## Build Instructions

> :warning: **I can give you my guarantee that this code is safe, but you can't (and shouldn't) trust it. I highly recommend you read through the code and see and understand the code for yourself, before you build the program. This should be general practice among all the Open Source Software you download and use.**

- Get a local version of the repository using Git, or downloading zipfile Github provides.
  - ```git clone https://github.com/sacreative10/openglraytracing.git```

- Make a build directory and change directories into build.
  - ``` mkdir build && cd $_ ```

- Configure the CMake project, using the build type you want. (Note, you can also open the repo in Visual Studio and build from there instead. This goes for all IDEs supporting CMake). You can also add any configuration settings you want (.i.e. Release or Debug).
  - ``` cmake .. -G Ninja # using ninja build for example ```

- Make the project using the build system you chose in the previous step.
  - ``` ninja -v -j5 ```

- Lastly, run the executable file.

## Fetching the Binary from Github

> You can do so. But you still need to download the project for the shaders (TODO: maybe include shaders in executable). But you still need to put the executable in a 1 level deep (from project root) folder, so the shader fetching works.
