#pragma once

using JText      = std::string;
using JTextView  = std::string_view;
using JWText     = std::wstring;
using JWTextView = std::wstring_view;

#define CLASS_PTR(klassName)\
class klassName;\
using klassName##UPtr = std::unique_ptr<klassName>;\
using klassName##Ptr = std::shared_ptr<klassName>;\
using klassName##WPtr = std::weak_ptr<klassName>;


inline JWText String2WString(const JText& InString)
{
	USES_CONVERSION;
	return JWText{A2W(InString.c_str())};
}

inline JText WString2String(const JWText& InWString)
{
	USES_CONVERSION;
	return JText{W2A(InWString.c_str())};
}
