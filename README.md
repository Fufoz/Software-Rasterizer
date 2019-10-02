# Softy
Softy is a software renderer implementation written from scratch simply for educational purposes.

## Things implemented
At the moment the following things are implemented:
 * Depth buffer visualisation
 * Normal mapping
 * Parallax occlusion mapping
 * Flat/Gouraud/Phong shading models
 * Multisample anti-aliasing (4xmsaa)

## Build instructions(from root directory)
In order to build renderer library and examples you need to
pass the following flags to cmake:

```
cmake -BBuild -H. -DCMAKE_INSTALL_PREFIX=./
    -DSDL_ROOT=<Absolute_path_to_SDL_root_dir>
    -DCMAKE_BUILD_TYPE=Release
```
This will create bin direcroty with all examples inside.

Note > for Windows platform you also need to pass Visual Studio generator to cmake, for example (-G"Visual Studio 15 2017 Win64").

## Running examples
Run Every demo app from root directory like so
```
    <demo_install_prefix>\bin\<demo_name>.exe
```
