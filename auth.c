#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "file/Auth.h"
#include "file/shop.h"

bool authenticateUser(const char *username, const char *password, Data *storedData)
{
    FILE *file = fopen("user.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open user data file.\n");
        return false;
    }

    char buffer[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + sizeof(Data) + 3];

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char storedUsername[MAX_USERNAME_LENGTH];
        char storedPassword[MAX_PASSWORD_LENGTH];
        int storedMultiplier;
        int storedLevel;
        int storedIsNewbie;
        char* storedScore = NULL;

        sscanf(buffer, "%[^;];%[^;];%lld;%d;%d;%ms", storedUsername, storedPassword, &storedMultiplier, &storedLevel, &storedIsNewbie, &storedScore);

        if (strcmp(username, storedUsername) == 0 && strcmp(password, storedPassword) == 0)
        {
            storedData->multiplier = storedMultiplier;
            storedData->level = storedLevel;
            storedData->isNewbie = storedIsNewbie;
            mpz_init_set_str(storedData->score, storedScore, 10);
            free(storedScore);

            fclose(file);
            return true;
        }
        free(storedScore);
    }

    fclose(file);
    return false;
}

bool registerUser(const char *username, const char *password, Data *userData)
{
    // Check if username or password is empty or contains spaces
    if (username == NULL || password == NULL || username[0] == '\0' || password[0] == '\0')
    {
        printf("Invalid username or password.\n");
        return false;
    }

    // Check if username or password contains spaces
    for (int i = 0; username[i] != '\0'; i++)
    {
        if (username[i] == ' ')
        {
            printf("Username cannot contain spaces.\n");
            return false;
        }
    }

    for (int i = 0; password[i] != '\0'; i++)
    {
        if (password[i] == ' ')
        {
            printf("Password cannot contain spaces.\n");
            return false;
        }
    }

    // Open the file in read mode to check if the user already exists
    FILE *file = fopen("user.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open user data file.\n");
        return false;
    }

    char buffer[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + 2]; // +2 for delimiter and null terminator

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char storedUsername[MAX_USERNAME_LENGTH];
        sscanf(buffer, "%[^;];", storedUsername);

        if (strcmp(username, storedUsername) == 0)
        {
            fclose(file);
            printf("Username already exists.\n");
            return false;
        }
    }

    fclose(file);

    // Open the file in append mode to register the user
    file = fopen("user.txt", "a");
    if (file == NULL)
    {
        printf("Failed to open user data file.\n");
        return false;
    }

    char *scoreStr;
    gmp_asprintf(&scoreStr, "%Zd", userData->score);

    userData->multiplier = 1;
    fprintf(file, "%s;%s;%lld;%d;%d;%s\n", username, password, userData->multiplier, userData->level, userData->isNewbie, scoreStr);

    fclose(file);

    printf("User registered successfully!\n");
    return true;
}

void createAuthwin(SDL_Renderer *renderer, TTF_Font *font, const char *username, const char *password)
{
    // Render username input field
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Rect usernameRect = {290, 110, 200, 32};
    SDL_SetRenderDrawColor(renderer, 90, 88, 88, 30);
    SDL_RenderFillRect(renderer, &usernameRect);
    // Render username text
    char user_str[100];
    sprintf(user_str, "Username: %s", username);
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *usernameSurface = TTF_RenderText_Blended(font, user_str, textColor);
    SDL_Texture *usernameTexture = SDL_CreateTextureFromSurface(renderer, usernameSurface);
    SDL_Rect usernameTextRect = {105, 105, usernameSurface->w, usernameSurface->h};
    SDL_RenderCopy(renderer, usernameTexture, NULL, &usernameTextRect);

    // Render password input field
    SDL_Rect passwordRect = {290, 160, 200, 32};
    SDL_SetRenderDrawColor(renderer, 90, 88, 88, 30);
    SDL_RenderFillRect(renderer, &passwordRect);

    // Render password text
    char password_str[100];
    sprintf(password_str, "Password: %s", password);
    SDL_Surface *passwordSurface = TTF_RenderText_Blended(font, password_str, textColor);
    SDL_Texture *passwordTexture = SDL_CreateTextureFromSurface(renderer, passwordSurface);
    SDL_Rect passwordTextRect = {105, 155, passwordSurface->w, passwordSurface->h};
    SDL_RenderCopy(renderer, passwordTexture, NULL, &passwordTextRect);
    // render login output

    SDL_Rect loginrectdraw = {95, 205, 210, 40};
    SDL_SetRenderDrawColor(renderer, 90, 88, 88, 30);
    SDL_RenderFillRect(renderer, &loginrectdraw);

    // Render login page text
    SDL_Surface *logsur = TTF_RenderText_Blended(font, "      Login", textColor);
    SDL_Texture *logtextur = SDL_CreateTextureFromSurface(renderer, logsur);
    SDL_Rect logr = {105, 205, logsur->w, logsur->h};
    SDL_RenderCopy(renderer, logtextur, NULL, &logr);

    SDL_Rect registerrectdraw = {345, 205, 210, 40};
    SDL_SetRenderDrawColor(renderer, 90, 88, 88, 30);
    SDL_RenderFillRect(renderer, &registerrectdraw);

    SDL_Surface *regsur = TTF_RenderText_Blended(font, "    sign up", textColor);
    SDL_Texture *regtextur = SDL_CreateTextureFromSurface(renderer, regsur);
    SDL_Rect regr = {355, 205, regsur->w, regsur->h};
    SDL_RenderCopy(renderer, regtextur, NULL, &regr);

    // Cleanup resources
    SDL_FreeSurface(regsur);
    SDL_DestroyTexture(regtextur);
    SDL_FreeSurface(logsur);
    SDL_DestroyTexture(logtextur);
    SDL_FreeSurface(passwordSurface);
    SDL_DestroyTexture(passwordTexture);
    SDL_FreeSurface(usernameSurface);
    SDL_DestroyTexture(usernameTexture);
}
