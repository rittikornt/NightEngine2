/*!
  @file ResourceManager.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ResourceManager
*/

#include "Core/Serialization/ResourceManager.hpp"
#include "Core/Serialization/Serialization.hpp"

#include "Core/Container/MurmurHash2.hpp"
#include "Core/Container/Hashmap.hpp"

#include "Graphic/Model.hpp"
#include "Graphic/Material.hpp"

namespace Core
{
  using namespace Graphic;

  Graphic::Material* ResourceManager::LoadMaterialResource(const Container::String& fileName)
  {
    using namespace Container;
    static Container::Hashmap<U64, Material> hashmap;

    //Generate unique key for each Material File
    Container::String newKeyStr{ fileName };

    //Convert to U64 Hash key
    U64 key = Container::ConvertToHash(newKeyStr.c_str(), newKeyStr.size());

    //Try lookup
    auto it = hashmap.find(key);
    if (it != hashmap.end())
    {
      return &(it->second);
    }

    //Generate new Material
    Material newMat;

    Serialization::Deserialize(newMat
      , fileName
      , FileSystem::DirectoryType::Materials);

    hashmap.insert({ key, newMat });

    return &(hashmap[key]);
  }

  Texture* ResourceManager::LoadTextureResource(const Container::String& filePath
    , Texture::Channel channel, Texture::FilterMode filterMode
    , Texture::WrapMode wrapMode, bool hdrImage)
  {
    using namespace Container;
    static Container::Hashmap<U64, Texture> hashmap;

    //Generate unique key for each Texture Setting
    Container::String newKeyStr{ filePath };
    newKeyStr += std::to_string(static_cast<unsigned>(channel));
    newKeyStr += std::to_string(static_cast<unsigned>(filterMode));
    newKeyStr += std::to_string(static_cast<unsigned>(wrapMode));
    //Convert to U64 Hash key
    U64 key = Container::ConvertToHash(newKeyStr.c_str(), newKeyStr.size());

    //Try lookup
    auto it = hashmap.find(key);
    if (it != hashmap.end())
    {
      return &(it->second);
    }

    //Generate new Texture
    if (hdrImage)
    {
      Texture newTexture = Texture::LoadHDRTexture(filePath
        , channel, filterMode, wrapMode);
      hashmap.insert({ key, newTexture });
    }
    else
    {
      Texture newTexture = Texture::LoadTexture(filePath
        , channel, filterMode, wrapMode);
      hashmap.insert({ key, newTexture });
    }

    return &(hashmap[key]);
  }

  Graphic::Model* ResourceManager::LoadModelResource(const Container::String & filePath)
  {
    using namespace Container;
    static Container::Hashmap<U64, Model> hashmap;

    //Generate unique key for each Texture Setting
    Container::String newKeyStr{ filePath };

    //Convert to U64 Hash key
    U64 key = Container::ConvertToHash(newKeyStr.c_str(), newKeyStr.size());

    //Try lookup
    auto it = hashmap.find(key);
    if (it != hashmap.end())
    {
      return &(it->second);
    }

    //Generate new Model
    Model newModel{ filePath };
    hashmap.insert({ key, newModel });

    return &(hashmap[key]);
  }

}