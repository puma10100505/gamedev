#include "BlockGame.h"
#include "PlayGround.h"
#include <iostream>
#include <algorithm>

void Actor::SetPosition(int x, int y) 
{
    Geometry.x = x;
    Geometry.y = y;
}

const glm::vec2& Actor::GetPosition() 
{
    Position.x = Geometry.x;
    Position.y = Geometry.y;

    return Position;
}

void Character::SetVelocity(float vx, float vy)
{
    Velocity.x = vx;
    Velocity.y = vy;
}

const glm::vec2& Character::GetVelocity() const 
{
    return Velocity;
}

void Actor::SetWidth(int w)
{
    Geometry.w = w;
}

const int32_t Actor::GetWidth() const 
{
    return Geometry.w;
}

void Actor::SetHeight(int h)
{
    Geometry.h = h;
}

const int32_t Actor::GetHeight() const 
{
    return Geometry.h;
}

void Actor::Draw(SDL_Renderer* InRenderer) 
{
    SDL_RenderFillRect(InRenderer, &Geometry);
}

bool Actor::IsContact(const Actor* InOther, SDL_Rect* OutIntersectRect)
{
    return InOther != nullptr && 
        SDL_IntersectRect(&(this->Geometry), &(InOther->Geometry), 
            OutIntersectRect);
}

void Character::Move(float Delta)
{
    glm::vec2 TargetPos = GetPosition() + Velocity * Delta;

    SetPosition(TargetPos.x, TargetPos.y);
}

void Character::Update(float DeltaTime) 
{
    Move(DeltaTime);
}

uint32_t World::CreatePadCharacter(int x, int y, int w, int h) 
{
    PadCharacter* Pad = new PadCharacter();
    Pad->SetPosition(x, y);
    Pad->SetWidth(w);
    Pad->SetHeight(h);

    uint32_t TempInstID = static_cast<uint32_t>(AllActors.size());
    Pad->SetInstanceID(TempInstID);

    AllActors.emplace_back(Pad);
    DynamicActors[TempInstID] = Pad;

    return TempInstID;
}

uint32_t World::CreateBallCharacter(int x, int y, int r) 
{
    BallCharacter* Ball = new BallCharacter();
    Ball->SetPosition(x, y);
    Ball->SetRadius(r);

    uint32_t TempInstID = static_cast<uint32_t>(AllActors.size());
    Ball->SetInstanceID(TempInstID);

    AllActors.emplace_back(Ball);
    DynamicActors[TempInstID] = Ball;

    return TempInstID;
}

uint32_t World::CreateWall(int x, int y, int w, int h) 
{
    WallActor* Wall = new WallActor();
    Wall->SetPosition(x, y);
    Wall->SetWidth(w);
    Wall->SetHeight(h);

    uint32_t TempInstID = static_cast<uint32_t>(AllActors.size());
    Wall->SetInstanceID(TempInstID);

    AllActors.emplace_back(Wall);
    StaticActors[TempInstID] = Wall;

    return TempInstID;
}

Actor* World::FindStaticActorByInstanceID(uint32_t InInstanceID) 
{
    if (StaticActors.find(InInstanceID) == StaticActors.end()) 
    {
        return nullptr;
    }

    return StaticActors[InInstanceID];
}

Character* World::FindDynamicActorByInstanceID(uint32_t InInstanceID)
{
    if (DynamicActors.find(InInstanceID) == DynamicActors.end()) 
    {
        return nullptr;
    }

    return DynamicActors[InInstanceID];
}

void World::Destroy() 
{
    for (Actor* ActorItem: AllActors) 
    {
        ActorItem->Destroy();

        delete ActorItem;

        ActorItem = nullptr;
    }
}

void Engine::Initialize() 
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "init sdl failed" << std::endl;
        return;
    }

    LastTickTime = SDL_GetTicks();

    Window = SDL_CreateWindow("Block Game", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ScreenWidth, ScreenHeight
        , SDL_WINDOW_ALLOW_HIGHDPI);
    if (Window == nullptr)
    {
        std::cout << "init window failed" << std::endl;
        return;
    }

    Renderer = SDL_CreateRenderer(Window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (Renderer == nullptr)
    {
        std::cout << "create renderer failed" << std::endl;
        return;
    }
}

void Engine::CreateWorld() 
{
    // TODO: Create actors
}

int32_t Engine::OnWindowEvent()
{
    if (SDL_PollEvent(&WindowEvent))
    {
        if (WindowEvent.type == SDL_QUIT)
        {
            std::cout << "SDL Quit" << std::endl;
            Quit = true;
            return -1;
        }
    }
}

void Engine::OnKeyboardEvent() 
{
    const uint8_t* KeyboardState = SDL_GetKeyboardState(nullptr);

    // TODO: Actor event manager for de-coupling
}

void Engine::Run() 
{
    while (!Quit) 
    {
        while (!SDL_TICKS_PASSED(SDL_GetTicks(), LastTickTime + FixedTick));

        float DeltaTime = (SDL_GetTicks() - LastTickTime) / 1000.f;

        LastTickTime = SDL_GetTicks();

        if (OnWindowEvent() < 0) continue;

        // Fool implementation, invoke every actor's keyboard process function
        for_each(GameWorld.GetAllActors().begin(), GameWorld.GetAllActors().end(), [](Actor* ActorItem) {
            ActorItem->OnKeyboardEvent(SDL_GetKeyboardState(nullptr));
        }); 

        // Game Logic
        UpdateWorld(DeltaTime);

        // Render the World
        DrawWorld(); 

        SDL_RenderPresent(Renderer);
    }
}

void Engine::Start() 
{
    for_each(GameWorld.GetAllActors().begin(), GameWorld.GetAllActors().end(), [](Actor* ActorItem) {
        ActorItem->Start();
    });
}

void Engine::UpdateWorld(float DeltaTime)
{
    GameWorld.Update(DeltaTime);
}

void Engine::DrawWorld()
{
    GameWorld.Draw(Renderer);
}

void Engine::Shutdown() 
{
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}