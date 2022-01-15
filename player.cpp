#include <iostream>
#include <cstring>

#include "player.h"
#include "pokemon.h"
#include "pokedex.h"
#include "map.h"

using namespace std;

player initPlayer(char* name, map_* b, int x, int y){
	player p;
	strcpy(p.name, name);
	// p.name = name;
	p.pos.x = x;
	p.pos.y = y;
	p.pokeballs = 15;
	p.potions = 15;

	p.pokedex = initPokedex();
	p.team = initTeam();

	setTab(JOUEUR, b, p.pos.x, p.pos.y);

	return p;
}

team_ initTeam(){
	team_ t;
	t.sizemax = 6;
	t.pokemons = (pokemon_*)malloc(t.sizemax*sizeof(pokemon_));
	t.nbpkmn = 0;
	return t;
}

void delTeam(team_ pteam){
	free(pteam.pokemons);
}

int addPokeTeam(player* p, pokemon_ poke){
	if (p->team.sizemax == p->team.nbpkmn){
		p->team.sizemax *= 2;
		p->team.pokemons = (pokemon_*)realloc(p->team.pokemons, p->team.sizemax*sizeof(pokemon_));
		return 1;
	} else {

		// if (p->team.pokemons == NULL){
		// 	return -1;
		// }
		// if (!isKnown(poke.s, p->pokedex)){
		// 	learn(poke.s, &(p->pokedex));
		// }
		p->team.pokemons[p->team.nbpkmn] = poke;
		p->team.nbpkmn += 1;
	}
	return 0;
}

int getFirstAliveIndex(player p){
	for (int i = 0; i < p.team.nbpkmn; i++){
		if (p.team.pokemons[i].pv > 0){
			return i;
		}
	}
	return -1;
}

void movePlayer(player* p, map_* b, char direction){
	int xmv = 0;
	int ymv = 0;
	int c;

	if (64 < direction && direction < 69){
		setTab(0, b, p->pos.x, p->pos.y);

		if (direction == 65 && p->pos.x > 0) {
			xmv = -1;
		} else if (direction == 66 && p->pos.x < b->height-1){
			xmv = 1;
		} else if (direction == 67 && p->pos.y < b->width-1){
			ymv = 1;
		} else if (direction == 68 && p->pos.y > 0){
			ymv = -1;
		}

		p->pos.x += xmv;
		p->pos.y += ymv;

		setTab(JOUEUR, b, p->pos.x, p->pos.y);
	}
}