/*!
  @file Texture.cpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Implementation of Texture
*/
#include <Graphic/Texture.hpp>
#include "Core/Macros.hpp"
#include "Core/Logger.hpp"

#include "Core/Serialization/ResourceManager.hpp"

//// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

using namespace Core;

namespace Graphic
{
  Texture::Texture(const std::string& filePath, Channel channel
		, FilterMode filterMode, WrapMode wrapMode
    , bool hdrImage)
    : m_filePath(filePath)
	{
    auto t = ResourceManager::LoadTextureResource(filePath
      , channel, filterMode, wrapMode, hdrImage);
    if (t != nullptr)
    {
      *this = *t;

      //Assign Filename, path
      auto dirIndex = filePath.find_last_of('/');
      auto lastIndex = filePath.size() - dirIndex;
      m_name = filePath.substr(dirIndex + 1, lastIndex);
      m_filePath = filePath;
    }
	}

	Texture::~Texture()
	{
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

  //*****************************************************
  // Static Method
  //*****************************************************

  Texture Texture::LoadTexture(const std::string& filePath
    , Channel internalFormat
    , FilterMode filterMode, WrapMode wrapMode)
  {
    Debug::Log << Logger::MessageType::INFO
      << "Texture Loading: " << filePath << '\n';

    //Load Image
    int width, height, channels;

    //TODO: detect Alpha channel from file extension
    unsigned loadChannel = (internalFormat == Channel::RGB
      || internalFormat == Channel::SRGB
      || internalFormat == Channel::RGB16F
      || internalFormat == Channel::RGB32F) ? STBI_rgb : STBI_rgb_alpha;

    //Flip Img vertically 
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imgData = stbi_load(filePath.c_str()
      , &width, &height, &channels, loadChannel);

    Channel format = loadChannel == STBI_rgb ?
      Channel::RGB : Channel::RGBA;

    //Generate Actual Texture Data based on the loaded file
    Texture t = GenerateTextureData(imgData, width, height
      , internalFormat, format
      , filterMode, wrapMode);

    stbi_image_free(imgData);

    return t;
  }

  Texture Texture::LoadHDRTexture(const std::string & filePath
    , Channel internalFormat, FilterMode filterMode, WrapMode wrapMode)
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
    Texture t = GenerateTextureData(imgData, width, height
      , internalFormat, Channel::RGB
      , filterMode, wrapMode);

    stbi_image_free(imgData);

    return t;
  }

  Texture Texture::GenerateNullTexture(int width, int height
    , Channel internalformat
    , Channel format 
    , FilterMode filterMode, WrapMode wrapMode)
  {
    Debug::Log << Logger::MessageType::INFO
      << "Generating Null Texture\n";

    Texture texture;
    //Generate Object
    glGenTextures(1, &(texture.m_textureID));
    glBindTexture(GL_TEXTURE_2D, texture.m_textureID);

    //Choose target based on channel
    GLenum pixelTarget = internalformat == Channel::RGB16F
      || internalformat == Channel::RGB32F
      || internalformat == Channel::RGBA16F
      || internalformat == Channel::RGBA32F
      || internalformat == Channel::RG16F
      || internalformat == Channel::RED ? GL_FLOAT : GL_UNSIGNED_BYTE;

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
      , static_cast<GLint>(filterMode));

    texture.m_name = "NullTexture";
    texture.m_internalFormat = internalformat;
    return texture;
  }

  Texture Texture::GenerateTextureData(void* imgData
    , int width, int height
    , Channel internalFormat, Channel format
    , FilterMode filterMode, WrapMode wrapMode)
  {
    Texture texture;
    //Generate Object
    glGenTextures(1, &(texture.m_textureID));
    glBindTexture(GL_TEXTURE_2D, texture.m_textureID);

    //Option
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S
      , static_cast<GLint>(wrapMode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T
      , static_cast<GLint>(wrapMode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER
      , static_cast<GLint>(filterMode));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER
      , static_cast<GLint>(filterMode));

    if (imgData != nullptr)
    {
      //Choose target based on channel
      GLenum pixelTarget = internalFormat == Channel::RGB16F
        || internalFormat == Channel::RGB32F
        || internalFormat == Channel::RGBA16F
        || internalFormat == Channel::RGBA32F
        || internalFormat == Channel::RG16F
        || internalFormat == Channel::RED ? GL_FLOAT : GL_UNSIGNED_BYTE;

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
    texture.m_internalFormat = internalFormat;
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
} // Graphic