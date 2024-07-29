#pragma once

constexpr uint32_t djb2_impl(const char* InString, uint32_t PrevHash)
{
	int32_t c;
	while ((c    = *InString++))
		PrevHash = ((PrevHash << 5) + PrevHash) + c;

	return PrevHash;
}

constexpr uint32_t StringHash(const char* InString)
{
	return djb2_impl(InString, 5381);
}
