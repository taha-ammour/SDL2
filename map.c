#include "file/Map.h"
#include <stdlib.h>

Map* createMap(SDL_Texture* tileset, int tileWidth, int tileHeight, int numColumns, int numRows, int** tiles) {
    Map* map = malloc(sizeof(Map));
    if (!map) {
        return NULL;
    }

    map->tileset = tileset;
    map->tileWidth = tileWidth;
    map->tileHeight = tileHeight;
    map->numColumns = numColumns;
    map->numRows = numRows;
    map->tiles = tiles;

    return map;
}

void destroyMap(Map* map) {
    for (int i = 0; i < map->numRows; i++) {
        free(map->tiles[i]);
    }
    free(map->tiles);
    free(map);
}

void renderMap(SDL_Renderer* renderer, Map* map) {
    int x, y;
    int* tile;
    SDL_Rect srcRect, destRect;
    destRect.w = map->tileWidth;
    destRect.h = map->tileHeight;

    for (int row = 0; row < map->numRows; row++) {
        for (int col = 0; col < map->numColumns; col++) {
            x = col * map->tileWidth;
            y = row * map->tileHeight;
            tile = &(map->tiles[row][col]);

            srcRect.x = (*tile % 8) * map->tileWidth;
            srcRect.y = (*tile / 8) * map->tileHeight;
            srcRect.w = map->tileWidth;
            srcRect.h = map->tileHeight;

            destRect.x = x;
            destRect.y = y;

            SDL_RenderCopy(renderer, map->tileset, &srcRect, &destRect);
        }
    }
}
