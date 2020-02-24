/*!
  @file Handle.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Handle
*/
#pragma once
#include "Core/Macros.hpp"
#include "Core/Container/Slotmap.hpp"
#include "Core/Container/MurmurHash2.hpp"

namespace NightEngine
{
	using namespace Container;

  namespace EC
  {
		//! @brief Handle for Object lookup, destroy
		struct HandleObject
		{
			using LookupFN = void* (*)(const SlotmapID& id);
			using DestroyFN = void (*)(const SlotmapID& id);

			SlotmapID m_slotmapID;	//Id for lookup
			LookupFN  m_lookupFN = nullptr;		//function for lookup
			DestroyFN m_destroyFN = nullptr;	//function for destroy

      //! @brief Constructor
			HandleObject() = default;
			HandleObject(SlotmapID slotmapID, LookupFN lookupFN, DestroyFN destroyFN)
				: m_slotmapID(slotmapID), m_lookupFN(lookupFN), m_destroyFN(destroyFN){}

      //! @brief Check Validity
      bool IsValid(void) const
      {
        //TODO: Should also check if lookup successfully or not (valid generation?)
        return m_lookupFN != nullptr
          && m_destroyFN != nullptr;
      }

      //! @brief Check Equality
			bool operator==(const HandleObject& rhs) const
			{
				return m_slotmapID == rhs.m_slotmapID 
					&& m_lookupFN == rhs.m_lookupFN;
			}

      //! @brief Get the actual object
			template<class T>
			inline T* Get() const
			{
				ASSERT_TRUE(m_lookupFN != nullptr);
				return static_cast<T*>((*m_lookupFN)(m_slotmapID));
			}

      //! @brief Get void pointer to object
      inline void* GetPointer() const
      {
        ASSERT_TRUE(m_lookupFN != nullptr);
        return (*m_lookupFN)(m_slotmapID);
      }

      //! @brief Destroy the object
			inline void Destroy()
			{
				ASSERT_TRUE(m_destroyFN != nullptr);
				(*m_destroyFN)(m_slotmapID);
			}

      //**********************************************************
      // Static Method
      //**********************************************************

      //! @brief Static Lookup function
      static void* LookupHandle(void* lookupFNPtr
        , U64 index, U64 generation)
      {
        LookupFN lookupFN = static_cast<LookupFN>(lookupFNPtr);
        ASSERT_TRUE(lookupFN != nullptr);

        return lookupFN(SlotmapID(index, generation));
      }

      //! @brief Static Lookup function
      template<class T>
      static inline T* LookupHandle(void* lookupFNPtr
        , U64 index, U64 generation)
      {
        LookupFN lookupFN = static_cast<LookupFN>(lookupFNPtr);
        ASSERT_TRUE(lookupFN != nullptr);

        return static_cast<T*>(lookupFN(SlotmapID(index, generation)));
      }
		};

		//! @brief HandleObject Wrapper with Type Infomation
		template<class T>
		struct Handle
		{
			HandleObject m_handle;  //Internal HandleObject

      //! @brief Constructor
			Handle() = default;

      //! @brief Constructor for handleObject
			explicit Handle(const HandleObject& handle)
				: m_handle(handle) {}

      //! @brief Check Validity
      bool IsValid(void) const
      {
        return m_handle.IsValid();
      }

      //! @brief Get the data
			inline T* Get() const
			{
				return m_handle.Get<T>();
			}

      //! @brief Destroy the data
			inline void Destroy()
			{
				m_handle.Destroy();
			}

      //! @brief Get the dereferenced data
      T* operator->()
      {
        return Get();
      }

      //! @brief Get the dereferenced data
      const T* operator->() const
      {
        return Get();
      }

      //! @brief Get the dereferenced data
			T& operator*()
			{
				return *Get();
			}

      //! @brief Get the dereferenced data
			const T& operator*() const
			{
				return *Get();
			}

      inline int GetSlotMapID() const
      {
        return m_handle.m_slotmapID.m_index;
      }
		};

    //!brief Struct for converting HandleObject to Hash
    struct HandleObjectHash
    {
      std::size_t operator()(const HandleObject& handle) const
      {
        return NightEngine::Container::ConvertToHash(reinterpret_cast<const char*>(&handle)
          , sizeof(handle));
      }
    };
  }
}