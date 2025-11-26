# Runes

A skylanders figure editor.

It is not yet ready for public use.

# Building

## Windows

Requires:
* Visual Studio 2022 Developer Console
* Qt 6.9.0

Compiling:
* In the Visual Studio 2022 Developer Console
* cd into the Runes folder
* run `mkdir build`
* cd into the build folder
* run `cmake -S .. -B . -DCMAKE_PREFIX_PATH=%QT_DIR%\6.9.0\msvc2022_64`
* run `msbuild Runes.sln /p:Configuration=Debug /p:Platform=x64`

## Usage

Requires a salt.txt file in the Resources folder, its contents should be the salt used in the MD5 hash that generates the encryption key.

`Runes <Path to encrypted dump file>`