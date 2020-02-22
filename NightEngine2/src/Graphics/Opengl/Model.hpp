/*!
  @file Model.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Model
*/
#pragma once
#include "Graphics/Opengl/Mesh.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <vector>

namespace Rendering
{
  class Model
  {
    public:
      //! @brief Default Constructor
      Model(void) = default;

      //! @brief Constructor for loading Model from path
      Model(const std::string& path, bool allowPrint = true);

      //! @brief Draw the Model
      void Draw(void);

      //! @brief Get model meshes
      std::vector<Mesh>& GetMeshes(void) { return m_meshes; }
    private:
      void LoadModel(const std::string& path);
      void ProcessNode(aiNode* node, const aiScene* scene);
      Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
      std::vector<Texture> ProcessMaterial(aiMaterial* mat
        , aiTextureType type, Texture::Channel channel);

      std::vector<Mesh> m_meshes;
      std::string       m_directory;
  };
}