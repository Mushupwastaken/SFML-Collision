# Simple SFML GJK & EPA Collision Support
#### The _intersects_ function returns the penetration vector when successful, allowing us to properly resolve collision.
# Sample C++ Code (class ver.):
```cpp
//Initalizing
ne::CircleColliderShape     colliderA(50.f);
ne::RectangleColliderShape  colliderB({20.f, 20.f});

//Collision code
std::optional penetration_vector = colliderA.intersects(colliderB);
if(penetration_vector.has_value())
{
    colliderA.move(-penetration_vector.value());
}
```

# Sample C++ Code (ECS ver.):
```cpp
//Initalizing
entt::registry registry;

auto entityA = registry.create();
collider.emplace<ColliderComponent>(50.f);
collider.emplace<TransformableComponent>();

auto entityB = registry.create();
collider.emplace<ColliderComponent>(50.f);
collider.emplace<TransformableComponent>();

//Collision code
auto* colliderA         =    colliderA.try_get<ColliderComponent>()
auto* transformableA    =    colliderA.try_get<TransformableComponent>()
auto* colliderB         =    colliderA.try_get<ColliderComponent>()
auto* transformableB    =    colliderA.try_get<TransformableComponent>()

if(!colliderA || !transformableA || !colliderB || !transformableB)
{
    std::cerr << "Failed to gather necessary Components!\n";
}
else
{
    std::optional penetration_vector = resolveCollision(colliderA, transformableA, colliderB, transformableB);
    if(penetration_vector.has_value())
    {
        transformableA.move(-penetration_vector.value());
    }
}
```
