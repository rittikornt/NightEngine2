/*!
  @file FileSystem.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of FileSystem
*/
#pragma once
#include "Core/Container/String.hpp"

#include <memory>
#include <fstream>
#include <vector>

namespace NightEngine
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
      ErrorLog,
      Cubemaps,
      Archetypes,
      Materials,
      Scenes,
			Count
		};
		enum class FileFilter : unsigned
		{
			FileName = 0,
			FullPath
		};

    //! @brief Create file into specific directory Output only
    std::unique_ptr<std::ofstream> CreateFileTo(const Container::String& fileName, DirectoryType dir, bool append = false);

		//! @brief Open file from specific directory for IO
		std::unique_ptr<std::ifstream> OpenFile(const Container::String& fileName, DirectoryType dir);

		//! @brief Open file from specific directory for IO
		Container::String OpenFileAsString(const Container::String& fileName, DirectoryType dir);

		//! @brief Get all the files in path
		void GetAllFilesInDirectory(DirectoryType dir, std::vector<std::string>& output
			, FileFilter filter = FileFilter::FileName, std::string extension = "", bool removeExtension = false
			, bool searchRecursively = false, bool clearContainer = true);

		/////////////////////////////////////////////////////////////////////

		//! @brief Remove the trailing extension "."
		void RemoveExtension(std::string& str);

		//! @brief Remove the file directory path
		void RemoveFileDirectoryPath(std::string& str, DirectoryType dir);

		//! @brief Check if file exist
		bool IsFileExist(Container::String fileName, DirectoryType dir);

		//! @brief Get Full file path
    Container::String GetFilePath(const Container::String& fileName, DirectoryType dir);
  }
}