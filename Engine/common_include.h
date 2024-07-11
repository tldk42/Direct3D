#pragma once

using Text      = std::string;
using TextView  = std::string_view;
using WText     = std::wstring;
using WTextView = std::wstring_view;

#define CLASS_PTR(klassName)\
class klassName;\
using klassName##UPtr = std::unique_ptr<klassName>;\
using klassName##Ptr = std::shared_ptr<klassName>;\
using klassName##WPtr = std::weak_ptr<klassName>;

class ICoreInterface
{
public:
	virtual ~ICoreInterface() = default;

public:
#pragma region Core Interface
	virtual void Initialize() = 0;
	virtual void Update(float_t DeltaTime) = 0;
	virtual void Render() = 0;
	virtual void Release() = 0;
#pragma endregion
};
