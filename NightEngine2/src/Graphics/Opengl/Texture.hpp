/*!
  @file Texture.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Texture
*/
#pragma once
// System Headers
#include <glad/glad.h>

// Standard Headers
#include <string>
#include <unordered_set>

#include "Core/Reflection/ReflectionMacros.hpp"
#include "Core/EC/Handle.hpp"

namespace Rendering
{
  class FrameBufferObject;

  //! @brief TextureIdentifier Struct for Texture creation
  struct TextureIdentifier
  {
    GLuint m_textureID = ~(0);
    std::string m_name = "";
    std::string m_filePath = "";
    GLenum m_internalFormat = GL_INVALID_ENUM;
    GLenum m_filterMode = GL_INVALID_ENUM;
  };

  //! @brief Texture Data for serialization
  struct TextureData
  {
    std::string m_filePath = "";
    GLenum m_channel = GL_INVALID_ENUM;
    GLenum m_filterMode = GL_INVALID_ENUM;
  };

  //! @brief Texture Class
	class Texture
	{
    REFLECTABLE_TYPE();
	public:
    enum class TextureType : GLenum
    {
      TEXTURE_2D = GL_TEXTURE_2D,
      TEXTURE_2D_MULTISAMPLE = GL_TEXTURE_2D_MULTISAMPLE
    };
		enum class Channel : GLenum
		{
			RED = GL_RED,
      RG = GL_RG,
      RGB = GL_RGB,
			RGBA = GL_RGBA,
      SRGB = GL_SRGB,
      SRGBA = GL_SRGB_ALPHA,
      SRGB8_ALPHA8 = GL_SRGB8_ALPHA8, //SRGB8 with linear Alpha
      RG16F = GL_RG16F,
      RGB16F = GL_RGB16F,
      RGB32F = GL_RGB32F,
      RGBA16F = GL_RGBA16F,
      RGBA32F = GL_RGBA32F,
      RGBA12 = GL_RGBA12,
      INVALID = GL_INVALID_ENUM
		};
		enum class TextureUnit : GLenum
		{
			TEXTURE_0 = GL_TEXTURE0,
			TEXTURE_1 = GL_TEXTURE1,
			TEXTURE_2 = GL_TEXTURE2,
			TEXTURE_3 = GL_TEXTURE3,
			TEXTURE_4 = GL_TEXTURE4,
			TEXTURE_5 = GL_TEXTURE5,
			TEXTURE_6 = GL_TEXTURE6,
			TEXTURE_7 = GL_TEXTURE7,
			TEXTURE_8 = GL_TEXTURE8,
			TEXTURE_9 = GL_TEXTURE9,
			TEXTURE_10 = GL_TEXTURE10,
			TEXTURE_11 = GL_TEXTURE11,
			TEXTURE_12 = GL_TEXTURE12,
			TEXTURE_13 = GL_TEXTURE13,
			TEXTURE_14 = GL_TEXTURE14,
			TEXTURE_15 = GL_TEXTURE15,
			TEXTURE_16 = GL_TEXTURE16,
			TEXTURE_17 = GL_TEXTURE17,
			TEXTURE_18 = GL_TEXTURE18,
			TEXTURE_19 = GL_TEXTURE19,
			TEXTURE_20 = GL_TEXTURE20,
			TEXTURE_21 = GL_TEXTURE21,
			TEXTURE_22 = GL_TEXTURE22,
			TEXTURE_23 = GL_TEXTURE23,
			TEXTURE_24 = GL_TEXTURE24,
			TEXTURE_25 = GL_TEXTURE25,
			TEXTURE_26 = GL_TEXTURE26,
			TEXTURE_27 = GL_TEXTURE27,
			TEXTURE_28 = GL_TEXTURE28,
			TEXTURE_29 = GL_TEXTURE29,
			TEXTURE_30 = GL_TEXTURE30,
			TEXTURE_31 = GL_TEXTURE31
		};
		enum class WrapMode : GLint
		{
			REPEAT = GL_REPEAT,
			MIRRORED = GL_MIRRORED_REPEAT,
			CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
			CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
		};
		enum class FilterMode: GLint
		{
			LINEAR = GL_LINEAR,
			NEAREST = GL_NEAREST,
      TRILINEAR = GL_LINEAR_MIPMAP_LINEAR,
      TRINEAREST = GL_NEAREST_MIPMAP_NEAREST
		};

    //! @brief Constructor
    Texture(void) = default;
    Texture& operator=(const Texture&) = default;

    //! @brief Constructor
    Texture(const Texture&);
    Texture(const TextureIdentifier& textureIdentifier);
    Texture(unsigned int id, const std::string& name);

    //! @brief Constructor to load from path
		Texture(const std::string& filePath, Channel channel = Channel::RGB
		, FilterMode filterMode = FilterMode::LINEAR
    , WrapMode wrapMode = WrapMode::REPEAT
    , bool hdrImage = false);

    //! @brief Destructors
    ~Texture(void);

    //! @brief Deallocate texture object
    void Release(void);

    //! @brief Deallocate all loaded texture object
    static void ReleaseAllLoadedTextures(void);

    //! @brief Bind
		void Bind(void) const;

    //! @brief Bind to TextureUnit
		void BindToTextureUnit(TextureUnit textureUnit) const;

    //! @brief Bind to TextureUnit
    void BindToTextureUnit(int index) const;

    //! @brief Unbind
		void Unbind(void) const;

    //! @brief Set name (and internal debug name)
    void SetName(const char* name);

    //! @brief Get Internal Format
    Channel GetInternalFormat(void) { return m_internalFormat; }

    //! @brief Get Opengl Generated ID
		GLuint GetID(void) const { return m_textureID; }

    //! @brief Get File Path
    std::string GetFilePath(void) const { return m_filePath; }

    //! @brief Get Texture Data
    TextureData GetTextureData(void) const { return TextureData{m_filePath, (GLenum)m_internalFormat, (GLenum)m_filterMode}; }

    //! @brief Check if the texture is valid
		GLuint IsValid(void) const { return m_textureID != ~(0);}

    //! @brief Clear texture variables
    inline void Clear(void) { m_textureID = ~(0); m_name.clear(); m_filePath.clear(); }

    //*****************************************************
    // Static Method
    //*****************************************************
    //! @brief Constructor to load from path
    static NightEngine::EC::Handle<Rendering::Texture> LoadTextureHandle(const std::string& filePath, Channel channel = Channel::RGB
      , FilterMode filterMode = FilterMode::LINEAR
      , WrapMode wrapMode = WrapMode::REPEAT
      , bool hdrImage = false);

    //! @brief Load file and Generate Texture
    static TextureIdentifier LoadTexture(const std::string& filePath, Channel internalFormat = Channel::RGB
      , FilterMode filterMode = FilterMode::LINEAR, WrapMode wrapMode = WrapMode::REPEAT);

    //! @brief Load file and Generate Texture
    static TextureIdentifier LoadHDRTexture(const std::string& filePath, Channel internalFormat = Channel::RGB
      , FilterMode filterMode = FilterMode::LINEAR, WrapMode wrapMode = WrapMode::REPEAT);

    //! @brief Generate Null texture for FrameBuffer Attachment
    static TextureIdentifier GenerateNullTexture(int width, int height
      , Channel internalformat = Channel::RGB
      , Channel format = Channel::RGB
      , FilterMode filterMode = FilterMode::LINEAR, WrapMode wrapMode = WrapMode::REPEAT);

    //! @brief Generate Texture from imgData
    static TextureIdentifier GenerateTextureData(void* imgData
      , int width, int height
      , Channel internalFormat, Channel format
      , FilterMode filterMode, WrapMode wrapMode);

    //! @brief Set opengl blend mode
    static void SetBlendMode(bool enable);

    //! @brief Active the texture unit
    static void ActiveTextureUnit(TextureUnit textureUnit);
	private:
		GLuint      m_textureID = ~(0);
    std::string m_name;
    std::string m_filePath; //path to be serialize/deserialize

    Channel     m_internalFormat = Channel::RGB;
    FilterMode  m_filterMode = FilterMode::TRINEAREST;
	};
} // Rendering

