# Tanks Kombat
	
version 1.0

## Introduction

Tanks Kombat is cross-platform tile-based action game developed using Cocos2d-x and C++ 11, 
the game is made in retrogaming style with lo-res sprites and reminiscences of 8 and 16 bit games of past eras.

Currently supported Win32, Win10 (UWP) and Android platforms. Tanks Kombat is open source under the GPL

![Title](/Screenshots/a.png?raw=true)
![Gameplay](/Screenshots/b.png?raw=true)

## Downloading

Tanks Kombat binaries and packages for Android, Win32, Win10/Mobile you can download here:

https://github.com/EugenyN/TanksKombat/releases
or https://drive.google.com/folderview?id=0B67JMJKGzz6WT2d5SUdrU2VSX00&usp=sharing

## How do I play?

Tanks Kombat is a very simple action game in which the reaction speed is more important than solving puzzles or strategic planning. You can play against your computer or against another player in the hotseat mode.

Your tanks and other objects placed in the grid map. The tanks moved right one cell. The wall prevents your moving and bullets flying. You can fall into a hole or push your opponent into it. Bonuses add to the ammunition.

The winner is a tank that destroyed all opponents.

Immediately after damaging the tank can not shoot for a while and can not be damaged again. It's called "protection time".

There are several modifications of the game. "STANDARD" and "PROFI" slightly different armor, ammunition, and other parameters. In the "QUICK DEATH" there are a lot of armor and there is no protection time. In "FIRE AND IRON", you have only one shot, the bonus also gives only one extra shot, but the bonuses appear frequently.

Keyboard control:

* TANK 1 - Up Arrow, Down Arrow, Left Arrow, Right Arrow, Enter
* TANK 2 - W, S, A, D, Tab
* TANK 3 - I, K, J, L, O
* TANK 4 - Numpad 8, 2, 4, 6, Minus

## Code and Licensing

Tanks Kombat is open source under the GPL
Please see the file named [LICENSE](LICENSE).

To build Tanks Kombat, you will need to setup cocos2d-x environment. This version is built with cocos2d-x v3.17, which can be obtained from [Cocos2d-x offical website](http://cocos2d-x.org/filedown/cocos2d-x-3.17.zip).

## Credits

Programming, game design and some art:

* Eugeny Novikov

Third-party code and libraries:
* https://github.com/cocos2d/cocos2d-x
* https://github.com/cpinan/SneakyInput-Cocos2dx-2.0.x
* https://github.com/klmr/multifunction
* http://gamedevelopment.tutsplus.com/tutorials/generate-random-cave-levels-using-cellular-automata--gamedev-9664
* http://www.raywenderlich.com/4970/how-to-implement-a-pathfinding-with-cocos2d-tutorial
* http://www.nolithius.com/game-development/cocos2d-iphone-repeating-sprite

Music:
* Abrove Speed by Rob F. / EG / CONSCIOUSNESS
	
Graphics, fonts and sound effects:
* Some 8-bit, 16-bit retro games.
* DNS Design & Wildcop
* zironid_n http://opengameart.org/content/tanks
* M-Tee http://www.romhacking.net/forum/index.php?topic=16132.40
* Onscreen Controls by Kenney Vleugels www.kenney.nl

Tools:
* Visual Studio, Eclipse IDE, Paint.NET, IcoFX 2, Audactiy, TexturePacker
