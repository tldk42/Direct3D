#pragma once
#include <comdef.h>
#include <source_location>

#include "common_include.h"
#include "Utils/Logger.h"


inline void CheckResult(HRESULT InResult, const std::source_location& InLocation = std::source_location::current())
{
	if (!SUCCEEDED(InResult))
	{
		_com_error err(InResult);
		LPCTSTR    errMsg = err.ErrorMessage();

		LOG_CORE_ERROR("Function:{0}, Line:{1}, Error:{2}", InLocation.file_name(), InLocation.line(),
					   WString2String( errMsg));

		assert(false);
	}
}
