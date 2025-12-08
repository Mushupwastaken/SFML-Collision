# Simple SFML GJK & EPA Collision Support
#### The _intersects_ function returns the penetration vector when successful, allowing us to properly resolve collision.\
#### Example: The following C++ code:
```
ne::CircleColliderShape     colliderA(50.f);
ne::RectangleColliderShape  colliderB({20.f, 20.f});

std::optional penetration_vector = colliderA.intersects(colliderB);
if(penetration_vector != std::nullopt)
{
    colliderA.move(-penetration_vector.value());
}
```
