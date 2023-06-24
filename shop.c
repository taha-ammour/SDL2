#include "file/Auth.h"
#include "file/shop.h"
#include <math.h>

char *formatNumberWithSuffix(mpz_t number)
{
    mpz_t base;
    mpz_init(base);
    mpz_set_ui(base, 1000);

    char *suffixes[] = {"", "k", "M", "B", "T"};
    int suffixIndex = 0;

    mpz_t quotient, remainder;
    mpz_init(quotient);
    mpz_init(remainder);

    mpz_tdiv_qr(quotient, remainder, number, base);
    while (mpz_cmp_ui(quotient, 0) >= 1 && suffixIndex < sizeof(suffixes) / sizeof(suffixes[0]) - 1)
    {
        mpz_tdiv_qr(quotient, remainder, quotient, base);
        suffixIndex++;
    }

    char numberString[BUFFER_SIZE];
    mpz_get_str(numberString, 10, number);
    size_t length = strlen(numberString);

    size_t bufferSize = BUFFER_SIZE;
    char *formattedNumber = (char *)malloc(bufferSize * sizeof(char));
    if (formattedNumber == NULL)
    {
        // Handle memory allocation failure
        mpz_clears(base, quotient, remainder, NULL);
        return NULL;
    }

    if (suffixIndex > 0)
    {
        int decimalIndex = length % 3;
        if (decimalIndex == 0)
            decimalIndex = 3;

        memmove(numberString + decimalIndex + 1, numberString + decimalIndex, length - decimalIndex + 1);
        numberString[decimalIndex] = '.';

        size_t suffixLength = strlen(suffixes[suffixIndex]);
        if (length + suffixLength + 4 < BUFFER_SIZE)
        { // Increase buffer size to accommodate the decimal places and suffix
            snprintf(numberString + length + 1, 4, "%.2s", suffixes[suffixIndex]);
        }
    }

    strncpy(formattedNumber, numberString, bufferSize - 1);
    formattedNumber[bufferSize - 1] = '\0';

    mpz_clears(base, quotient, remainder, NULL);
    return formattedNumber;
}

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
        char storedPassword[MAX_PASSWORD_LENGTH];

        sscanf(buffer, "%[^;];%[^;];", storedUsername, storedPassword);

        if (strcmp(username, storedUsername) == 0)
        {
            char scoreStr[MAX_USERNAME_LENGTH];
            gmp_sprintf(scoreStr, "%Zd", userData->score);
            fprintf(tempFile, "%s;%s;%lld;%d;%d;%d;%s\n", username, storedPassword, userData->multiplier, userData->diffadd, userData->level, userData->isNewbie, scoreStr);
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

void drawshop(SDL_Renderer *renderer, TTF_Font *font, Data *userdata, bool isUnlocked[])
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect text_rect;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    char *scoreStr;
    gmp_asprintf(&scoreStr, "score: %Zd", userdata->score);

    char *result = formatNumberWithSuffix(userdata->score);

    text_surface = TTF_RenderText_Blended(font, result, color);
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

    char Multlvl[100];
    sprintf(Multlvl, "Mult: x%d.00", userdata->multiplier);
    text_surface = TTF_RenderText_Blended(font18, Multlvl, color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = (800 - text_surface->w) / 2;
    text_rect.y = 40;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    int y = 72;
    for (int i = 0; i < 5; i++)
    {
        if (isUnlocked[i])
        {
            SDL_Rect itemRect = {0, y, 800, 76};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderFillRect(renderer, &itemRect);
            char textshow[100];

            int co = i + 1;
            size_t mult = 2 * pow(2, i);
            if (i < 2)
            {
                sprintf(textshow, "up %d unlocked buy %02d multiplier for %lld", co, mult, 50 * (size_t)pow(userdata->multiplier, i + 1));
            }
            if(i>=2){
                sprintf(textshow, "up %d unlocked buy %02d multiplier and add tries for %lld", co, mult, 50 * (size_t)pow(userdata->multiplier, i + 1));
            }
            text_surface = TTF_RenderText_Blended(font18, textshow, color);
            text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            text_rect.x = (800 - text_surface->w) / 4 + 50;
            text_rect.y = y;
            text_rect.w = text_surface->w;
            text_rect.h = text_surface->h;

            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_FreeSurface(text_surface);
            SDL_DestroyTexture(text_texture);
            y += 86;
        }
        else
        {
            SDL_Rect itemRect = {0, y, 800, 76};
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
            SDL_RenderFillRect(renderer, &itemRect);
            switch (i)
            {
            case 0:
                text_surface = TTF_RenderText_Blended(font, "Not Unlocked Req lvl     50", color);
                break;

            case 1:
                text_surface = TTF_RenderText_Blended(font, "Not Unlocked Req lvl    100", color);
                break;
            case 2:
                text_surface = TTF_RenderText_Blended(font, "Not Unlocked Req lvl    500", color);
                break;
            case 3:
                text_surface = TTF_RenderText_Blended(font, "Not Unlocked Req lvl  1.000", color);
                break;
            case 4:
                text_surface = TTF_RenderText_Blended(font, "Not Unlocked Req lvl 10.000", color);
                break;
            }

            text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            text_rect.x = (800 - text_surface->w) / 2;
            text_rect.y = y;
            text_rect.w = text_surface->w;
            text_rect.h = text_surface->h;

            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_FreeSurface(text_surface);
            SDL_DestroyTexture(text_texture);
            y += 86;
        }
    }
    y = 77;
    for (int i = 0; i < 5; i++)
    {
        if (isUnlocked[i])
        {
            SDL_Rect itemRect = {10, y, 70, 65};
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderFillRect(renderer, &itemRect);

            y += 86;
        }
    }
    y = 77;
    for (int i = 0; i < 5; i++)
    {
        if (isUnlocked[i])
        {
            SDL_Rect itemRect = {720, y, 70, 65};
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            SDL_RenderFillRect(renderer, &itemRect);
            text_surface = TTF_RenderText_Blended(font, "BUY", color);
            text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            text_rect.x = itemRect.x;
            text_rect.y = itemRect.y;
            text_rect.w = text_surface->w;
            text_rect.h = text_surface->h;

            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_FreeSurface(text_surface);
            SDL_DestroyTexture(text_texture);

            y += 86;
        }
    }
}
