#pragma once

class JObject
{
public:
	virtual ~JObject() {}

	virtual void BeginPlay() {}
	virtual void Tick(float DeltaTime) {}

	template <class T>
	static T* LoadResource(const std::string& Path)
	{
		// implementation goes here
	}

	template <class T>
	static T* SpawnObject()
	{
		// implementation goes here
	}
};
