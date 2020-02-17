/*!
  @file ResourceManager.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of ResourceManager
*/
#include "Core/Logger.hpp"
#include "Core/Utility/Utility.hpp"

#include "Core/Serialization/ResourceManager.hpp"
#include "Core/Serialization/Serialization.hpp"

#include "Core/Container/MurmurHash2.hpp"
#include "Core/Container/Hashmap.hpp"

#include "Graphic/Opengl/Model.hpp"
#include "Graphic/Opengl/Material.hpp"

#include <mutex>
#include <future>

namespace Core
{
  using namespace Graphic;
  using namespace Container;

  template<typename T>
  static Container::Hashmap<U64, T>& GetHashMap()
  {
    static Container::Hashmap<U64, T> hashmap;
    return hashmap;
  }

  void ResourceManager::ClearAllData(void)
  {
    Debug::Log << Logger::MessageType::INFO
      << " ResourceManager:ClearAllData()\n";

    //TODO: CLear things
    auto& hashmap = GetHashMap<Material>();
    hashmap.clear();
    auto& hashmap2 = GetHashMap<Texture>();
    hashmap2.clear();
    auto& hashmap3 = GetHashMap<Model>();
    hashmap3.clear();
  }

  Graphic::Material* ResourceManager::LoadMaterialResource(const Container::String& fileName)
  {
    Container::Hashmap<U64, Material>& hashmap = GetHashMap<Material>();

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
    hashmap.insert({ key, Material() });
    auto& newMat = hashmap[key];

    Serialization::Deserialize(newMat
      , fileName
      , FileSystem::DirectoryType::Materials);

    return &newMat;
  }

  void ResourceManager::RefreshMaterialTextureUniforms()
  {
    Container::Hashmap<U64, Material>& hashmap = GetHashMap<Material>();

    for (auto& pair : hashmap)
    {
      pair.second.RefreshTextureUniforms();
    }
  }

  Texture* ResourceManager::LoadTextureResource(const Container::String& filePath
    , Texture::Channel channel, Texture::FilterMode filterMode
    , Texture::WrapMode wrapMode, bool hdrImage)
  {
    Container::Hashmap<U64, Texture>& hashmap = GetHashMap<Texture>();

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

  /////////////////////////////////////////////////////////////////

  Graphic::Model* ResourceManager::LoadModelResource(const Container::String& filePath)
  {
    Container::Hashmap<U64, Model>& hashmap = GetHashMap<Model>();

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

    Core::Utility::StopWatch stopWatch{ true };
    {
      //Generate new Model
      Model newModel{ filePath };
      hashmap.insert({ key, newModel });
    }
    stopWatch.Stop();
    Debug::Log << Logger::MessageType::INFO 
      << "Loaded Model: " << filePath << " [" << stopWatch.GetElapsedTimeMilli() << " ms]\n";

    return &(hashmap[key]);
  }

  static std::mutex g_modelsMutex;
  static void LoadModels_Task(Container::String filePath, U64 key)
  {
    Container::Hashmap<U64, Model>& hashmap = GetHashMap<Model>();

    //Load Mesh
    Model newModel{ filePath, false };

    std::lock_guard<std::mutex> guard(g_modelsMutex);
    Debug::Log << Logger::MessageType::INFO
      << "Loading Model: " << filePath << '\n';
    hashmap.insert({ key, newModel });
  }

  void ResourceManager::PreloadModelsResourceAsync(const Container::Vector<Container::String>& filePaths)
  {
    Container::Hashmap<U64, Model>& hashmap = GetHashMap<Model>();
    Container::Vector<std::future<void>> futures;

    Debug::Log << Logger::MessageType::INFO
      << "**************************************************************\n";
    Core::Utility::StopWatch stopWatch{ true };
    {
      //Launch Async Tasks
      for (auto filePath : filePaths)
      {
        //Convert to U64 Hash key
        U64 key = Container::ConvertToHash(filePath.c_str(), filePath.size());

        //Only load model that is unloaded
        auto it = hashmap.find(key);
        if (it == hashmap.end())
        {
          futures.push_back(std::async(std::launch::async, LoadModels_Task, filePath, key));
        }
      }

      for (int i = 0; i < futures.size(); ++i)
      {
        futures[i].wait();
      }
    }
    stopWatch.Stop();

    Debug::Log << Logger::MessageType::INFO
      << "Loaded Models: [" << stopWatch.GetElapsedTimeMilli() << " ms]\n";
    Debug::Log << Logger::MessageType::INFO
      << "**************************************************************\n";
  }
}