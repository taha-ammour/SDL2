#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <gmp.h>
#include "file/Auth.h"
#include "file/shop.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

SDL_Color white = {255, 255, 255};
SDL_Color black = {0, 0, 0};

TTF_Font *font38;
TTF_Font *font28;
TTF_Font *font18;

SDL_Texture *startexture;
SDL_Texture *rockTexture;

SDL_Surface *rocksurface;
SDL_Surface *iconSurface;
SDL_Surface *starsurface;

Mix_Chunk *Clicksound;
Mix_Chunk *Clicksound2;
Mix_Chunk *HIT;
Mix_Chunk *FALSEWAV;
Mix_Chunk *TRUEWAV;
Mix_Chunk *Reset_sound;

SDL_Window *window;
SDL_Renderer *renderer;

Data userData;

#define MAIN_MENU 4
#define MENU_START_GAME 0
#define MENU_OPTIONS 1
#define MENU_SHOP 2
#define MENU_QUIT 3
#define MAX_WORD_LENGTH 100

bool fmenu = true;
int set = MENU_AUTH;

char username[100] = {0};
char password[100] = {0};

const int easylivetry = 9;
const int medlivetry = 6;
const int hardlivetry = 3;

int livesrem;

typedef enum
{
    EASY,
    MED,
    HARD
} Difficulty;
typedef struct
{
    int x, y;
    int speed;
} Star;
typedef struct
{
    int x;
    int y;
    float speedX;
    float speedY;
    float size;
} Rock;

// function intialisation
// TODO: MAKE the function for the hangman and apply words based on difficulty
void Sdlinti();
char *getRandomWord(const char *filename);
void score_txt(TTF_Font *font, int score, int x, int y);
Uint32 timer_callback(Uint32 interval, void *param);
void draw_menu(TTF_Font *font38, int selected_item, int selected_options);
void draw_txt(TTF_Font *font38, const char *timer_text, int x, int y);
void draw_txt_g(TTF_Font *font38, const char *timer_text, bool *guessed_letters);
void draw_options(TTF_Font *font38, int selected_options);
void draw_tries(TTF_Font *font, int selected_options, int try);
void drawHangman(int wrongGuesses, int selected_options);
bool check_game_over(bool *guessed_letters, int word_length);
void draw_admin(TTF_Font *font, bool istrue, char *word);
void updateStars(Star *stars);
void initRocks(Rock *rocks);
void updateRocks(Rock *rocks);
// Timer callback function
Uint32 timer_callback(Uint32 interval, void *param)
{
    int *counter = (int *)param;
    if (set == 0)
    {
        (*counter)--;
    }

    return interval;
}

int main(int argc, char *argv[])
{

    Sdlinti();
    srand(time(NULL));

    int selected_item = MENU_START_GAME;
    Difficulty selected_options;
    selected_options = EASY;
    int counter = 6 * 60; // 6 minutes

    SDL_TimerID timer_id = SDL_AddTimer(1000, timer_callback, &counter);
    bool quit = false;
    bool timer_started = false;
    char timer_text[32];
    livesrem = 100 / 9;
    int diffinco = 9;

    Star stars[100];
    for (int i = 0; i < 100; i++)
    {
        stars[i].x = rand() % WINDOW_WIDTH;
        stars[i].y = rand() % WINDOW_HEIGHT;
        stars[i].speed = 1 + rand() % 3;
    }
    Rock rocks[3];
    initRocks(rocks);
    startexture = SDL_CreateTextureFromSurface(renderer, starsurface);
    SDL_FreeSurface(starsurface);

    rockTexture = SDL_CreateTextureFromSurface(renderer, rocksurface);
    SDL_FreeSurface(rocksurface);

    SDL_SetWindowIcon(window, iconSurface);
    SDL_FreeSurface(iconSurface);

    int score = 0;
    bool initialized = true;
    char *word = getRandomWord("res/textfiles/EASY.txt");
    free(word);
    switch (selected_options)
    {
    case EASY:
        word = getRandomWord("res/textfiles/EASY.txt");
        break;
    case MED:
        word = getRandomWord("res/textfiles/MED.TXT");
        break;
    case HARD:
        word = getRandomWord("res/textfiles/HARD.TXT");
        break;
    }
    int word_length = strlen(word);
    char incorrect_guesses_letters[9];
    char correct_guesses_letters[100];
    memset(incorrect_guesses_letters, 0, sizeof(incorrect_guesses_letters));
    memset(correct_guesses_letters, 0, sizeof(correct_guesses_letters));

    bool *guessed_letters = malloc(word_length * sizeof(bool));
    memset(guessed_letters, false, word_length * sizeof(bool));

    int incorrect_guesses = 0;
    int correct_guesses = 0;
    bool select_user = false;
    bool select_pass = false;
    bool isUnlocked[5] = {false};

    for (int i = 0; i < 5; i++)
    {
        isUnlocked[i] = false;
    }

    while (!quit)
    {
        SDL_Event event;
        // Initialize the game variables

        if (!initialized)
        {
            free(guessed_letters);
            guessed_letters = NULL;
            memset(incorrect_guesses_letters, 0, sizeof(incorrect_guesses_letters));
            memset(correct_guesses_letters, 0, sizeof(correct_guesses_letters));

            free(word);
            switch (selected_options)
            {
            case EASY:
                word = getRandomWord("res/textfiles/EASY.txt");
                break;
            case MED:
                word = getRandomWord("res/textfiles/MED.TXT");
                break;
            case HARD:
                word = getRandomWord("res/textfiles/HARD.TXT");
                break;
            }

            word_length = strlen(word);

            guessed_letters = malloc(word_length * sizeof(bool));
            memset(guessed_letters, false, word_length * sizeof(bool));
            correct_guesses = 0;

            initialized = true;
        }
        // Handle other input events, such as key presses or mouse events
        while (SDL_PollEvent(&event))
        {
            if (set == MAIN_MENU)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEMOTION:
                    int MouseX = event.button.x;
                    int MouseY = event.button.y;
                    Uint32 mouseState = SDL_GetMouseState(&MouseX, &MouseY);

                    if (MouseX >= 240 && MouseX <= 526 && MouseY >= 160 && MouseY <= 200)
                    {
                        selected_item = MENU_START_GAME;
                        if (Mix_Playing(-1) == 0)
                        {
                            Mix_PlayChannel(-1, Clicksound, 0);
                        }
                    }
                    else if (MouseX >= 285 && MouseX <= 478 && MouseY >= 205 && MouseY <= 247)
                    {
                        selected_item = MENU_OPTIONS;
                        if (Mix_Playing(-1) == 0)
                        {
                            Mix_PlayChannel(-1, Clicksound, 0);
                        }
                    }
                    else if (MouseX >= 314 && MouseX <= 435 && MouseY >= 258 && MouseY <= 301)
                    {
                        selected_item = MENU_SHOP;
                        if (Mix_Playing(-1) == 0)
                        {
                            Mix_PlayChannel(-1, Clicksound, 0);
                        }
                    }
                    else if (MouseX >= 315 && MouseX <= 430 && MouseY >= 314 && MouseY <= 345)
                    {
                        selected_item = MENU_QUIT;
                        if (Mix_Playing(-1) == 0)
                        {
                            Mix_PlayChannel(-1, Clicksound, 0);
                        }
                    }

                    break;
                case SDL_MOUSEBUTTONDOWN:

                    if (MouseX >= 240 && MouseX <= 526 && MouseY >= 160 && MouseY <= 200)
                    {
                        set = MENU_START_GAME;
                        Mix_PlayChannel(-1, HIT, 0);
                    }
                    else if (MouseX >= 285 && MouseX <= 478 && MouseY >= 205 && MouseY <= 247)
                    {
                        set = MENU_OPTIONS;
                        Mix_PlayChannel(-1, HIT, 0);
                    }
                    else if (MouseX >= 314 && MouseX <= 435 && MouseY >= 258 && MouseY <= 301)
                    {
                        set = MENU_SHOP;
                        Mix_PlayChannel(-1, HIT, 0);
                    }
                    else if (MouseX >= 315 && MouseX <= 430 && MouseY >= 314 && MouseY <= 345)
                    {
                        set = MENU_QUIT;
                        Mix_PlayChannel(-1, HIT, 0);
                    }
                    break;
                }
            }
            else if (set == MENU_OPTIONS)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        set = MAIN_MENU;
                        Mix_PlayChannel(-1, HIT, 0);
                        break;
                    case SDLK_UP:
                        // scrolling up in options menu
                        selected_options--;
                        if (selected_options < EASY || selected_options == -1)
                        {
                            selected_options = HARD;
                        }
                        Mix_PlayChannel(-1, Clicksound2, 0);

                        break;
                    case SDLK_DOWN:
                        selected_options++;
                        if (selected_options > HARD)
                        {
                            selected_options = EASY;
                        }
                        Mix_PlayChannel(-1, Clicksound2, 0);

                        break;
                    case SDLK_RETURN:
                        set = MAIN_MENU;
                        Mix_PlayChannel(-1, HIT, 0);
                        break;
                    }
                    break;
                }
            }
            else if (set == MENU_START_GAME)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_r:
                        if (counter <= 0 || incorrect_guesses >= diffinco)
                        {
                            mpz_add_ui(userData.score, userData.score, score);
                            updateUserData(username, &userData);
                            set = MAIN_MENU;
                            initialized = false;
                            incorrect_guesses = 0;
                            score = 0;
                            switch (selected_options)
                            {
                            case EASY:
                                counter = 6 * 60;
                                livesrem = 100 / 9;
                                diffinco = 9;
                                break;
                            case MED:
                                counter = 4 * 60;
                                livesrem = 100 / 6;
                                diffinco = 6;
                                break;
                            case HARD:
                                counter = 3 * 60;
                                livesrem = 100 / 3;
                                diffinco = 3;
                                break;
                            }
                            Mix_PlayChannel(-1, Reset_sound, 0);
                        }
                        break;
                    }
                    if (event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z')
                    {
                        char letter = event.key.keysym.sym;
                        bool found_letter = false;

                        // Check if the letter has already been guessed
                        bool already_guessed = false;
                        for (int i = 0; i < correct_guesses + incorrect_guesses; i++)
                        {
                            if (correct_guesses_letters[i] == letter)
                            {
                                already_guessed = true;
                                break;
                            }
                        }

                        // If the letter has not been guessed before, check if it is in the word
                        if (!already_guessed)
                        {
                            // Check if the letter has already been guessed incorrectly
                            bool already_incorrect = false;
                            for (int i = 0; i < incorrect_guesses; i++)
                            {
                                if (incorrect_guesses_letters[i] == letter)
                                {
                                    already_incorrect = true;
                                    break;
                                }
                            }

                            // If the letter has not been guessed incorrectly before, check if it is in the word
                            if (!already_incorrect && incorrect_guesses < diffinco)
                            {
                                for (int i = 0; i < word_length; i++)
                                {
                                    if (word[i] == letter)
                                    {
                                        guessed_letters[i] = true;
                                        found_letter = true;
                                    }
                                }

                                // If the letter was not found in the word, add it to the list of incorrect guesses
                                if (!found_letter && !check_game_over(guessed_letters, word_length))
                                {

                                    if (initialized)
                                    {
                                        incorrect_guesses_letters[incorrect_guesses] = letter;
                                        incorrect_guesses++;
                                        Mix_PlayChannel(-1, FALSEWAV, 0);
                                        score -= livesrem / 2;
                                        if (score <= 0)
                                        {
                                            score = 0;
                                        }
                                    }
                                }
                                else if (found_letter)
                                {
                                    if (initialized)
                                    {
                                        correct_guesses_letters[correct_guesses] = letter;
                                        correct_guesses++;
                                        score += livesrem;
                                        Mix_PlayChannel(-1, TRUEWAV, 0);
                                    }
                                }
                            }
                        }
                    }

                    break;
                }
            }
            else if (set == MENU_AUTH)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    int MouseX = event.button.x;
                    int MouseY = event.button.y;
                    Uint32 mouseState = SDL_GetMouseState(&MouseX, &MouseY);
                    if (MouseX >= 95 && MouseX <= 305 && MouseY >= 205 && MouseY <= 245)
                    {

                        // Authenticate the credentials
                        bool isAuthenticated = authenticateUser(username, password, &userData);
                        if (isAuthenticated)
                        {
                            printf("Login successful!\n");
                            set = MAIN_MENU;
                        }
                        else
                        {
                            printf("Invalid credentials.\n");
                        }
                    }
                    if (MouseX >= 290 && MouseX <= 490 && MouseY >= 110 && MouseY <= 142)
                    {
                        select_user = true;
                        select_pass = false;
                    }
                    if (MouseX >= 290 && MouseX <= 490 && MouseY >= 160 && MouseY <= 192)
                    {
                        select_pass = true;
                        select_user = false;
                    }
                    if (MouseX >= 345 && MouseX <= 555 && MouseY >= 205 && MouseY <= 245)
                    {
                        if (registerUser(username, password, &userData))
                        {
                            printf("Registration Successful!");
                            set = MAIN_MENU;
                        }
                    }

                    break;
                case SDL_TEXTINPUT:
                    if (select_user)
                    {
                        if (strlen(username) < sizeof(username) - 1)
                        {
                            strncat(username, event.text.text, sizeof(username) - strlen(username) - 1);
                        }
                    }
                    if (select_pass)
                    {
                        if (strlen(password) < sizeof(password) - 1)
                        {
                            strncat(password, event.text.text, sizeof(password) - strlen(password) - 1);
                        }
                    }

                    break;

                case SDL_KEYDOWN:
                    if (select_user)
                    {
                        if (event.key.keysym.sym == SDLK_BACKSPACE)
                        {
                            if (strlen(username) > 0)
                            {
                                username[strlen(username) - 1] = '\0';
                            }
                        }
                    }
                    if (select_pass)
                    {
                        if (event.key.keysym.sym == SDLK_BACKSPACE)
                        {
                            if (strlen(password) > 0)
                            {
                                password[strlen(password) - 1] = '\0';
                            }
                        }
                    }

                    break;
                }
            }
            else if (set == MENU_SHOP)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_r:
                        set = MAIN_MENU;
                        break;
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    int MouseX = event.button.x;
                    int MouseY = event.button.y;
                    Uint32 mouseState = SDL_GetMouseState(&MouseX, &MouseY);
                    printf("Mouse x :%d, Mouse y: %d\n", MouseX, MouseY);
                    if (MouseX >= 720 && MouseX <= 790 && MouseY >= 79 && MouseY <= 143 && isUnlocked[0])
                    {
                        if (mpz_cmp_ui(userData.score, 50 * pow(userData.multiplier, 1)) >= 0)
                        {
                            mpz_sub_ui(userData.score, userData.score, 50 * pow(userData.multiplier, 1));
                            userData.multiplier += 2;
                        }
                    }
                    else if (MouseX >= 720 && MouseX <= 790 && MouseY >= 164 && MouseY <= 228 && isUnlocked[1])
                    {
                        if (mpz_cmp_ui(userData.score, 50 * pow(userData.multiplier, 2)) >= 0)
                        {
                            mpz_sub_ui(userData.score, userData.score, 50 * pow(userData.multiplier, 2));
                            userData.multiplier += 4;
                        }
                    }
                    else if (MouseX >= 720 && MouseX <= 790 && MouseY >= 250 && MouseY <= 316 && isUnlocked[2])
                    {
                        if (mpz_cmp_ui(userData.score, 50 * pow(userData.multiplier, 3)) >= 0)
                        {
                            mpz_sub_ui(userData.score, userData.score, 50 * pow(userData.multiplier, 3));
                            userData.multiplier += 8;
                        }
                    }
                    else if (MouseX >= 720 && MouseX <= 790 && MouseY >= 338 && MouseY <= 400 && isUnlocked[3])
                    {
                        if (mpz_cmp_ui(userData.score, 50 * pow(userData.multiplier, 4)) >= 0)
                        {
                            mpz_sub_ui(userData.score, userData.score, 50 * pow(userData.multiplier, 4));
                            userData.multiplier += 16;
                        }
                    }
                    else if (MouseX >= 720 && MouseX <= 790 && MouseY >= 423 && MouseY <= 486 && isUnlocked[4])
                    {
                       if (mpz_cmp_ui(userData.score, 50 * pow(userData.multiplier, 5)) >= 0)
                        {
                            mpz_sub_ui(userData.score, userData.score, 50 * pow(userData.multiplier, 5));
                            userData.multiplier += 32;
                        }
                    }

                    break;
                }
            }
            else if (set == MENU_QUIT)
            {
                quit = true;
            }
        }
        // Update game logic based on user input or other factors
        if (set == MENU_OPTIONS)
        {
            switch (selected_options)
            {
            case EASY:
                counter = 6 * 60;
                livesrem = 100 / 9;
                diffinco = 9;
                break;
            case MED:
                counter = 4 * 60;
                livesrem = 100 / 6;
                diffinco = 6;
                break;
            case HARD:
                counter = 3 * 60;
                livesrem = 100 / 3;
                diffinco = 3;
                break;
            }
        }
        else if (set == MENU_START_GAME)
        {
            if (counter >= 0 && check_game_over(guessed_letters, word_length))
            {
                initialized = false;
            }
        }
        else if (set == MENU_SHOP)
        {
            if (userData.level >= 50)
            {
                isUnlocked[0] = true;
            }
            if (userData.level >= 100)
            {
                isUnlocked[1] = true;
            }
            if (userData.level >= 500)
            {
                isUnlocked[2] = true;
            }
            if (userData.level >= 1000)
            {
                isUnlocked[3] = true;
            }
            if (userData.level >= 10000)
            {
                isUnlocked[4] = true;
            }
            if (!userData.isNewbie)
            {
                mpz_add_ui(userData.score, userData.score, 300);
                userData.isNewbie = true;
                updateUserData(username, &userData);
            }
        }

        // Render game objects based on the updated game logic
        if (set == MAIN_MENU)
        {
            draw_menu(font38, selected_item, selected_options);
            updateStars(stars);
        }
        if (set == MENU_OPTIONS)
        {
            draw_options(font38, selected_options);
            updateStars(stars);
        }
        if (set == MENU_START_GAME)
        {
            if (counter > 0 && incorrect_guesses < diffinco)
            {
                sprintf(timer_text, "%02d:%02d", counter / 60, counter % 60);
                draw_txt(font28, timer_text, 0, 0);
                updateStars(stars);
                score_txt(font28, score, WINDOW_WIDTH - 200, 0);
                draw_txt_g(font28, word, guessed_letters);
                if (!strcmp(username, "admin"))
                {
                    draw_admin(font28, true, word);
                }
                draw_tries(font28, selected_options, incorrect_guesses);
                drawHangman(incorrect_guesses, selected_options);
            }
            else
            {
                if (counter <= 0)
                {
                    sprintf(timer_text, "Time's up!");
                }
                else if (incorrect_guesses >= diffinco)
                {
                    sprintf(timer_text, "Out of tries!");
                }

                draw_txt(font38, timer_text, 100, 100);
                score_txt(font38, score, WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT / 2);
            }
        }
        if (set == MENU_AUTH)
        {
            createAuthwin(renderer, font28, username, password);
            updateRocks(rocks);
        }
        if (set == MENU_SHOP)
        {
            drawshop(renderer, font28, &userData, isUnlocked);
        }

        SDL_RenderPresent(renderer);
    }
    // Clean up and exit
    SDL_DestroyTexture(startexture);
    SDL_RemoveTimer(timer_id);
    Mix_FreeChunk(Clicksound);
    Mix_CloseAudio();
    Mix_Quit();
    TTF_CloseFont(font38);
    TTF_CloseFont(font28);
    TTF_CloseFont(font18);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

void draw_menu(TTF_Font *font38, int selected_item, int selected_options)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect text_rect;

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // tittle
    text_surface = TTF_RenderText_Blended(font38, "the Hangman Game", color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 170;
    text_rect.y = 50;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    // difficulty level
    char text_difficulty[100];
    sprintf(text_difficulty, "Difficulty : %s", (selected_options == EASY) ? " Easy" : ((selected_options == MED) ? "Medium" : "Hard"));
    text_surface = TTF_RenderText_Blended(font28, text_difficulty, color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 440;
    text_rect.y = 420;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    // Draw the menu items
    text_surface = TTF_RenderText_Blended(font38, "Start Game", (selected_item == MENU_START_GAME) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 250;
    text_rect.y = 150;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    text_surface = TTF_RenderText_Blended(font38, "Options", (selected_item == MENU_OPTIONS) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 290;
    text_rect.y = 200;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    text_surface = TTF_RenderText_Blended(font38, "Shop", (selected_item == MENU_SHOP) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 320;
    text_rect.y = 250;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    text_surface = TTF_RenderText_Blended(font38, "Quit", (selected_item == MENU_QUIT) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 320;
    text_rect.y = 300;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void draw_txt(TTF_Font *font, const char *text, int x, int y)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect text_rect = {x, y, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &text_rect.w, &text_rect.h);
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void draw_options(TTF_Font *font38, int selected_options)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Rect text_rect;

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the Difficulty title
    SDL_Surface *text_surface = TTF_RenderText_Blended(font38, "Difficulty Level", color);
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 230;
    text_rect.y = 50;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);

    // Draw the difficulty levels
    char easy_text[32], med_text[32], hard_text[32];
    sprintf(easy_text, "Easy%s", (selected_options == EASY) ? " <--" : "");
    sprintf(med_text, "Medium%s", (selected_options == MED) ? " <--" : "");
    sprintf(hard_text, "Hard%s", (selected_options == HARD) ? " <--" : "");

    text_surface = TTF_RenderText_Blended(font38, easy_text, (selected_options == EASY) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 100;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);

    text_surface = TTF_RenderText_Blended(font38, med_text, (selected_options == MED) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 150;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);

    text_surface = TTF_RenderText_Blended(font38, hard_text, (selected_options == HARD) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 200;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

void score_txt(TTF_Font *font, int score, int x, int y)
{
    char score_str[100];
    sprintf(score_str, "Score: %d", score);

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, score_str, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int text_width = surface->w;
    int text_height = surface->h;

    SDL_Rect dst_rect;
    dst_rect.x = WINDOW_WIDTH - 20 - surface->w;
    dst_rect.y = y;
    dst_rect.w = text_width;
    dst_rect.h = text_height;

    SDL_RenderCopy(renderer, texture, NULL, &dst_rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void drawHangman(int wrongGuesses, int selected_options)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Draw the noose
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xC0);
    for (int i = 0; i < 3; i++)
    {
        SDL_RenderDrawLine(renderer, 250 + i, 50 + i, 250 + i, 350);
        SDL_RenderDrawLine(renderer, 250 + i, 50 + i, 450, 50 + i);
        SDL_RenderDrawLine(renderer, 450 + i, 50, 450 + i, 100);
        SDL_RenderDrawLine(renderer, 200, 350 + i, 300, 350 + i);
    }

    // Draw the person
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xC0);
    switch (selected_options)
    {
    case EASY:
        for (int i = 0; i < 4; i++)
        {
            if (wrongGuesses >= 3)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 160, 450 + i, 250); // Body
            }
            if (wrongGuesses >= 6)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 400 + i, 150); // Left arm
            }
            if (wrongGuesses >= 6)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 500 + i, 150); // Right arm
            }
            if (wrongGuesses >= 9)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 400 + i, 300); // Left leg
            }
            if (wrongGuesses >= 9)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 500 + i, 300); // Right leg
            }
        }

        // Draw the head
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xC0);
        if (wrongGuesses >= 3)
        {
            int centerX = 451;
            int centerY = 130;
            int radius = 30;
            for (double i = 0; i < 360; i += 1)
            {
                double angle = i * M_PI / 180;
                int x = centerX + (int)(radius * cos(angle));
                int y = centerY + (int)(radius * sin(angle));

                SDL_RenderDrawPoint(renderer, x, y);
                SDL_RenderDrawPoint(renderer, x + 1, y);
                SDL_RenderDrawPoint(renderer, x, y + 1);
                SDL_RenderDrawPoint(renderer, x - 1, y);
                SDL_RenderDrawPoint(renderer, x, y - 1);
            }
        }
        break;
    case MED:
        for (int i = 0; i < 4; i++)
        {
            if (wrongGuesses >= 2)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 160, 450 + i, 250); // Body
            }
            if (wrongGuesses >= 3)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 400 + i, 150); // Left arm
            }
            if (wrongGuesses >= 4)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 500 + i, 150); // Right arm
            }
            if (wrongGuesses >= 5)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 400 + i, 300); // Left leg
            }
            if (wrongGuesses >= 6)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 500 + i, 300); // Right leg
            }
        }

        // Draw the head
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xC0);
        if (wrongGuesses >= 1)
        {
            int centerX = 451;
            int centerY = 130;
            int radius = 30;
            for (double i = 0; i < 360; i += 1)
            {
                double angle = i * M_PI / 180;
                int x = centerX + (int)(radius * cos(angle));
                int y = centerY + (int)(radius * sin(angle));

                SDL_RenderDrawPoint(renderer, x, y);
                SDL_RenderDrawPoint(renderer, x + 1, y);
                SDL_RenderDrawPoint(renderer, x, y + 1);
                SDL_RenderDrawPoint(renderer, x - 1, y);
                SDL_RenderDrawPoint(renderer, x, y - 1);
            }
        }
        break;
    case HARD:
        for (int i = 0; i < 4; i++)
        {
            if (wrongGuesses >= 1)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 160, 450 + i, 250); // Body
            }
            if (wrongGuesses >= 2)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 400 + i, 150); // Left arm
            }
            if (wrongGuesses >= 2)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 500 + i, 150); // Right arm
            }
            if (wrongGuesses >= 3)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 400 + i, 300); // Left leg
            }
            if (wrongGuesses >= 3)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 500 + i, 300); // Right leg
            }
        }

        // Draw the head
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xC0);
        if (wrongGuesses >= 1)
        {
            int centerX = 451;
            int centerY = 130;
            int radius = 30;
            for (double i = 0; i < 360; i += 1)
            {
                double angle = i * M_PI / 180;
                int x = centerX + (int)(radius * cos(angle));
                int y = centerY + (int)(radius * sin(angle));

                SDL_RenderDrawPoint(renderer, x, y);
                SDL_RenderDrawPoint(renderer, x + 1, y);
                SDL_RenderDrawPoint(renderer, x, y + 1);
                SDL_RenderDrawPoint(renderer, x - 1, y);
                SDL_RenderDrawPoint(renderer, x, y - 1);
            }
        }
        break;
    default:
        for (int i = 0; i < 4; i++)
        {
            if (wrongGuesses >= 3)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 160, 450 + i, 250); // Body
            }
            if (wrongGuesses >= 6)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 400 + i, 150); // Left arm
            }
            if (wrongGuesses >= 6)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 200, 500 + i, 150); // Right arm
            }
            if (wrongGuesses >= 9)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 400 + i, 300); // Left leg
            }
            if (wrongGuesses >= 9)
            {
                SDL_RenderDrawLine(renderer, 450 + i, 250, 500 + i, 300); // Right leg
            }
        }

        // Draw the head
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xC0);
        if (wrongGuesses >= 3)
        {
            int centerX = 451;
            int centerY = 130;
            int radius = 30;
            for (double i = 0; i < 360; i += 1)
            {
                double angle = i * M_PI / 180;
                int x = centerX + (int)(radius * cos(angle));
                int y = centerY + (int)(radius * sin(angle));

                SDL_RenderDrawPoint(renderer, x, y);
                SDL_RenderDrawPoint(renderer, x + 1, y);
                SDL_RenderDrawPoint(renderer, x, y + 1);
                SDL_RenderDrawPoint(renderer, x - 1, y);
                SDL_RenderDrawPoint(renderer, x, y - 1);
            }
        }
        break;
    }
}

void draw_txt_g(TTF_Font *font, const char *text, bool *guessed_letters)
{

    SDL_Color color = {255, 255, 255, 255};
    SDL_Rect rect;
    SDL_Surface *surface = TTF_RenderText_Blended(font, "Word: ", color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect.x = 250;
    rect.y = 400;
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    int x = 380;
    int y = 400;
    int letter_width = 0;
    int letter_height = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (guessed_letters[i])
        {
            char letter[2] = {text[i], '\0'};
            surface = TTF_RenderText_Blended(font, letter, color);
        }
        else
        {
            surface = TTF_RenderText_Blended(font, "_", color);
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(texture, NULL, NULL, &letter_width, &letter_height);
        rect.x = x;
        rect.y = y;
        rect.w = letter_width;
        rect.h = letter_height;
        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        x += letter_width + 10; // Add some spacing between letters
    }
}

bool check_game_over(bool *guessed_letters, int word_length)
{
    for (int i = 0; i < word_length; i++)
    {
        if (!guessed_letters[i])
        {
            return false;
        }
    }
    userData.level++;
    return true;
}

void draw_tries(TTF_Font *font, int selected_options, int try)
{
    char try_str[32];
    sprintf(try_str, "Tries: %d:%s", try, (selected_options == EASY) ? "9" : (selected_options == MED) ? "6"
                                                                                                       : "3");
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, try_str, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst_rect = {WINDOW_WIDTH / 2 - 100, 0, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &dst_rect.w, &dst_rect.h);
    SDL_RenderCopy(renderer, texture, NULL, &dst_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Sdlinti()
{

    // intialization of SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        return;
    if (TTF_Init() != 0)
    {
        SDL_Quit();
        return;
    }
    // initialize music
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Clicksound = Mix_LoadWAV("res/sfx/Click.wav");
    HIT = Mix_LoadWAV("res/sfx/hit.wav");
    Clicksound2 = Mix_LoadWAV("res/sfx/Click2.wav");
    FALSEWAV = Mix_LoadWAV("res/sfx/false.wav");
    TRUEWAV = Mix_LoadWAV("res/sfx/found.wav");
    Reset_sound = Mix_LoadWAV("res/sfx/reset.wav");

    starsurface = IMG_Load("res/gfx/star.png");
    iconSurface = IMG_Load("res/gfx/player.png");
    rocksurface = IMG_Load("res/gfx/spacerock.png");

    // create window
    window = SDL_CreateWindow("Hangman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        TTF_Quit();
        SDL_Quit();
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }

    font28 = TTF_OpenFont("fonts/Talk Comic.ttf", 28);
    font38 = TTF_OpenFont("fonts/Talk Comic.ttf", 38);
    font18 = TTF_OpenFont("fonts/Talk Comic.ttf", 18);
    if (!font28 || !font38 || !font18)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }
}

char *getRandomWord(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return NULL;
    }

    char word[MAX_WORD_LENGTH];
    char *randomWord = NULL;
    int count = 0;

    while (fscanf(file, "%s", word) == 1)
    {
        if (rand() % (++count) == 0)
        {
            randomWord = strdup(word);
        }
    }

    fclose(file);
    return randomWord;
}

void draw_admin(TTF_Font *font, bool istrue, char *word)
{

    if (istrue)
    {
        SDL_Color color = {255, 10, 10, 255};
        SDL_Rect rect;

        SDL_Surface *surface = TTF_RenderText_Blended(font, "Admin: ", color);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        rect.x = 250;
        rect.y = 360;
        rect.w = surface->w;
        rect.h = surface->h;
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        int x = 380;
        int y = 360;
        int letter_width = 0;
        int letter_height = 0;
        for (int i = 0; i < strlen(word); i++)
        {
            char letter[2] = {word[i], '\0'};
            surface = TTF_RenderText_Blended(font, letter, color);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_QueryTexture(texture, NULL, NULL, &letter_width, &letter_height);

            rect.x = x;
            rect.y = y;
            rect.w = letter_width;
            rect.h = letter_height;
            SDL_RenderCopy(renderer, texture, NULL, &rect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            x += letter_width + 10;
        }
    }
}

void updateStars(Star *stars)
{
    for (int i = 0; i < 100; i++)
    {
        stars[i].y += stars[i].speed;

        if (stars[i].y >= WINDOW_HEIGHT)
        {
            stars[i].y = 0;
            stars[i].x = rand() % WINDOW_WIDTH;
        }
    }
    for (int i = 0; i < 100; i++)
    {

        SDL_Rect starRect = {stars[i].x, stars[i].y, 4, 4};
        SDL_RenderCopy(renderer, startexture, NULL, &starRect);
    }
}

void initRocks(Rock *rocks)
{
    for (int i = 0; i < 3; i++)
    {
        rocks[i].x = (i % 2 == 0) ? 0 : 740;     // Alternate between left and right side
        rocks[i].y = rand() % WINDOW_HEIGHT + 1; // Random starting y position
        rocks[i].speedY = rand() % 5 + 1;        // Random speed between 1 and 5
        rocks[i].speedX = rand() % 5 + 1;
        rocks[i].size = rand() % 20 + 10; // Random size between 10 and 30
    }
}

void updateRocks(Rock *rocks)
{
    for (int i = 0; i < 3; i++)
    {
        if (rocks[i].y < 0 || rocks[i].y >= WINDOW_HEIGHT - (3 * rocks[i].size))
        {
            rocks[i].speedY = -rocks[i].speedY; // Reverse the direction of the rock
        }
        if (rocks[i].x < 0 || rocks[i].x >= WINDOW_WIDTH - (3 * rocks[i].size))
        {
            rocks[i].speedX = -rocks[i].speedX; // Reverse the direction of the rock
        }

        rocks[i].y += rocks[i].speedY * (rand() % 2 + 1);
        rocks[i].x += rocks[i].speedX * (rand() % 2 + 1);

        SDL_Rect rockRect = {rocks[i].x, rocks[i].y, 3 * rocks[i].size, 3 * rocks[i].size};
        SDL_RenderCopy(renderer, rockTexture, NULL, &rockRect);
    }
}
