#include <cstdint>

// ゼロオーバーヘッド文字列ID
class HashedString final {
	struct ConstCharWrapper final {
		constexpr ConstCharWrapper(const char* str) noexcept : str{ str } {}
		const char* str;
	};

	static constexpr uint64_t offset = 14695981039346656037ull;
	static constexpr uint64_t prime = 1099511628211ull;

	// コンパイル時ハッシュ関数
	static constexpr uint64_t helper(uint64_t partial, const char* str) noexcept {
		return str[0] == 0 ? partial : helper((partial ^ str[0]) * prime, str + 1);
	}

public:
	using hash_type = uint64_t;

	template<size_t N>
	constexpr HashedString(const char(&str)[N]) noexcept
		: hash{ helper(offset, str) }, str{ str }
	{}

	explicit constexpr HashedString(ConstCharWrapper wrapper) noexcept
		: hash{ helper(offset, wrapper.str) }, str{ wrapper.str }
	{}

	constexpr operator const char* () const noexcept { return str; }

	constexpr operator hash_type() const noexcept { return hash; }

	constexpr bool operator==(const HashedString& other) const noexcept {
		return hash == other.hash;
	}

private:
	const hash_type hash;
	const char* str;
};

constexpr bool operator!=(const HashedString& lhs, const HashedString& rhs) noexcept {
	return !(lhs == rhs);
}

// リテラル定義
constexpr HashedString operator"" _hs(const char* str, size_t) noexcept {
	return HashedString{ str };
}
