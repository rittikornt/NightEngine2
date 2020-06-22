/*!
  @file FileSystem.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of FileSystem
*/
#include "Core/Serialization/FileSystem.hpp"

#include "Core/Macros.hpp"
#include <sys/stat.h>

#include <filesystem>	//Required C++17

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

		void GetAllFilesInDirectory(DirectoryType dir, std::vector<std::string>& output
			, FileFilter filter, std::string extension, bool removeExtension, bool searchRecursively, bool clearContainer)
		{
			if (clearContainer)
			{
				output.clear();
			}

			std::string path = GetFilePath("", dir);

			if (searchRecursively)
			{
				for (const auto& entry : std::filesystem::recursive_directory_iterator(path))
				{
					auto filePath = entry.path().string();
					if (filePath.find(extension) != std::string::npos)
					{
						std::replace(filePath.begin(), filePath.end(), '\\', '/');
						output.emplace_back(filePath);
					}
				}
			}
			else
			{
				for (const auto& entry : std::filesystem::directory_iterator(path))
				{
					auto filePath = entry.path().string();
					if (filePath.find(extension) != std::string::npos)
					{
						std::replace(filePath.begin(), filePath.end(), '\\', '/');
						output.emplace_back(filePath);
					}
				}
			}

			//Filtering
			switch (filter)
			{
			case NightEngine::FileSystem::FileFilter::FileName:
			{
				for (int i = 0; i < output.size(); ++i)
				{
					auto pos = output[i].find_last_of('/') + 1;
					if (pos < output[i].size())
					{
						auto fileNameWithExtension = output[i].substr(pos);
						output[i] = fileNameWithExtension;
					}
				}
				break;
			}
			case NightEngine::FileSystem::FileFilter::FullPath:
			{
				//Do nothing
				break;
			}
			}

			//Extension Parsing
			if (removeExtension)
			{
				for (int i = 0; i < output.size(); ++i)
				{
					auto pos = output[i].find_last_of('.');
					if (pos > 0)
					{
						auto fileName = output[i].substr(0, pos);
						output[i] = fileName;
					}
				}
			}
		}

		/////////////////////////////////////////////////////////////////////
		void RemoveExtension(std::string& str)
		{
			auto pos = str.find_last_of('.');
			if (pos > 0)
			{
				auto prefix = str.substr(0, pos);
				str = prefix;
			}
		}

    void RemoveFileDirectoryPath(std::string& str, DirectoryType dir)
    {
			Container::String dirPath{ g_dirSubPath[static_cast<unsigned>(dir)] };
			if (str.size() > dirPath.size())
			{
				auto pos = str.find(dirPath);
				if (pos > 0 && pos < str.size())
				{
					auto postfix = str.substr(pos + dirPath.size() + 1, str.size() - pos);
					str = postfix;
				}
			}
    }

		bool IsFileExist(Container::String fileName, DirectoryType dir)
		{
			struct stat buffer;
			Container::String path{ GetFilePath(fileName, dir) };
			return (stat(path.c_str(), &buffer) == 0);
		}

		Container::String GetFilePath(const Container::String& fileName, DirectoryType dir)
		{
      Container::String path{ dir == DirectoryType::Script ? PROJECT_DIR_SOURCE : PROJECT_DIR_SOURCE_ASSETS };

			path += g_dirSubPath[static_cast<unsigned>(dir)];
			path += (dir != DirectoryType::Assets) ? "/" : "";
			path += fileName;
			return path;
		}
	}
}