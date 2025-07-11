# Runes

A skylanders figure editor.

It is not yet ready for public use.

# Building

## Windows

Requires MSYS2, use UCRT64 and not MINGW64.

Dependencies:
* Qt 6.7.0, can be installed with `pacman -S mingw-w64-ucrt-x86_64-qt6-base=6.7.0-2 mingw-w64-ucrt-x86_64-qt6-tools=6.7.0-1`
* yaml-cpp, can be installed with `pacman -S mingw-w64-x86_64-yaml-cpp`
* hidapi, can be installed with `pacman -S mingw-w64-ucrt-x86_64-hidapi`
* libfmt, can be installed with `pacman -S mingw-w64-ucrt-x86_64-fmt`

Compiling:
* Run `qmake "CONFIG+=debug"` (`qmake` for release) and `make` to compile
* Run `windeployqt debug` (`windeployqt release` for release) to include the Qt binaries

## Usage

Requires a salt.txt file in the Resources folder, its contents should be the salt used in the MD5 hash that generates the encryption key.

`Runes <Path to encrypted dump file>`