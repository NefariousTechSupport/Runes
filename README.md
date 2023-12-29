# Runes

A skylanders figure editor.

It is not yet ready for public use.

# Building

## Windows

Requires MSYS2.

Dependencies: Install Qt and run the following `pacman -S mingw-w64-x86_64-yaml-cpp`

Compiling: Run `make` to compile

## Usage

Requires a salt.txt file in the Resources folder, its contents should be the salt used in the MD5 hash that generates the encryption key.

`Runes <Path to encrypted dump file>`