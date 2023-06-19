#include <stdlib.h>
#include "file/Auth.h"

bool authenticateUser(const char* username, const char* password) {
    FILE* file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Failed to open user data file.\n");
        return false;
    }

    char buffer[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2]; // +2 for delimiter and null terminator

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        char storedUsername[MAX_USERNAME_LENGTH];
        char storedPassword[MAX_PASSWORD_LENGTH];
        sscanf(buffer, "%[^;];%s", storedUsername, storedPassword);

        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}

bool registerUser(const char* username, const char* password) {
    FILE* file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("Failed to open user data file.\n");
        return false;
    }

    fprintf(file, "%s;%s\n", username, password);
    fclose(file);

    return true;
}
void createAuthwin(SDL_Renderer* renderer, TTF_Font* font, const char* username, const char* password) {

    // Render username input field
    SDL_Rect usernameRect = { 100, 100, 200, 30 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &usernameRect);

    // Render username text
    char user_str[100];
    sprintf(user_str, "Username: %d", username);


    SDL_Color textColor = { 255, 255, 255 };
    SDL_Surface* usernameSurface = TTF_RenderText_Solid(font, user_str, textColor);
    SDL_Texture* usernameTexture = SDL_CreateTextureFromSurface(renderer, usernameSurface);
    SDL_Rect usernameTextRect = { 105, 105, usernameSurface->w, usernameSurface->h };
    SDL_RenderCopy(renderer, usernameTexture, NULL, &usernameTextRect);

    // Render password input field
    SDL_Rect passwordRect = { 100, 150, 200, 30 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &passwordRect);

    // Render password text
    SDL_Surface* passwordSurface = TTF_RenderText_Solid(font, password, textColor);
    SDL_Texture* passwordTexture = SDL_CreateTextureFromSurface(renderer, passwordSurface);
    SDL_Rect passwordTextRect = { 105, 155, passwordSurface->w, passwordSurface->h };
    SDL_RenderCopy(renderer, passwordTexture, NULL, &passwordTextRect);

    SDL_Surface* logsur = TTF_RenderText_Solid(font, "login page", textColor);
    SDL_Texture* logtextur = SDL_CreateTextureFromSurface(renderer, logsur);
    SDL_Rect logr = { 105, 205, logsur->w, logsur->h };
    SDL_RenderCopy(renderer, logtextur, NULL, &logr);


    // Cleanup resources    
    SDL_FreeSurface(logsur);
    SDL_DestroyTexture(logtextur);
    SDL_FreeSurface(passwordSurface);
    SDL_DestroyTexture(passwordTexture);
    SDL_FreeSurface(usernameSurface);
    SDL_DestroyTexture(usernameTexture);

}