#pragma once

namespace Utils
{
	class FBXLoader
	{
	public:
		FBXLoader();
		~FBXLoader();

	public:
		void LoadFBX(const char* InFileName);
		
	};

}
