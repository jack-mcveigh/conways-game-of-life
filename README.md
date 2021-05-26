# Conway's Game of Life implementation
## Author: Jack McVeigh
### Email: <jmcveigh55@gmail.com>
### License: Unlicense
---
---
## Purpose
	* Implement John Conway's famous "Game of Life" simulation.
	* Explore SDL2.

## To Do
	* Add different modes:
		1. Random (Default)
		2. Patterns - Known groups like Gliders.
		3. Drawing - Allow user to select cells with mouse click/drag.

		- Currently only random is implemented with the below rules:
			1. Middle region of the grid. (25% of the board centered in the middle.)
			2. Each cell in the region has a probability that it will spawn in the alive state. The default is 25% but can be changed using the "-p" flag.

---

## Compile
	* Make sure to have SDL2 and SDL2_ttf installed. If not, via homebrew try:
`brew install SDL2 SDL2_ttf`
	* Complile the program, in the base directory run:
`make`

## Usage
	* Print the Usage statement:
`./game_of_life -h`

	* Run with default settings (100x100 grid of 8x8 cells):
`./game_of_life`

	* Run with custom cell size settings (NxN grid of DxD cells. Note N * D must equal 800.)
`./game_of_life -n N -d D`

	* Run with custom cell alive probability (P%)
`./game_of_life -p P`

	* Set cell color (0xRRGGBB)
`./game_of_life -c RRGGBB`

	* Set background color (0xRRGGBB)
`./game_of_life -b RRGGBB`
