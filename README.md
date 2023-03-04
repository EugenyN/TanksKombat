# Tanks Kombat

Tanks Kombat is a cross-platform tile-based action game developed using Axmol Engine (fork of Cocos2d-x) and C++, 
the game is made in retro style with lo-res sprites and reminiscences of 8 and 16 bit games of past eras.

released for Windows and Android platforms.

![scr1](/Screenshots/scr1.png) ![scr2](/Screenshots/scr2.png)
![scr3](/Screenshots/scr3.png) ![scr4](/Screenshots/scr4.png)


## Description and rules

Tanks Kombat is a very simple action game where the reaction speed is more important than solving puzzles or strategic planning. You can play against your computer or another player in the local multiplayer mode.

Your tanks and other objects are placed in the grid map. The tanks move right one cell. The wall prevents your moving and bullets flying. You can fall into a hole or push your opponent into it. Bonuses refill the ammunition.

The winner is a tank that destroys all opponents.

Immediately after damaging the tank can not shoot for a while and can not be damaged again. It's called "protection time".

There are several modifications of the game. "STANDARD" and "PROFI" slightly change armor, ammunition, and other parameters. In the "QUICK DEATH" there are a lot of armor and there is no protection time. In "FIRE AND IRON" you have only one shot, the bonus also gives only one extra shot, but the bonuses appear frequently.

#### Keyboard control

* TANK 1 - Up Arrow, Down Arrow, Left Arrow, Right Arrow, Enter
* TANK 2 - W, S, A, D, Tab
* TANK 3 - I, K, J, L, O
* TANK 4 - Numpad 8, 2, 4, 6, Minus

## Download

You can download Tanks Kombat binaries for Windows and Android in [Release page](https://github.com/EugenyN/TanksKombat/releases).

Also the game is available on Itch.io: https://eugenyn.itch.io/tanks-kombat

## Build from source

#### Prerequisites

1. Download [Axmol](https://github.com/axmolengine/axmol) game engine.
2. Config ```axmol```, run ```python setup.py``` from axmol root directory.

#### Windows

1. Install Visual Studio 2019/2022 (for toolchain)
2. Install [CMake](https://cmake.org/) 3.14+  
3. Use CMake to build project files:
   * for 32 bit Visual Studio 2022: ```cmake -S . -B build -G "Visual Studio 17 2022" -A Win32```
   * for 64 bit Visual Studio 2022: ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64```
4. Build and run project.
	
Also you can use built-in CMake projects support in your IDE (use Visual Studio as toolchain)

#### Android

1. Install Android Studio
2. Install SDK Build-Tools, NDK r23c+, CMake 3.10+ from Android Studio ```SDK Manager```
3. Open ```proj.android``` in Android Studio, wait for ```Gradle sync``` finish.
4. Build and run project.

## Credits

* Programming, game design and some art by Eugeny Novikov
* Music: "Abrove Speed" by Rob F. / EG / CONSCIOUSNESS
* Sprites, BG tiles, Bitmap font: DNS Design & Wildcop, [zironid_n](http://opengameart.org/content/tanks), [M-Tee](http://www.romhacking.net/forum/index.php?topic=16132.40), some 8-bit, 16-bit retro games
* Onscreen Controls by [Kenney Vleugels](https://www.kenney.nl/)

## Third-party code and libraries

* [Axmol Game Engine](https://github.com/axmolengine/axmol) fork of [Cocos2d-x-4.0](https://github.com/cocos2d/cocos2d-x)
* [SneakyInput-Cocos2dx-2.0.x](https://github.com/cpinan/SneakyInput-Cocos2dx-2.0.x)
* [Random Cave Using Cellular Automata](http://gamedevelopment.tutsplus.com/tutorials/generate-random-cave-levels-using-cellular-automata--gamedev-9664)
* [A* Pathfinding with Cocos2D](http://www.raywenderlich.com/4970/how-to-implement-a-pathfinding-with-cocos2d-tutorial) (Objective-C)
* [Cocos2d Repeating Sprite example](https://gist.github.com/Nolithius/6694990) (Objective-C)

## License

Copyright (c) 2016 Eugeny Novikov. The source code is under the MIT license.