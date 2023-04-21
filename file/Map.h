// Map.h

#pragma once
#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>

typedef struct {
    SDL_Texture* tileset;
    int tileWidth;
    int tileHeight;
    int numColumns;
    int numRows;
    int** tiles;
} Map;

Map* createMap(SDL_Texture* tileset, int tileWidth, int tileHeight, int numColumns, int numRows, int** tiles);

void destroyMap(Map* map);

void renderMap(SDL_Renderer* renderer, Map* map);

#endif // MAP_H
