/*!
  @file FileSystem.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of FileSystem
*/
#pragma once
#include "Core/Container/String.hpp"

#include <memory>
#include <fstream>

namespace Core
{
  namespace FileSystem
  {
    //! @brief Directory flag
		enum class DirectoryType : unsigned
		{
			Assets = 0,
			Models,
			Textures,
			Shaders,
      Script,
      Cubemaps,
      Archetypes,
      Materials,
			Count
		};

    //! @brief Create file into specific directory Output only
    std::unique_ptr<std::ofstream> CreateFileTo(const Container::String& fileName, DirectoryType dir, bool append = false);

		//! @brief Open file from specific directory for IO
		std::unique_ptr<std::ifstream> OpenFile(const Container::String& fileName, DirectoryType dir);

		//! @brief Open file from specific directory for IO
		Container::String OpenFileAsString(const Container::String& fileName, DirectoryType dir);

		/////////////////////////////////////////////////////////////////////

		//! @brief Check if file exist
		bool IsFileExist(Container::String fileName, DirectoryType dir);

		//! @brief Get Full file path
    Container::String GetFilePath(const Container::String& fileName, DirectoryType dir);
  }
}