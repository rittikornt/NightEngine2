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

namespace Graphic
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

    //! @brief Initialize
    void		Init(void);

    //! @brief Bind
    void		Bind(void) const;

    //! @brief Unbind
		void		Unbind(void) const;

    //! @brief Attach shader file
		void		AttachShaderFile(const std::string& filename);

    //! @brief Link the shader
    void		Link() const;

    //! @brief Get Shader Program ID
		GLuint  GetProgramID() const { return m_programID; }

    //! @brief Set the uniform block binding point
    void    SetUniformBlockBindingPoint(const std::string& uniformBlockName, unsigned bufferPointIndex);

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

    //! @brief Set Uniform Function
    template<typename T> 
    void	SetUniform(const std::string& name, T&& value) const
		{
      using namespace Core;
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
		GLuint	create_shader(std::string const & filename);

		// Disable Copying
		Shader(Shader const &) = delete;

		// Private Member Variables
		GLuint m_programID;
    std::vector<std::string> m_filePath;  //Save shader path to be serialized
	};

} // Graphic

