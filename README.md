# Simple SFML GJK & EPA Collision Support
#### The _intersects_ / _resolveCollision_ function returns the penetration vector when successful, allowing us to properly resolve collision.
># Sample C++ Code (class ver.):
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
***
># Sample C++ Code (ECS ver.):
```cpp
//Initalizing
entt::registry registry;

auto entityA = registry.create();
collider.emplace<ne::ColliderComponent>(entityA, ne::Circle{50.f});
collider.emplace<ne::TransformableComponent>(entityA, ne::TransformableComponent::Identity);

auto entityB = registry.create();
collider.emplace<ne::ColliderComponent>(entityB, ne::Circle{50.f});
collider.emplace<ne::TransformableComponent>(entityB, ne::TransformableComponent::Identity);

//Collision code
auto* colliderA			=   registry.try_get<ne::ColliderComponent>(entityA);
auto* transformableA    =   registry.try_get<ne::TransformableComponent>(entityA);
auto* colliderB         =   registry.try_get<ne::ColliderComponent>(entityB);
auto* transformableB    =   registry.try_get<ne::TransformableComponent>(entityB);

if(!colliderA || !transformableA || !colliderB || !transformableB)
{
    std::cerr << "Failed to gather necessary Components!\n";
}
else
{
    std::optional penetration_vector = ne::resolveCollision(*colliderA, *transformableA, *colliderB, *transformableB);
    if(penetration_vector.has_value())
    {
        transformableA->move(-penetration_vector.value());
    }
}
```
