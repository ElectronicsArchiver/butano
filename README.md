# Butano

Modern C++ high level GBA engine


## Features

* Create and display sprites, backgrounds, text, raster effects and more with only one line of C++ code.
* Custom standard library without heap allocations nor exceptions, based on [ETL](https://www.etlcpp.com/).
* Ìmport and use your own assets [with ease](https://gvaliente.github.io/butano/import.html).
* Multiple development tools like [asserts](https://gvaliente.github.io/butano/group__assert.html), 
[emulator logging](https://gvaliente.github.io/butano/group__log.html) and 
[code profiling](https://gvaliente.github.io/butano/group__profiler.html).
* Based on modern C++ concepts like shared ownership and RAII.
* Detailed [documentation](https://gvaliente.github.io/butano).
* Multiple [examples](https://gvaliente.github.io/butano/examples.html) of most aspects of the engine.
* The source code of a full game ([Butano Fighter](#butano-fighter)) is provided with this project.


## Supported platforms

Butano is built on top of the [devkitARM](https://devkitpro.org/) toolchain, so it supports Windows, 
macOS and Unix-like platforms.


## What's new

Curious about what was added or improved recently? 
Check out the [changelog page](https://gvaliente.github.io/butano/changelog.html) in the documentation.


## Getting started

The best way to get started is to read the 
[download, install and start using Butano guide](https://gvaliente.github.io/butano/getting_started.html).


## Third party libraries

Butano would have not been possible without these libraries:

* GBA hardware access and more provided by [Tonclib](https://www.coranac.com/projects/#tonc).
* Music and sound effects provided by [Maxmod](https://maxmod.devkitpro.org).
* Fast number to string conversion provided by [posprintf](http://danposluns.com/danposluns/gbadev/posprintf/index.html).
* Fast math routines provided by [gba-modern](https://github.com/JoaoBaptMG/gba-modern).


## Butano Fighter

This is Butano Fighter!

Stay alive while you shoot at everything that moves!

The full source code and the creative commons assets of this fun game come with Butano.

Get the latest Butano Fighter ROM from the [releases page](https://github.com/GValiente/butano/releases).

![](docs_tools/images/bf_title.gif)

![](docs_tools/images/bf_intro.gif)

![](docs_tools/images/bf_bomb.gif)


## Related projects

* [GBA-NICCC](https://github.com/GValiente/gba-niccc): Game Boy Advance port of the Atari ST demo STNICCC 2000.


## Contact and support

If you spotted a bug, need a feature or have an awesome idea,
the best you can do is to head over to the [issues page](https://github.com/GValiente/butano/issues) 
and to the [gbadev Discord server](https://discord.gg/2WS7bpJ).


## License

Butano is licensed under the zlib license, see the [LICENSE](LICENSE) file for details.
