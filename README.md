# Conway's Game of Life implementation
## Author: Jack McVeigh <jmm826@drexel.edu>
## License: Unlicense
---
---
## Purpose
	* Implement John Conway's famous "Game of Life" simulation.
	* Explore SDL2.

## To Do
	* Add paterns. Currently just creates random cells following the below rules:
		1. Middle region of the grid. (50% of the board centered in the middle.)
		2. Each cell in the region has a 1/5th chance of being alive to start.

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
