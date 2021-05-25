# Conway's Game of Life implementation
## Author: Jack McVeigh <jmm826@drexel.edu>
---
---
## Purpose
	* Implement John Horton Conway's famous "Game of Life" simulation.
	* Explore SDL2.

## To Do
	* Add paterns. Currently just creates random cells following the below rules:
		1. Middle region of the grid. (50% of the board centered in the middle.)
		2. Each cell in the region has a 1/5th chance of being alive to start.
	* Allow user to adjust initial spawn probability with "-p" flag.

---

## Compile
	* Complile the program. Make sure to have SDL2 and SDL2_ttf installed. In the base directory:
`make`

## Usage
	* Print the Usage statement:
`./game_of_life -h`

	* Run with default settings (100x100 grid of 8x8 cells):
`./game_of_life`

	* Run with custom settings (NxN grid of DxD cells. Note N * D must equal 800.)
`./game_of_life -n N -d D`
