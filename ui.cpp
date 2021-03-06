#include <ncurses.h>
#include <string.h>
#include <unistd.h>

#include "ui.h"
#include "pokemon.h"
#include "pokedex.h"
#include "player.h"

window_ initWindow(int sx, int sy, int posx, int posy){
	/* Inits a window_ with the corresponding sizes and position */
	window_ win;
	win.sx = sx;
	win.sy = sy;
	win.posx = posx;
	win.posy = posy;
	return win;
}

int menulist(window_ wmenu, char const ** choices, int menulength, int offsetx, int offsety, int wcl){
	/* Creates a menu based on the choices and menulength given 
	The menusize argument allows scrolling for too long menus
	*/

	// Clears the window if needed (default yes)
	if(wcl){
		wempty(wmenu);
		box(wmenu.w, ACS_VLINE, ACS_HLINE);
	}

	int menusize = min(wmenu.sx, menulength);

	int selection = 0;
	int shift = 0;
	int c = 0;

	while(c != 120){ //while true
		// prints all the choices, then prints the selected choice with reverse attribute
		for (int i = 0; i < menusize; i++){
			mvprintw(wmenu.posx+i, wmenu.posy, choices[shift+i]);
		}
		attron(A_REVERSE);
		mvprintw(wmenu.posx+selection-shift, wmenu.posy, choices[selection]);
		attroff(A_REVERSE);
		
		wrefresh(wmenu.w);
		// wrefresh(win);
				
		c = getch();
		switch (c){
			case 65 :	// up
				if (selection==shift){
					
				}
				
				shift += -(selection!=0)*(selection-1<shift)+(!selection)*(menulength-menusize);
				selection += -1+(!selection)*menulength;
				break;
			case 66 :	// down
				selection += 1;
				selection %= menulength;
				shift += (selection>shift+menusize-1)-shift*(selection==0);
				break;
			case 9 :
			case 10 :
			case 32 :
			case 67 :
				return selection;

			// Case to be ignored, if returns -1 the value should not be used as a choice, i.e. it allows to get out of the menu
			case 127 : // Backspace
			case 68 : // Left Arrow
				return -1;

			case 120 :		//DEBUG
				return 120;
			default:
				break;
		}

	}
	return 120;
}

int pokedex(window_ wpokedex, player_ p){
	/* Creates a pokedex based on the choices and menulength given 
	The menusize argument allows scrolling for too long menus
	*/

	int size = min(PKDXS, wpokedex.sx);
	int selection = 0;
	int shift = 0;
	int c = 0;

	while(true){
		// prints all the pokemons, then prints the selected pokemons with reverse attribute

		wempty(wpokedex);
		for (int i = 0; i < size; i++){
			msgbox(wpokedex, getPokeName(shift+i), i, 0, 0);
			if (p.pokedex.knownSpecies[shift+i]){
				msgbox(wpokedex, "*", i, wpokedex.sy-2, 0);
			}
		}

		// for (int i = 0; i < size; i++){
		// 	mvprintw(wmenu.posx+i, wmenu.posy, getPokeName(shift+i));
		// }
		attron(A_REVERSE);
		mvprintw(wpokedex.posx+selection-shift, wpokedex.posy, getPokeName(selection));
		attroff(A_REVERSE);
		
		wrefresh(wpokedex.w);
		// wrefresh(win);
				
		c = getch();
		switch (c){
			case 65 :	// up
				shift += -(selection!=0)*(selection-1<shift)+(!selection)*(PKDXS-size);
				selection += -1+(!selection)*PKDXS;
				break;
			case 66 :	// down
				selection += 1;
				selection %= PKDXS;
				shift += (selection>shift+size-1)-shift*(selection==0);
				break;

			// Currently unused, would be used to implement a more complete pokedex with all information about the selected pokemon
			/*
			case 9 :
			case 10 :
			case 32 :
				return selection;
			*/

			// Case to be ignored, if returns -1 the value should not be used as a choice, i.e. it allows to get out of the pokedex
			case 127 : // Backspace
			case 68 : // Left Arrow
				return -1;
			default:
				break;
		}

	}
	return 120;
}

int getReturn(){
	switch(getch()){
		case 127 : // Backspace
		case 68 : // Left Arrow
			return -1;
	};
	return 0;
}

int pokedexList(window_ wpokedex, player_ p){
	wempty(wpokedex);
	for (int i = 0; i < PKDXS; i++){
		msgbox(wpokedex, getPokeName(i), i, 0, 0);
		if (p.pokedex.knownSpecies[i]){
			msgbox(wpokedex, "*", i, 18, 0);
		}
	}
	return getReturn(); // Allows to read before quitting
}

int pokemonList(window_ wlist, player_ p){
	/*
	Creates a list with pokemons
	Differs from menu by not reversing a selected line
	*/
	char pv[8];
	pokemon_ poke;
	pokemon_ * team = p.team.pokemons;

	wempty(wlist);
	for (int i = 0; i < p.team.nbpkmn; i++){
		poke = team[i];
		msgbox(wlist, poke.name, i, 0, 0);
		sprintf(pv, "%3d/%-3d", poke.pv, poke.pvmax);
		msgbox(wlist, pv, i, 14, 0);
	}
	return getReturn(); // Allows to read before quitting
}

int bagList(window_ wbag, player_ p){
	char qty[4];
	item_ * items = p.bag;

	wempty(wbag);
	for (int i = 0; i < INVENTSIZE; i++){
		msgbox(wbag, items[i].name, i, 0, 0);
		sprintf(qty, "%2d", items[i].qty);
		msgbox(wbag, qty, i, 17, 0);
	}
	return getReturn(); // Allows to read before quitting
}

int pokemonMenu(window_ wmenu, player_ p){
	/* Creates a menu with pokemons */
	char const * pokenames[6];
	char pv[8];
	int choix=0;
	pokemon_ poke;

	wempty(wmenu);
	for (int i = 0; i < p.team.nbpkmn; i++){
		poke = p.team.pokemons[i];
		pokenames[i] = poke.name;
		sprintf(pv, "%3d/%-3d", poke.pv, poke.pvmax);
		msgbox(wmenu, pv, i, 22, 0);
	}

	do{
		choix = menulist(wmenu, pokenames, p.team.nbpkmn, 0, 0, 0);
	} while (choix!=-1 && p.team.pokemons[choix].pv==0); // If the user doesn't quit the menu, checks if the pokemon has non-zero life

	return choix;
}

int bagMenu(window_ wbag, player_ p){
	char qty[4];
	char const* items[INVENTSIZE];
	int choix = 0;

	wempty(wbag);
	for (int i = 0; i < INVENTSIZE; i++){
		items[i] = p.bag[i].name;
		sprintf(qty, "%2d", p.bag[i].qty);
		msgbox(wbag, qty, i, 22, 0);
		
	}
	
	do{
		choix = menulist(wbag, items, INVENTSIZE, 0, 0, 0);
	} while (choix != -1 && p.bag[choix].qty==0);
	
	return choix;
}

int pauseMenu(window_ wpause, player_ p){
	char const* pause[] = {"Pokedex", "Equipe", "Inventaire", "Quitter le jeu"};
	int menulen = 4;
	int choix;

	box(wpause.w, ACS_HLINE, ACS_VLINE);

	do{
		choix = menulist(wpause, pause, menulen);

		switch (choix)
		{
		case 0:
			// Afficher pokedex // menulist
			// pokedexList(wpause, p);
			pokedex(wpause, p);
			break;
		case 1:
			// Afficher equipe
			pokemonList(wpause, p);
			break;
		case 2:
			// Afficher inventaire
			bagList(wpause, p);
			break;
		case 3:
			// Quit
			wempty(wpause);
			return -1;
		default:
			break;
		}
	} while (choix!=-1);

	clear();
	return 0;
}

void wempty(window_ w, int boxed){
	/* Clears a window */
	werase(w.w);
	// wclear(w.w);
	// for (int i = w.posx; i < w.posx+w.sx; i++){
	// 	for (int j = w.posy; j < w.posy+w.sy; j++){
	// 		mvprintw(i, j, " ");
	// 	}
	// }
	if (boxed){
		box(w.w, ACS_VLINE, ACS_HLINE);
	}
}
	
void msgbox(window_ wmsgbox, char const* msg, int offsetx, int offsety, int wcl){
	/* Prints a message in a box, clears it if needed */
	if (wcl){
		wempty(wmsgbox);
	}
	mvprintw(wmsgbox.posx+offsetx, wmsgbox.posy+offsety, msg);
	wrefresh(wmsgbox.w);
}

void pkmnInfoDisplay(window_ wfight, int posx, int posy, pokemon_ poke){
	/* Shows a pokemon info at given position. Shows name and pv stats with a healthbar
	Positions are relative to the parent window
	*/

	posx += wfight.posx;
	posy += wfight.posy;

	char hp[21] = "                    ";
	float hbar;
	float hbarmax = 19;
	hbar = (int)((float)(poke.pv)/poke.pvmax*hbarmax);
	for (int i = 0; i < hbar; i++){
		hp[i] = '=';
	}

	attron(A_BOLD);
	mvprintw(posx, posy, "%-25s", poke.name);
	attroff(A_BOLD);
	mvprintw(posx+1, posy, "PV : %3d/%-3d", poke.pv, poke.pvmax);
	mvprintw(posx+2, posy, "   : %s", hp);
	wrefresh(wfight.w);
}



void splashscreen(window_ win){

	// Index, FG, BG
	init_pair(1, COLOR_RED, COLOR_RED);
	init_pair(2, COLOR_WHITE, COLOR_WHITE);

	//create the pokeball

	window_ pokeball;
	pokeball.sx=15;
	pokeball.sy=37;
	pokeball.posx=LINES/2-2*pokeball.sx/3;
	pokeball.posy=COLS/2-pokeball.sy/2;
	pokeball.w = derwin(win.w, pokeball.sx, pokeball.sy , pokeball.posx, pokeball.posy);
	box(pokeball.w, ACS_VLINE, ACS_HLINE);

	int offx=0;
	int color;
	int artwidth = 37;
	char art[] ="             @@@@@@@@@@@             .         @@@@@@$$$$$$$@@@@@@         .     @@@@@$$$$$$$$$$$$$$$$$@@@@@     .   @@@@$$$$$$$$$$$$$$$$$$$$$$$@@@@   .  @@@$$$$$$$$$$$$$$$$$$$$$$$$$$$@@@  . @@@$$$$$$$$$$$@@@@@@@$$$$$$$$$$$@@@ . @@@$$$$$$$$$@@@     @@@$$$$$$$$$@@@ . @@@@@@@@@@@@@@       @@@@@@@@@@@@@@ . @@@         @@@     @@@         @@@ . @@@           @@@@@@@           @@@ .  @@@                           @@@  .   @@@@                       @@@@   .     @@@@@                 @@@@@     .        @@@@@@         @@@@@@        .            @@@@@@@@@@@@@            ";
	for (int i = 0; i < (int)strlen(art); i++){
		if (art[i]=='$'){
			color = 1;
		} else if (art[i]=='@'){
			color = 2;
		}
		if (art[i]=='.'){
			offx += 1;
		} else if(art[i]!=' '){
			attron(COLOR_PAIR(color));
			mvprintw(pokeball.posx+offx, pokeball.posy+i%(artwidth+1), "%c", art[i]);
			attroff(COLOR_PAIR(color));
		}
	}

	// create the welcome message

	window_ welcome;
	welcome.sx=1;
	welcome.sy=38;
	welcome.posx=pokeball.posx+pokeball.sx+2;
	welcome.posy=COLS/2-welcome.sy/2;
	welcome.w = derwin(win.w, welcome.sx+2, welcome.sy+2, welcome.posx-1, welcome.posy-1);
	box(welcome.w, ACS_VLINE, ACS_HLINE);

	char msg [60];
	sprintf(msg, "Bienvenue dans le monde des pokemons !");
	msgbox(welcome, msg, 0, 1);

	//create the "play" button

	button(win, (char*)"PLAY", welcome.posx+welcome.sx+2, COLS/2-1);
	
}

void greetScreen(window_ w, char* name){
	msgbox(w, "Bonjour, je suis le Professeur Okitac.", LINES/2, COLS/2-25);
	msgbox(w, "Je ne crois pas te connaitre, comment t'appelles-tu ?", LINES/2+1, COLS/2-25, 0);
	curs_set(1);
	echo();
	mvgetnstr(LINES/2+3, COLS/2-25, name, 14);
	noecho();
	curs_set(0);

	int txtposx = LINES/2-5;
	msgbox(w, "Bienvenue dans le monde des Pokemon !", txtposx, COLS/2-25);
	sleep(2);
	msgbox(w, "Tu devras m'aider a remplir le Pokedex local,", txtposx+2, COLS/2-25, 0);
	msgbox(w, "voici un Pikachu qui deviendra ton meilleur compagnon.", txtposx+3, COLS/2-25, 0);
	sleep(4);
	msgbox(w, "Tu peux consulter le Pokedex en appuyant sur P ou Espace.", txtposx+5, COLS/2-25, 0);
	sleep(2);
	msgbox(w, "Il est maintenant l'heure pour toi de", txtposx+7, COLS/2-25, 0);
	msgbox(w, "tous les capturer !", txtposx+8, COLS/2-25, 0);
	sleep(1);
	button(w, (char*)"C'est parti !", txtposx+11, COLS/2-6);

	wempty(w);
}

void button(window_ w, char* text, int posx, int posy, int sx, int sy){
	window_ button; 
	button.sx=sx;
	if (!sy){
		button.sy=strlen(text);
	}
	button.posx=posx;
	button.posy=posy;
	button.w= derwin(w.w, button.sx+2, button.sy+2, button.posx-1, button.posy-1);
	box(button.w, ACS_VLINE, ACS_HLINE);

	attron(A_REVERSE);
	attron(A_BOLD);
	msgbox(button, text);
	attroff(A_REVERSE);
	attroff(A_BOLD);
	getch();
}