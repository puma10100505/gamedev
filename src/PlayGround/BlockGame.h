#pragma once
#include "PlayGround.h"
#include "glm/glm.hpp"
#include <unordered_map>
#include <vector>
#include <functional>

extern "C" {
#include "SDL2/SDL.h"
}

// typedef std::function<void(SDL_Scancode)> TKeyboardEventDelegator;

using TKeyboardEventDelegator = std::function<void(const uint8_t*)>;

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

    virtual bool Initialize() { return true; }
    
    virtual void Start() {
        RegisterKeyboardEvent();
    }

    virtual void Update(float DeltaTime) {}
    virtual void Draw(SDL_Renderer* InRenderer);
    virtual void Destroy() {}
    inline void RegisterKeyboardEvent() {
        KeyboardEventDelegator = std::bind(&Actor::OnKeyboardEvent, this, std::placeholders::_1);
    }

public:
    virtual void OnKeyboardEvent(const uint8_t* KeyboardState){};

protected:
    SDL_Rect Geometry;  // Position and Size
    glm::vec2 Position; // Position, need sync to Geometry
    uint32_t InstanceID;

    TKeyboardEventDelegator KeyboardEventDelegator;
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

    virtual void Move(float Delta);

    virtual void Update(float DeltaTime) override;

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
    inline std::vector<Actor*>& GetAllActors() { return AllActors; }

    void Update(float DeltaTime);
    void Draw(SDL_Renderer* Renderer);
    void Destroy();

private:
    std::unordered_map<uint32_t, Character*> DynamicActors;
    std::unordered_map<uint32_t, Actor*> StaticActors;
    std::vector<Actor*> AllActors;
};

class Engine
{
public:
    Engine(int32_t Tick, int32_t SWidth = 500, int32_t SHeight = 800): 
        FixedTick(Tick), ScreenWidth(SWidth), ScreenHeight(SHeight) {}
    ~Engine() {}

public:
    void Initialize();
    void CreateWorld();
    void Start();
    void Run();
    void UpdateWorld(float DeltaTime);
    void DrawWorld();
    void Shutdown();
    
private:
    int32_t OnWindowEvent();
    void OnKeyboardEvent();

private:
    int32_t ScreenWidth;
    int32_t ScreenHeight;
    SDL_Window* Window = nullptr;
    SDL_Renderer* Renderer = nullptr;
    World GameWorld;
    uint32_t LastTickTime;
    int32_t FixedTick;
    bool Quit = false;
    SDL_Event WindowEvent;
};