/*!
  @file ResourceManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ResourceManager
*/

#include "Core/Container/String.hpp"
#include "Graphics/Opengl/Texture.hpp"

#include "Core/EC/Handle.hpp"

//Forward Declaration
namespace NightEngine::Rendering::Opengl
{
  class Model;
  class Material;
}

namespace NightEngine
{
  class ResourceManager
  {
    public:
    //! @brief Static Function for Load and Cache Material File
    static void ClearAllData(void);

    //! @brief Static Function for Load and Cache Material File
    static EC::Handle<NightEngine::Rendering::Opengl::Material> LoadMaterialResource(const Container::String& fileName);

    //! @brief Static Function refreshing material texture binding unit
    static void RefreshMaterialTextureUniforms();

    ////////////////////////////////////////////////////////////////
    
    //! @brief Static Function for Load and Cache Texture
    static EC::Handle<NightEngine::Rendering::Opengl::Texture> LoadTextureResource(const Container::String& filePath
      , NightEngine::Rendering::Opengl::Texture::Format channel = NightEngine::Rendering::Opengl::Texture::Format::RGB
      , NightEngine::Rendering::Opengl::Texture::FilterMode filterMode = NightEngine::Rendering::Opengl::Texture::FilterMode::LINEAR
      , NightEngine::Rendering::Opengl::Texture::WrapMode wrapMode = NightEngine::Rendering::Opengl::Texture::WrapMode::REPEAT
      , bool hdrImage = false);

    static EC::Handle<NightEngine::Rendering::Opengl::Texture>  GetBlackTexture(void);

    static EC::Handle<NightEngine::Rendering::Opengl::Texture>  GetWhiteTexture(void);
    ////////////////////////////////////////////////////////////////

    //! @brief Static Function for Load and Cache Model
    static NightEngine::Rendering::Opengl::Model* LoadModelResource(const Container::String& filePath);
    
    static void PreloadModelsResourceAsync(const Container::Vector<Container::String>& filePaths);
  };
}