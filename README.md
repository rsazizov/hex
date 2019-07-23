# Hex

![Hex](https://github.com/rsazizov/hex/raw/master/screenshot.png "Hex")

[Hex](https://en.wikipedia.org/wiki/Hex_(board_game)) is a connection game played on an 11x11 grid. My implmentation features ncurses based UI and a multiplayer.

## Requirements

* gcc
* Make
* ncurses

On Ubuntu, ncurses can be install with the following command:

```
sudo apt install libncurses5-dev
```

A proper unicode font that supports "⬡"(U+2B21) and "⬢"(U+2B22) symbols is also required (Ubuntu Mono is fine).

## Building

```
make
./hex
```
