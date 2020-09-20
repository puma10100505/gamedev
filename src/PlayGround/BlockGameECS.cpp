#include "BlockGameECS.h"
#include "PlayGround.h"
#include <typeinfo>

void Actor::Update(float DeltaTime) 
{
    UpdateComponent(DeltaTime);
    UpdateActor(DeltaTime);
}

void Actor::UpdateComponent(float DeltaTime) 
{
    for_each(components.begin(), components.end(), [&](ActorComponent* component) {
        component->Update(DeltaTime);
    });
}

void Actor::AddComponent(class ActorComponent* Component)
{
    components.emplace_back(Component);
}

void Actor::RemoveComponent(class ActorComponent* Component)
{
    for (std::vector<ActorComponent*>::iterator it = components.begin(); 
        it != components.end(); ++it) 
    {
        if (*it == Component) 
        {
            components.erase(it);
            break;
        }
    }
    
}

template<typename T>
T* Actor::FindComponent()
{
    ActorComponent* ret_comp = nullptr;

    for (const ActorComponent* component: components) 
    {
        if (typeid(component).name() == typeid(T).name())
        {
            ret_comp = const_cast<ActorComponent*>(component);
            break;
        }
    }

    return dynamic_cast<T*>(ret_comp);
}

// ------------------------------------

int BlockGameECS(int, char**) 
{
    Actor actor(nullptr);

    TransformComponent* trans_comp = new TransformComponent();
    MovementComponent* move_comp = new MovementComponent();

    actor.AddComponent(trans_comp);
    actor.AddComponent(move_comp);

    MovementComponent* move = actor.FindComponent<MovementComponent>();

    std::cout << actor.CountOfComponent() << std::endl;
    std::cout << (move == nullptr) << std::endl;

    return 0;
}