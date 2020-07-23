/*!
  @file Model.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Model
*/
#include "Graphics/Opengl/Model.hpp"
#include "Graphics/Opengl/Vertex.hpp"
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/Material.hpp"

#include "Core/Logger.hpp"
#include "Core/EC/Factory.hpp"

#include "Core/Serialization/FileSystem.hpp"

using namespace NightEngine;
using namespace NightEngine::EC;

namespace Rendering
{
  Model::Model(const std::string& path, bool allowPrint)
  {
    if (allowPrint)
    {
      Debug::Log << Logger::MessageType::INFO
        << "Loading Model: " << path << '\n';
    }

    LoadModel(path);
  }

  void Model::Draw(void)
  {
    for(size_t i=0; i < m_meshes.size(); ++i)
    {
      m_meshes[i].Draw();
    }
  }

  //*****************************************
  // Private Methods
  //*****************************************
  void Model::LoadModel(const std::string& path)
  {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path
    , aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    //Check for Load error
    if(scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
    || scene->mRootNode == nullptr)
    {
      Debug::Log << Logger::MessageType::ERROR_MSG 
      << "Assimp: " << importer.GetErrorString() << '\n';
      return;
    }

    //Save the model directory
    m_directory = path.substr(0, path.find_last_of('/') + 1);
    m_name = path.substr(path.find_last_of('/') + 1, path.size() - m_directory.size());
    ProcessAINode(scene->mRootNode, scene);
  }

  void Model::ProcessAINode(aiNode* rootNode, const aiScene* scene)
  {
    //Material indices to load (Per SubMesh)
    std::vector<int> materialIndices;
    materialIndices.reserve(scene->mNumMaterials);
    std::unordered_map<int, Handle<Material>> handleMap;

    //Vertices data
    std::vector<Vertex> vertices; 
    std::vector<unsigned> indices;
    std::vector<aiNode*> stack;

    aiNode* currNode = rootNode;
    while (currNode != nullptr)
    {
      //Get this Node's Mesh and Process it
      for (size_t i = 0; i < currNode->mNumMeshes; ++i)
      {
        aiMesh* mesh = scene->mMeshes[currNode->mMeshes[i]];
        AddMesh(mesh, vertices, indices);

        int matIndex = mesh->mMaterialIndex >= 0
          && mesh->mMaterialIndex < scene->mNumMaterials ? mesh->mMaterialIndex : -1;
        materialIndices.emplace_back(matIndex);
      }

      //Save all the children node to be processed later
      for (size_t i = 0; i < currNode->mNumChildren; ++i)
      {
        stack.emplace_back(currNode->mChildren[i]);
      }

      //Traverse next node in the stack
      if (stack.size() > 0)
      {
        currNode = stack[stack.size() - 1];
        stack.pop_back();
      }
      else
      {
        currNode = nullptr;
      }
    }

    // Load Materials
    m_validMaterialCount = 0;
    for (int i = 0; i < materialIndices.size(); ++i)
    {
      if (materialIndices[i] == -1
        || !AddMaterial(materialIndices[i], scene, handleMap))
      {
        m_materials.emplace_back();
      }
      else
      {
        ++m_validMaterialCount;
      }
    }
  }

  void Model::AddMesh(aiMesh* mesh
    ,std::vector<Vertex>& vertices, std::vector<unsigned>& indices)
  {
    vertices.clear();
    indices.clear();

    //Process Data into Vertex
    for(size_t i=0; i < mesh->mNumVertices; ++i)
    {
      Vertex vertex;

      //Position
      vertex.m_position.x = mesh->mVertices[i].x;
      vertex.m_position.y = mesh->mVertices[i].y;
      vertex.m_position.z = mesh->mVertices[i].z;
      
      //Normal
      if (mesh->mNormals != nullptr)
      {
        vertex.m_normal.x = mesh->mNormals[i].x;
        vertex.m_normal.y = mesh->mNormals[i].y;
        vertex.m_normal.z = mesh->mNormals[i].z;
      }
      
      //Texture Coordinate
      if(mesh->mTextureCoords[0])
      {
        vertex.m_texCoord.x = mesh->mTextureCoords[0][i].x;
        vertex.m_texCoord.y = mesh->mTextureCoords[0][i].y;
      }
      else
      {
        vertex.m_texCoord = glm::vec2(0.0f,0.0f);
      }

      //Tangent
      if (mesh->mTangents != nullptr)
      {
        vertex.m_tangent.x = mesh->mTangents[i].x;
        vertex.m_tangent.y = mesh->mTangents[i].y;
        vertex.m_tangent.z = mesh->mTangents[i].z;
      }

      vertices.push_back(vertex);
    }

    //Process Data into Indices
    for(size_t i=0; i < mesh->mNumFaces; ++i)
    {
      aiFace face = mesh->mFaces[i];
      for(size_t j = 0; j < face.mNumIndices; ++j)
      {
        indices.emplace_back(face.mIndices[j]);
      }
    }

    m_meshes.emplace_back(vertices, indices, false);
  }

  bool Model::AddMaterial(int index, const aiScene* scene
    , std::unordered_map<int, Handle<Material>>& handleMap)
  {
    std::vector<std::string> diffuseTextures;
    std::vector<std::string> normalsTextures;

    std::vector<std::string> roughnessTextures;
    std::vector<std::string> metallicTextures;
    std::vector<std::string> opacityTextures;

    bool added = false;
    if(index >= 0 && index < scene->mNumMaterials)
    {
      aiMaterial* material = scene->mMaterials[index];
      
      //This is specifically for loading sponza scene
      bool hasTexture = GetTextures(diffuseTextures, material, aiTextureType_DIFFUSE);
      hasTexture |= GetTextures(normalsTextures, material, aiTextureType_NORMALS);
      
      hasTexture |= GetTextures(roughnessTextures, material, aiTextureType_SPECULAR);
      hasTexture |= GetTextures(metallicTextures, material, aiTextureType_AMBIENT);
      hasTexture |= GetTextures(opacityTextures, material, aiTextureType_OPACITY);

      //Create Material Handle
      if(hasTexture)
      {
        //Only need to create Material for specifics index once
        auto it = handleMap.find(index);
        if (it != handleMap.end())
        {
          m_materials.emplace_back(it->second);
        }
        else
        {
          //Textures
          std::string blackTexPath = FileSystem::GetFilePath("Blank/000.png", FileSystem::DirectoryType::Textures);
          std::string whiteTexPath = FileSystem::GetFilePath("Blank/100.png", FileSystem::DirectoryType::Textures);
          std::string diffTexPath = diffuseTextures.size() > 0 ?
            diffuseTextures[0] : whiteTexPath;

          bool useNormal = normalsTextures.size() > 0;
          std::string normalTexPath = useNormal ?
            normalsTextures[0] : blackTexPath;

          std::string roughnessTexPath = roughnessTextures.size() > 0 ?
            roughnessTextures[0] : "";
          std::string metallicTexPath = metallicTextures.size() > 0 ?
            metallicTextures[0] : "";
          bool useOpacityMask = opacityTextures.size() > 0;
          std::string opacityTexPath = useOpacityMask ?
            opacityTextures[0] : whiteTexPath;

          {
            //Init material
            Handle<Material> handle = Factory::Create<Material>("Material");
            
            std::string name = "mat_" + m_name + "[" + std::to_string(index) + "]";
            handle.Get()->SetName(name);

            {
              handle->InitShader(DEFAULT_VERTEX_SHADER_PBR
                , DEFAULT_FRAG_SHADER_PBR);
              handle->InitPBRTexture(diffTexPath
                , useNormal, normalTexPath
                , roughnessTexPath, metallicTexPath, blackTexPath
                , useOpacityMask, opacityTexPath);
            }

            m_materials.emplace_back(handle);
            Debug::Log << "Created Material: " << name << '\n';

            // Save handle for this index
            // so we only need to create Material for specifics index once
            handleMap.insert({ index, handle });
          }
        }

        added = true;
      }
    }

    return added;
  }

  bool Model::GetTextures(std::vector<std::string>& textures
    , aiMaterial* mat, aiTextureType type)
  {
    for(size_t i=0; i < mat->GetTextureCount(type); ++i)
    {
      aiString str;
      mat->GetTexture(type, i, &str);

      //Assume that path in the material is relative, not absolute path
      std::string path{m_directory};
      path += str.C_Str();

      //Push to textures
      std::replace(path.begin(), path.end(), '\\', '/');
      textures.emplace_back(path);
    }

    return textures.size() > 0;
  }
}