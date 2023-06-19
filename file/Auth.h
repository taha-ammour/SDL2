
#pragma once
#ifndef AUTH_H
#define AUTH_H

#include <SDL2/SDL.h>
#include <SDl2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define MENU_AUTH 7
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} User;


bool authenticateUser(const char* username, const char* password);
bool registerUser(const char* username, const char* password);

void createAuthwin(SDL_Renderer* renderer, TTF_Font* font, const char* username, const char* password);

#endif 
