/*!
  @file Cubemap.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Cubemap
*/
#pragma once

#include "Graphic/Shader.hpp"
#include "Graphic/Vertex.hpp"
#include "Graphic/VertexArrayObject.hpp"
#include "Graphic/Texture.hpp"
#include <vector>

namespace Graphic
{
  class Cubemap
  {
    public:
      //! @brief Constructor
      Cubemap(void): m_id(~(0)){};
      
      //! @brief Destructor
      ~Cubemap(void);

      //! @brief Initliaze with texture names
      void Init(std::vector<std::string>& fileNames, const std::string& vertexShader
        , const std::string& fragmentShader, Texture::Channel channel
        , bool generateMipmap = false);

      //! @brief Initliaze
      void Init(int width, int height
        , const std::string& vertexShader
        , const std::string& fragmentShader
        , Texture::Channel channel
        , Texture::FilterMode minFilter = Texture::FilterMode::LINEAR
        , Texture::FilterMode magFilter = Texture::FilterMode::LINEAR
        , bool generateMipmap = false);

      //! @brief Initliaze as depth cubemap
      void InitDepthCubemap(unsigned widthHeight);

      //! @brief Bind
      void Bind(void);

      //! @brief Unbind
      void Unbind(void);

      //! @brief Bind to texture unit
      void BindToTextureUnit(int index);

      //! @brief Draw this cubemap
      void Draw(void);

      //! @brief Get ID of the cubemap
      unsigned GetID(void) { return m_id; }

      //! @brief Get Shader of this Cubemap
      Shader& GetShader(void){return m_shader;}

      //! @brief Get cubemap filenames
      static std::vector<std::string> GetCubemapFileNames(std::string name);
    private:
    unsigned int      m_id;
    Shader            m_shader;
    VertexArrayObject m_vao;
  };
}