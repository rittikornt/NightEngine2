/*!
  @file Cubemap.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Cubemap
*/
#pragma once

#include "Graphics/Opengl/Shader.hpp"
#include "Graphics/Opengl/Vertex.hpp"
#include "Graphics/Opengl/VertexArrayObject.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include <vector>

namespace NightEngine::Rendering::Opengl
{
  class Cubemap
  {
    public:
      //! @brief Constructor
      Cubemap(void): m_id(~(0u)){};
      
      //! @brief Destructor
      ~Cubemap(void);

      //! @brief Initliaze with texture names
      void Init(std::vector<std::string>& fileNames, const std::string& vertexShader
        , const std::string& fragmentShader, Texture::Format channel
        , bool generateMipmap = false);

      //! @brief Initliaze
      void Init(int width, int height
        , const std::string& vertexShader
        , const std::string& fragmentShader
        , Texture::Format channel
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

      //! @brief Refresh Texture Uniforms binding unit
      void RefreshTextureUniforms(void);

      //! @brief Get cubemap filenames
      static std::vector<std::string> GetCubemapFileNames(std::string name);
    private:
    unsigned int      m_id;
    Shader            m_shader;
    VertexArrayObject m_vao;
  };
}