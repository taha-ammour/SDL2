
#pragma once
#ifndef SHOP_H
#define SHOP_H

#include <SDL2/SDL.h>
#include <SDl2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <gmp.h>

typedef struct {
    mpz_t score;
    size_t multiplier;
    int level;
    int diffadd;
    bool isNewbie;
} Data;
extern TTF_Font* font18;
#define BUFFER_SIZE 1024

bool updateUserData(const char *username, const Data *userData);
char* formatNumberWithSuffix(mpz_t number);
void drawshop(SDL_Renderer* renderer,TTF_Font* font ,Data* userdata, bool isUnlocked[]);


#endif 
