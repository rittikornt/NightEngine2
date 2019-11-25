/*!
  @file ResourceManager.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ResourceManager
*/

#include "Core/Container/String.hpp"
#include "Graphic/Opengl/Texture.hpp"

//Forward Declaration
namespace Graphic
{
  class Model;
}

namespace Core
{
  class ResourceManager
  {
    public:
    //TODO: Return by Handle instead of raw pointer

    //! @brief Static Function for Load and Cache Material File
    static Graphic::Material* LoadMaterialResource(const Container::String& fileName);
    
    //! @brief Static Function for Load and Cache Texture
    static Graphic::Texture* LoadTextureResource(const Container::String& filePath
      , Graphic::Texture::Channel channel = Graphic::Texture::Channel::RGB
      , Graphic::Texture::FilterMode filterMode = Graphic::Texture::FilterMode::LINEAR
      , Graphic::Texture::WrapMode wrapMode = Graphic::Texture::WrapMode::REPEAT
      , bool hdrImage = false);

    //! @brief Static Function for Load and Cache Model
    static Graphic::Model* LoadModelResource(const Container::String& filePath);
  };
}