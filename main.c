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
void score_txt(TTF_Font *font, int score, int x, int y);
Uint32 timer_callback(Uint32 interval, void *param);
void draw_menu(TTF_Font *font38, int selected_item, int selected_options);
void draw_txt(TTF_Font *font38, const char *timer_text, int x, int y);
void draw_options(TTF_Font *font38, int selected_options);
void drawHangman(int wrongGuesses);
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

    // intialization of SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0)
        return 1;
    if (TTF_Init() != 0)
    {
        SDL_Quit();
        return 1;
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
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    font28 = TTF_OpenFont("fonts/Talk Comic.ttf", 28);
    font38 = TTF_OpenFont("fonts/Talk Comic.ttf", 38);
    if (!font28 || !font38)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    // Randomly select a word from the word list
    srand(time(NULL));
    int word_index = rand() % WORD_LIST_SIZE;
    char *word = word_list[word_index];
    int word_length = strlen(word);

    // Initialize the game variables
    int incorrect_guesses = 0;
    int correct_guesses = 0;
    char guessed_letters[26];

    // import sounds
    Clicksound = Mix_LoadWAV("res/sfx/Click.wav");
    HIT = Mix_LoadWAV("res/sfx/hit.wav");
    Clicksound2 = Mix_LoadWAV("res/sfx/Click2.wav");

    int selected_item = MENU_START_GAME;
    enum Difficulty selected_options;
    selected_options = EASY;
    int counter = 6 * 60; // 6 minutes
    livesrem = 100 / 9;
    SDL_TimerID timer_id = SDL_AddTimer(1000, timer_callback, &counter);
    SDL_Event event;
    bool quit = false;
    bool ignore_up_down_events = false;
    bool timer_started = false;
    while (!quit)
    {
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

                        set = 0;
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
                                            break;
                                        case MED:
                                            // Start game with Medium difficulty
                                            counter = 4 * 60;
                                            livesrem = 100 / 6;
                                            break;
                                        case HARD:
                                            // Start game with Hard difficulty
                                            counter = 2 * 60;
                                            livesrem = 100 / 3;
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
        char timer_text[32];
        if (set == MAIN_MENU)
        {
            draw_menu(font38, selected_item, selected_options);
        }
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
                        if (counter <= 0)
                        {
                            set = 4; // exit options menu
                            switch (selected_options)
                            {
                            case EASY:
                                // Start game with Easy difficulty
                                counter = 6 * 60;
                                livesrem = 100 / 9;
                                break;
                            case MED:
                                // Start game with Medium difficulty
                                counter = 4 * 60;
                                livesrem = 100 / 6;
                                break;
                            case HARD:
                                // Start game with Hard difficulty
                                counter = 2 * 60;
                                livesrem = 100 / 3;
                                break;
                            }
                            ignore_up_down_events = false;
                        }
                        break;
                    }
                    // FIXME:

                    char letter = event.key.keysym.sym;
                    if (letter >= 'a' && letter <= 'z')
                    {
                        // Check if the letter has already been guessed
                        int already_guessed = 0;
                        for (int i = 0; i < correct_guesses + incorrect_guesses; i++)
                        {
                            if (guessed_letters[i] == letter)
                            {
                                already_guessed = 1;
                                break;
                            }
                        }
                        if (!already_guessed)
                        {
                            // Check if the letter is in the word
                            int letter_found = 0;
                            for (int i = 0; i < word_length; i++)
                            {
                                if (word[i] == letter)
                                {
                                    letter_found = 1;
                                    break;
                                }
                            }
                            if (letter_found)
                            {
                                guessed_letters[correct_guesses + incorrect_guesses] = letter;
                                correct_guesses++;
                            }
                            else
                            {
                                guessed_letters[correct_guesses + incorrect_guesses] = letter;
                                incorrect_guesses++;
                            }
                        }
                    }
                    break;
                }
            }

            if (counter > 0)
            {
                sprintf(timer_text, "%02d:%02d", counter / 60, counter % 60);
                draw_txt(font28, timer_text, 0, 0);
                score_txt(font28, score, WINDOW_WIDTH - 170, 0);
                drawHangman(6);
                for (int i = 0; i < word_length; i++)
                {
                    int letter_guessed = 0;
                    for (int j = 0; j < correct_guesses; j++)
                    {
                        if (guessed_letters[j] == word[i])
                        {
                            letter_guessed = 1;
                            break;
                        }
                    }
                    if (letter_guessed)
                    {
                        // Draw the letter
                    }
                    else
                    {
                        // Draw a blank space
                    }
                }
            }
            else
            {
                sprintf(timer_text, "Time's up!");
                draw_txt(font38, timer_text, 100, 100);
                score_txt(font38, score, WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT / 2);
            }
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

void drawHangman(int wrongGuesses)
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
        if (wrongGuesses >= 3)
        {
            SDL_RenderDrawLine(renderer, 450 + i, 200, 500 + i, 150); // Right arm
        }
        if (wrongGuesses >= 4)
        {
            SDL_RenderDrawLine(renderer, 450 + i, 250, 400 + i, 300); // Left leg
        }
        if (wrongGuesses >= 5)
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

    SDL_RenderPresent(renderer);
}