/*!
  @file Model.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Model
*/
#pragma once
#include "Graphics/Opengl/Mesh.hpp"

#include "Core/EC/Handle.hpp"
#include "Core/Reflection/ReflectionMacros.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <vector>
#include <unordered_map>

namespace Rendering
{
  class Model
  {
    REFLECTABLE_TYPE();
    public:
      //! @brief Default Constructor
      Model(void) = default;

      //! @brief Constructor for loading Model from path
      Model(const std::string& path, bool allowPrint = true);

      //! @brief Draw the Model
      void Draw(void);

      //! @brief Get model meshes
      inline std::vector<Mesh>& GetMeshes(void) { return m_meshes; }

      //! @brief Get loaded materials
      inline std::vector<NightEngine::EC::Handle<Rendering::Material>>& GetMaterials(void) { return m_materials; }

      //! @brief Check if some of the loaded materials is valid or not
      inline bool IsValidMaterials(void) { return m_validMaterialCount > 0; }
    private:
      void LoadModel(const std::string& path);

      void ProcessAINode(aiNode* node, const aiScene* scene);

      void AddMesh(aiMesh* mesh
        , std::vector<Vertex>& vertices, std::vector<unsigned>& indices);

      bool AddMaterial(int index, const aiScene* scene
        , std::unordered_map<int, NightEngine::EC::Handle<Rendering::Material>>& handleMap);

      bool GetTextures(std::vector<std::string>& textures
        , aiMaterial* mat, aiTextureType type);

      std::vector<Mesh> m_meshes;
      std::vector <NightEngine::EC::Handle<Rendering::Material>> m_materials;
      unsigned              m_validMaterialCount = 0;

      std::string       m_directory;
      std::string       m_name;
  };
}