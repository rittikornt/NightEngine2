/*!
  @file Shader.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Shader
*/
#pragma once

// System Headers
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

#include "Core/Reflection/ReflectionMacros.hpp"

// Standard Headers
#include <string>
#include <vector>

namespace Rendering
{
	class Shader
	{
    REFLECTABLE_TYPE();
	public:
    //! @brief Constructor
		Shader() : m_programID(~0) {}

    //! @brief Assignment Operator
    Shader& operator=(const Shader& rhs);

    //! @brief Destructor
    ~Shader();

    //! @brief Allocate Shader Object
    void		Create(void);

    //! @brief Deallocate Shader Object
    void		Release(void);

    //! @brief Deallocate All Shader Object
    static void ReleaseAllLoadedShaders(void);

    //! @brief Bind
    void		Bind(void) const;

    //! @brief Unbind
		void		Unbind(void) const;

    //! @brief Attach shader file
		bool		AttachShaderFile(const std::string& filename);

    //! @brief Attach shader file
    bool		AttachShaderFileFromPathNoAssert(const std::string& filePath);

    //! @brief Link the shader
    bool		Link() const;

    //! @brief Link the shader
    bool		LinkNoAssert() const;

    //! @brief Get Shader Program ID
		inline GLuint  GetProgramID() const { return m_programID; }

    //! @brief Recompile this shader based on the filePath
    void    RecompileShader(void);

    //! @brief Clear Shader Variable
    void Clear(void) { m_programID = ~(0); m_filePath.clear(); }

    //**************************************
    //  SetUniform Overloads
    //**************************************
		static void	SetUniform(unsigned int location, int value);
		static void	SetUniform(unsigned int location, float value);
		static void	SetUniform(unsigned int location, bool value);
		static void	SetUniform(unsigned int location, const glm::vec2& value);
		static void	SetUniform(unsigned int location, const glm::vec3& value);
		static void	SetUniform(unsigned int location, const glm::vec4& value);
		static void	SetUniform(unsigned int location, glm::mat4 const & matrix);

    //! @brief Set the uniform block binding point
    void    SetUniformBlockBindingPoint(const std::string& uniformBlockName, unsigned bufferPointIndex);

    //! @brief Set Uniform Function
    template<typename T> 
    void	SetUniform(const std::string& name, T&& value) const
		{
      using namespace NightEngine;
			//Note: Need to Bind() first before calling this method
			int location = glGetUniformLocation(m_programID, name.c_str());
			if (location == -1)
			{
        Debug::Log << Logger::MessageType::ERROR_MSG 
          <<"Missing Uniform: " << name << '\n';

        CHECKGL_ERROR();
			}
			else
			{
				SetUniform(location, std::forward<T>(value));
			}
		}

    //! @brief Check if this uniform is available
    bool IsValidUniform(std::string const & name) const
    {
      return glGetUniformLocation(m_programID, name.c_str()) != -1;
    }

	private:
		GLuint	CreateShaderObject(const std::string& filename);

    std::string LoadShaderSourceCode(const std::string& filePath);

    bool CompileShader(GLuint shaderID, const char* sourceCode);

		// Private Member Variables
		GLuint m_programID;
    std::vector<std::string> m_filePath;  //Save shader path to be serialized
	};

  //! @brief Two Shader are the same if they have the same programID
  inline bool operator== (Shader const& lhs, Shader const& rhs)
  {
    return (lhs.GetProgramID() == rhs.GetProgramID());
  }
} // Rendering

