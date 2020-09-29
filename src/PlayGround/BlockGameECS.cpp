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
    for (std::shared_ptr<UActorComponent>& component_ptr: components) 
    {
        if (UActorComponent* componnet_raw_ptr = component_ptr.get())
        {
            componnet_raw_ptr->Update(DeltaTime);
        }
    }
}

void AActor::RemoveComponent(class UActorComponent* DelComponent)
{
    for (std::vector<std::shared_ptr<UActorComponent>>::iterator it = components.begin(); it != components.end(); ++it) 
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

    std::shared_ptr<T> NewComponent = std::make_shared<T>();

    components.push_back(NewComponent);

    GetWorld()->AddComponent(NewComponent);
}

template<typename T>
T* AActor::FindComponent()
{
    T* ret_comp = nullptr;

    for (std::shared_ptr<UActorComponent>& component: components) 
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

template<typename T>
void UWorld::AddComponent(std::shared_ptr<T> NewComp) 
{
    if (T* comp = NewComp.get()) 
    {
        ComponentMap[comp->GetComponentType()].emplace_back(NewComp);
    }
}

template<typename T>
void UWorld::FindComponentsOfType(FComponentList<T>& components)
{
    FComponentList<UActorComponent>& base_components = ComponentMap[T::GetStaticComponentType()];

    for (std::shared_ptr<UActorComponent> component: base_components) 
    {
        components.emplace_back(std::dynamic_pointer_cast<T>(component));
    }
}

// ------------------------------------

int BlockGameECS(int, char**) 
{
    std::shared_ptr<UWorld> world = std::make_shared<UWorld>();

    AActor actor(world);
    AActor actor2(world);

    actor.AddComponent<UTransformComponent>();
    actor.AddComponent<UMovementComponent>();
    actor.AddComponent<URendererComponent>();

    actor2.AddComponent<UTransformComponent>();

    UMovementComponent* move = actor.FindComponent<UMovementComponent>();

    std::cout << "Count of Actor's component: " << actor.CountOfComponent() << std::endl;    
    std::cout << "has MovementComponent: " << (move != nullptr) << std::endl;

    FComponentList<UTransformComponent> components;
    world->FindComponentsOfType<UTransformComponent>(components);
    std::cout << "Count of UTransformComponent: " << components.size() << std::endl;


    return 0;
}