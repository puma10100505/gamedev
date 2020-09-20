#pragma once

#include "PlayGround.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "macro.h"
#include "glm/glm.hpp"

class Actor 
{
public:
    enum State 
    {
        EActive,
        EPaused,
        EDead
    };

    Actor(class World* OwningWorld): world_(OwningWorld) {}
    virtual ~Actor() {}

    virtual void BeginPlay(){}
    void Update(float DeltaTime);
    void UpdateComponent(float DeltaTime);
    virtual void UpdateActor(float DeltaTime){}

    inline int32_t CountOfComponent() { return static_cast<int32_t>(components.size()); }
    void AddComponent(class ActorComponent* Component);
    void RemoveComponent(class ActorComponent* Component);
    
    template<typename T>
    T* FindComponent();

protected:
    GETSETPTR(class World, world);
    GETSETVAR(State, state, State::EActive);


    /* Transform */
    GETSETVAR(glm::vec2, position, glm::vec2(0, 0));
    GETSETVAR(float, scale, 1.f);
    GETSETVAR(float, rotation, 0.f);

    std::vector<class ActorComponent*> components;
    
};


/*--------------- Components --------------------*/
class ActorComponent
{
public:
    ActorComponent() {}
    virtual ~ActorComponent() {}

    virtual void Update(float DeltaTime){}
};

class TransformComponent: public ActorComponent
{
public: 
    TransformComponent(){}
    virtual ~TransformComponent(){}

    void Update(float DeltaTime) override {}
    
};

class MovementComponent: public ActorComponent 
{
public:
    MovementComponent(){}
    virtual ~MovementComponent() {}

    void Update(float DeltaTime) override {}
};

class RendererComponent: public ActorComponent
{
public:
    RendererComponent() {}
    virtual ~RendererComponent(){}

    void Update(float DeltaTime) override {}
};

/* --------------------------------------------- */


class World 
{
public:

private:

};

class Engine 
{
public:

private:

};