#include "PlayGround.h"
#include <iostream>
#include <vector>
#include "glm/glm.hpp"
extern "C" {
#include "SDL2/SDL.h"
}

int32_t SCREEN_WIDTH = 500;
int32_t SCREEN_HEIGHT = 800;
int32_t WallThinkness = 6;

struct STBall 
{
    float x;
    float y;
    float Radius;

    SDL_Rect Geometry;

    glm::dvec2 Velocity;

    void Reset() 
    {
        x = SCREEN_WIDTH / 2; 
        y = SCREEN_HEIGHT / 2;
        Radius = WallThinkness; 

        Geometry.x = x;
        Geometry.y = y;
        Geometry.w = Radius;
        Geometry.h = Radius;

        Velocity = glm::vec2{-200, -200};
    }

    STBall() 
    {
        Reset();
    }
};

struct STPad 
{
    float Width;
    float Height;
    float x;
    float y;
    float MoveSpeed;
    int32_t Direction;

    SDL_Rect Geometry;

    bool HitByBall(const STBall& InBall)
    {
        if ((InBall.Geometry.y >= y && InBall.Velocity.y > 0) && (InBall.Geometry.x >= x && InBall.Geometry.x <= x + Width - InBall.Radius))
        {
            return true;
        }

        return false;
    }

    STPad()
    {
        Width = 60;
        Height = WallThinkness;
        x = SCREEN_WIDTH / 2 - Width / 2;
        y = SCREEN_HEIGHT - 2 * Height - 60;
        MoveSpeed = 260.f;
        Direction = 0;

        Geometry.x = x;
        Geometry.y = y;
        Geometry.w = Width;
        Geometry.h = Height;
    }
};

int CreateSimpleSDL2Window(int, char**) 
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
    {
        std::cout << "init sdl failed" << std::endl;
        return -1;
    }    

    SDL_Window* MainWindow = SDL_CreateWindow("Hello sdl window", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    if (MainWindow == nullptr) 
    {
        std::cout << "create window failed" << std::endl;
        return -2;
    }

    SDL_Renderer* Renderer = SDL_CreateRenderer(
        MainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (Renderer == nullptr) 
    {
        std::cout << "create renderer failed" << std::endl;
        return -3;
    }

    SDL_Rect WallTop{0, 0, SCREEN_WIDTH, WallThinkness};
    SDL_Rect WallBottom{0, SCREEN_HEIGHT - WallThinkness, SCREEN_WIDTH, WallThinkness};
    SDL_Rect WallLeft{0, 0, WallThinkness, SCREEN_HEIGHT};
    SDL_Rect WallRight{SCREEN_WIDTH - WallThinkness, 0, WallThinkness, SCREEN_HEIGHT};

    struct STBall Ball;
    Ball.Radius = 12;
    Ball.Velocity.x = -100;
    Ball.Velocity.y = 200;
    
    std::vector<struct STBall> BallList;
    BallList.push_back(Ball);
    Ball.Radius = 6;
    Ball.Velocity.x = -200;
    Ball.Velocity.y = 300;

    BallList.push_back(Ball);
    
    struct STPad PadPlayer1;
    struct STPad PadPlayer2;

    PadPlayer2.x = SCREEN_WIDTH / 2 - PadPlayer2.Width / 2;
    PadPlayer2.y = WallThinkness + 60;
    PadPlayer2.Geometry.x = PadPlayer2.x;
    PadPlayer2.Geometry.y = PadPlayer2.y;

    uint32_t LastTick = SDL_GetTicks();

    // Loop
    SDL_Event WindowEvent;
    while (true) 
    {
        while (!SDL_TICKS_PASSED(SDL_GetTicks(), LastTick + 16));

        float DeltaTime = (SDL_GetTicks() - LastTick) / 1000.0f;
        LastTick = SDL_GetTicks();

        if (SDL_PollEvent(&WindowEvent)) 
        {
            if (WindowEvent.type == SDL_QUIT) 
            {
                std::cout << "SDL Quit" << std::endl;
                break;
            }
        }

        PadPlayer1.Direction = 0;   
        PadPlayer2.Direction = 0;

        // Keyboard event
        const uint8_t* KeyboardState = SDL_GetKeyboardState(nullptr);
        if (KeyboardState[SDL_SCANCODE_A]) 
        {
            PadPlayer1.Direction = -1;
        } 
        
        if (KeyboardState[SDL_SCANCODE_D])
        {
            PadPlayer1.Direction = 1;
        }
        
        if (KeyboardState[SDL_SCANCODE_J])
        {
            PadPlayer2.Direction = -1;
        }
        
        if (KeyboardState[SDL_SCANCODE_L])
        {
            PadPlayer2.Direction = 1;
        }
        
        std::cout << PadPlayer1.Direction << " " << PadPlayer1.MoveSpeed << std::endl;

        // Player1 Move
        PadPlayer1.x += PadPlayer1.Direction * PadPlayer1.MoveSpeed * DeltaTime;

        if (PadPlayer1.x <= WallThinkness) 
        {
            PadPlayer1.x = WallThinkness;
        }

        if (PadPlayer1.x >= SCREEN_WIDTH - PadPlayer1.Width - WallThinkness) 
        {
            PadPlayer1.x = SCREEN_WIDTH - PadPlayer1.Width - WallThinkness;
        }

        PadPlayer1.Geometry.x = PadPlayer1.x;

        
        // Player2 Move
        PadPlayer2.x += PadPlayer2.Direction * PadPlayer2.MoveSpeed * DeltaTime;

        if (PadPlayer2.x <= WallThinkness) 
        {
            PadPlayer2.x = WallThinkness;
        }

        if (PadPlayer2.x >= SCREEN_WIDTH - PadPlayer2.Width - WallThinkness) 
        {
            PadPlayer2.x = SCREEN_WIDTH - PadPlayer2.Width - WallThinkness;
        }

        PadPlayer2.Geometry.x = PadPlayer2.x;
        

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);

        // TODO: Draw Game World
        SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(Renderer, &WallTop);
        SDL_RenderFillRect(Renderer, &WallBottom);
        SDL_RenderFillRect(Renderer, &WallLeft);
        SDL_RenderFillRect(Renderer, &WallRight);

        // SDL_RenderFillRect(Renderer, &Ball.Geometry);
        for(const struct STBall& Ball: BallList)
        {
            SDL_RenderFillRect(Renderer, &Ball.Geometry);
        }

        SDL_RenderFillRect(Renderer, &PadPlayer1.Geometry);
        SDL_RenderFillRect(Renderer, &PadPlayer2.Geometry);

        for(struct STBall& Ball: BallList)
        {
            // Move the ball
            Ball.x += Ball.Velocity.x * DeltaTime;
            Ball.y += Ball.Velocity.y * DeltaTime;
            Ball.Geometry.x = Ball.x;
            Ball.Geometry.y = Ball.y;
        

            // Hit the top
            if (Ball.y < WallThinkness && Ball.Velocity.y < 0) 
            {
                // all.Velocity.y *= -1;
                // player2 defeated
                Ball.Reset();
            }

            // Hit the left
            if (Ball.x < WallThinkness && Ball.Velocity.x < 0) 
            {
                Ball.Velocity.x *= -1;
            }

            // Hit the right
            if (Ball.x >= SCREEN_WIDTH - WallThinkness && Ball.Velocity.x > 0)
            {
                Ball.Velocity.x *= -1;
            }

            SDL_Rect HitInfo;
            if (SDL_IntersectRect(&(Ball.Geometry), &(PadPlayer1.Geometry), &HitInfo)) 
            {
                Ball.Velocity.y *= -1;
            }

            if (SDL_IntersectRect(&(Ball.Geometry), &(PadPlayer2.Geometry), &HitInfo)) 
            {
                Ball.Velocity.y *= -1;
            }

            if (Ball.y >= SCREEN_HEIGHT - WallThinkness) 
            {
                // GameOver
                Ball.Reset();
            }
        }
        // ---------------------

        SDL_RenderPresent(Renderer);
    }

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(MainWindow);
    SDL_Quit();

    return 0;
}