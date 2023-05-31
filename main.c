#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

SDL_Color white = {255, 255, 255};
SDL_Color black = {0, 0, 0};

TTF_Font *font38;
TTF_Font *font28;

Mix_Chunk *Clicksound;
Mix_Chunk *Clicksound2;
Mix_Chunk *HIT;
Mix_Chunk *FALSEWAV;

SDL_Window *window;
SDL_Renderer *renderer;

#define MAIN_MENU 4
#define MENU_START_GAME 0
#define MENU_OPTIONS 1
#define MENU_QUIT 2
#define WORD_LIST_SIZE 10

char *word_list[WORD_LIST_SIZE] = {"hangman", "computer", "keyboard", "mouse", "monitor", "printer", "scanner", "software", "hardware", "database"};

bool fmenu = true;
int set = MAIN_MENU;
int score = 100;

const int easylivetry = 9;
const int medlivetry = 6;
const int hardlivetry = 3;

int livesrem;

enum Difficulty
{
    EASY,
    MED,
    HARD
};

// function intialisation
// TODO: MAKE the function for the hangman and apply words based on difficulty
void Sdlinti();
void score_txt(TTF_Font *font, int score, int x, int y);
Uint32 timer_callback(Uint32 interval, void *param);
void draw_menu(TTF_Font *font38, int selected_item, int selected_options);
void draw_txt(TTF_Font *font38, const char *timer_text, int x, int y);
void draw_txt_g(TTF_Font *font38, const char *timer_text, bool *guessed_letters);
void draw_options(TTF_Font *font38, int selected_options);
void draw_tries(TTF_Font *font, int selected_options, int try);
void drawHangman(int wrongGuesses, int selected_options);
bool check_game_over(bool *guessed_letters, int word_length);
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

    // Randomly select a word from the word list
    srand(time(NULL));

    // import sounds
    Clicksound = Mix_LoadWAV("res/sfx/Click.wav");
    HIT = Mix_LoadWAV("res/sfx/hit.wav");
    Clicksound2 = Mix_LoadWAV("res/sfx/Click2.wav");
    FALSEWAV = Mix_LoadWAV("res/sfx/false.wav");

    int selected_item = MENU_START_GAME;
    enum Difficulty selected_options;
    selected_options = EASY;
    int counter = 6 * 60; // 6 minutes

    SDL_TimerID timer_id = SDL_AddTimer(1000, timer_callback, &counter);
    SDL_Event event;
    bool quit = false;
    bool ignore_up_down_events = false;
    bool timer_started = false;
    char timer_text[32];
    livesrem = 100 / 9;
    int diffinco = 9;
    while (!quit)
    {
        

        // Initialize the game variables
        int incorrect_guesses = 0;
        int correct_guesses = 0;
        score = 0;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                if (ignore_up_down_events)
                {
                    break;
                }
                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    // scrolling up in main menu
                    selected_item--;
                    if (selected_item < MENU_START_GAME)
                    {
                        selected_item = MENU_QUIT;
                    }
                    Mix_PlayChannel(-1, Clicksound, 0);
                    break;
                case SDLK_DOWN:
                    // scrolling down in main menu
                    selected_item++;
                    if (selected_item > MENU_QUIT)
                    {
                        selected_item = MENU_START_GAME;
                    }
                    Mix_PlayChannel(-1, Clicksound, 0);

                    break;
                case SDLK_RETURN:
                    switch (selected_item)
                    {
                    case MENU_START_GAME:
                        Mix_PlayChannel(-1, HIT, 0);
                        // Handle start game
                        printf("nop");
                        s:
                        int word_index = rand() % WORD_LIST_SIZE;
                        char *word = word_list[word_index];
                        int word_length = strlen(word);
                        char incorrect_guesses_letters[9];
                        char correct_guesses_letters[100];
                        memset(incorrect_guesses_letters, 0, sizeof(incorrect_guesses_letters));
                        memset(correct_guesses_letters, 0, sizeof(correct_guesses_letters));

                        // Initialize the game variables
                        int correct_guesses = 0;
                        bool* guessed_letterss = malloc(word_length * sizeof(bool));
                        memset(guessed_letterss, false,word_length * sizeof(bool));
                        set = 0;
                        while (set == 0)
                        {

                            SDL_RenderPresent(renderer);
                            while (SDL_PollEvent(&event))
                            {
                                switch (event.type)
                                {
                                case SDL_QUIT:
                                    quit = true;
                                    set = -1;
                                    break;
                                case SDL_KEYDOWN:
                                    switch (event.key.keysym.sym)
                                    {
                                    case SDLK_r:
                                        if (counter <= 0 || check_game_over(guessed_letterss, word_length) || incorrect_guesses >= diffinco)
                                        {
                                            set = 4; // exit options menu
                                            ignore_up_down_events = false;
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
                                                        guessed_letterss[i] = true;
                                                        found_letter = true;
                                                    }
                                                }

                                                // If the letter was not found in the word, add it to the list of incorrect guesses
                                                if (!found_letter && !check_game_over(guessed_letterss, word_length))
                                                {
                                                    incorrect_guesses_letters[incorrect_guesses] = letter;
                                                    incorrect_guesses++;
                                                    Mix_PlayChannel(-1, FALSEWAV, 0);

                                                    score -= livesrem;
                                                    if(score <= 0){
                                                        score = 0;
                                                    }
                                                }
                                                else if (found_letter && !check_game_over(guessed_letterss, word_length))
                                                {
                                                    correct_guesses_letters[correct_guesses] = letter;
                                                    correct_guesses++;
                                                    score += livesrem;
                                                }

                                            }
                                        }
                                    }

                                    break;
                                }
                            }

                            if (counter > 0 && !check_game_over(guessed_letterss, word_length) && incorrect_guesses < diffinco)
                            {
                                sprintf(timer_text, "%02d:%02d", counter / 60, counter % 60);
                                draw_txt(font28, timer_text, 0, 0);
                                score_txt(font28, score, WINDOW_WIDTH - 170, 0);
                                draw_txt_g(font28, word, guessed_letterss);
                                draw_tries(font28, selected_options, incorrect_guesses);
                                drawHangman(incorrect_guesses, selected_options);
                                SDL_Delay(16);
                            }
                            else
                            {
                                if (counter <= 0)
                                {
                                    sprintf(timer_text, "Time's up!");
                                    free(guessed_letterss);
                                }
                                else if (check_game_over(guessed_letterss, word_length))
                                {
                                    free(guessed_letterss);
                                    goto s;
                                }
                                else if (incorrect_guesses >= diffinco)
                                {
                                    sprintf(timer_text, "Out of tries!");
                                    free(guessed_letterss);
                                }

                                draw_txt(font38, timer_text, 100, 100);
                                score_txt(font38, score, WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT / 2);
                            }
                            
                        }
                        
                        break;
                    case MENU_OPTIONS:
                        // handle options
                        Mix_PlayChannel(-1, HIT, 0);
                        set = 1;
                        ignore_up_down_events = true;
                        while (set == 1)
                        {
                            draw_options(font38, selected_options); // draw the options menu and highlight the selected difficulty
                            SDL_RenderPresent(renderer);
                            while (SDL_PollEvent(&event))
                            {
                                switch (event.type)
                                {
                                case SDL_QUIT:
                                    quit = true;
                                    set = -1;
                                    break;
                                case SDL_KEYDOWN:
                                    switch (event.key.keysym.sym)
                                    {
                                    case SDLK_ESCAPE:
                                        set = 4; // exit options menu
                                        Mix_PlayChannel(-1, HIT, 0);
                                        ignore_up_down_events = false;
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
                                        // scrolling down in option menu
                                        selected_options++;
                                        if (selected_options > HARD)
                                        {
                                            selected_options = EASY;
                                        }
                                        Mix_PlayChannel(-1, Clicksound2, 0);

                                        break;
                                    case SDLK_RETURN:
                                        set = 4;
                                        Mix_PlayChannel(-1, HIT, 0);
                                        ignore_up_down_events = false;
                                        switch (selected_options)
                                        {
                                        case EASY:
                                            // Start game with Easy difficulty
                                            counter = 6 * 60;
                                            livesrem = 100 / 9;
                                            diffinco = 9;
                                            break;
                                        case MED:
                                            // Start game with Medium difficulty
                                            counter = 4 * 60;
                                            livesrem = 100 / 6;
                                            diffinco = 6;
                                            break;
                                        case HARD:
                                            // Start game with Hard difficulty
                                            counter = 2 * 60;
                                            livesrem = 100 / 3;
                                            diffinco = 3;
                                            break;
                                        }
                                        break;
                                    }
                                    break;
                                }
                            }
                        }

                        break;
                    case MENU_QUIT:
                        quit = 1;
                        break;
                    }
                    break;
                }
                break;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the menu

        if (set == MAIN_MENU)
        {
            draw_menu(font38, selected_item, selected_options);
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up and exit
    SDL_RemoveTimer(timer_id);
    Mix_FreeChunk(Clicksound);
    Mix_CloseAudio();
    Mix_Quit();
    TTF_CloseFont(font38);
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
    char text_difficulty[32];
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

    text_surface = TTF_RenderText_Blended(font38, "Quit", (selected_item == MENU_QUIT) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 320;
    text_rect.y = 250;
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
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect text_rect;

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the Difficulty tittle
    text_surface = TTF_RenderText_Blended(font38, "Difficulty Level", color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 230;
    text_rect.y = 50;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

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

    text_surface = TTF_RenderText_Blended(font38, med_text, (selected_options == MED) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 150;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    text_surface = TTF_RenderText_Blended(font38, hard_text, (selected_options == HARD) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 200;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void score_txt(TTF_Font *font, int score, int x, int y)
{

    char score_str[32];
    sprintf(score_str, "Score: %d", score);

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, score_str, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst_rect = {x, y, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &dst_rect.w, &dst_rect.h);
    SDL_RenderCopy(renderer, texture, NULL, &dst_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
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

    SDL_RenderPresent(renderer);
}

void draw_txt_g(TTF_Font *font, const char *text, bool *guessed_letters)
{

    SDL_Surface *surface = TTF_RenderText_Blended(font, "Word: ", (SDL_Color){255, 255, 255, 255});
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {250, 400, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    int x = 380;
    int y = 400;
    for (int i = 0; i < strlen(text); i++)
    {
        if (guessed_letters[i])
        {
            char letter[2] = {text[i], '\0'};
            surface = TTF_RenderText_Blended(font, letter, (SDL_Color){255, 255, 255, 255});
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            rect = (SDL_Rect){x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
        else
        {
            surface = TTF_RenderText_Blended(font, "_", (SDL_Color){255, 255, 255, 255});
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            rect = (SDL_Rect){x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
        x += 30;
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
    if (!font28 || !font38)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return;
    }
}