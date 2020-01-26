/*!
  @file Cubemap.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Cubemap
*/
#include "Graphic/Opengl/Cubemap.hpp"
#include "Graphic/Opengl/OpenglAllocationTracker.hpp"

#include "Core/Logger.hpp"
#include "Core/Macros.hpp"
#include "Core/Serialization/FileSystem.hpp"

#include <glad/glad.h>

#include <stb_image.h>

using namespace Core;

namespace Graphic
{
  const std::vector<float> cubemapVertices{
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
  };

  const std::vector<unsigned int> cubemapindices{
    // right
    0, 1, 2,
    3, 4, 5,
    // left
    6, 7, 8,
    9, 10, 11,
    // top
    12, 13, 14,
    15, 16, 17,
    // bottom
    18, 19, 20,
    21, 22, 23,
    // back
    24, 25, 26,
    27, 28, 29,
    // front
    30, 31, 32,
    33, 34, 35
  };

  Cubemap::~Cubemap(void)
  {
    if(m_id != ~(0))
    {
      glDeleteTextures(1, &m_id);
      DECREMENT_ALLOCATION(Cubemap, m_id);
    }
  }

  void Cubemap::Init(std::vector<std::string>& fileNames,const std::string& vertexShader
  , const std::string& fragmentShader, Texture::Channel channel
  , bool generateMipmap)
  {
    //Init Texures
    glGenTextures(1, &m_id);
    INCREMENT_ALLOCATION(Cubemap, m_id);
    Bind();
    {
      //Load right, left, top, bottom, front, back in order
      int width, height, nrChannels;
      for (unsigned int i = 0; i < fileNames.size(); i++)
      {
        std::string filePath = FileSystem::GetFilePath(fileNames[i]
          , FileSystem::DirectoryType::Cubemaps);

        Debug::Log << Logger::MessageType::INFO
          << "Loading Texture: " << filePath << '\n';

        unsigned loadChannel = (channel == Texture::Channel::RGB
          || channel == Texture::Channel::SRGB) ? STBI_rgb : STBI_rgb_alpha;

        //Default loading image
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(filePath.c_str()
          , &width, &height, &nrChannels, loadChannel);
        if (data != nullptr)
        {
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
            , 0, static_cast<GLint>(channel)
            , width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
          stbi_image_free(data);
        }
        else
        {
          Debug::Log << Logger::MessageType::ERROR_MSG
            << "Cubemap texture failed to load at path: " << fileNames[i] << '\n';
          stbi_image_free(data);
          ASSERT_TRUE(false);
        }
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      if (generateMipmap)
      {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
      }
    }
    Unbind();

    //Init Shaders
    m_shader.Create();
    m_shader.AttachShaderFile(vertexShader);
    m_shader.AttachShaderFile(fragmentShader);
    m_shader.Link();

    //Cubemap samplerCube to use TextureUnit0
    m_shader.Bind();
    {
      m_shader.SetUniform("u_cubemap", 0);
    }
    m_shader.Unbind();

    //Init VAO
    AttributePointerInfo info{ 1, std::vector<unsigned>{3}
    ,  std::vector<size_t>{sizeof(float) * 3}, std::vector<bool>{false} };
    m_vao.Init();
    m_vao.Build(BufferMode::Static, cubemapVertices
      , cubemapindices, info);

    CHECKGL_ERROR();
  }

  void Cubemap::Init(int width, int height
    , const std::string & vertexShader, const std::string & fragmentShader
    , Texture::Channel internalFormat
    , Texture::FilterMode minFilter
    , Texture::FilterMode magFilter
    , bool generateMipmap)
  {
    //Init Texures
    glGenTextures(1, &m_id);
    INCREMENT_ALLOCATION(Cubemap, m_id);
    Bind();
    {
      //Choose target based on channel
      GLenum pixelTarget = internalFormat == Texture::Channel::RGB16F
        || internalFormat == Texture::Channel::RGB32F
        || internalFormat == Texture::Channel::RGBA16F
        || internalFormat == Texture::Channel::RGBA32F
        || internalFormat == Texture::Channel::RED ? GL_FLOAT : GL_UNSIGNED_BYTE;

      //Init right, left, top, bottom, front, back in order
      for (unsigned int i = 0; i < 6; i++)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i
          , 0, static_cast<GLint>(internalFormat)
          , width, height, 0, GL_RGB, pixelTarget, nullptr);
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter));
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter));
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

      if (generateMipmap)
      {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
      }
    }
    Unbind();

    //Init Shaders
    m_shader.Create();
    m_shader.AttachShaderFile(vertexShader);
    m_shader.AttachShaderFile(fragmentShader);
    m_shader.Link();

    //Cubemap samplerCube to use TextureUnit0
    m_shader.Bind();
    {
      m_shader.SetUniform("u_cubemap", 0);
    }
    m_shader.Unbind();

    //Init VAO
    AttributePointerInfo info{ 1, std::vector<unsigned>{3}
    ,  std::vector<size_t>{sizeof(float) * 3}, std::vector<bool>{false} };
    m_vao.Init();
    m_vao.Build(BufferMode::Static, cubemapVertices
      , cubemapindices, info);

    CHECKGL_ERROR();
  }

  void Cubemap::InitDepthCubemap(unsigned widthHeight)
  {
    //Init Texures
    glGenTextures(1, &m_id);
    INCREMENT_ALLOCATION(Cubemap, m_id);
    Bind();

    //Init 6 cubemap textures with depth component only
    for (unsigned int i = 0; i < 6; ++i)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT
        , widthHeight, widthHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
      
      CHECKGL_ERROR();
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    Unbind();

    ////Init Shaders
    //m_shader.Init();
    //m_shader.AttachShaderFile(vertexShader);
    //m_shader.AttachShaderFile(fragmentShader);
    //m_shader.Link();

    ////Cubemap samplerCube to use TextureUnit0
    //m_shader.Bind();
    //m_shader.SetUniform("u_cubemap", 0);

    ////Init VAO
    //AttributePointerInfo info{ 1, std::vector<unsigned>{3}
    //,  std::vector<size_t>{sizeof(float) * 3}, std::vector<bool>{false} };
    //m_vao.Init();
    //m_vao.Build(BufferMode::Static, cubemapVertices
    //  , cubemapindices, info);

    CHECKGL_ERROR();
  }

  void Cubemap::Bind(void)
  {
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    CHECKGL_ERROR();
  }
  
  void Cubemap::Unbind(void)
  {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    CHECKGL_ERROR();
  }

  void Cubemap::BindToTextureUnit(int index)
  {
    glActiveTexture(GL_TEXTURE0 + index);
    Bind();
  }

  void Cubemap::Draw(void)
  {
    glDepthMask(GL_FALSE);

    m_shader.Bind();
    {
      //Bind Texture to Unit0
      glActiveTexture(GL_TEXTURE0);
      Bind();
      {
        //Draw mesh
        m_vao.Draw();
      }
      Unbind();
    }
    m_shader.Unbind();
    
    glDepthMask(GL_TRUE);

    CHECKGL_ERROR();
  }

  std::vector<std::string> Cubemap::GetCubemapFileNames(std::string name)
  {
    return std::vector<std::string>
    {
      name + "/" + name + "_right.tga",
      name + "/" + name + "_left.tga",
      name + "/" + name + "_top.tga",
      name + "/" + name + "_bottom.tga",
      name + "/" + name + "_back.tga",
      name + "/" + name + "_front.tga"
    };
  }
}