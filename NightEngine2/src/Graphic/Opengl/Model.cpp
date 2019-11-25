/*!
  @file Model.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Model
*/
#include "Graphic/Opengl/Model.hpp"
#include "Graphic/Opengl/Vertex.hpp"
#include "Graphic/Opengl/Texture.hpp"

#include "Core/Logger.hpp"

using namespace Core;

namespace Graphic
{
  Model::Model(const std::string& path)
  {
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
    Debug::Log << Logger::MessageType::INFO
      << "Loading Model: " << path << '\n';

    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path
    , aiProcess_Triangulate | aiProcess_FlipUVs
      | aiProcess_CalcTangentSpace);

    //Check for Load error
    if(scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
    || scene->mRootNode == nullptr)
    {
      Debug::Log << Logger::MessageType::ERROR_MSG 
      << "Assimp: " << import.GetErrorString() << '\n';
      return;
    }

    //Save the model directory
    m_directory = path.substr(0, path.find_last_of('/') + 1);
    ProcessNode(scene->mRootNode, scene);
  }

  void Model::ProcessNode(aiNode* node, const aiScene* scene)
  {
    //Get this Node's Mesh and Process it
    for(size_t i=0; i < node->mNumMeshes; ++i)
    {
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      m_meshes.push_back(ProcessMesh(mesh, scene));
    }

    //Recursively process all the children node
    for(size_t i=0;i < node->mNumChildren; ++i)
    {
      ProcessNode(node->mChildren[i], scene);
    }
  }

  Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
  {
    std::vector<Vertex> vertices;
    std::vector<unsigned> indices;

    //Process Data into Vertex
    for(size_t i=0; i < mesh->mNumVertices; ++i)
    {
      Vertex vertex;

      //Position
      vertex.m_position.x = mesh->mVertices[i].x;
      vertex.m_position.y = mesh->mVertices[i].y;
      vertex.m_position.z = mesh->mVertices[i].z;
      
      //Normal
      vertex.m_normal.x = mesh->mNormals[i].x;
      vertex.m_normal.y = mesh->mNormals[i].y;
      vertex.m_normal.z = mesh->mNormals[i].z;
      
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
      vertex.m_tangent.x = mesh->mTangents[i].x;
      vertex.m_tangent.y = mesh->mTangents[i].y;
      vertex.m_tangent.z = mesh->mTangents[i].z;

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

    //if(mesh->mMaterialIndex >=0)
    //{
    //  //TODO: do something with texture data
    //  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    //  std::vector<Texture> diffuseMaps = ProcessMaterial(material,
    //                                    aiTextureType_DIFFUSE, Texture::Channel::SRGB);
    //  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //  std::vector<Texture> specularMaps = ProcessMaterial(material,
    //                                    aiTextureType_SPECULAR, Texture::Channel::RGB);
    //  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //}

    return Mesh(vertices, indices, false);
  }
  
  std::vector<Texture> Model::ProcessMaterial(aiMaterial* mat
    , aiTextureType type, Texture::Channel channel)
  {
    std::vector<Texture> textures;
    for(size_t i=0; i < mat->GetTextureCount(type); ++i)
    {
      aiString str;
      mat->GetTexture(type, i, &str);

      //Assume that path in the material is relative, not absolute path
      std::string path{m_directory};
      path+= str.C_Str();

      //Push to textures
      textures.emplace_back(path, channel);
    }
    return textures;
  }
}