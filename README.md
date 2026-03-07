# Simple SFML GJK & EPA Collision Support
#### The _intersects_ / _resolveCollision_ function returns the penetration vector when successful, allowing us to properly resolve collision.
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
***
# Sample C++ Code (DOD ver.):
```cpp
//Initalizing
entt::registry registry;

auto entityA = registry.create();
collider.emplace<ne::Collider>(entityA, ne::shapes::Circle{50.0f});
collider.emplace<sf::Transformable>(entityA, sf::Transformable::Identity);

auto entityB = registry.create();
collider.emplace<ne::Collider>(entityB, ne::shapes::Circle{50.0f});
collider.emplace<sf::Transformable>(entityB, sf::Transformable::Identity);

//Collision code (NOTE: using a view is recommeneded)
auto* colliderA			=   registry.try_get<ne::Collider>(entityA);
auto* transformableA    =   registry.try_get<sf::Transformable>(entityA);
auto* colliderB         =   registry.try_get<ne::Collider>(entityB);
auto* transformableB    =   registry.try_get<sf::Transformable>(entityB);

if(!colliderA || !transformableA || !colliderB || !transformableB)
{
    std::cerr << "Failed to gather necessary Components!\n";
}
else
{
    if(const std::optional manifold = ne::findPenetration(*colliderA, *transformableA, *colliderB, *transformableB);)
    {
        transformableA->move(manifold->normal * -manifold->depth);
    }
}
```
