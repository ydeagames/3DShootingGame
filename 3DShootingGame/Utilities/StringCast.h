// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-12-09 17:14:07 +0900
// Modified: 2019-12-09 19:28:46 +0900

#pragma once

template <typename T>
struct string_traits;

template <>
struct string_traits<std::string>
{
	typedef char char_trait;
	static int byte_convert(const int codepage, LPCSTR data, int data_length,
		LPWSTR buffer, int buffer_size)
	{
		return ::MultiByteToWideChar(codepage, 0, data, data_length, buffer, buffer_size);
	}
};

template <>
struct string_traits<std::wstring>
{
	typedef wchar_t char_trait;
	static int byte_convert(const int codepage, LPCWSTR data, int data_length,
		LPSTR buffer, int buffer_size)
	{
		return ::WideCharToMultiByte(codepage, 0, data, data_length, buffer, buffer_size, NULL, NULL);
	}
};

template <typename Td, typename Ts>
struct string_cast_imp
{
	static Td cast(const Ts& source)
	{
		int length = string_traits<Ts>::byte_convert(CP_ACP, source.data(), source.length(), NULL, 0);
		if (length == 0)
		{
			return Td();
		}
		std::vector< typename string_traits<Td>::char_trait > buffer(length);
		string_traits<Ts>::byte_convert(CP_ACP, source.data(), source.length(), &buffer[0], length);
		return Td(buffer.begin(), buffer.end());
	}
};

template <typename Td>
struct string_cast_imp<Td, Td>
{
	static const Td& cast(const Td& source)
	{
		return source;
	}
};

template <typename Td, typename Ts>
Td string_cast(const Ts& source)
{
	return string_cast_imp<Td, Ts>::cast(source);
}

template <typename T>
struct string_type_of;

template <>
struct string_type_of<const char*>
{
	typedef std::string wrap;
};

template <typename Td, typename Ts>
Td string_cast(Ts* source)
{
	return string_cast_imp<Td, typename string_type_of<const Ts*>::wrap>::cast(source);
}


