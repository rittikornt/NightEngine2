/*!
  @file RenderBufferObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of RenderBufferObject
*/
#pragma once
#include <glad/glad.h>

namespace Rendering
{
  //Forward declaration
  class Texture;

  class RenderBufferObject
  {
    public:
      enum class Format : unsigned
      {
        DEPTH24 = GL_DEPTH_COMPONENT24,
        DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8
      };

      //! @brief Constructor
      RenderBufferObject(void):m_id(~(0)) {}
      
      //! @brief Destructor
      ~RenderBufferObject(void);

      //! @brief Initialize the Frame Buffer Object
      void Init(int width, int height
        , Format format = Format::DEPTH24_STENCIL8);

      //! @brief Set buffer setting
      void SetBuffer(int width, int height
        , Format format);

      //! @brief Get Object ID
      unsigned int GetID(void) const { return m_id; }

      //! @brief Get RBO format
      Format GetFormat(void) const { return m_format; }

      //! @brief Bind
      void Bind(void);

      //! @brief Unbind
      void Unbind(void);

    private:
      unsigned int m_id;
      Format m_format;
  };

}