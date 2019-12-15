/*!
  @file VertexArrayObject.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of VertexArrayObject
*/
#include "Graphic/Opengl/VertexArrayObject.hpp"
#include "Graphic/Opengl/Vertex.hpp"
#include "Graphic/Opengl/OpenglAllocationTracker.hpp"

#include "Core/Macros.hpp"

//"min" in std::min was override by Window Macros
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

namespace Graphic
{
  VertexArrayObject::VertexArrayObject(BufferMode mode
    , const Vertex* vertexArray, size_t vertexArraySize
    , const unsigned* indexArray, size_t indexArraySize)
  {
    Init();
    Build(mode, vertexArray, vertexArraySize
      , indexArray, indexArraySize);
  }

  VertexArrayObject::~VertexArrayObject(void)
	{
		if (!(m_objectID & (~0)))
		{
			glDeleteVertexArrays(1, &m_objectID);
      DECREMENT_ALLOCATION(VertexArrayObject);
		}

    if (!(m_instanceBufferID & (~0)))
    {
      glDeleteBuffers(1, &m_instanceBufferID);
      DECREMENT_ALLOCATION(VertexArrayObjectInstanceBuffer);
    }
	}

	void VertexArrayObject::Init(void)
	{
		glGenVertexArrays(1, &m_objectID);
    INCREMENT_ALLOCATION(VertexArrayObject);

		m_vbo.Init();
		m_ebo.Init();
	}

  void VertexArrayObject::InitInstanceDraw(size_t dataSize, void* data)
  {
    glGenBuffers(1, &m_instanceBufferID);
    INCREMENT_ALLOCATION(VertexArrayObjectInstanceBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, m_instanceBufferID);
      glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECKGL_ERROR();
  }

  /////////////////////////////////////////////////////////

  void VertexArrayObject::FillData(const std::vector<float>& floatArray
    , const std::vector<unsigned>& indexArray)
  {
    m_vbo.FillVertex(floatArray);
    m_ebo.FillIndex(indexArray);
  }

  void VertexArrayObject::FillData(const Vertex * vertexArray, size_t vertexArraySize
    , const unsigned * indexArray, size_t indexArraySize)
  {
    m_vbo.FillVertex(vertexArray, vertexArraySize);
    m_ebo.FillIndex(indexArray, indexArraySize);
  }

  void VertexArrayObject::FillData(const std::vector<Vertex>& vertexArray
    , const std::vector<unsigned>& indexArray)
  {
    m_vbo.FillVertex(vertexArray);
    m_ebo.FillIndex(indexArray);
  }

  void VertexArrayObject::Clear(void)
  {
    m_vbo.Clear();
  }

  /////////////////////////////////////////////////////////

  void VertexArrayObject::Build(BufferMode mode)
  {
    Bind();

    //Build, from existing vertices/indices
    m_vbo.Build(mode);
    m_ebo.Build(mode);

    SetupAttributePointer();
    Unbind();

    m_vbo.Unbind();
    m_ebo.Unbind();

    CHECKGL_ERROR();
  }

  void VertexArrayObject::Build(BufferMode mode
    , float* floatArray, size_t floatArraySize)
  {
    Bind();
    {
      //Fill and Build VBO, EBO
      m_vbo.FillVertex(floatArray, floatArraySize);
      m_vbo.Build(mode);
    }
    Unbind();

    m_vbo.Unbind();
  }

  void VertexArrayObject::Build(BufferMode mode
    , float* floatArray, size_t floatArraySize
    , AttributePointerInfo& attributeInfo)
  {
    Bind();
    {
      //Fill and Build VBO, EBO
      m_vbo.FillVertex(floatArray, floatArraySize);
      m_vbo.Build(mode);

      SetupAttributePointer(attributeInfo);
    }
    Unbind();

    m_vbo.Unbind();
  }

  void VertexArrayObject::Build(BufferMode mode
    , const std::vector<float>& floatArray
    , const std::vector<unsigned>& indexArray
    , AttributePointerInfo& attributeInfo)
  {
    Bind();
    {
      //Fill and Build VBO, EBO
      m_vbo.FillVertex(floatArray);
      m_vbo.Build(mode);
      m_ebo.FillIndex(indexArray);
      m_ebo.Build(mode);

      SetupAttributePointer(attributeInfo);
    }
    Unbind();

    m_vbo.Unbind();
    m_ebo.Unbind();
  }

  void VertexArrayObject::Build(BufferMode mode
    , const std::vector<Vertex>& vertexArray
    , const std::vector<unsigned>& indexArray)
  {
    Bind();

    //Fill and Build VBO, EBO
    m_vbo.FillVertex(vertexArray);
    m_vbo.Build(mode);
    m_ebo.FillIndex(indexArray);
    m_ebo.Build(mode);

    SetupAttributePointer();
    Unbind();

    m_vbo.Unbind();
    m_ebo.Unbind();

    CHECKGL_ERROR();
  }

	void VertexArrayObject::Build(BufferMode mode
		, const Vertex* vertexArray, size_t vertexArraySize
		, const unsigned* indexArray, size_t indexArraySize)
	{
		Bind();

			//Fill and Build VBO, EBO
			m_vbo.FillVertex(vertexArray, vertexArraySize);
			m_vbo.Build(mode);
			m_ebo.FillIndex(indexArray, indexArraySize);
			m_ebo.Build(mode);

      SetupAttributePointer();
		Unbind();

		m_vbo.Unbind();
		m_ebo.Unbind();

    CHECKGL_ERROR();
	}

  /////////////////////////////////////////////////////////

	void VertexArrayObject::Draw(DrawMode drawMode) const
	{
		Bind();
    {
		  m_ebo.Draw(drawMode);
    }
		Unbind();
	}

  void VertexArrayObject::DrawVBO(DrawMode drawMode /*= DrawMode::TRIANGLES*/) const
  {
    Bind();
    {
      m_vbo.Draw(drawMode);
    }
    Unbind();
  }

  void VertexArrayObject::DrawInstanced(size_t amount) const
  {
    Bind();
    {
      m_ebo.DrawInstanced(amount);
    }
    Unbind();
  }

	inline void VertexArrayObject::Bind(void) const
	{
		glBindVertexArray(m_objectID);
	}

	inline void VertexArrayObject::Unbind(void) const
	{
		glBindVertexArray(0);
	}

  void VertexArrayObject::SetupAttributePointer(void)
  {
    AttributePointerInfo info = Vertex::s_attributePointerInfo;
    SetupAttributePointer(info);
  }

  void VertexArrayObject::SetupAttributePointer(AttributePointerInfo& attributeInfo)
  {
    GLsizei strideSize = attributeInfo.GetStrideSize();
    size_t  float4Size = sizeof(float) * 4;

    //Setup the Attribute Pointer
    GLuint offset = 0;
    for (unsigned i = 0; i < attributeInfo.m_attributeCount; ++i)
    {
      //Setup instance Rendering
      if (attributeInfo.m_divisor[i] == 1)
      {
        //TODO: Remove this
        if (m_instanceBufferID == ~(0))
        {
          //Assume instance buffer is generated
          //ASSERT_MSG(m_instanceBufferID != ~(0)
          //  , "Attempt to use instance buffer before calling InitInstanceDraw(,)");
          continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_instanceBufferID);

        //If bigger than float 4, it need to be separate into N float 4 size
        if (attributeInfo.m_size[i] > float4Size)
        {
          GLuint instanceOffset = 0;
          unsigned count = i + (attributeInfo.m_size[i] / float4Size);
          unsigned dimension = attributeInfo.m_dimension[i];

          for (unsigned j = i; j < count; ++j)
          {
            unsigned curDimension = MIN(dimension, 4);

            //Enable and Set Attribute Pointer
            glEnableVertexAttribArray(j);
            glVertexAttribPointer(j, curDimension
              , GL_FLOAT, false
              , attributeInfo.m_size[i], (void*)instanceOffset);

            //Mark as instance
            glVertexAttribDivisor(j, 1);

            dimension -= 4;
            instanceOffset += float4Size;
          }
        }
        else
        {
          //Enable and Set Attribute Pointer
          glEnableVertexAttribArray(i);
          glVertexAttribPointer(i, attributeInfo.m_dimension[i]
            , GL_FLOAT, attributeInfo.m_normalized[i]
            , attributeInfo.m_size[i], (void*)0);

          //Mark as instance
          glVertexAttribDivisor(i, 1);
        }

        //Unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECKGL_ERROR();
      }
      else
      {
        //Normal attribute should not be bigger than 4 float
        ASSERT_TRUE(attributeInfo.m_size[i] <= float4Size);

        //Enable and Set Attribute Pointer
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attributeInfo.m_dimension[i]
          , GL_FLOAT, attributeInfo.m_normalized[i]
          , strideSize, (void*)offset);
      }

      //Move offset
      offset += attributeInfo.m_size[i];
    }
  }

} // Graphic
