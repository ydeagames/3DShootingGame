#pragma once

class Object
{
	virtual std::wstring GetName()
	{
		return L"Object";
	}

	virtual bool IsDestroyed();
};