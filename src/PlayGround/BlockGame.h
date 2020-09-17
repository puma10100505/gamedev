#pragma once
#include "PlayGround.h"
#include "glm/glm.hpp"
#include <unordered_map>
#include <vector>

extern "C" {
#include "SDL2/SDL.h"
}

class Actor
{
public:
    Actor() {}
    virtual ~Actor() {}

    void SetPosition(int x, int y);
    const glm::vec2& GetPosition();

    void SetWidth(int w);
    const int32_t GetWidth() const;

    void SetHeight(int h);
    const int32_t GetHeight() const;

    bool IsContact(const Actor* InOther, SDL_Rect* OutIntersectRect);

    inline void SetInstanceID(uint32_t InID) { InstanceID = InID; }
    const uint32_t GetInstanceID() const { return InstanceID; }

    virtual void Draw(SDL_Renderer* InRenderer);

protected:
    SDL_Rect Geometry;  // Position and Size
    glm::vec2 Position; // Position, need sync to Geometry
    uint32_t InstanceID;
    
};

// Static Actor
class WallActor: public Actor 
{
public:
    WallActor(){}
    virtual ~WallActor(){}
};

// Dynamic Actor
class Character: public Actor 
{
public:
    Character() {}
    virtual ~Character(){}

public:
    void SetVelocity(float vx, float vy);
    const glm::vec2& GetVelocity() const;

    void Move(float Delta);

protected:
    glm::vec2 Velocity; // 2D Velocity
};


class BallCharacter: public Character 
{
public:
    BallCharacter() {}
    virtual ~BallCharacter(){}

public:
    inline void SetRadius(float r) { Geometry.w = Geometry.h = r; }
    inline const int32_t GetRadius() const { return Geometry.x; }
};

class PadCharacter: public Character 
{
public:
    PadCharacter(){}
    virtual ~PadCharacter(){}
};

class World
{
public:
    World(){}
    ~World(){}

public:
    void Initialize();

    uint32_t CreatePadCharacter(int x, int y, int w, int h);
    uint32_t CreateBallCharacter(int x, int y, int r);
    uint32_t CreateWall(int x, int y, int w, int h);

    Actor* FindStaticActorByInstanceID(uint32_t InInstanceID);
    Character* FindDynamicActorByInstanceID(uint32_t InInstanceID);

    void Destroy();

private:
    std::unordered_map<uint32_t, Character*> DynamicActors;
    std::unordered_map<uint32_t, Actor*> StaticActors;
    std::vector<Actor*> AllActors;
};

class Engine
{
public:
    Engine() {}
    ~Engine() {}

public:
    void Initialize();
    void Startup();

    void Shutdown();
    
private:

};