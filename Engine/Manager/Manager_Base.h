#pragma once
#include "Utils/HelperFunction.h"

template <class SingletonType>
class TSingleton
{
public:
	[[nodiscard]] static SingletonType& Get()
	{
		static SingletonType instance;
		return instance;
	}
};

template <class ManagedType, class Manager>
class Manager_Base : public TSingleton<Manager>
{
public:
	template <class ReturnType = ManagedType, typename... Args>
	ReturnType* CreateOrLoad(const std::wstring& InName, Args&&... InArgs);

	template <class ReturnType = ManagedType>
	ReturnType* FetchResource(const std::wstring& InName);

	void SafeRemove(const std::wstring& InName);

protected:
	std::unordered_map<std::wstring, std::unique_ptr<ManagedType>> mManagedList;
};

template <class ReturnType, class ClassType>
template <class ReturnClass, typename... Args>
ReturnClass* Manager_Base<ReturnType, ClassType>::CreateOrLoad(const std::wstring& InName, Args&&... InArgs)
{
	// Usage
	std::wstring id = ParseFile(InName);

	if (ReturnClass* resource = FetchResource<ReturnClass>(id))
	{
		return resource;
	}

	std::unique_ptr<ReturnClass> obj    = std::make_unique<ReturnClass>(InName, std::forward<Args>(InArgs)...);
	ReturnClass*                 rawPtr = obj.get();
	mManagedList.try_emplace(id, std::move(obj));

	return rawPtr;
}

template <class ManagedType, class Manager> template <class ReturnType> ReturnType* Manager_Base<ManagedType, Manager>::
FetchResource(const std::wstring& InName)
{
	auto it = mManagedList.find(InName);
	if (it != mManagedList.end())
	{
		return dynamic_cast<ReturnType*>(it->second.get());
	}
	return nullptr;
}

template <class ManagedType, class Manager> void Manager_Base<ManagedType, Manager>::SafeRemove(const std::wstring& InName)
{
	mManagedList.erase(InName);
}
