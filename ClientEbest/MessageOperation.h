#pragma once
#include <json/value.h>

class CMessageOperation : public IJsonSerializable
{
public:
	void Serialize(Json::Value& root)
	{
		// serialize primitives
		root["type"] = type;
	}
	void Deserialize(Json::Value& root)
	{
		// deserialize primitives
		type = root.get("type", NULL).asString();
	}
private:
	JSONCPP_STRING   type;
};

