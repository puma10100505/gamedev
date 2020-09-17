#include "BlockGame.h"
#include "PlayGround.h"

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