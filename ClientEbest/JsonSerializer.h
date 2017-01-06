#pragma once
#include "IJsonSerializable.h"

class CJsonSerializer
{
public:
	static bool Serialize(IJsonSerializable* pObj, std::string& output);
	static bool Deserialize(IJsonSerializable* pObj, std::string& input);
	CJsonSerializer();
	~CJsonSerializer();
};

