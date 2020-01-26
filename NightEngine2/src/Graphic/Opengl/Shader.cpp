/*!
  @file Shader.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Shader
*/
// Local Headers
#include "Graphic/Opengl/Shader.hpp"
#include "Graphic/Opengl/OpenglAllocationTracker.hpp"

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
  Shader& Shader::operator=(const Shader& rhs)
  {
    m_programID = rhs.m_programID;
    m_filePath = rhs.m_filePath;

    return *this;
  }

  Shader::~Shader()
  {
    if (IS_ALLOCATED(Shader, m_programID))
    {
      Debug::Log << Logger::MessageType::WARNING
        << "Shader Leak: " << m_programID << '\n';
    }
  }

  void Shader::Create(void)
  {
    m_programID = glCreateProgram();
    INCREMENT_ALLOCATION(Shader, m_programID);
    m_filePath.reserve(2);
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
    }
  }

  static void ReleaseShaderID(GLuint shaderID)
  {
    glDeleteProgram(shaderID);
    DECREMENT_ALLOCATION(Shader, shaderID);
    CHECKGL_ERROR();

    //TODO: Unload the reference in the ResourceManager as well
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

	/////////////////////////////////////////////////////////////////////////

	void Shader::AttachShaderFile(const std::string& filename)
	{
    //TODO: Load Shader through ResourceManager and cache it
    Debug::Log << Logger::MessageType::INFO 
      << "Loading Shader: " << filename << '\n';

		// Load GLSL Shader Source from File
		std::string path = PROJECT_DIR_SOURCE_SHADER;
    path += filename;
		std::ifstream fd(path);
		auto src = std::string(std::istreambuf_iterator<char>(fd),
			(std::istreambuf_iterator<char>()));

		GLint status;

		// Create a Shader Object
		const char * source = src.c_str();
		auto shader = create_shader(filename);
		glShaderSource(shader, 1, &source, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

		// Display the Build Log on Error
		if (status == false)
		{
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char[]> buffer(new char[length]);
			glGetShaderInfoLog(shader, length, nullptr, buffer.get());
			
			Debug::Log << Logger::MessageType::ERROR_MSG 
        <<filename.c_str() << '\n' << buffer.get();
      ASSERT_TRUE(false);
			//fprintf(stderr, "%s\n%s", filename.c_str(), buffer.get());
		}

		// Attach the Shader and Free Allocated Memory
		glAttachShader(m_programID, shader);
		glDeleteShader(shader);

    CHECKGL_ERROR();

    m_filePath.push_back(path);
	}

	void Shader::Link() const
	{
		GLint status;

		glLinkProgram(m_programID);
		glGetProgramiv(m_programID, GL_LINK_STATUS, &status);

		//Check for Link Error
		if (!status)
		{
			GLint length;
			glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &length);
			std::unique_ptr<char[]> buffer(new char[length]);
			glGetProgramInfoLog(m_programID, length, nullptr, buffer.get());

			Debug::Log << Logger::MessageType::ERROR_MSG 
        << buffer.get();
      ASSERT_TRUE(false);
			//fprintf(stderr, "%s", buffer.get());
		}
		ASSERT_TRUE(status == true);

    CHECKGL_ERROR();
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

	GLuint Shader::create_shader(std::string const & filename)
	{
		auto index = filename.rfind(".");
		auto ext = filename.substr(index + 1);
		if (ext == "comp") return glCreateShader(GL_COMPUTE_SHADER);
		else if (ext == "frag") return glCreateShader(GL_FRAGMENT_SHADER);
		else if (ext == "geom") return glCreateShader(GL_GEOMETRY_SHADER);
		else if (ext == "vert") return glCreateShader(GL_VERTEX_SHADER);
		
		return false;
	}

} // Graphic

