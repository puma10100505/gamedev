#pragma once

#include "PlayGround.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include "macro.h"
#include "glm/glm.hpp"

const static int CT_TRANSFORM_COMPONENT = 0x01;

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

    AActor(std::shared_ptr<class UWorld>& InWorld) {
        World = InWorld;
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

    std::vector<std::shared_ptr<class UActorComponent>> components;   
};

#define UACTORCOMPONENT_TYPE(type)                                                              \
public:                                                                                         \
    static inline int32_t GetStaticComponentType() { return type; }                             \
    virtual int32_t GetComponentType() const override { return GetStaticComponentType(); }        

/*--------------- Components --------------------*/
class UActorComponent
{
public:
    UActorComponent() {}
    virtual ~UActorComponent() {}

    virtual void Update(float DeltaTime) {}

    inline AActor* GetOwner() { return Owner.get(); }
    inline void SetOwner(AActor* OwnerActor) { Owner.reset(OwnerActor); }

    virtual int32_t GetComponentType() const { return 0; }

protected:
    std::shared_ptr<AActor> Owner;
};

class UTransformComponent: public UActorComponent
{
    UACTORCOMPONENT_TYPE(CT_TRANSFORM_COMPONENT)
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

template<typename T>
using FComponentList = std::vector<std::shared_ptr<T>>;

template<typename T>
using FWorldComponentMap = std::unordered_map<uint32_t, FComponentList<T>>;


class UWorld 
{
    static uint32_t ActorId;
public:
    UWorld(){}
    ~UWorld() {}
    
    template<typename T>
    void AddComponent(std::shared_ptr<T> NewComp);

    template<typename T>
    void FindComponentsOfType(FComponentList<T>& components);

    void Update();
    void Initialize();
    void Destroy();

private:
    FWorldComponentMap<UActorComponent> ComponentMap;
    std::unordered_map<uint32_t, std::weak_ptr<AActor>> ActorMap;
};

class Engine 
{
public:

private:

};