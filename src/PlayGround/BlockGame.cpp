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
    Pad->SetSpeedFactor(200);

    uint32_t TempInstID = static_cast<uint32_t>(AllActors.size());
    Pad->SetInstanceID(TempInstID);

    AllActors.emplace_back(Pad);
    DynamicActors[TempInstID] = Pad;

    Pad->SetWorld(this);

    return TempInstID;
}

uint32_t World::CreateBallCharacter(int x, int y, int r) 
{
    BallCharacter* Ball = new BallCharacter();
    Ball->SetPosition(x, y);
    Ball->SetRadius(r);
    Ball->SetOriginPosition(glm::vec2(x, y));
    Ball->SetVelocity(-200, -300);

    uint32_t TempInstID = static_cast<uint32_t>(AllActors.size());
    Ball->SetInstanceID(TempInstID);

    AllActors.emplace_back(Ball);
    DynamicActors[TempInstID] = Ball;

    Ball->SetWorld(this);

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

    Wall->SetWorld(this);

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

void World::Draw(SDL_Renderer* Renderer)
{
    for_each(AllActors.begin(), AllActors.end(), [&](Actor* ActorInst){
        ActorInst->Draw(Renderer);
    });
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
    // GameWorld.CreateBallCharacter()
    GameWorld.SetEngine(this);

    GameWorld.CreateBallCharacter(ScreenWidth / 2, ScreenHeight / 2, 6);
    
    GameWorld.CreatePadCharacter(ScreenWidth / 2, ScreenHeight - 60, 90, 6);
    
    GameWorld.CreateWall(0, 0, ScreenWidth, GameWorld.GetWallThinkness());
    GameWorld.CreateWall(0, ScreenHeight - GameWorld.GetWallThinkness(), 
        ScreenWidth, GameWorld.GetWallThinkness());
    GameWorld.CreateWall(0, 0, GameWorld.GetWallThinkness(), ScreenHeight);
    GameWorld.CreateWall(ScreenWidth - GameWorld.GetWallThinkness(), 0, 
        GameWorld.GetWallThinkness(), ScreenHeight);
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

    return 0;
}

void Engine::OnKeyboardEvent() 
{
    // TODO: Actor event manager for de-coupling
    for_each(GameWorld.GetAllActors().begin(), GameWorld.GetAllActors().end(), [](Actor* ActorItem) {
        const uint8_t* KS = SDL_GetKeyboardState(nullptr);
        std::cout << "after get keyboard stat: " << *KS << std::endl;
        ActorItem->OnKeyboardEvent(KS);
    }); 
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
        OnKeyboardEvent();

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
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Renderer);
    GameWorld.Draw(Renderer);
}

void Engine::Shutdown() 
{
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

void BallCharacter::Update(float DeltaTime) 
{
    Move(DeltaTime);

    const glm::vec2& NewPos = GetPosition();

    const World* CurrentWorld = GetWorld();
    const Engine* CurrentEngine = GetWorld()->GetEngine();

    if (NewPos.y < CurrentWorld->GetWallThinkness() && Velocity.y < 0)
    {
        Velocity.y *= -1;
    }

    if (NewPos.x < CurrentWorld->GetWallThinkness() && Velocity.x < 0)
    {
        Velocity.x *= -1;
    }

    if (NewPos.x >= CurrentEngine->GetScreenWidth() - CurrentWorld->GetWallThinkness() && 
        Velocity.x > 0)
    {
        Velocity.x *= -1;
    }

    if (NewPos.y >= CurrentEngine->GetScreenHeight() - CurrentWorld->GetWallThinkness())
    {
       Reset();
    }
}

void BallCharacter::Draw(SDL_Renderer* Renderer) 
{
    SDL_SetRenderDrawColor(Renderer, 255, 0, 255, 255);
    // SDL_RenderClear(Renderer);
    SDL_RenderFillRect(Renderer, &Geometry);
}


void PadCharacter::Update(float DeltaTime)
{
    Move(DeltaTime);

    const World* CurrentWorld = GetWorld();
    const Engine* CurrentEngine = GetWorld()->GetEngine();

    const glm::vec2& NewPos = GetPosition();

    std::cout << "pad new pos: " << NewPos.x << " " << NewPos.y << std::endl;

    if (NewPos.x <= CurrentWorld->GetWallThinkness()) 
    {
        SetPosition(CurrentWorld->GetWallThinkness(), NewPos.y);
    }

    int32_t RightBound = CurrentEngine->GetScreenWidth() - Geometry.w - CurrentWorld->GetWallThinkness();
    if (NewPos.x >= RightBound)
    {
        SetPosition(RightBound, NewPos.y);
    }
}

void PadCharacter::Draw(SDL_Renderer* Renderer)
{
    SDL_SetRenderDrawColor(Renderer, 0, 0, 255, 255);
    // SDL_RenderClear(Renderer);
    SDL_RenderFillRect(Renderer, &Geometry);    
}

void PadCharacter::OnKeyboardEvent(const uint8_t* KeyboardState)
{
    Direction = 0;

    std::cout << "entry of pad key board event" << *KeyboardState << std::endl;

    if (KeyboardState[SDL_SCANCODE_A]) 
    {
        Direction = -1;
    } 
    
    if (KeyboardState[SDL_SCANCODE_D])
    {
        Direction = 1;
    }

    Velocity.x = Direction * SpeedFactor;

    std::cout << "after keyboard event v.x: " << Velocity.x << std::endl; 
}

void WallActor::Draw(SDL_Renderer* Renderer) 
{
    SDL_SetRenderDrawColor(Renderer, 0, 255, 0, 255);
    // SDL_RenderClear(Renderer);
    SDL_RenderFillRect(Renderer, &Geometry);
}

void World::Update(float DeltaTime) 
{
    for_each(GetAllActors().begin(), GetAllActors().end(), [&](Actor* ActorItem) {
        ActorItem->Update(DeltaTime);
    });
}


int BlockGame(int, char**) 
{
    Engine GameEngine(16);

    GameEngine.Initialize();

    GameEngine.CreateWorld();

    GameEngine.Start();

    GameEngine.Run();

    GameEngine.Shutdown();

    return 0;
}