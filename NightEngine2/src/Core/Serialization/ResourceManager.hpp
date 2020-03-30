/*!
  @file ResourceManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ResourceManager
*/

#include "Core/Container/String.hpp"
#include "Graphics/Opengl/Texture.hpp"

#include "Core/EC/Handle.hpp"

//Forward Declaration
namespace Rendering
{
  class Model;
}

namespace NightEngine
{
  class ResourceManager
  {
    public:
    //TODO: Return by Handle instead of raw pointer

    //! @brief Static Function for Load and Cache Material File
    static void ClearAllData(void);

    //! @brief Static Function for Load and Cache Material File
    static EC::Handle<Rendering::Material> LoadMaterialResource(const Container::String& fileName);

    //! @brief Static Function refreshing material texture binding unit
    static void RefreshMaterialTextureUniforms();

    ////////////////////////////////////////////////////////////////
    
    //! @brief Static Function for Load and Cache Texture
    static Rendering::Texture* LoadTextureResource(const Container::String& filePath
      , Rendering::Texture::Channel channel = Rendering::Texture::Channel::RGB
      , Rendering::Texture::FilterMode filterMode = Rendering::Texture::FilterMode::LINEAR
      , Rendering::Texture::WrapMode wrapMode = Rendering::Texture::WrapMode::REPEAT
      , bool hdrImage = false);

    //! @brief Static Function for Load and Cache Model
    static Rendering::Model* LoadModelResource(const Container::String& filePath);
    
    static void PreloadModelsResourceAsync(const Container::Vector<Container::String>& filePaths);
  };
}