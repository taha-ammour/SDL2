#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <SDL2/SDL_mixer.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 500

#define MAIN_MENU 4
#define MENU_START_GAME 0
#define MENU_OPTIONS 1
#define MENU_QUIT 2

int set = MAIN_MENU;
enum Difficulty
{
    EASY,
    MED,
    HARD
};
//function intialisation
Uint32 timer_callback(Uint32 interval, void *param);
void draw_menu(SDL_Renderer *renderer, TTF_Font *font, int selected_item);
void draw_txt(SDL_Renderer *renderer, TTF_Font *font, const char *timer_text, int x, int y);
void draw_options(SDL_Renderer *renderer, TTF_Font *font, int selected_options);
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
        return 1;
    if (TTF_Init() != 0)
    {
        SDL_Quit();
        return 1;
    }
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    SDL_Window *window = SDL_CreateWindow("Hangman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("fonts/Talk Comic.ttf", 38);
    if (!font)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Mix_Chunk* Clicksound = Mix_LoadWAV("res/sfx/Click.wav");
    int selected_item = MENU_START_GAME;
    enum Difficulty selected_options;
    selected_options = EASY;
    int counter = 6 * 60; // 5 minutes
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
                    selected_item--;
                    if (selected_item < MENU_START_GAME)
                    {
                        selected_item = MENU_QUIT;
                    }
                    Mix_PlayChannel(-1, Clicksound, 0);
                    break;
                case SDLK_DOWN:
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
                        // Handle start game
                        printf("nop");

                        set = 0;
                        break;
                    case MENU_OPTIONS:
                        set = 1;
                        ignore_up_down_events = true;
                        while (set == 1)
                        {
                            draw_options(renderer, font, selected_options); // draw the options menu and highlight the selected difficulty
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
                                        ignore_up_down_events = false;
                                        break;
                                    case SDLK_UP:
                                        selected_options--;
                                        if (selected_options < EASY || selected_options == -1)
                                        {
                                            selected_options = HARD;
                                        }
                                        break;
                                    case SDLK_DOWN:
                                        selected_options++;
                                        if (selected_options > HARD)
                                        {
                                            selected_options = EASY;
                                        }
                                        break;
                                    case SDLK_RETURN:
                                        set = 4;
                                        ignore_up_down_events = false;
                                        switch (selected_options)
                                        {
                                        case EASY:
                                            counter = 6*60;
                                            break;
                                        case MED:
                                            // Start game with Medium difficulty
                                            counter = 4*60;
                                            break;
                                        case HARD:
                                            // Start game with Hard difficulty
                                            counter = 2*60;
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
            draw_menu(renderer, font, selected_item);
        }
        else if (set == 0)
        {

            ignore_up_down_events = true;
            if (selected_item == MENU_START_GAME)
            {
                if (counter > 0)
                {
                    sprintf(timer_text, "%02d:%02d", counter / 60, counter % 60);
                    draw_txt(renderer, font, timer_text, 0, 0);
                }
                else
                {
                    sprintf(timer_text, "Time's up!");
                    draw_txt(renderer, font, timer_text, 100, 100);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up and exit
    SDL_RemoveTimer(timer_id);
    Mix_FreeChunk(Clicksound);
    Mix_CloseAudio();
    Mix_Quit();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

void draw_menu(SDL_Renderer *renderer, TTF_Font *font, int selected_item)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect text_rect;

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    text_surface = TTF_RenderText_Blended(font, "the Hangman Game", color);
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 170;
    text_rect.y = 50;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    // Draw the menu items with anti-aliasing
    text_surface = TTF_RenderText_Blended(font, "Start Game", (selected_item == MENU_START_GAME) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 250;
    text_rect.y = 150;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    text_surface = TTF_RenderText_Blended(font, "Options", (selected_item == MENU_OPTIONS) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 290;
    text_rect.y = 200;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);

    text_surface = TTF_RenderText_Blended(font, "Quit", (selected_item == MENU_QUIT) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 320;
    text_rect.y = 250;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void draw_txt(SDL_Renderer *renderer, TTF_Font *font, const char *timer_text, int x, int y)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Blended(font, timer_text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    SDL_Rect text_rect = {255, 255, 255, 0};
    SDL_QueryTexture(texture, NULL, NULL, &text_rect.w, &text_rect.h);
    text_rect.x = x;
    text_rect.y = y;
    SDL_RenderCopy(renderer, texture, NULL, &text_rect);
    SDL_DestroyTexture(texture);
}

void draw_options(SDL_Renderer *renderer, TTF_Font *font, int selected_options)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect text_rect;

    // Clear the renderer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the options menu with anti-aliasing
    text_surface = TTF_RenderText_Blended(font, "Difficulty Level", color);
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

    text_surface = TTF_RenderText_Blended(font, easy_text, (selected_options == EASY) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 100;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    text_surface = TTF_RenderText_Blended(font, med_text, (selected_options == MED) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 150;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    text_surface = TTF_RenderText_Blended(font, hard_text, (selected_options == HARD) ? color : (SDL_Color){128, 128, 128, 255});
    text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    text_rect.x = 50;
    text_rect.y = 200;
    text_rect.w = text_surface->w;
    text_rect.h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}
