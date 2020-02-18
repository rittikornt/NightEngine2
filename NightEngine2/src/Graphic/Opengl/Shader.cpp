/*!
  @file Shader.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Shader
*/
// Local Headers
#include "Graphic/Opengl/Shader.hpp"
#include "Graphic/Opengl/OpenglAllocationTracker.hpp"
#include "Graphic/ShaderTracker.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

// System Headers
#include <glm/gtc/type_ptr.hpp>

// Standard Headers
#include <fstream>
#include <memory>

using namespace Core;

namespace Graphic
{
  static void ReleaseShaderID(GLuint shaderID)
  {
    glDeleteProgram(shaderID);
    DECREMENT_ALLOCATION(Shader, shaderID);
    CHECKGL_ERROR();
  }

  REGISTER_DEALLOCATION_FUNC(Shader, ReleaseShaderID)

  /////////////////////////////////////////////////////////////////////////

  Shader& Shader::operator=(const Shader& rhs)
  {
    m_programID = rhs.m_programID;
    m_filePath = rhs.m_filePath;

    return *this;
  }

  Shader::~Shader()
  {
    CHECK_LEAK(Shader, m_programID);
  }

  void Shader::Create(void)
  {
    m_programID = glCreateProgram();
    INCREMENT_ALLOCATION(Shader, m_programID);
    m_filePath.reserve(2);

    ShaderTracker::Add(*this);
  }

  void Shader::Release(void)
  {
    if (m_programID != (~0))
    {
      //TODO: This delete need ref count too
      glDeleteProgram(m_programID);
      CHECKGL_ERROR();
      DECREMENT_ALLOCATION(Shader, m_programID);
      //TODO: Unload the reference in the ResourceManager as well

      ShaderTracker::Remove(*this);
    }
  }

  void Shader::ReleaseAllLoadedShaders(void)
  {
    OpenglAllocationTracker::DeallocateAllObjects("Shader", ReleaseShaderID);
  }

  void Shader::Bind() const
	{
		glUseProgram(m_programID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	/////////////////////////////////////////////////////////////////////////

	void Shader::SetUniform(unsigned int location, int value)
	{
		glUniform1i(location, value);
	}

	void Shader::SetUniform(unsigned int location, float value)
	{
		glUniform1f(location, value);
	}

	void Shader::SetUniform(unsigned int location, bool value)
	{
		glUniform1i(location, value);
	}

	void Shader::SetUniform(unsigned int location, const glm::vec2 & value)
	{
		glUniform2f(location, value.x, value.y);
	}

	void Shader::SetUniform(unsigned int location, const glm::vec3 & value)
	{
		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::SetUniform(unsigned int location, const glm::vec4 & value)
	{
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void Shader::SetUniform(unsigned int location, glm::mat4 const & matrix)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

  void Shader::SetUniformBlockBindingPoint(const std::string& uniformBlockName, unsigned bufferPointIndex)
  {
    unsigned int uniformBlockIndex = glGetUniformBlockIndex(m_programID, uniformBlockName.c_str());
    if (uniformBlockIndex == GL_INVALID_INDEX)
    {
      Debug::Log << Logger::MessageType::ERROR_MSG
        << "Trying to retrieve invalid Uniform Block Index\n";
      ASSERT_TRUE(false);
    }

    glUniformBlockBinding(m_programID, uniformBlockIndex, bufferPointIndex);
  }

	/////////////////////////////////////////////////////////////////////////

  bool Shader::AttachShaderFile(const std::string& filename)
	{
    //TODO: Load Shader through ResourceManager and cache it
    Debug::Log << Logger::MessageType::INFO 
      << "Loading Shader: " << filename << '\n';

		// Load GLSL Shader Source from File
    std::string path = PROJECT_DIR_SOURCE_SHADER + filename;
    m_filePath.emplace_back(path);
		auto srcCodeStr = LoadShaderSourceCode(path);

		// Create a Shader Object
		auto shaderID = CreateShaderObject(filename);
    bool success = CompileShader(shaderID, srcCodeStr.c_str());
    ASSERT_TRUE(success);

		// Attach the Shader and Free Allocated Memory
    if (success)
    {
  		glAttachShader(m_programID, shaderID);
    }
		glDeleteShader(shaderID);

    CHECKGL_ERROR();

    return success;
	}

  bool Shader::AttachShaderFileFromPath(const std::string& filePath)
  {
    //TODO: Load Shader through ResourceManager and cache it
    Debug::Log << Logger::MessageType::INFO
      << "Loading Shader: " << filePath << '\n';

    // Load GLSL Shader Source from File
    m_filePath.emplace_back(filePath);
    auto srcCodeStr = LoadShaderSourceCode(filePath);

    // Create a Shader Object
    auto shaderID = CreateShaderObject(filePath);
    bool success = CompileShader(shaderID, srcCodeStr.c_str());

    // Attach the Shader and Free Allocated Memory
    if (success)
    {
      glAttachShader(m_programID, shaderID);
    }
    glDeleteShader(shaderID);

    CHECKGL_ERROR();

    return success;
  }

  bool Shader::Link() const
	{
		glLinkProgram(m_programID);

		//Check for Link Error
    GLint status;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &status);
		if (!status)
		{
			GLint length;
			glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char[]> buffer(new char[length]);
			glGetProgramInfoLog(m_programID, length, nullptr, buffer.get());

			Debug::Log << Logger::MessageType::ERROR_MSG 
        << buffer.get();
		}
		ASSERT_TRUE(status == true);
    CHECKGL_ERROR();

    return status;
	}

  bool Shader::LinkNoAssert() const
  {
    glLinkProgram(m_programID);

    //Check for Link Error
    GLint status;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &status);
    if (!status)
    {
      GLint length;
      glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &length);
      std::unique_ptr<char[]> buffer(new char[length]);
      glGetProgramInfoLog(m_programID, length, nullptr, buffer.get());

      Debug::Log << Logger::MessageType::ERROR_MSG
        << buffer.get();
    }
    CHECKGL_ERROR();

    return status;
  }

  void Shader::RecompileShader(void)
  {
    if (IS_ALLOCATED(Shader, m_programID))
    {
      Debug::Log << Logger::MessageType::INFO
        << "**********************************\n";
      Debug::Log << Logger::MessageType::INFO
        << "RecompileShader: " << m_programID << '\n';

      // Create a new temp Shader here
      // Do all of this again without Asseting, its okay to fail compiling
      Shader tempShader;
      tempShader.Create();

      //Attach Shader to the program and link the program
      bool success = true;
      for (auto path : m_filePath)
      {
        success &= tempShader.AttachShaderFileFromPath(path);
      }
      success &= tempShader.LinkNoAssert();

      //If Compiled and linked successfully
      if (success)
      {
        //Release old program
        this->Release();
        *this = tempShader;

        //Remove tempShader pointer from the Tracker, add this pointer instead
        ShaderTracker::Remove(tempShader);
        ShaderTracker::Add(*this);
      }
      else
      {
        Debug::Log << Logger::MessageType::ERROR_MSG
          << "Shader::RecompileShader: Failed [" << m_programID << "]\n";
        
        //Release the tempShader that failed to compile
        tempShader.Release();
      }
      Debug::Log << Logger::MessageType::INFO
        << "**********************************\n";
    }
  }

	/////////////////////////////////////////////////////////////////////////

	GLuint Shader::CreateShaderObject(const std::string const& filename)
	{
		auto index = filename.rfind(".");
		auto ext = filename.substr(index + 1);
    if (ext == "comp")
    {
      return glCreateShader(GL_COMPUTE_SHADER);
    }
    else if (ext == "frag")
    {
      return glCreateShader(GL_FRAGMENT_SHADER);
    }
    else if (ext == "geom")
    {
      return glCreateShader(GL_GEOMETRY_SHADER);
    }
    else if (ext == "vert")
    {
      return glCreateShader(GL_VERTEX_SHADER);
    }
		
		return false;
	}

  std::string Shader::LoadShaderSourceCode(const std::string& filePath)
  {
    std::ifstream fd(filePath);
    std::string sourceCode = std::string(std::istreambuf_iterator<char>(fd),
      (std::istreambuf_iterator<char>()));

    //TODO: some preprocessing shader code here before returning
    return sourceCode;
  }

  bool Shader::CompileShader(GLuint shaderID, const char* sourceCode)
  {
    glShaderSource(shaderID, 1, &sourceCode, nullptr);
    glCompileShader(shaderID);

    // Display the Build Log on Error
    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (status == false)
    {
      GLint length;
      glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
      std::unique_ptr<char[]> buffer(new char[length]);
      glGetShaderInfoLog(shaderID, length, nullptr, buffer.get());

      Debug::Log << Logger::MessageType::ERROR_MSG
        << m_filePath[m_filePath.size()-1].c_str() << '\n' << buffer.get();
    }

    return status;
  }
} // Graphic

