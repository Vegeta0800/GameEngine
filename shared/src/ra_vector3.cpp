
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "math/ra_vector3.h"

//Define the zero, up, down, left, right, front, behind and scale Vector3.
const Math::Vec3 Math::Vec3::zero		= { 0.0f, 0.0f, 0.0f };		//Zero
const Math::Vec3 Math::Vec3::unit_x		= { 1.0f, 0.0f, 0.0f };		//Right
const Math::Vec3 Math::Vec3::unit_y		= { 0.0f, 1.0f, 0.0f };		//Up
const Math::Vec3 Math::Vec3::unit_z		= { 0.0f, 0.0f, 1.0f };		//Front
const Math::Vec3 Math::Vec3::neg_unit_x = { -1.0f, 0.0f, 0.0f };	//Left
const Math::Vec3 Math::Vec3::neg_unit_y = { 0.0f, -1.0f, 0.0f };	//Down 
const Math::Vec3 Math::Vec3::neg_unit_z = { 0.0f, 0.0f, -1.0f };	//Behind
const Math::Vec3 Math::Vec3::unit_scale = { 1.0f, 1.0f, 1.0f };		//Scale
