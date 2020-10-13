/*!
  @file Texture.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Texture
*/
#include "Graphics/Opengl/Texture.hpp"
#include "Graphics/Opengl/OpenglAllocationTracker.hpp"

#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

#include "Core/Serialization/ResourceManager.hpp"

//// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

#include <unordered_map>

using namespace NightEngine;

namespace NightEngine::Rendering::Opengl
{
  static void ReleaseTextureID(GLuint shaderID)
  {
    glDeleteTextures(1, &shaderID);
    CHECKGL_ERROR();

    DECREMENT_ALLOCATION(Texture, shaderID);
  }

  REGISTER_DEALLOCATION_FUNC(Texture, ReleaseTextureID)

  static Texture::FilterMode GetMagFilterMode(Texture::FilterMode filterMode)
  {
    switch (filterMode)
    {
    case Texture::FilterMode::TRILINEAR:
      return Texture::FilterMode::LINEAR;
    case Texture::FilterMode::TRINEAREST:
      return Texture::FilterMode::NEAREST;
    }
    return filterMode;
  }
  /////////////////////////////////////////////////////////////////////////

  Texture::Texture(const Texture& texture)
    : m_textureID(texture.m_textureID)
    , m_name(texture.m_name)
    , m_filePath(texture.m_filePath)
    , m_internalFormat(texture.m_internalFormat)
    , m_filterMode(texture.m_filterMode)
  {
  }

  Texture::Texture(const TextureIdentifier& textureIdentifier)
    : m_textureID(textureIdentifier.m_textureID)
    , m_name(textureIdentifier.m_name)
    , m_filePath(textureIdentifier.m_filePath)
    , m_internalFormat((Format)textureIdentifier.m_internalFormat)
    , m_filterMode((FilterMode)textureIdentifier.m_filterMode)
  {
  }

  Texture::Texture(unsigned int id, const std::string & name)
    : m_textureID(id), m_name(name)
  {
  }

  Texture::Texture(const std::string& filePath, Format channel
		, FilterMode filterMode, WrapMode wrapMode
    , bool hdrImage)
    : m_filePath(filePath)
	{
    auto t = ResourceManager::LoadTextureResource(filePath
      , channel, filterMode, wrapMode, hdrImage);
    if (t.IsValid())
    {
      *this = (*t);

      //Assign Filename, path
      auto dirIndex = filePath.find_last_of('/');
      auto lastIndex = filePath.size() - dirIndex;
      m_name = filePath.substr(dirIndex + 1, lastIndex);
      m_filePath = filePath;
    }
	}

	Texture::~Texture()
	{
    if (m_textureID != ~(0))
    {
      CHECK_LEAK(Texture, m_textureID);
    }
    //if (IS_ALLOCATED(Texture, m_textureID))
    //{
    //  Debug::Log << Logger::MessageType::WARNING
    //    << "Texture Leak: " << m_textureID << '\n';
    //}
	}

  void Texture::Release(void)
  {
    if (m_textureID != ~(0))
    {
      //TODO: This need ref count
      glDeleteTextures(1, &m_textureID);
      CHECKGL_ERROR();
      DECREMENT_ALLOCATION(Texture, m_textureID);
    }
  }

  void Texture::ReleaseAllLoadedTextures(void)
  {
    OpenglAllocationTracker::DeallocateAllObjects("Texture", ReleaseTextureID);
  }

	void Texture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}

	void Texture::BindToTextureUnit(TextureUnit textureUnit) const
	{
		glActiveTexture(static_cast<GLenum>(textureUnit));
		glBindTexture(GL_TEXTURE_2D, m_textureID);
	}

  void Texture::BindToTextureUnit(int index) const
  {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
  }

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

  void Texture::SetName(const char* name)
  {
    Bind();
    glObjectLabel(GL_TEXTURE, GetID(), -1, name);
    Unbind();
  }

  //*****************************************************
  // Static Method
  //*****************************************************
  NightEngine::EC::Handle<NightEngine::Rendering::Opengl::Texture> Texture::LoadTextureHandle(const std::string& filePath
    , Format channel, FilterMode filterMode, WrapMode wrapMode, bool hdrImage)
  {
    auto handle = ResourceManager::LoadTextureResource(filePath
      , channel, filterMode, wrapMode, hdrImage);
    if (handle.IsValid())
    {
      //Assign Filename, path
      auto dirIndex = filePath.find_last_of('/');
      auto lastIndex = filePath.size() - dirIndex;
      handle->m_name = filePath.substr(dirIndex + 1, lastIndex);
      handle->m_filePath = filePath;
    }
    return handle;
  }

  TextureIdentifier Texture::LoadTexture(const std::string& filePath
    , Format internalFormat
    , FilterMode filterMode, WrapMode wrapMode)
  {
    Debug::Log << Logger::MessageType::INFO
      << "Texture Loading: " << filePath << '\n';

    //Load Image
    int width, height, channels;

    //TODO: detect Alpha channel from file extension
    unsigned loadChannel = (internalFormat == Format::RGB
      || internalFormat == Format::SRGB
      || internalFormat == Format::RGB16F
      || internalFormat == Format::RGB32F) ? STBI_rgb : STBI_rgb_alpha;

    //Flip Img vertically 
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imgData = stbi_load(filePath.c_str()
      , &width, &height, &channels, loadChannel);

    Format format = loadChannel == STBI_rgb ?
      Format::RGB : Format::RGBA;

    //Generate Actual Texture Data based on the loaded file
    TextureIdentifier t = GenerateTextureData(imgData, width, height
      , internalFormat, format
      , filterMode, wrapMode);

    stbi_image_free(imgData);

    return t;
  }

  TextureIdentifier Texture::LoadHDRTexture(const std::string & filePath
    , Format internalFormat, FilterMode filterMode, WrapMode wrapMode)
  {
    Debug::Log << Logger::MessageType::INFO
      << "Texture(HDR) Loading: " << filePath << '\n';

    //Load Image
    int width, height, channels;

    //Flip Img vertically 
    stbi_set_flip_vertically_on_load(true);

    //Load the HDR image
    float* imgData = stbi_loadf(filePath.c_str()
      , &width, &height, &channels, 0);

    //Generate Actual Texture Data based on the loaded file
    TextureIdentifier t = GenerateTextureData(imgData, width, height
      , internalFormat, Format::RGB
      , filterMode, wrapMode);

    stbi_image_free(imgData);

    return t;
  }

  TextureIdentifier Texture::GenerateRenderTexture(int width, int height
    , Format internalformat
    , Format format 
    , FilterMode filterMode, WrapMode wrapMode)
  {
    Debug::Log << Logger::MessageType::INFO
      << "Generating Null Texture\n";

    TextureIdentifier texture;
    //Generate Object
    glGenTextures(1, &(texture.m_textureID));
    INCREMENT_ALLOCATION(Texture, texture.m_textureID);
    glBindTexture(GL_TEXTURE_2D, texture.m_textureID);

    //Choose target based on channel
    GLenum pixelTarget = internalformat == Format::RGB16F
      || internalformat == Format::RGB32F
      || internalformat == Format::RGBA16F
      || internalformat == Format::RGBA32F
      || internalformat == Format::RG16F
      || internalformat == Format::RED ? GL_FLOAT : GL_UNSIGNED_BYTE;

    //https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalformat)
      , width, height, 0, static_cast<GLenum>(format)
      , pixelTarget, NULL);

    //Option
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S
      , static_cast<GLint>(wrapMode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T
      , static_cast<GLint>(wrapMode));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER
      , static_cast<GLint>(filterMode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER
      , static_cast<GLint>(GetMagFilterMode(filterMode)));

    //Default max anisotropy setting
    GLfloat value = 1.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);

    CHECKGL_ERROR();
    texture.m_name = "GeneratedRT";
    texture.m_internalFormat = (GLenum)internalformat;
    texture.m_filterMode = (GLenum)filterMode;
    return texture;
  }

  TextureIdentifier Texture::GenerateTextureData(void* imgData
    , int width, int height
    , Format internalFormat, Format format
    , FilterMode filterMode, WrapMode wrapMode)
  {
    TextureIdentifier texture;
    //Generate Object
    glGenTextures(1, &(texture.m_textureID));
    INCREMENT_ALLOCATION(Texture, texture.m_textureID);
    glBindTexture(GL_TEXTURE_2D, texture.m_textureID);

    Debug::Log << Logger::MessageType::INFO
      << "Texture Alloc: " << texture.m_textureID << '\n';

    //Option
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S
      , static_cast<GLint>(wrapMode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T
      , static_cast<GLint>(wrapMode));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER
      , static_cast<GLint>(filterMode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER
      , static_cast<GLint>(GetMagFilterMode(filterMode)));

    //Default max anisotropy setting
    GLfloat value = 1.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &value);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);

    if (imgData != nullptr)
    {
      //Choose target based on channel
      GLenum pixelTarget = internalFormat == Format::RGB16F
        || internalFormat == Format::RGB32F
        || internalFormat == Format::RGBA16F
        || internalFormat == Format::RGBA32F
        || internalFormat == Format::RG16F
        || internalFormat == Format::RED ? GL_FLOAT : GL_UNSIGNED_BYTE;

      //Choose channel based on channel
      glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat)
        , width, height, 0, static_cast<GLenum>(format)
        , pixelTarget, imgData);

      //Generate Mipmap
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
      Debug::Log << Logger::MessageType::ERROR_MSG
        << "Texture: Failed to load texture" << '\n';
      ASSERT_TRUE(false);
    }

    CHECKGL_ERROR();
    texture.m_internalFormat = (GLenum)internalFormat;
    texture.m_filterMode = (GLenum)filterMode;
    return texture;
  }

  void Texture::SetBlendMode(bool enable)
	{
		if (enable)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

  void Texture::ActiveTextureUnit(TextureUnit textureUnit)
  {
    glActiveTexture(static_cast<GLenum>(textureUnit));
  }
} // Rendering
