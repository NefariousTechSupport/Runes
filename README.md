# Runes

A skylanders figure editor.

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
* run `cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QT_DIR%\6.9.0\msvc2022_64`
* run `msbuild Runes.sln /p:Configuration=Release /p:Platform=x64`

# Usage

Simply run Runes.exe

***I am not responsible for any corruption done to figure data, you are responsible for backing up your skylanders.***

Any figures released in Imaginators can be ***unrepairably damaged*** using this tool, **do not load them at all** until they are confirmed to work.
All other types of figures can be reset in order to restore functionality, albeit with loss of progression.

Portals are supported
- Xbox 360 Portals using WinUSB drivers
- Non-Xbox USB Portals using WinUSB/HidUsb drivers

Drivers can be installed using [Zadig](https://zadig.akeo.ie/). You may have to select "Options > List All Devices"

The following skylander types are supported:
- Cores
- Giants
- Trap Masters
- SuperChargers (Characters, not Vehicles)
- Sidekicks/Minis

The following are not supported (do not try loading these):
- Swap-Force Skylanders (Top and Bottom Halves)
- Traps
- Vehicles
- Magic Items
- Anything released for Imaginators

Backups of real figures are stored in `%AppData%\NefariousTechSupport\Runes\dumps`, these are overwritten as you save changes to your figure.
