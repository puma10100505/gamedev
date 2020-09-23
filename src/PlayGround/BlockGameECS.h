#pragma once

#include "PlayGround.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "macro.h"
#include "glm/glm.hpp"


class UActorComponent;

class AActor 
{
public:
    enum EActorState 
    {
        EActive,
        EPaused,
        EDead
    };

    AActor(class UWorld* InWorld): World(InWorld) {
        std::cout << "constructor of Actor class" << std::endl;
    }

    virtual ~AActor() {
        std::cout << "destructor of Actor class" << std::endl;
    }

    virtual void BeginPlay(){}
    void Update(float DeltaTime);
    void UpdateComponent(float DeltaTime);
    virtual void UpdateActor(float DeltaTime){}

    inline int32_t CountOfComponent() { return components.size(); }
    
    template<typename T>
    inline void AddComponent(); // { components.emplace_back(std::make_unique<T>()); }

    void RemoveComponent(class UActorComponent* Component);
    
    template<typename T>
    T* FindComponent();

    inline UWorld* GetWorld() { return World.get(); }

public:
    std::shared_ptr<UWorld> World;
    GETSETVAR(EActorState, State, EActorState::EActive);

    /* Transform */
    GETSETVAR(glm::vec2, position, glm::vec2(0, 0));
    GETSETVAR(float, scale, 1.f);
    GETSETVAR(float, rotation, 0.f);

    std::vector<std::unique_ptr<class UActorComponent>> components;   
};


/*--------------- Components --------------------*/
class UActorComponent
{
public:
    static int32_t ComponentCategory;

    UActorComponent() {}
    virtual ~UActorComponent() {}

    virtual void Update(float DeltaTime) {}

    inline AActor* GetOwner() { return Owner.get(); }
    inline void SetOwner(AActor* OwnerActor) { Owner.reset(OwnerActor); }

protected:
    std::shared_ptr<AActor> Owner;
};

class UTransformComponent: public UActorComponent
{
public: 
    UTransformComponent(){}
    virtual ~UTransformComponent(){
        std::cout << "destructor of TransformComponent class" << std::endl;
    }

    void Update(float DeltaTime) override {}
    
};

class UMovementComponent: public UActorComponent 
{
public:
    UMovementComponent(){}
    virtual ~UMovementComponent() {}

    void Update(float DeltaTime) override {}
};

class URendererComponent: public UActorComponent
{
public:
    URendererComponent() {}
    virtual ~URendererComponent(){}

    void Update(float DeltaTime) override {}
};

/* --------------------------------------------- */


class UWorld 
{
public:
    UWorld(){}
    ~UWorld() {}
    
    void AddComponent();

private:
    std::unordered_map<uint32_t, std::vector<std::unique_ptr<UActorComponent>>> ComponentMap;
};

class Engine 
{
public:

private:

};