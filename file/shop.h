
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
    float multiplier;
    int level;
    bool isNewbie;
} Data;
bool updateUserData(const char *username, const Data *userData);
const char* getSuffix(int exponent);
void drawshop(SDL_Renderer* renderer,TTF_Font* font ,Data* userdata);
#endif 
