#pragma once

template <typename Td, typename Ts>
Td string_cast(const Ts& source)
{
	return string_cast_imp<Td, Ts>::cast(source);
}

template <typename Td>
struct string_cast_imp<Td, Td>
{
	static const Td& cast(const Td& source)
	{
		return source;
	}
};

template <typename Td, typename Ts>
struct string_cast_imp
{
	static Td cast(const Ts& source)
	{
		int length = string_traits<Ts>::byte_convert(CP_ACP, source.data(), source.length(),
			NULL, 0);
		if (length == 0)
		{
			return Td();
		}
		vector< typename string_traits<Td>::char_trait > buffer(length);
		string_traits<Ts>::byte_convert(CP_ACP, source.data(), source.length(),
			&buffer[0], length);
		return Td(buffer.begin(), buffer.end());
	}
};

template <typename T>
struct string_traits;

template <>
struct string_traits<string>
{
	typedef char char_trait;
	static int byte_convert(const int codepage, LPCSTR data, int data_length,
		LPWSTR buffer, int buffer_size)
	{
		return ::MultiByteToWideChar(codepage, 0, data, data_length, buffer, buffer_size);
	}
};

template <typename Td, typename Ts>
Td string_cast(Ts* source)
{
	return string_cast_imp<Td, typename string_type_of<const Ts*>::wrap >::cast(source);
}

template <typename T>
struct string_type_of;

template <>
struct string_type_of<const char*>
{
	typedef string wrap;
};