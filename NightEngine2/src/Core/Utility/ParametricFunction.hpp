/*!
  @file ParametricFunction.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ParametricFunction
*/
#pragma once
#include "glm/vec3.hpp"

namespace Core
{
  namespace Utility
  {
		namespace Parametric
		{
			using Vector3 = glm::vec3;
			float SmoothStart3(float t);
			float SmoothStart5(float t);
			float SmoothStop3(float t);
			float SmoothStop5(float t);

			float NormalizedBezier3(float b, float c, float t)
			{
				//A and D are start and end [0,1]
				float s = 1.0f - t;
				float t2 = t * t;
				float s2 = s * s;
				float t3 = t2 * t;
				return (3.0f * b * s2 * t) + (3.0f * c * s * t2) + (t3);
			}

			float Blend(float start, float end, float weightEnd, float t)
			{
				return start + (weightEnd * (end - start));
			}

			float SmoothStep3(float t)
			{
				return Blend(SmoothStart3(t), SmoothStop3(t), 0.5f, t);
			}

			float SmoothStep5(float t)
			{
				return Blend(SmoothStart5(t), SmoothStop5(t), 0.5f, t);
			}

			float SmoothStop2(float t)
			{
				float value = (1 - t);
				return 1 - (value * value);
			}

			float SmoothStop3(float t)
			{
				float value = (1 - t);
				return 1 - (value * value * value);
			}

			float SmoothStop4(float t)
			{
				float value = (1 - t);
				return 1 - (value * value * value * value);
			}

			float SmoothStop5(float t)
			{
				float value = (1 - t);
				return 1 - (value * value * value * value);
			}

			float SmoothStart2(float t)
			{
				return t * t;
			}

			float SmoothStart3(float t)
			{
				return t * t * t;
			}

			float SmoothStart4(float t)
			{
				return t * t * t * t;
			}

			float SmoothStart5(float t)
			{
				return t * t * t * t;
			}

			Vector3 Spiral(float t);

			float RangeMapTemplate(float value, float inStart, float inEnd
				, float outStart, float outEnd)
			{
				//Put into [0,1] range
				float o = value - inStart;  //[0, inEnd - inStart]
				o /= (inEnd - inStart);			//[0,1]

				//Apply Easing Function
				//o = o;

				//Back to normal Range
				o *= (outEnd - outStart); //[0, outRange]
				return o + outStart;
			}
		} // Parametric
  } // Utility
} // Core

