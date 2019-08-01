#pragma once

class Object
{
public:
	static float objectTime;

protected:
	float time = -1;

public:
	virtual ~Object() = default;

	virtual std::wstring GetType() const
	{
		return L"Object";
	}

	virtual std::wstring GetName() const
	{
		return L"Object";
	}

	virtual bool IsDestroyed() const
	{
		return (time >= 0 && objectTime >= time);
	}

	static void Destroy(Object& object, float delay = 0)
	{
		if (object.time < 0)
			object.time = objectTime + delay;
	}
};