#include "BlockGameECS.h"
#include "PlayGround.h"
#include <typeinfo>

void AActor::Update(float DeltaTime) 
{
    UpdateComponent(DeltaTime);
    UpdateActor(DeltaTime);
}

void AActor::UpdateComponent(float DeltaTime) 
{
    for (std::unique_ptr<UActorComponent>& component_ptr: components) 
    {
        if (UActorComponent* componnet_raw_ptr = component_ptr.get())
        {
            componnet_raw_ptr->Update(DeltaTime);
        }
    }
}

void AActor::RemoveComponent(class UActorComponent* DelComponent)
{
    for (std::vector<std::unique_ptr<UActorComponent>>::iterator it = components.begin(); it != components.end(); ++it) 
    {
        UActorComponent* comp = it->get();
        if (comp == nullptr) 
        {
            continue;
        }

        if (comp == DelComponent) 
        {
            components.erase(it);
            break;
        }
    }
}

template<typename T>
void AActor::AddComponent() 
{
    if (!GetWorld()) 
    {
        std::cout << "Not found world" << std::endl;
        return;
    }

    std::unique_ptr<T> NewComponent = std::make_unique<T>();
    
    //components.emplace_back(std::make_unique<T>());

    components.emplace_back(std::move(NewComponent));

    // UWorld* CurrWorld = GetWorld();
    
}

template<typename T>
T* AActor::FindComponent()
{
    T* ret_comp = nullptr;

    for (std::unique_ptr<UActorComponent>& component: components) 
    {        
        UActorComponent* actor_comp = component.get();
        if (typeid(*actor_comp).name() == typeid(T).name())
        {
            ret_comp = dynamic_cast<T*>(actor_comp);
            break;
        }
    }

    return ret_comp;
}

// ------------------------------------

int BlockGameECS(int, char**) 
{
    AActor actor(nullptr);

    actor.AddComponent<UTransformComponent>();
    actor.AddComponent<UMovementComponent>();
    actor.AddComponent<URendererComponent>();

    UMovementComponent* move = actor.FindComponent<UMovementComponent>();

    std::cout << actor.CountOfComponent() << std::endl;    
    std::cout << (move == nullptr) << std::endl;

    return 0;
}