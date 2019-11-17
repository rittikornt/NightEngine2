/*!
  @file Slotmap.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of Slotmap
*/
#pragma once
#include <utility>

#include "Core/Container/Vector.hpp"
#include "Core/Container/PrimitiveType.hpp"

namespace Core
{
	namespace Container
	{
    //Forward Declaration
    template<typename T> class Slotmap;

    //! @brief Id for accessing data in Slotmap
		struct SlotmapID
		{
      U64 m_index: 47;	      //Array index for direct lookup
      U64 m_generation: 17;	  //For validate reference

			SlotmapID(void): m_index(~0), m_generation(~0){}
			explicit SlotmapID(U64 index, U64 generation)
				: m_index(index), m_generation(generation) {}

			bool operator==(const SlotmapID& rhs) const
			{
				return m_index == rhs.m_index 
					&& m_generation == rhs.m_generation;
			}
		};

		//! @brief Label on each SlotEntry that store active flag and generation count
		struct SlotEntryLabel
		{
      U64 m_active : 1;
			U64 m_generation: 17;

			U32 m_lastActiveIndex : 23;		//Store index of last active slot
      U32 m_nextActiveIndex : 23;   //Store index of next active slot
		};

    //! @brief Store Label along with the data, can store maximum 2^32 slot 
    // due to u32 used in freelist (can be expand to store max of 2^46)
		template<typename T>
		using SlotEntry = std::pair<SlotEntryLabel, T>;

    //! @brief Container for storing T
		template<typename T>
		class Slotmap
		{
		public:
      //! @brief Iterator for iterating Slotmap directly
      struct Iterator
      {
        Slotmap<T>* m_slotmap;
        U32 m_index;  //Current reference index in the slotmap

        //! @brief Constructor
        Iterator(Slotmap<T>* slotmapPtr)
          : m_slotmap(slotmapPtr), m_index(slotmapPtr->m_head) {}

        //! @brief Get referenced object
        T* Get(void)
        {
          ASSERT_TRUE(!IsEnd());
          return &(m_slotmap->m_array[m_index].second);
        }

        //! @brief Get referenced object safely
        T* GetSafely(void)
        {
          return IsEnd()? nullptr: &(m_slotmap->m_array[m_index].second);
        }

        //! @brief Next referenced object
        void Next(void)
        {
          //Next index's nextActiveIndex
          m_index = m_slotmap->GetSlotEntryLabel(m_index).m_nextActiveIndex;
        }

        //! @brief Last referenced object
        void Last(void)
        {
          //Last index's nextActiveIndex
          m_index = m_slotmap->GetSlotEntryLabel(m_index).m_lastActiveIndex;
        }

        //! @brief Get next referenced object
        T* GetNext(void)
        {
          Next();
          return GetSafely();
        }

        //! @brief Get last referenced object
        T& GetLast(void)
        {
          Last();
          return GetSafely();
        }

        //! @brief Check if this iterator is null
        bool IsEnd()
        {
          return m_index == Slotmap<T>::NullIndex;
        }
      };

      //! @brief Constructor for initialization
			explicit Slotmap(size_t reserveAmount
				, U32 expandRate) : m_expandRate(expandRate)
			{
				m_array.reserve(reserveAmount);
				m_freelist.reserve(expandRate);
			}

			//! @brief Create slot and return the id for access
			SlotmapID CreateSlot(void);

      //! @brief Destroy the slot of a specific id
			void			Destroy(SlotmapID id);

      //! @brief Get the slot corresponding with a specific id
			T*				Get(SlotmapID id);

      //! @brief Get the reference to the internal array, useful for traversal
			Container::Vector<SlotEntry<T>>& GetArray();

      //! @brief Get SloatEntryLabel on index
      SlotEntryLabel& GetSlotEntryLabel(U32 index);

      //! @brief Set the reserve option
			void			Reserve(size_t reserveAmount, U32 expandRate);

      //! @brief Get the amount of total active slot
			size_t		Size() { return m_array.size() - m_freelist.size(); }

      //! @brief Get Iterator for Slotmap traversal
      Iterator GetIterator(void) { return Slotmap<T>::Iterator(this); }

		private:
			U32 m_expandRate;

			Container::Vector<SlotEntry<T>> m_array;  //Actual data
			Container::Vector<U32> m_freelist;        //Freelist of index

			U32 m_head = NullIndex;
			U32 m_tail = NullIndex;
      U32 m_lastCreated = NullIndex;

      static const U32 NullIndex = (0x7FFFFF);  //For checking null index (~0) for 23 bits 
		};

    //***************************************
		// Slotmap Definition
    //***************************************
		template <typename T>
		SlotmapID Slotmap<T>::CreateSlot()
		{
			//If no more free slot, expand the slot
			if (m_freelist.empty())
			{
				//Expand array
				size_t size = m_array.size();
				for (int i = m_expandRate - 1; i >= 0; --i)
				{
					//Active: false, generation: 0, last/nextIndex: none
					SlotEntry<T> entry{ { false,0, NullIndex, NullIndex } , T() };
					m_array.emplace_back(std::move(entry));
					m_freelist.emplace_back(size + i);
				}
			}

			//Request new slot index from freelist
			U32 newIndex = m_freelist.back();
			m_freelist.pop_back();
			
			//Setup slot Label
			m_array[newIndex].first.m_active = true;

      //Set head for 1st slot
      if (Size() == 1)
      {
        m_head = newIndex;
      }
      else
      {
        //Set next/last Reference index
        GetSlotEntryLabel(m_lastCreated).m_nextActiveIndex = newIndex;
        GetSlotEntryLabel(newIndex).m_lastActiveIndex = m_lastCreated;
      }
      m_tail = newIndex;          //New index is always the tail
      m_lastCreated = newIndex;   //Save lastCreated 

			return SlotmapID{ newIndex, m_array[newIndex].first.m_generation };
		}

		template <typename T>
		void Slotmap<T>::Destroy(SlotmapID id)
		{
      //Get Label of slotID
      SlotEntryLabel& label = GetSlotEntryLabel(id.m_index);

      //Trying to Destroy invalid id is an error
      ASSERT_TRUE(label.m_generation == id.m_generation);

      //4 Case id.m_index: SingleSlotLeft, Head, Tail, Normal
      //Set SlotEntryLabel's next/last Index
      if (id.m_index == m_head && id.m_index == m_tail)
      {
        m_head = m_tail = NullIndex;  //Null head and tail
      }
      else if (id.m_index == m_head)
      {
        m_head = label.m_nextActiveIndex;  //New head
        GetSlotEntryLabel(m_head).m_lastActiveIndex = NullIndex; //Null last of (new head)
      }
      else if (id.m_index == m_tail)
      {
        m_tail = label.m_lastActiveIndex;  //New tail
        GetSlotEntryLabel(m_tail).m_nextActiveIndex = NullIndex; //Null next of (new tail)
      }
      else
      {
        U32 last = label.m_lastActiveIndex;
        U32 next = label.m_nextActiveIndex;
        GetSlotEntryLabel(last).m_nextActiveIndex = next; //Next of (last) = (next)
        GetSlotEntryLabel(next).m_lastActiveIndex = last; //Last of (next) = (last)
      }

      //TODO: Set new head/tail
      //Check if destroying lastCreated
      if (id.m_index == m_lastCreated)
      {
        m_lastCreated = GetSlotEntryLabel(m_lastCreated).m_lastActiveIndex;  //Last of LastCreated
      }

      //Clear SlotEntryLabel
      label.m_nextActiveIndex = NullIndex;
      label.m_lastActiveIndex = NullIndex;

      //increment generation and push index to freelist
      ++(label.m_generation);
      label.m_active = false;
      m_freelist.push_back(id.m_index);
		}

		template <typename T>
		T* Slotmap<T>::Get(SlotmapID id)
		{
			SlotEntry<T>& entry = m_array[id.m_index];

			//If generation match, return the object
			return id.m_generation == entry.first.m_generation ?
				&(entry.second) : nullptr;
		}

		template<typename T>
		inline Container::Vector<SlotEntry<T>>& Slotmap<T>::GetArray()
		{
			return m_array;
		}

    template<typename T>
    inline SlotEntryLabel& Slotmap<T>::GetSlotEntryLabel(U32 index)
    {
      ASSERT_TRUE(index != NullIndex);
      return m_array[index].first;
    }

		template<typename T>
		inline void Slotmap<T>::Reserve(size_t reserveAmount
			, U32 expandRate)
		{
			m_expandRate = expandRate;
			m_array.reserve(reserveAmount);
			m_freelist.reserve(expandRate);
		}
	}
}