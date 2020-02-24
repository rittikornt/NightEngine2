/*!
  @file FileSystem.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of FileSystem
*/
#include "Core/Serialization/FileSystem.hpp"

#include "Core/Macros.hpp"
#include <sys/stat.h>

namespace NightEngine
{
	namespace FileSystem
	{
		static Container::String g_dirSubPath[] = {"","Models","Textures"
      ,"Shaders", "Script", "Cubemaps", "Archetypes", "Materials", "Scenes"};

		std::unique_ptr<std::ofstream> CreateFileTo(const Container::String& fileName, DirectoryType dir, bool append)
		{
			//File path and mode
			Container::String path{ GetFilePath(fileName, dir) };
			int mode = append ? std::ofstream::out | std::ofstream::app : std::ofstream::out;

			//Open or create the file
			return std::unique_ptr<std::ofstream>{ new std::ofstream(path, mode) };
		}

		std::unique_ptr<std::ifstream> OpenFile(const Container::String& fileName, DirectoryType dir)
		{
			Container::String path{ GetFilePath(fileName, dir) };

			//Error, if file doesn't exist
			ASSERT_TRUE(IsFileExist(fileName, dir));

			//If exist, open and return the ofstream
			return std::unique_ptr<std::ifstream>{ new std::ifstream{ path } };
		}

		Container::String OpenFileAsString(const Container::String& fileName, DirectoryType dir)
		{
			Container::String path{ GetFilePath(fileName, dir) };

			//Error, if file doesn't exist
			ASSERT_TRUE(IsFileExist(fileName, dir));

			//If exist, open and return the ofstream
			std::ifstream out{ path };
			return Container::String(std::istreambuf_iterator<char>(out),
				(std::istreambuf_iterator<char>()));
		}

		/////////////////////////////////////////////////////////////////////

		bool IsFileExist(Container::String fileName, DirectoryType dir)
		{
			struct stat buffer;
			Container::String path{ GetFilePath(fileName, dir) };
			return (stat(path.c_str(), &buffer) == 0);
		}

		Container::String GetFilePath(const Container::String & fileName, DirectoryType dir)
		{
      Container::String path{ dir == DirectoryType::Script ? PROJECT_DIR_SOURCE : PROJECT_DIR_SOURCE_ASSETS };
      path += g_dirSubPath[static_cast<unsigned>(dir)];
      path += "/";
      path += fileName;
			return path;
		}
	}
}