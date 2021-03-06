#include <iostream>
#include <ncurses.h>
#include <unistd.h>

#include <mcheck.h>

#include "map.h"
#include "player.h"
#include "pokemon.h"
#include "pokedex.h"
#include "fight.h"
#include "ui.h"

int main(){

	mtrace();

	// Creates pointers to place windows of ncurses in
	window_ wgame;// = initWindow(LINES, COLS, 0, 0);
	wgame.w = initscr();
	wgame.sx = LINES;
	wgame.sy = COLS;
	wgame.posx = 0;
	wgame.posy = 0;
	
	window_ wpause = initWindow(4, 20, LINES/2-2, COLS/2-10);

	// inits the screen with the whole term as a window
	wpause.w = derwin(wgame.w, wpause.sx+2, wpause.sy+2, wpause.posx-1, wpause.posy-1);

	// First refresh allows curs_set() to work
	refresh();
	noecho();
	curs_set(0);
	start_color();
	use_default_colors();

	// Rand seed init
	srand(time(NULL));

	// Init of the sides of the map, then the map itself is initialised and zeroed
	int mapxsize = 9*LINES/10;
	int mapysize = 9*COLS/10;
	map_ b = initMap(mapxsize, mapysize);

	// Splash Screen
	splashscreen(wgame);
	
	// Greets player and gets his name
	char nom[15];
	greetScreen(wgame, nom);
	
	// Creates a player and puts it in the map
	player_ p = initPlayer(nom, &b, b.height/2, b.width/2);

	// Adds the first pokemon to the player's team
	addPokeTeam(&p, initPokemon(PIKACHU));

	wempty(wgame, 0);

	// First refresh of the map to see it when starting
	refreshMap(wgame.w, b);

	// Will receive the code returned by getch();
	int c = 0;

	// Stores the kind of area the player is in
	int area;
	// encounter ?
	int e;
	// encounter probability, 1 over ...
	int ep = 50;
	// Probability in Grass Modifier
	int pgm = ep/2; // Currently grass is unused

	pokemon_ wild;

	// Allows the player to quit the game via the menu
	int end = 0;

	// Beginning of the game, stops when all species have been encountered, or when X is hit
	while (!end && !pokedexFull(p.pokedex) && getFirstAliveIndex(p)!=-1){
		c = getch();
		
		// 410 is the "screen resize" code
		if (c==410){
			clear();
		}

		// Moves the player if c is the code of an arrow
		area = movePlayer(&p, &b, c);
		
		refreshMap(wgame.w, b);
		
		if (c==32 || c==112 || c==115){
			if (pauseMenu(wpause, p) == -1){
				end = -1;
			};
			refreshMap(wgame.w, b);
			//Menu de pause
		} else {
			// Approx. one chance in ep of triggering an encounter
			e = rand()%(ep-area*pgm);
			if (!e){
				do{
					wild = initPokemon((species)(rand()%PKDXS));
				} while (wild.legend==1 && p.pokedex.knownSpecies[(int)(wild.s)]);
				
				fight(wgame, &p, &wild);
				clear();
				refreshMap(wgame.w, b);
			}
		}
	}

	wempty(wgame);

	if (pokedexFull(p.pokedex)){
		msgbox(wgame, "Felicitations, vous avez complete le pokedex !", LINES/2, COLS/2-23);
		//Fin Reussie
	}

	if (getFirstAliveIndex(p)==-1){
		msgbox(wgame, "Vous avez ete battu !", LINES/2-1, COLS/2-11);
		msgbox(wgame, "Game Over", LINES/2+1, COLS/2-5, 0);
	}

	if (end){
		msgbox(wgame, "Revenez bientot !", LINES/2-1, COLS/2-9, 0);
		msgbox(wgame, "Game Over", LINES/2+1, COLS/2-5, 0);
	}

	sleep(3);

	msgbox(wgame, "Jeu programme par Lucie et Philippe", LINES/2, COLS/2-18);

	getch();

	// Free allocated memory
	delMap(b);
	delPlayer(p);

	// Closes ncurses
	endwin();

	muntrace();

	return 0;
	
}