//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0600
#include <SDKDDKVer.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

// Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// COM
#include <wrl/client.h>

// DirectX
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

// DXGI
#if defined(NTDDI_WIN10_RS2)
#include <dxgi1_6.h>
#else
#include <dxgi1_5.h>
#endif

// STL
#include <exception>
#include <memory>
#include <stdexcept>

#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <algorithm>
#include <random>

#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <deque>

// DirectXTK
#include <Keyboard.h>
#include <Mouse.h>
#include <SimpleMath.h>
#include <CommonStates.h>
#include <WICTextureLoader.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <GeometricPrimitive.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <Effects.h>
#include <Model.h>

// DXGI (Debug)
#ifdef _DEBUG
#include <dxgidebug.h>
#endif

// PhysX
#include <PxPhysicsAPI.h>

namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}