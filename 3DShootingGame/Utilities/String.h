#pragma once

class String
{
private:
	std::wstring str;

public:
	String(const std::wstring& str)
		: str(str) {}

public:
	inline operator LPCWSTR() { return str.c_str(); };
	inline operator LPWSTR() { return &str[0]; };
	inline operator std::wstring() { return str; };

public:
	template <typename ... Args>
	static String Format(const std::wstring format, Args const & ... args) {
		int final_n, n = ((int)format.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
		std::unique_ptr<wchar_t[]> formatted;
		while (1) {
			formatted.reset(new wchar_t[n]); /* Wrap the plain char array into the unique_ptr */
			//wcscpy_s(&formatted[0], n, format.c_str());
			final_n = swprintf(&formatted[0], n, format.c_str(), args...);
			if (final_n < 0 || final_n >= n)
				n += abs(final_n - n + 1);
			else
				break;
		}
		return std::wstring(formatted.get());
	}
};

