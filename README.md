# VWolf

Project inspired by the [Game engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) by [The Cherno](https://www.youtube.com/@TheCherno), this repo contains code to make a game engine core with and a demo app to test its implementation. 

![Demo](Demo.gif)

## Objective

This project pretends to have a basic render with the required capabilities like geometry handling, lighting, materials, textures and shadows. The goal is to create the core renderer and scene management to allow the creation of multiple applications with it.

## Project Structure

### Sandbox

**TO BE EXPLAINED**

### VWolf

**TO BE EXPLAINED**

## Current features

- Window management
- Input (Keyboard and mouse) management
- Basic application structure
- Two set of drivers 
    - OpenGL
    - DirectX 12
- Shape helper with mesh objects
- ImGui window management
- Material and lighting basics
    - Multiple shaders
    - Material system
    - Multiple lights
- Shader examples

## Dependencies

- OpenGL (Glad)
- GLFW
- ImGui
- GLM
- DirectX 12
- Windows.h
- Boost

## Installation mode

This projects requires premake5 to be included in your local machine. Right now the **premake5.lua** file is in the root directory and it gives the structure of the dependencies. Will suggest this [video](https://www.youtube.com/watch?v=sULV3aB2qeU&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=7) to learn how to set up the project, but in summary:

- Pull the project with the dependencies
- Create **premake5.lua** files for the dependencies.
- Download premake5 for your OS
- Run the *generate* script inside the **scripts/{OS}** folder.
- Open project file.

**NOTE:** In the Mac version, the Boost libraries have to be set up manually (by adding the folder inside *Library Search Paths*).

## Future goals

- Cleanup
    - Refactor code to break unnecessary OOP (inheritance) among files
    - Revisit public interface
    - Decouple camera interaction and camera math
    - Divide sub namespaces
- Window management
    - Clean events code
    - Event queue
    - Multithreading
- Render
    - Texture system
    - Shadows
    - Skybox
    - Framebuffer (Image buffer)
- Expand driver integration
    - Cocoa and Metal
    - GLFW and Vulkan
- Scene management
    - ECS
- Physics system
- Audio

## References

- [Game engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) by [The Cherno Channel](https://www.youtube.com/@TheCherno)
- [DirectX 12 Render](https://www.youtube.com/playlist?list=PLU2nPsAdxKWQw1qBS9YdFi9hUMazppjV7) by [Game engine series Channel](https://www.youtube.com/@GameEngineSeries)
