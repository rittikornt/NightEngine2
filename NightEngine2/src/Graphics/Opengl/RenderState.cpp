/*!
  @file RenderState.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of RenderState
*/

#include "Graphics/Opengl/RenderState.hpp"
#include <glad/glad.h>

namespace Rendering
{
  namespace RenderSetup
  {
    void WriteStencilAlways(int refBits)
    {
      glStencilMask(refBits);  //Only write specific bit
      glStencilFunc(GL_ALWAYS, refBits, refBits); //Always pass
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  //Only replace if pass
    }

    void PassStencilIfBitNotSet(int refBits)
    {
      glStencilMask(0x00);  //Disable Write

      //Pass if (stencilVal & refBits) != (refBits & refBits)
      glStencilFunc(GL_NOTEQUAL, refBits, refBits);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }

    void PassStencilIfBitSet(int refBits)
    {
      glStencilMask(0x00);  //Disable Write

      //Pass if (stencilVal & refBits) == (refBits & refBits)
      glStencilFunc(GL_EQUAL, refBits, refBits);
      glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }
  }
}