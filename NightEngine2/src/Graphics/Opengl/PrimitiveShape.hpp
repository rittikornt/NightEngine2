/*!
  @file PrimitiveShape.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of PrimitiveShape
*/
#pragma once

#include "Graphics/Opengl/Vertex.hpp"

namespace NightEngine::Rendering::Opengl
{
  //! @brief Namespace for containing pre-defined vertices,indices
  namespace PrimitiveShape
  {

    //! @brief Cube Primitive
    namespace Cube
    {
      namespace Textured
      {
        static const std::vector<Vertex> texturedCubeVertices{
          // positions                                      // texture Coords
          { { -0.5f, -0.5f, -0.5f }  ,{ 0.0f,  0.0f, -1.0f }  ,{ 0.0f, 0.0f }  },
        { { 0.5f, -0.5f, -0.5f }  ,{ 0.0f,  0.0f, -1.0f }   ,{ 1.0f, 0.0f }   },
        { { 0.5f,  0.5f, -0.5f }  ,{ 0.0f,  0.0f, -1.0f }   ,{ 1.0f, 1.0f }   },
        { { 0.5f,  0.5f, -0.5f }  ,{ 0.0f,  0.0f, -1.0f }   ,{ 1.0f, 1.0f }   },
        { { -0.5f,  0.5f, -0.5f } ,{ 0.0f,  0.0f, -1.0f }  ,{ 0.0f, 1.0f }    },
        { { -0.5f, -0.5f, -0.5f } ,{ 0.0f,  0.0f, -1.0f }  ,{ 0.0f, 0.0f }    },

        { { -0.5f, -0.5f,  0.5f } ,{ 0.0f,  0.0f, 1.0f }  ,{ 0.0f, 0.0f }     },
        { { 0.5f, -0.5f,  0.5f }  ,{ 0.0f,  0.0f, 1.0f }   ,{ 1.0f, 0.0f }    },
        { { 0.5f,  0.5f,  0.5f }  ,{ 0.0f,  0.0f, 1.0f }   ,{ 1.0f, 1.0f }    },
        { { 0.5f,  0.5f,  0.5f }  ,{ 0.0f,  0.0f, 1.0f }   ,{ 1.0f, 1.0f }    },
        { { -0.5f,  0.5f,  0.5f } ,{ 0.0f,  0.0f, 1.0f }  ,{ 0.0f, 1.0f }     },
        { { -0.5f, -0.5f,  0.5f } ,{ 0.0f,  0.0f, 1.0f }  ,{ 0.0f, 0.0f }     },
                                                                             
        { { -0.5f,  0.5f,  0.5f } ,{ -1.0f,  0.0f,  0.0f }  ,{ 1.0f, 0.0f }   },
        { { -0.5f,  0.5f, -0.5f } ,{ -1.0f,  0.0f,  0.0f }  ,{ 1.0f, 1.0f }   },
        { { -0.5f, -0.5f, -0.5f } ,{ -1.0f,  0.0f,  0.0f }  ,{ 0.0f, 1.0f }   },
        { { -0.5f, -0.5f, -0.5f } ,{ -1.0f,  0.0f,  0.0f }  ,{ 0.0f, 1.0f }   },
        { { -0.5f, -0.5f,  0.5f } ,{ -1.0f,  0.0f,  0.0f }  ,{ 0.0f, 0.0f }   },
        { { -0.5f,  0.5f,  0.5f } ,{ -1.0f,  0.0f,  0.0f }  ,{ 1.0f, 0.0f }   },
                                                                              
        { { 0.5f,  0.5f,  0.5f }  ,{ 1.0f,  0.0f,  0.0f }   ,{ 1.0f, 0.0f }   },
        { { 0.5f,  0.5f, -0.5f }  ,{ 1.0f,  0.0f,  0.0f }   ,{ 1.0f, 1.0f }   },
        { { 0.5f, -0.5f, -0.5f }  ,{ 1.0f,  0.0f,  0.0f }   ,{ 0.0f, 1.0f }   },
        { { 0.5f, -0.5f, -0.5f }  ,{ 1.0f,  0.0f,  0.0f }   ,{ 0.0f, 1.0f }   },
        { { 0.5f, -0.5f,  0.5f }  ,{ 1.0f,  0.0f,  0.0f }   ,{ 0.0f, 0.0f }   },
        { { 0.5f,  0.5f,  0.5f }  ,{ 1.0f,  0.0f,  0.0f }   ,{ 1.0f, 0.0f }   },
                                                                              
        { { -0.5f, -0.5f, -0.5f } ,{ 0.0f, -1.0f,  0.0f }  ,{ 0.0f, 1.0f }    },
        { { 0.5f, -0.5f, -0.5f }  ,{ 0.0f, -1.0f,  0.0f }   ,{ 1.0f, 1.0f }   },
        { { 0.5f, -0.5f,  0.5f }  ,{ 0.0f, -1.0f,  0.0f }   ,{ 1.0f, 0.0f }   },
        { { 0.5f, -0.5f,  0.5f }  ,{ 0.0f, -1.0f,  0.0f }   ,{ 1.0f, 0.0f }   },
        { { -0.5f, -0.5f,  0.5f } ,{ 0.0f, -1.0f,  0.0f }  ,{ 0.0f, 0.0f }    },
        { { -0.5f, -0.5f, -0.5f } ,{ 0.0f, -1.0f,  0.0f }  ,{ 0.0f, 1.0f }    },
                                                                              
        { { -0.5f,  0.5f, -0.5f } ,{ 0.0f,  1.0f,  0.0f }  ,{ 0.0f, 1.0f }    },
        { { 0.5f,  0.5f,  -0.5f } ,{ 0.0f,  1.0f,  0.0f }  ,{ 1.0f, 1.0f }    },
        { { 0.5f,  0.5f,  0.5f }  ,{ 0.0f,  1.0f,  0.0f }   ,{ 1.0f, 0.0f }   },
        { { 0.5f,  0.5f,  0.5f }  ,{ 0.0f,  1.0f,  0.0f }   ,{ 1.0f, 0.0f }   },
        { { -0.5f,  0.5f,  0.5f } ,{ 0.0f,  1.0f,  0.0f }  ,{ 0.0f, 0.0f }    },
        { { -0.5f,  0.5f, -0.5f } ,{ 0.0f,  1.0f,  0.0f }  ,{ 0.0f, 1.0f }    }
        };
        static const std::vector<unsigned> texturedCubeindices{
          // back
          2, 1, 0,
          5, 4, 3,
          // front
          6, 7, 8,
          9, 10, 11,
          // left
          12, 13, 14,
          15, 16, 17,
          // right
          20, 19, 18,
          23, 22, 21,
          // bottom
          24, 25, 26,
          27, 28, 29,
          // top
          32, 31, 30,
          35, 34, 33
        };
      }

      ////////////////////////////////////////////////////////

      static const std::vector<float> cubeVertices{
        -1.0f , -1.0f,  1.0f ,   // top right
         1.0f , -1.0f,  1.0f ,   // bottom right
         1.0f ,  1.0f,  1.0f ,   // bottom left
        -1.0f ,  1.0f,  1.0f ,   // top left 

        -1.0f, -1.0f, -1.0f ,  // top right
         1.0f,  -1.0f,-1.0f  ,  // bottom right
         1.0f,   1.0f,-1.0f  ,  // bottom left
        -1.0f,  1.0f, -1.0f  // top left 
      };

      static const std::vector<unsigned> cubeIndices{
        // front
        0, 1, 2,
        2, 3, 0,
        // right
        1, 5, 6,
        6, 2, 1,
        // back
        7, 6, 5,
        5, 4, 7,
        // left
        4, 0, 3,
        3, 7, 4,
        // bottom
        4, 5, 1,
        1, 0, 4,
        // top
        3, 2, 6,
        6, 7, 3
      };

      static AttributePointerInfo info{ 1, std::vector<unsigned>{3}
      ,  std::vector<size_t>{sizeof(float) * 3}, std::vector<bool>{false} };
    }

    //! @brief Quad Primitive
    namespace Quad
    {
      static const std::vector<float>texturedVertices
      {
          //Position, Normal, Texcoord
          1.0f,  1.0f, 0.0f  , 1.0f, 0.0f, 0.0f  , 1.0f, 1.0f ,   // top right
          1.0f, -1.0f, 0.0f  , 0.0f, 1.0f, 0.0f  , 1.0f, 0.0f ,   // bottom right
          -1.0f, -1.0f, 0.0f , 0.0f, 0.0f, 1.0f  , 0.0f, 0.0f ,   // bottom left
          -1.0f,  1.0f, 0.0f , 1.0f, 1.0f, 1.0f  , 0.0f, 1.0f      // top left 
      };

      static AttributePointerInfo texturedInfo{ 3
        , std::vector<unsigned>{3, 3, 2}
      , std::vector<size_t>{sizeof(glm::vec3)
        , sizeof(glm::vec3),sizeof(glm::vec2) }
      , std::vector<bool>{ false,false,false}
      , std::vector<int>{ 0, 0, 0} };

      //////////////////////////////////////////////////////

      static const std::vector<float> vertices{
         1.0f,  1.0f, 0.0f ,   // top right
         1.0f, -1.0f, 0.0f , // bottom right
        -1.0f, -1.0f, 0.0f , // bottom left
        -1.0f,  1.0f, 0.0f   // top left 
      };
      static const std::vector<unsigned> indices{  // note that we start from 0!
        3, 2, 1  // first Triangle
        ,3, 1, 0   // second Triangle
      };

      static AttributePointerInfo info{ 1, std::vector<unsigned>{3}
      ,  std::vector<size_t>{sizeof(float) * 3}, std::vector<bool>{false} };
    }

    //! @brief Quad Primitive
    namespace Triangle
    {
      static AttributePointerInfo texturedInfo{ 1
        , std::vector<unsigned>{2}
      , std::vector<size_t>{sizeof(glm::vec2)}
      , std::vector<bool>{ false}
      , std::vector<int>{ 0, 0, 0} };

      //////////////////////////////////////////////////////

      static const std::vector<float> vertices{
        //Position
        -1.0f,  -1.0f,   // bottom left
        1.0f, -1.0f,   // bottom right
        -1.0f, 1.0f  // top left
      };
      static const std::vector<unsigned> indices{
        0, 1, 2
      };

      static AttributePointerInfo info{ 1
        , std::vector<unsigned>{2}
      , std::vector<size_t>{sizeof(glm::vec2)}
      , std::vector<bool>{ false}
      , std::vector<int>{ 0 } };
    }
  }
}