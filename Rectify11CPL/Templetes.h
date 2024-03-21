#pragma once
// DirectUI class creation helper

namespace DirectUI {
	template<typename Name>
	class StandardCreator {
		// Unknown as to what this implements
	};
	template<typename NewClass, typename BaseClass, typename Creator>
	class ClassInfo : ClassInfoBase
	{
	public:
		HRESULT CreateInstance(Element* rootElement, unsigned long* unknown, Element** result) override
		{
			return NewClass::CreateInstance(rootElement, unknown, result);
		}
		IClassInfo* GetBaseClass() override
		{
			return BaseClass::GetClassInfoPtr();
		}
		HRESULT DoRegister() { return ClassInfoBase::Register(); }

		void Destroy() override
		{
			delete this;
			NewClass::Class = NULL;
		}

		static HRESULT Create(ClassInfo** result)
		{
			ClassInfo* theClass = new ClassInfo();

			// DirectUI expects fields to be initialized to zero
			memset(theClass->data, 0, sizeof(theClass->data));

			HRESULT hr = theClass->Initialize(g_hInst, (UCString)NewClass::DoGetClassName(), false, NULL, 0);
			if (SUCCEEDED(hr))
			{
				*result = theClass;
			}
			return hr;
		}
		static HRESULT Register()
		{
			HRESULT hr = S_OK;
			
			// register base class
			if (BaseClass::GetClassInfoPtr() == NULL)
			{
				hr = BaseClass::Register();
			}
			else
			{
				BaseClass::GetClassInfoPtr()->AddRef();
			}

			if (SUCCEEDED(hr))
			{
				CritSecLock lock(Element::GetFactoryLock());

				IClassInfo* baseclassptr = BaseClass::GetClassInfoPtr();
				IClassInfo* newrclass = NewClass::GetClassInfoPtr();

				// ensure that the class does not exist
				if (DirectUI::ClassInfoBase::ClassExist(&baseclassptr, NULL, 0, newrclass, g_hInst, (UCString)NewClass::DoGetClassName(), false))
				{
					NewClass::Class = baseclassptr;
				}
				else
				{
					ClassInfo* classInfo = NULL;
					if (SUCCEEDED(hr = Create(&classInfo)))
					{
						if (SUCCEEDED(hr = classInfo->DoRegister()))
						{
							NewClass::Class = (IClassInfo*)classInfo;
						}
						else
						{
							classInfo->Destroy();
						}
					}
				}
			}
			return hr;
		}
	};
}