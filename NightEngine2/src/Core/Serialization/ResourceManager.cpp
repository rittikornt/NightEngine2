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

#include "Graphics/Opengl/Model.hpp"
#include "Graphics/Opengl/Material.hpp"

#include "Core/EC/Factory.hpp"

#include <mutex>
#include <future>

namespace NightEngine
{
  using namespace Rendering;
  using namespace Container;

  template<typename T>
  static Container::Hashmap<U64, T>& GetContainer()
  {
    static Container::Hashmap<U64, T> hashmap;
    return hashmap;
  }

  void ResourceManager::ClearAllData(void)
  {
    Debug::Log << Logger::MessageType::INFO
      << " ResourceManager:ClearAllData()\n";

    //TODO: CLear things
    auto& hashmap = GetContainer<EC::Handle<Rendering::Material>>();
    hashmap.clear();
    auto& container1 = Factory::GetTypeContainer<Material>();
    container1.Clear();

    auto& hashmap2 = GetContainer<EC::Handle<Rendering::Texture>>();
    hashmap2.clear();
    auto& container2 = Factory::GetTypeContainer<Texture>();
    container2.Clear();

    auto& hashmap3 = GetContainer<Model>();
    hashmap3.clear();
  }

  EC::Handle<Rendering::Material> ResourceManager::LoadMaterialResource(const Container::String& fileName)
  {
    Container::Hashmap<U64, EC::Handle<Rendering::Material>>& hashmap = GetContainer< EC::Handle<Rendering::Material>>();

    //Generate unique key for each Material File
    Container::String newKeyStr{ fileName };

    //Convert to U64 Hash key
    U64 key = Container::ConvertToHash(newKeyStr.c_str(), newKeyStr.size());

    //Try lookup
    auto it = hashmap.find(key);
    if (it != hashmap.end() && it->second.IsValid())
    {
      return (it->second);
    }

    //Generate new Material
    EC::Handle<Rendering::Material> newHandle = Factory::Create<Material>("Material");
    hashmap.insert({ key, newHandle });

    //Deserialize the material file
    Serialization::Deserialize( *(newHandle.Get())
      , fileName
      , FileSystem::DirectoryType::Materials);

    //Set Name without extension
    std::string nameNoExt = fileName;
    FileSystem::RemoveExtension(nameNoExt);
    newHandle->SetName(nameNoExt);

    //Set FilePath
    Container::String filePath{ FileSystem::GetFilePath(fileName, FileSystem::DirectoryType::Materials) };
    newHandle->SetFilePath(filePath);

    return newHandle;
  }

  void ResourceManager::RefreshMaterialTextureUniforms()
  {
    /*Container::Hashmap<U64, EC::Handle<Rendering::Material>>& hashmap = GetContainer< EC::Handle<Rendering::Material>>();
    for (auto& pair : hashmap)
    {
      pair.second.Get()->RefreshTextureUniforms();
    }*/

    //Actually Traverse all material, Refreshing all texture binding index
    auto& materialContainer = Factory::GetTypeContainer<Material>();
    auto it = materialContainer.GetIterator();
    while (!it.IsEnd())
    {
      it.Get()->RefreshTextureUniforms();
      it.Next();
    }
  }

  /////////////////////////////////////////////////////////////////////////////

  static EC::Handle<Rendering::Texture>  g_blackTexture;
  static EC::Handle<Rendering::Texture>  g_whiteTexture;

  EC::Handle<Rendering::Texture> ResourceManager::LoadTextureResource(const Container::String& filePath
    , Texture::Format channel, Texture::FilterMode filterMode
    , Texture::WrapMode wrapMode, bool hdrImage)
  {
    Container::Hashmap<U64, EC::Handle<Rendering::Texture>>& hashmap = GetContainer<EC::Handle<Rendering::Texture>>();

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
      return (it->second);
    }

    //Generate new Texture
    EC::Handle<Rendering::Texture> newHandle = Factory::Create<Texture>("Texture");
    if (hdrImage)
    {
      *(newHandle.Get()) = Texture::LoadHDRTexture(filePath
        , channel, filterMode, wrapMode);
    }
    else
    {
      *(newHandle.Get()) = Texture::LoadTexture(filePath
        , channel, filterMode, wrapMode);
    }
    hashmap.insert({ key, newHandle });

    return newHandle;
  }

  EC::Handle<Rendering::Texture> ResourceManager::GetBlackTexture(void)
  {
    if (!g_blackTexture.IsValid())
    {
      g_blackTexture = Texture::LoadTextureHandle(FileSystem::GetFilePath("Blank/000.png", FileSystem::DirectoryType::Textures)
        , Texture::Format::RGB);
    }
    return g_blackTexture;
  }

  EC::Handle<Rendering::Texture> ResourceManager::GetWhiteTexture(void)
  {
    if (!g_whiteTexture.IsValid())
    {
      g_whiteTexture = Texture::LoadTextureHandle(FileSystem::GetFilePath("Blank/100.png", FileSystem::DirectoryType::Textures)
        , Texture::Format::RGB);
    }
    return g_whiteTexture;
  }

  /////////////////////////////////////////////////////////////////

  Rendering::Model* ResourceManager::LoadModelResource(const Container::String& filePath)
  {
    Container::Hashmap<U64, Model>& hashmap = GetContainer<Model>();

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

    NightEngine::Utility::StopWatch stopWatch{ true };
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
    Container::Hashmap<U64, Model>& hashmap = GetContainer<Model>();

    //Load Mesh
    Model newModel{ filePath, false };

    std::lock_guard<std::mutex> guard(g_modelsMutex);
    Debug::Log << Logger::MessageType::INFO
      << "Loading Model: " << filePath << '\n';
    hashmap.insert({ key, newModel });
  }

  void ResourceManager::PreloadModelsResourceAsync(const Container::Vector<Container::String>& filePaths)
  {
    Container::Hashmap<U64, Model>& hashmap = GetContainer<Model>();
    Container::Vector<std::future<void>> futures;

    Debug::Log << Logger::MessageType::INFO
      << "**************************************************************\n";
    NightEngine::Utility::StopWatch stopWatch{ true };
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