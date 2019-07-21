#pragma once

class Object
{
public:
	bool destroyed = false;

	virtual ~Object() = default;

	virtual std::wstring GetType() const
	{
		return L"Object";
	}

	virtual std::wstring GetName() const
	{
		return L"Object";
	}

	static void Destroy(Object& object)
	{
		object.destroyed = true;
	}
};