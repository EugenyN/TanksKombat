# Tanks Kombat

Tanks Kombat is cross-platform tile-based action game developed using Cocos2d-x framework and C++, 
the game is made in retrogaming style with lo-res sprites and reminiscences of 8 and 16 bit games of past eras.

Currently supported Win32, UWP (Win10) and Android platforms. Tanks Kombat is open source under the GPL

![Title](/Screenshots/a.png)
![Gameplay](/Screenshots/b.png)

## Description and rules

Tanks Kombat is a very simple action game in which the reaction speed is more important than solving puzzles or strategic planning. You can play against your computer or another player in the local multiplayer mode.

Your tanks and other objects are placed in the grid map. The tanks move right one cell. The wall prevents your moving and bullets flying. You can fall into a hole or push your opponent into it. Bonuses refill the ammunition.

The winner is a tank that will destroy all opponents.

Immediately after damaging the tank can not shoot for a while and can not be damaged again. It's called "protection time".

There are several modifications of the game. "STANDARD" and "PROFI" slightly change armor, ammunition, and other parameters. In the "QUICK DEATH" there are a lot of armor and there is no protection time. In "FIRE AND IRON" you have only one shot, the bonus also gives only one extra shot, but the bonuses appear frequently.

Keyboard control:

* TANK 1 - Up Arrow, Down Arrow, Left Arrow, Right Arrow, Enter
* TANK 2 - W, S, A, D, Tab
* TANK 3 - I, K, J, L, O
* TANK 4 - Numpad 8, 2, 4, 6, Minus

## Download

You can download Tanks Kombat Win32 binaries and packages for Android, UWP (Win10) in [Release page](https://github.com/EugenyN/TanksKombat/releases).

## Build from source

* Download [latest](https://cocos2d-x.org/download) cocos2d-x version. (tested on 3.17.1)
* Copy all cocos2d-x files to `/cocos2d` folder except existing patched files.
* We dont use CMake, and all project files are already generated. Open project for a required platform in Visual Studio / Eclipse, build and run. The required platforms SDK (UWP workload, Android SDK) must be installed.

## Credits

* Programming, game design and some art by Eugeny Novikov
* Music: "Abrove Speed" by Rob F. / EG / CONSCIOUSNESS
* Sprites, BG tiles, Bitmap font: DNS Design & Wildcop, [zironid_n](http://opengameart.org/content/tanks), [M-Tee](http://www.romhacking.net/forum/index.php?topic=16132.40), some 8-bit, 16-bit retro games
* Onscreen Controls by [Kenney Vleugels](https://www.kenney.nl/)

## Third-party code and libraries

* [Cocos2d-x](https://github.com/cocos2d/cocos2d-x)
* [SneakyInput-Cocos2dx-2.0.x](https://github.com/cpinan/SneakyInput-Cocos2dx-2.0.x)
* [Multicast delegate for C++](https://github.com/klmr/multifunction)
* [Random Cave Using Cellular Automata](http://gamedevelopment.tutsplus.com/tutorials/generate-random-cave-levels-using-cellular-automata--gamedev-9664)
* [A* Pathfinding with Cocos2D](http://www.raywenderlich.com/4970/how-to-implement-a-pathfinding-with-cocos2d-tutorial) (Objective-C)
* [Cocos2d Repeating Sprite example](https://gist.github.com/Nolithius/6694990) (Objective-C)

## License

Tanks Kombat is open source under the GPL, please see the file named [LICENSE](LICENSE).