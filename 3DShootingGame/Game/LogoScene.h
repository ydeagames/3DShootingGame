#pragma once

class LogoScene
{
public:
	std::wstring GetName() const { return L"LogoScene"; }
	void Build(GameContext& context);
};

