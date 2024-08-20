#pragma once

static constexpr uint32_t JEngine_Version = 1;

const std::filesystem::path localSavePath("Game");

enum class EAssetType
{
	Folder,
	Level,
	Actor,
	Script
};

struct JAssetHeader
{
	char     Signature[8] = "JASSET\0";
	uint32_t Version      = JEngine_Version; // 엔진 버전
	uint32_t DataSize;
};

struct JAssetMetaData
{
	char     ClassName[64];
	uint32_t InstanceCount;
};

class ISerializable
{
public:
	virtual void*  GetDataPtrToVoidPtr() = 0;
	virtual size_t GetDataSize() = 0;
	virtual bool   IsModified() = 0;
	virtual void   ResetState() = 0;
};

class SerializableMember
{
public:
	std::vector<ISerializable*> Members{};

	void AddMember(ISerializable* element)
	{
		Members.push_back(element);
	}
};

class AutoSerializer
{
public:
	/** 멤버 자동 등록 */
	void AddMember(ISerializable* member)
	{
		Serializables.AddMember(member);
	}

	// Get size of total buffer.
	size_t GetSerializationBufferSize() const;

	// Get size of modified Members only.
	size_t GetModifiedSerializationBufferSize() const;

	// Serailize.
	void Serialize(char* Buffer);

	[[deprecated]] void Serialize(std::ofstream& OutFileStream) const;

	/**
	 * 역 직렬화 메서드
	 * 메모리에서 직접 버퍼를 읽음
	 */
	void DeSerialize(const char* Buffer) const;

	/**
	 * 역 직렬화 메서드
	 * 파일스트림에서 버퍼를 읽음 (더 느림!)
	 */
	[[deprecated]] void DeSerialize(std::ifstream& InFileStream) const;

	void ResetModifiedState();

private:
	SerializableMember Serializables{};
};

/**
 * \class JProperty
 * ArrayContainer의 vector에 Serialize목록을 넘겨줘야 해서 공통된 인터페이스 작성
 */
template <typename T>
class JProperty : public ISerializable
{
public:
	JProperty() = delete;

public:
	JProperty(AutoSerializer* serializer)
	{
		serializer->AddMember(this);
	}

	T Get()
	{
		return Data;
	}

	void Set(T val)
	{
		Data        = val;
		bIsModified = true;
	}

	T* GetPtr()
	{
		return &Data;
	}

	void* GetDataPtrToVoidPtr() override
	{
		return &Data;
	}

	size_t GetDataSize() override
	{
		return sizeof(Data);
	}

	bool IsModified() override
	{
		return bIsModified;
	}

	void ResetState() override
	{
		this->bIsModified = false;
	}

private:
	T    Data{};
	bool bIsModified{};
};
