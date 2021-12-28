#include <iostream>
#include <ncurses.h>

#include "map.h"
#include "player.h"
#include "pokemon.h"
#include "fight.h"

int main(){

	// Creates pointers to place windows of ncurses in
	WINDOW *win;

	// inits the screen with the whole term as a window
	win = initscr();

	// First refresh allows curs_set() to work
	refresh();
	noecho();
	curs_set(0);

	// Rand seed init
	srand(time(NULL));

	// Init of the sides of the map, then the map itself is initialised and zeroed
	int mapxsize = 3*LINES/4;
	int mapysize = 3*COLS/4;
	map_ b = initMap(mapxsize, mapysize);
	
	// Creates a player and puts it in the map
	player_ p = initPlayer("Philippe", &b, b.height/2, b.width/2);
	addPokeTeam(&p, initPokemon(PIKACHU));

	// First refresh of the map to see it when starting
	refreshMap(win, b);

	// Will receive the code returned by getch();
	int c = 0;

	// encounter ?
	int e;

	// Beginning of the game, stop when all species have been encountered, or when Backspace is hit
	while (!pokedexFull(p.pokedex) && c!=127){
		c = getch();
		
		// 410 is the "screen resize" code
		if (c==410){
			clear();
		}

		// Move the player if c is the code of an arrow
		movePlayer(&p, &b, c);
		
		e = rand()%20;
		if (!e){
				
			fight(win, &p);
			
			clear();
		}
		refreshMap(win, b);
		
	}

	// Closes ncurses
	endwin();
}