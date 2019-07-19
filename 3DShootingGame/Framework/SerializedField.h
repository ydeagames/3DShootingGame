#pragma once
#include <Utilities/picojson.h>

class ISerializedObject
{
	virtual void operator << (const picojson::value&) = 0;
	virtual void operator >> (picojson::value&) const = 0;
};

class ISerializable
{
	virtual void operator << (const ISerializedObject&) = 0;
	virtual void operator >> (ISerializedObject&) const = 0;
};

template<typename T>
class SerializableField
{
	T& operator=(const T& value);
};

template<typename T>
class SerializableObject
{
private:
	std::unordered_map<std::wstring, ISerializable> m_fields;
};

class SerializableRegistry
{
public:
	static std::unordered_map<std::wstring, ISerializable> s_registry;

};
