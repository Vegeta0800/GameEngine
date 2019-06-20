
//EXTERNAL INCLUDES
//INTERNAL INCLUDES
#include "math/ra_mat4x4.h"

//Define the 4x4 identity matrix.
const Math::Mat4x4 Math::Mat4x4::identity =
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

//Define the 4x4 zero matrix.
const Math::Mat4x4 Math::Mat4x4::zero =
{
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};
