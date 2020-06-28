/*!
  @file Macros.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Macros
*/
#pragma once
#include "Logger.hpp"
#include "EngineConfigs.hpp"

//TODO: Should this be in file system? 
#ifdef _WIN32

#define PROJECT_DIR_LOGFILE "./../Assets/"
#define PROJECT_DIR_SOURCE "./../NightEngine/"
#define PROJECT_DIR_SOURCE_ASSETS "./../Assets/"
#define PROJECT_DIR_SOURCE_SHADER "./../Assets/Shaders/"

#endif
#ifdef __APPLE__

#define PROJECT_DIR_LOGFILE "./../Assets/"
#define PROJECT_DIR_SOURCE "./../NightEngine/"
#define PROJECT_DIR_SOURCE_ASSETS "./../Assets/"
#define PROJECT_DIR_SOURCE_SHADER "./../Assets/Shaders/"
// #define PROJECT_DIR_LOGFILE "./Assets/"
// #define PROJECT_DIR_SOURCE "./NightEngine/"
// #define PROJECT_DIR_SOURCE_ASSETS "./Assets/"
// #define PROJECT_DIR_SOURCE_SHADER "./Assets/Shaders/"

#endif

//***********************************************
// Utilities
//***********************************************

#define CONCAT(X,Y) X##Y                      //make XY
#define STR_INNER(X) #X                       //indirection for correctness
#define STRINGIFY(X) STR_INNER(X)             //make �X�
#define STR_CAT(X,Y) STRINGIFY(CONCAT(X,Y))   //make �XY�
#define ENUMIFY(PREFIX, X) PREFIX_##X,        //for declaring enum

///////////////////////////////////////////////////////////////////////////////////////////

#if DEBUG_MODE
	//Debug Block
	#define DEBUG_BLOCK(x) do { x } while(0)

	//Trace
	#ifdef _WIN32
		#define TRACE() NightEngine::Debug::Log.Print(NightEngine::Logger::MessageType::INFO,__FUNCSIG__);
	#else
		#define TRACE() NightEngine::Debug::Log.Print(NightEngine::Logger::MessageType::INFO,__PRETTY_FUNCTION__);
	#endif

	//Debug Break
	#ifdef __linux__
		#include <signal.h>
		#define DEBUG_BREAK() do { raise(SIGTRAP); } while(0)
	#endif
	#ifdef _WIN32
		#define DEBUG_BREAK() do { __debugbreak(); } while(0)
	#endif
	#ifdef __APPLE__
		#include <signal.h>
		#define DEBUG_BREAK() do { raise(SIGTRAP); } while(0)
	#endif

	//Assert
	#define ASSERT_TRUE(EXP)																															\
		do																																									\
		{																																										\
			if(!(EXP))																																	\
			{																																									\
				NightEngine::Debug::Log << NightEngine::Logger::MessageType::ERROR_MSG << "Debug assertion \""		\
					<< #EXP << "\" FAILED in: " << __func__ << ", line: " << __LINE__				\
					<< ", file: " << __FILE__ << '\n';																				\
				DEBUG_BREAK();																																	\
			}																																									\
		} while(0)	
  #define ASSERT_MSG(EXP, ERRORMSG)																														\
		  do																																									\
		  {																																										\
			  if(!(EXP))																																	      \
			  {																																									\
				  NightEngine::Debug::Log << NightEngine::Logger::MessageType::ERROR_MSG << "Debug assertion \""		\
					  << #EXP << "\" FAILED in: " << __func__ << ", line: " << __LINE__				      \
					  << ", file: " << __FILE__ << '\n';                                            \
          NightEngine::Debug::Log << NightEngine::Logger::MessageType::ERROR_MSG << ERRORMSG << '\n';       \
				  DEBUG_BREAK();																																	\
			  }																																									\
		  } while(0)																																	
#else
	#define DEBUG_BLOCK(x)
	#define TRACE()
	#define ASSERT_TRUE(condition)
	#define DEBUG_BREAK()
#endif

//Opengl debugger
#if DEBUG_MODE
#define CHECKGL_ERROR() do {                                                         \
		GLenum err;                                                                  \
		while((err = glGetError()) != GL_NO_ERROR) {                                 \
			std::string text;                                                          \
			switch(err) {                                                              \
				case GL_INVALID_ENUM:                                                    \
					text = "GL_INVALID_ENUM";                                              \
					break;                                                                 \
				case GL_INVALID_VALUE:                                                   \
					text = "GL_INVALID_VALUE";                                             \
					break;                                                                 \
				case GL_INVALID_OPERATION:                                               \
					text = "GL_INVALID_OPERATION";                                         \
					break;                                                                 \
				case GL_STACK_OVERFLOW:                                                  \
					text = "GL_STACK_OVERFLOW";                                            \
					break;                                                                 \
				case GL_OUT_OF_MEMORY:                                                   \
					text = "GL_OUT_OF_MEMORY";                                             \
					break;                                                                 \
				case GL_STACK_UNDERFLOW:                                                 \
					text = "GL_STACK_UNDERFLOW";                                           \
					break;                                                                 \
				case GL_INVALID_FRAMEBUFFER_OPERATION:                                   \
					text = "GL_INVALID_FRAMEBUFFER_OPERATION";                             \
					break;                                                                 \
				case GL_CONTEXT_LOST:                                                    \
					text = "GL_CONTEXT_LOST";                                              \
					break;                                                                 \
				case GL_TABLE_TOO_LARGE:                                                 \
					text = "GL_TABLE_TOO_LARGE";                                           \
					break;                                                                 \
				default:                                                                 \
					text = "Unknown error";                                                \
			}                                                                          \
			NightEngine::Debug::Log << NightEngine::Logger::MessageType::ERROR_MSG <<											\
				"OpenGL error encountered! " << __func__ << ", line " << __LINE__        \
				<< ", file " << __FILE__ << ": " << text << std::endl;                   \
			DEBUG_BREAK();                                                             \
		}                                                                            \
	}                                                                              \
	while(0)
#else
	#define CheckGL()
#endif