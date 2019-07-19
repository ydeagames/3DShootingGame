// Copyright (c) 2019 YdeaGames
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php
#pragma once

#include <new>
#include <type_traits>
#include <functional>

// Optionalクラス (実用版)
template <typename T>
class Optional {
public:
	// 空のOptional
	Optional(std::nullptr_t it) : data(), data_pointer(nullptr) {}
	// 中身入りOptional
	Optional(const T& it) : data(), data_pointer(new(&data) T(it)) {}
	// コピーコンストラクタ
	Optional(const Optional<T>& it) : data(), data_pointer(it ? new(&data) T(*it) : nullptr) {}
	// ムーブコンストラクタ
	Optional(Optional<T>&& it) noexcept : data(), data_pointer(it ? new(&data) T(std::move(*it)) : nullptr) {}
	// 引数付き
	template <typename ...Args>
	Optional(Args ...args) : data(), data_pointer(new(&data) T(args...)) {}
	// デストラクタ
	~Optional() { if (data_pointer) data_pointer->~T(); }
	// 代入演算子 (空)
	Optional<T> operator=(std::nullptr_t it) {
		if (data_pointer) data_pointer->~T();
		data_pointer = nullptr;
		return *this;
	}
	// 代入演算子コピー
	Optional<T> operator=(const Optional& it) {
		if (data_pointer) data_pointer->~T();
		data_pointer = it ? new(&data) T(*it) : nullptr;
		return *this;
	}
	// 代入演算子ムーブ
	Optional<T> operator=(Optional&& it) {
		if (data_pointer) data_pointer->~T();
		data_pointer = it ? new(&data) T(std::move(*it)) : nullptr;
		return *this;
	}
	// 値取得
	T& operator*() const { return *data_pointer; }
	// 値取得
	T* operator->() const { return data_pointer; }
	// 値があるか確認
	operator bool() const { return data_pointer != nullptr; }
	// ＝
	bool operator==(const Optional& it) const { return data_pointer == it.data_pointer; }
	// ≠
	bool operator!=(const Optional& it) const { return data_pointer != it.data_pointer; }
	// 空でなければ実行
	template <typename Func>
	void ifPresent(Func func) const { if (data_pointer) func(*data_pointer); }
	// 空でなければ実行し結果を返す
	template <typename Func>
	Optional<T> map(Func func) const
	{
		if (data_pointer) return func(*data_pointer);
		else return nullptr;
	}
	// trueであればそのまま、falseであれば空を返す
	template <typename Func>
	Optional<T> filter(Func func) const
	{
		if (data_pointer)
			if (func(*data_pointer))
				return *this;
		return nullptr;
	}
	// 値取得 or デフォルト値
	const T& orElse(const T& els) const { return data_pointer ? *data_pointer : els; }
	// 値取得 or デフォルト値生成関数
	template <typename Func>
	T orElseGet(Func func) const
	{
		if (data_pointer) return *data_pointer;
		else return func();
	}

private:
	// データ (ここだけ少し難しいが、要するにT型が入る大きさの箱を作ってる)
	typename std::aligned_storage<sizeof(T), __alignof(T)>::type data;
	// データへのポインタ
	mutable T* data_pointer;
};