/*!
  @file ParametricFunction.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ParametricFunction
*/
#include "Core/Utility/ParametricFunction.hpp"
#include <math.h>

namespace NightEngine
{
	namespace Utility
	{
		namespace Parametric
		{

			Vector3 Spiral(float t)
			{
				return Vector3(t, t * cosf(t), t * sinf(t));
			}

		} // Parametric
	} // Utility
} // NightEngine

