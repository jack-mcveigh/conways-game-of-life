# Conway's Game of Life implementation
## Author: Jack McVeigh
### Email: <jmcveigh55@gmail.com>
### License: Unlicense

---

## Purpose
	* Implement John Conway's famous "Game of Life" simulation.
	* Explore SDL2.

## To Do
	* Add menu for selecting run mode.

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

	* Stepper mode, pause after each frame:
`./game_of_life -s`

	* Turn the grid on:
`./game_of_life -g`

	* Run with custom cell size settings (NxN grid of DxD cells. Note N * D must equal 800.)
`./game_of_life -n N -d D`

	* Run with custom cell alive probability (P%)
`./game_of_life -p P`

	* Set cell color (0xRRGGBB)
`./game_of_life -c RRGGBB`

	* Set background color (0xRRGGBB)
`./game_of_life -b RRGGBB`

	* Select mode. (r: random, p: pattern, d: drawing):

`./game_of_life -m r`

`./game_of_life -m p`

`./game_of_life -m d`

---

## Controls
	In Drawing Mode:
	* SPACE : Continue to simulation.
	* q : Quit and Exit.
	* LEFT CLICK : Select cell.

	During Simulation:
	* SPACE : Pause/Unpause.
	* q : Quit and Exit.
	* UP ARROW : Speed up.
	* DOWN ARROW : Slow down.
	* e : Export current state as csv.
