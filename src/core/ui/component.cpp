#include "core/ui/component.hpp"

UIComponent* UIComponent::FindTarget(Vector2 point){
    if (HitTest(point)) return this;
    return nullptr;
}
