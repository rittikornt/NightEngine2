/*!
  @file OpenglAllocationTracker.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of OpenglAllocationTracker
*/
#pragma once
#include "Core/Macros.hpp"
#include <unordered_set>
#include <unordered_map>

//! @brief Macros for incrementing allocation count
#define INCREMENT_ALLOCATION(TYPENAME, ID)	\
  OpenglAllocationTracker::Increment(STR_INNER(TYPENAME), ID)\

//! @brief Macros for decrementing allocation count
#define DECREMENT_ALLOCATION(TYPENAME, ID)	\
  OpenglAllocationTracker::Decrement(STR_INNER(TYPENAME), ID)\

//! @brief Macros for checking if object is allocated
#define IS_ALLOCATED(TYPENAME, ID)	\
  OpenglAllocationTracker::Exist(STR_INNER(TYPENAME), ID)\

//! @brief Macros for checking if object is leaking
#define CHECK_LEAK(TYPENAME, ID)	\
  OpenglAllocationTracker::CheckLeak(STR_INNER(TYPENAME), ID)\

//! @brief Macros registering deallocation function
#define REGISTER_DEALLOCATION_FUNC(TYPENAME, FUNC)	\
  static GLDeallocationFunctions TYPENAME##Registration{STR_INNER(TYPENAME),FUNC};\

namespace Graphic
{
  using DeallcateFunc = void (*)(unsigned int);
  
  struct OpenglAllocationTracker
	{
    static void Increment(const char* typeName, unsigned int id);

    static void Decrement(const char* typeName, unsigned int id);

    static bool Exist(const char* typeName, unsigned int id);

    static bool CheckLeak(const char* typeName, unsigned int id);

    static std::unordered_set<unsigned int>& GetAllocatedObjectID(const char* typeName);

    static void DeallocateAllLoadedObjects(void);

    static void DeallocateAllObjects(std::string typeName, DeallcateFunc func);

    static void PrintAllocationState(void);
	};

  class GLDeallocationFunctions
  {
  public:
    GLDeallocationFunctions(std::string typeName, DeallcateFunc func);

    static std::unordered_map<std::string, DeallcateFunc>& GetDataMap(void);

    static void RegisterDeallocateFunc(std::string typeName, DeallcateFunc func);

    static DeallcateFunc GetDeallocateFunc(std::string typeName);
  };
} // Graphic

