# adlib2vgm [![Actions Status](https://github.com/SudoMaker/adlib2vgm/workflows/Build/badge.svg)](https://github.com/SudoMaker/adlib2vgm/actions/workflows/push_pr_build_cmake.yml) [![Actions Status](https://github.com/SudoMaker/adlib2vgm/workflows/Release/badge.svg)](https://github.com/SudoMaker/adlib2vgm/actions/workflows/release_cmake.yml)

Converts file formats supported by AdPlug to VGM format.

## Notice
This tool is originally designed to work with the [RetroWave OPL3 Sound Card](https://www.tindie.com/products/sudomaker/retrowave-opl3-sound-card/).

Currently it doesn't produce files that's 100% compatible with the VGMPlay software.

## Why?
### Format
VGM is the next universal format for various vintage sound chips. It's ~~well~~ documented and extensible.

If one wants to build a player that uses/emulates these vintage sound chips, using VGM format will make things much easier.

### Programming Practises
It's almost impossible to directly integrate AdPlug into a modern C++ project that uses CMake & self-contained dependency management.

## Binary releases
See the [releases](https://github.com/SudoMaker/adlib2vgm/releases) section.

Currently built targets:
- Linux x86_64 (glibc 2.28+)
- Windows x86_64 (Windows 7+)


## Build requirements
- C++17 compatible compiler
- CMake 3.14+
- POSIX compatible environment

Windows users please use WSL or Cygwin, or maybe try Linux a bit ;-)

## Supported formats
- CMF: Creative Music File Format by Creative Technology
- RAD: Reality ADlib Tracker by Reality
- D00: EdLib by Vibrants
- DRO: DOSBox Raw OPL Format
- RIX: Softstar RIX OPL Music Format
- MID: MIDI Audio File Format
- MUS: AdLib MIDI Music Format by Ad Lib Inc.

Other formats supported by AdPlug can be added by copying the decoding code pieces to `Formats` directory + some minor modifications.

## Usage examples
```shell script
adlib2vgm -i ~/Downloads/ALLOYRUN.RAD -o ALLOYRUN.vgm
```

```shell script
adlib2vgm.exe -i D:\Downloads\ALLOYRUN.RAD -o ALLOYRUN.vgm
```

```shell script
adlib2vgm -i ~/Vibrants/Metal/Pop_Syndrom.d00 -o Pop_Syndrom.vgm
```

```shell script
adlib2vgm -m opl3 -i ~/Downloads/ALLOYRUN.RAD -o ALLOYRUN_OPL3.vgm
```

```shell script
adlib2vgm -f DRO2 -i some_dro2_file.dro -o foo.vgm
```

```shell script
adlib2vgm -i ./pal1/30.乐逍遥.RIX -o 30.乐逍遥.vgm
```

## TODO
- Better file format detection
- Loops
- Metadata

Contributions are welcomed!

## Notes
It's important to preserve the original files because most of them contains additional valuable information not representable by the VGM format.

## License
AGPLv3

If you see a possible license violation, don't hesitate to tell us.

## Acknowledgements
This project makes use of the following open source software:
- [AdPlug](https://github.com/adplug/adplug)
- [cxxopts](https://github.com/jarro2783/cxxopts)
