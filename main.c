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

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

SDL_Color white = {255, 255, 255};
SDL_Color black = {0, 0, 0};

TTF_Font *font38;
TTF_Font *font28;

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

#define MAIN_MENU 4
#define MENU_START_GAME 0
#define MENU_OPTIONS 1
#define MENU_QUIT 2
#define MAX_WORD_LENGTH 100

bool fmenu = true;
int set = MAIN_MENU;
int score = 100;

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
    float speed;
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
    while (!quit)
    {
        SDL_Event event;
        // Initialize the game variables
        int incorrect_guesses = 0;
        int correct_guesses = 0;
        score = 0;
        while (SDL_PollEvent(&event))
        {
            if (set == MAIN_MENU)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:

                    switch (event.key.keysym.sym)
                    {
                    case SDLK_UP:
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
                            set = MENU_START_GAME;
                            break;
                            case MENU_OPTIONS:
                            set = MENU_OPTIONS;
                            break;
                            case MENU_QUIT:
                            quit = true;
                            break;
                        }
                        break;
                    }

                    break;
                }
            }
            if (set == MENU_OPTIONS)
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                
                break;
                
                }
            }
            
        }
        if (set == MAIN_MENU)
        {
            draw_menu(font38, selected_item, selected_options);
            SDL_RenderPresent(renderer);
        }
    }

    // Clean up and exit
    SDL_DestroyTexture(startexture);
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

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    text_surface = TTF_RenderText_Blended(font38, med_text, (selected_options == MED) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 150;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

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
    char score_str[100];
    sprintf(score_str, "Score: %d", score);

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, score_str, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int text_width = surface->w;
    int text_height = surface->h;

    SDL_Rect dst_rect;
    dst_rect.x = x;
    dst_rect.y = y;
    dst_rect.w = text_width;
    dst_rect.h = text_height;

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
    if (!font28 || !font38)
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
        rocks[i].speed = rand() % 5 + 1;         // Random speed between 1 and 5
        rocks[i].size = rand() % 20 + 10;        // Random size between 10 and 30
    }
}
void updateRocks(Rock *rocks)
{
    for (int i = 0; i < 3; i++)
    {
        if (rocks[i].y < 0 || rocks[i].y >= WINDOW_HEIGHT)
        {
            // If rock reaches the left or right edge, reset its position
            rocks[i].x = (i % 2 == 0) ? 0 : 770;
            rocks[i].y = 0;
            rocks[i].speed = rand() % 5 + 1;
            rocks[i].size = rand() % 20 + 10;
        }
        else
        {
            rocks[i].y += rocks[i].speed;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        SDL_Rect rockRect = {rocks[i].x, rocks[i].y, 3 * rocks[i].size, 3 * rocks[i].size};
        SDL_RenderCopy(renderer, rockTexture, NULL, &rockRect);
    }
}