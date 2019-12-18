
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "math/ra_vector2.h"

//Define the zero, up, down, left, right and scale Vector2.
const Math::Vec2 Math::Vec2::zero		= { 0.0f, 0.0f };  //Zero
const Math::Vec2 Math::Vec2::unit_x		= { 1.0f, 0.0f };  //Right
const Math::Vec2 Math::Vec2::unit_y		= { 0.0f, 1.0f };  //Up
const Math::Vec2 Math::Vec2::neg_unit_x = { -1.0f, 0.0f }; //Left
const Math::Vec2 Math::Vec2::neg_unit_y = { 0.0f, -1.0f }; //Down
const Math::Vec2 Math::Vec2::unit_scale = { 1.0f, 1.0f };  //Scale
