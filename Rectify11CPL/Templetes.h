#pragma once



namespace DirectUI {
	template<typename Name>
	class StandardCreator {

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
			HRESULT hr = theClass->Initialize((HINSTANCE)0xFFFFFFFFFFFFFFFF, (UCString)NewClass::DoGetClassName(), false, NULL, 0);
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
				CritSecLock* lock = new DirectUI::CritSecLock(Element::GetFactoryLock());
				IClassInfo* baseclassptr = BaseClass::GetClassInfoPtr();
				IClassInfo* newrclass = NewClass::GetClassInfoPtr();
				if (DirectUI::ClassInfoBase::ClassExist(&baseclassptr, NULL, 0, newrclass, g_hInst, (UCString)NewClass::DoGetClassName(), false))
				{
					NewClass::Class = baseclassptr;
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