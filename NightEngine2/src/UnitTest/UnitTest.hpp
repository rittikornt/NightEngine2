/*!
  @file UnitTest.hpp
  @author Rittikorn Tangtrongchit
  @brief Contain the Interface of UnitTest
*/
#pragma once
#include <vector>

#include "Core/Reflection/ReflectionMacros.hpp"

namespace UnitTest
{
	//! @brief Run test with specific arguments list
	int RunTest(int argc, char * argv[]);

	//! @brief Overload version of RunTest
	int RunTest(std::vector<char*>& argv);

  //! @brief Data type for Unit testing
	namespace Reflection
	{
		struct TestReflection
		{
			int m_int;
			float m_float;
			std::vector<int> m_ints;
			int* m_intPtr;
			int** m_intPtrPtr;

		private:
			int m_private;
			int m_secret;

			REFLECTABLE_TYPE();
		};
	}

  //! @brief Data type for Unit testing, exposed for reflection registration
	namespace Serialization
	{
		struct PODStruct
		{
			bool m_pod_bool;
			int m_pod_int;
			unsigned int m_pod_unsigned;
			float m_pod_float;

			bool operator==(const PODStruct& rhs) const
			{
				return m_pod_bool == rhs.m_pod_bool &&
				m_pod_int == rhs.m_pod_int &&
				m_pod_unsigned == rhs.m_pod_unsigned &&
				m_pod_float == rhs.m_pod_float;
			}

			bool operator!=(const PODStruct& rhs) const
			{
				return !(*this == rhs);
			}
		};

		class ClassWithProtected
		{
		public:
			ClassWithProtected(void) = default;
			ClassWithProtected(const ClassWithProtected&) = default;
			ClassWithProtected(bool b, int i, unsigned u, float f
				, bool b2, int i2, unsigned u2, float f2)
				: m_class_bool(b), m_class_int(i), m_class_unsigned(u), m_class_float(f)
				, m_class_POD({ b2, i2, u2, f2 }) {}

			bool operator==(const ClassWithProtected& rhs) const
			{
				return m_class_bool == rhs.m_class_bool &&
					m_class_int == rhs.m_class_int &&
					m_class_unsigned == rhs.m_class_unsigned &&
					m_class_float == rhs.m_class_float && 
					m_class_POD == rhs.m_class_POD;
			}

			bool operator!=(const ClassWithProtected& rhs) const
			{
				return !(*this == rhs);
			}

			bool m_class_bool;
			int m_class_int;
			unsigned int m_class_unsigned;
			float m_class_float;
		protected:
			PODStruct m_class_POD;

			REFLECTABLE_TYPE();
		};

		class DerivedClass: public ClassWithProtected
		{
		public:
			DerivedClass(void) = default;
			DerivedClass(ClassWithProtected protect, PODStruct pod1, PODStruct pod2)
				: ClassWithProtected(protect), m_derived_POD(pod1), m_derived_POD_2(pod2) {}

			bool operator==(const DerivedClass& rhs) const
			{
				return (ClassWithProtected::operator==(rhs))
					&& m_derived_POD == rhs.m_derived_POD
					&& m_derived_POD_2 == rhs.m_derived_POD_2;
			}
			bool operator!=(const DerivedClass& rhs) const
			{
				return !(*this == rhs);
			}
		protected:
			PODStruct m_derived_POD;
			PODStruct m_derived_POD_2;

			REFLECTABLE_TYPE();
		};

		class SecondDerivedClass : public DerivedClass
		{
		public:
			SecondDerivedClass(void) = default;
			SecondDerivedClass(const DerivedClass& derived
			, int i, float f)
				: DerivedClass(derived), m_secondDerived_int(i)
				, m_secondDerived_float(f){}

			bool operator==(const SecondDerivedClass& rhs) const
			{
				return (ClassWithProtected::operator==(rhs))
					&& (DerivedClass::operator==(rhs))
					&& m_secondDerived_int == rhs.m_secondDerived_int
					&& m_secondDerived_float == rhs.m_secondDerived_float;
			}
			bool operator!=(const SecondDerivedClass& rhs) const
			{
				return !(*this == rhs);
			}
		protected:
			int m_secondDerived_int;
			float m_secondDerived_float;

			REFLECTABLE_TYPE();
		};

		class SecondPrivateDerivedClass : private DerivedClass
		{
		public:
			SecondPrivateDerivedClass(void) = default;
			SecondPrivateDerivedClass(const DerivedClass& derived
				, int i, float f)
				: DerivedClass(derived), m_secondPrivateDerived_int(i)
				, m_secondPrivateDerived_float(f) {}

			bool operator==(const SecondPrivateDerivedClass& rhs) const
			{
				return (ClassWithProtected::operator==(rhs))
					&& (DerivedClass::operator==(rhs))
					&& m_secondPrivateDerived_int == rhs.m_secondPrivateDerived_int
					&& m_secondPrivateDerived_float == rhs.m_secondPrivateDerived_float;
			}
			bool operator!=(const SecondPrivateDerivedClass& rhs) const
			{
				return !(*this == rhs);
			}
		protected:
			int m_secondPrivateDerived_int;
			float m_secondPrivateDerived_float;

			REFLECTABLE_TYPE();
		};

		class ThirdDerivedClass : public SecondPrivateDerivedClass
		{
		public:
			ThirdDerivedClass(void) = default;
			ThirdDerivedClass(SecondPrivateDerivedClass spdc,int i, float f)
				: SecondPrivateDerivedClass(spdc)
				, m_thridPrivateDerived_int(i)
				, m_thirdPrivateDerived_float(f) {}

			bool operator==(const ThirdDerivedClass& rhs) const
			{
				return  m_thridPrivateDerived_int == rhs.m_thridPrivateDerived_int
					&& m_thirdPrivateDerived_float == rhs.m_thirdPrivateDerived_float;
			}
			bool operator!=(const ThirdDerivedClass& rhs) const
			{
				return !(*this == rhs);
			}
		protected:
			int m_thridPrivateDerived_int;
			float m_thirdPrivateDerived_float;

			REFLECTABLE_TYPE();
		};
	}
}