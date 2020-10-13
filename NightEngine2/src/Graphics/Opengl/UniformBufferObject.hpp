/*!
  @file UniformBufferObject.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of UniformBufferObject
*/

#pragma once
#include <cstddef>  //std::size_t for gcc

namespace NightEngine::Rendering::Opengl
{
  class UniformBufferObject
  {
    public:
    //! @brief Constructor
    UniformBufferObject(void) : m_id(~(0)) {}

    //! @brief Destructor
    ~UniformBufferObject(void);

    //! @brief Initialization
    void Init(std::size_t size, unsigned bufferPointIndex);

    //! @brief Bind
    void Bind(void);

    //! @brief Unbind
    void Unbind(void);

    //! @brief Fill Buffer
    void FillBuffer(std::size_t offset, std::size_t dataSize, void* data);

    private:
    unsigned int m_id;
  };

}