#pragma once



namespace DirectUI {
	template<typename Name>
	class StandardCreator {

	};
	template<typename NewClass, typename BaseClass, typename Creator>
	class ClassInfo : ClassInfoBase
	{
	public:
		HRESULT WINAPI CreateInstance(Element* rootElement, unsigned long* unknown, Element** result)
		{
			return NewClass::CreateInstance(rootElement, unknown, result);
		}
		IClassInfo* WINAPI GetBaseClass()
		{
			return BaseClass::GetClassInfoPtr();
		}
		HRESULT DoRegister() { return ClassInfoBase::Register(); }
		void WINAPI Destroy()
		{
			delete this;
			NewClass::Class = NULL;
		}

		static HRESULT Create(ClassInfo** result)
		{
			ClassInfo* theClass = new ClassInfo();
			HRESULT hr = theClass->Initialize(g_hInst, (UCString)NewClass::DoGetClassName(), 0, 0, 0);
			if (SUCCEEDED(hr))
			{
				*result = theClass;
			}
			return hr;
		}
		static HRESULT Register()
		{
			HRESULT hr = S_OK;
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
				auto lock = Element::GetFactoryLock();
				auto lock2 = new DirectUI::CritSecLock(lock);
				IClassInfo* ptr = BaseClass::GetClassInfoPtr();
				IClassInfo* newrclass = NewClass::GetClassInfoPtr();
				if (DirectUI::ClassInfoBase::ClassExist(&ptr, NULL, 0, newrclass, g_hInst, (UCString)NewClass::DoGetClassName(), false))
				{
					NewClass::Class = ptr;
				}
				else
				{
					ClassInfo* classInfo = NULL;
					if (SUCCEEDED(hr = Create(&classInfo)))
					{
						if (FAILED(hr = classInfo->DoRegister()))
						{
							classInfo->Destroy();
						}
						else
						{
							NewClass::Class = (IClassInfo*)classInfo;
						}
					}
				}
			}
			return hr;
		}
	};
}