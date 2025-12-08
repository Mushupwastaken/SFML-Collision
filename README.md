# Simple SFML GJK & EPA Collision Support
#### The _intersects_ function returns the penetration vector when successful, allowing us to properly resolve collision.
# Sample C++ Code:***
```cpp
ne::CircleColliderShape     colliderA(50.f);
ne::RectangleColliderShape  colliderB({20.f, 20.f});

std::optional penetration_vector = colliderA.intersects(colliderB);
if(penetration_vector.has_value())
{
    colliderA.move(-penetration_vector.value());
}
```
