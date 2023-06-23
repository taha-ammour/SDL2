#include "file/Auth.h"
#include "file/shop.h"

bool updateUserData(const char *username, const Data *userData)
{
    // Open the file in read mode to check if the user exists
    FILE *file = fopen("user.txt", "r+");
    if (file == NULL)
    {
        printf("Failed to open user data file.\n");
        return false;
    }

    char buffer[MAX_USERNAME_LENGTH + MAX_PASSWORD_LENGTH + sizeof(Data) + 3];
    long int userDataOffset = -1;

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char storedUsername[MAX_USERNAME_LENGTH];
        sscanf(buffer, "%[^;];", storedUsername);

        if (strcmp(username, storedUsername) == 0)
        {
            userDataOffset = ftell(file) - strlen(buffer);
            break;
        }
    }

    if (userDataOffset == -1)
    {
        fclose(file);
        printf("User does not exist.\n");
        return false;
    }

    // Create a temporary file to write the updated data
    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        fclose(file);
        printf("Failed to open temporary file.\n");
        return false;
    }

    fseek(file, 0, SEEK_SET);

    // Copy the contents of the original file to the temporary file, updating the user's data
    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char storedUsername[MAX_USERNAME_LENGTH];
        char storedPassword[MAX_USERNAME_LENGTH];

        sscanf(buffer, "%[^;];%[^;];", storedUsername, storedPassword);

        if (strcmp(username, storedUsername) == 0)
        {
            char *scoreStr;
            gmp_asprintf(&scoreStr, "%Zd", userData->score);
            fprintf(tempFile, "%s;%s;%.2f;%d;%d;%s\n", username, storedPassword, userData->multiplier, userData->level, userData->isNewbie, scoreStr);
        }
        else
        {
            fputs(buffer, tempFile);
        }
    }

    fclose(file);
    fclose(tempFile);

    // Replace the original file with the temporary file
    if (remove("user.txt") != 0)
    {
        printf("Failed to remove user data file.\n");
        return false;
    }

    if (rename("temp.txt", "user.txt") != 0)
    {
        printf("Failed to rename temporary file.\n");
        return false;
    }

    printf("User data updated successfully!\n");
    return true;
}

void drawshop(SDL_Renderer *renderer, TTF_Font *font, Data *userdata)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect text_rect;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    char *scoreStr;
    gmp_asprintf(&scoreStr, "score: %Zd", userdata->score);

    text_surface = TTF_RenderText_Blended(font, scoreStr, color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 0;
    text_rect.y = 0;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    char text_level[30];
    sprintf(text_level, "Level:%d\n", userdata->level);

    text_surface = TTF_RenderText_Blended(font, text_level, color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 800 - text_surface->w;
    text_rect.y = 0;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    text_surface = TTF_RenderText_Blended(font, "SHOP", color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = (800 - text_surface->w) / 2;
    text_rect.y = 0;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    int y = 72;
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            SDL_Rect itemRect = {0, y, 800, 76};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &itemRect);
        }
        y += 86;
    }

}