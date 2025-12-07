# SFML GJK & EPA Collision Support
### Derived from sf::Shape, the inherited class, **ColliderShape** adds _intersects_ to a derived sf::Shape

The _intersects_ function returns a penetration vector, allowing us to properly resolve collision with the following C++ code:
```
std::optional penetration_vector = colliderA.intersects(colliderB);
if(penetration_vector != std::nullopt)
{
    colliderA.move(-penetration_vector.value());
}
```
Note: In derived classes, _resolveCollision_ can and should be modified.
