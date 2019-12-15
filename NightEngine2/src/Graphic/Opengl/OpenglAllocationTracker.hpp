/*!
  @file OpenglAllocationTracker.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of OpenglAllocationTracker
*/
#pragma once
#include "Core/Macros.hpp"

//! @brief Macros for incrementing allocation count
#define INCREMENT_ALLOCATION(TYPENAME)	\
  OpenglAllocationTracker::Increment(STR_INNER(TYPENAME))\

//! @brief Macros for decrementing allocation count
#define DECREMENT_ALLOCATION(TYPENAME)	\
  OpenglAllocationTracker::Decrement(STR_INNER(TYPENAME))\

namespace Graphic
{
	struct OpenglAllocationTracker
	{
    static void Increment(const char* typeName);

    static void Decrement(const char* typeName);

    static void PrintAllocationState();
	};
} // Graphic

