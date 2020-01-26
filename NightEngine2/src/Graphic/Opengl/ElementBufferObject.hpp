/*!
  @file ElementBufferObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of ElementBufferObject
*/
#pragma once
// System Headers
#include <glad/glad.h>
#include <vector>

namespace Graphic
{
	//Forward Declaration
  enum class BufferMode : GLenum;
  enum class DrawMode : GLenum;

	class ElementBufferObject
	{
	public:
		enum class DrawMethod : GLenum
		{
			FILL = GL_FILL,
			LINE = GL_LINE
		};

    //! @brief Set Global drawmode
		static void SetDrawMethod(DrawMethod mode);

    //! @brief Constructor
		ElementBufferObject() :m_objectID(~0){}

    //! @brief Destructor
    ~ElementBufferObject();

    //! @brief Deallocate All EBO Object
    static void ReleaseAllLoadedEBO(void);

    //! @brief Initialize
		void Init();

    //! @brief Fill index to array
    void FillIndex(const std::vector<unsigned>& indexArray);

    //! @brief Fill index to array
    void FillIndex(const unsigned* indexArray, size_t arraySize);

    //! @brief Add index to array
    void AddIndex(const unsigned& index);

    //! @brief Build the EBO
		void Build(BufferMode mode) const;

    //! @brief Get indices
    const std::vector<unsigned>& GetIndices(void) { return m_indices; }

    //! @brief Draw
		void Draw(DrawMode drawMode) const;

    //! @brief Draw instances
    void DrawInstanced(size_t amount) const;

    //! @brief Bind to opengl state
    void Bind() const;

    //! @brief Unbind to opengl state
    void Unbind() const;

		GLuint GetID() { return m_objectID; }
	private:
    GLuint m_objectID;
		std::vector<unsigned> m_indices;

		static DrawMethod s_drawMode;
	};

} // Graphic

