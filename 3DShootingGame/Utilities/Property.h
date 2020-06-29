// Copyright (c) 2019-2020 ydeagames
// Released under the MIT license
// https://github.com/ydeagames/3DShootingGame/blob/master/LICENSE
//
// Author: ${ydeagames}
// Created: 2019-07-19 15:29:42 +0900
// Modified: 2019-07-19 15:29:42 +0900

#pragma once

template<typename T>
struct Property
{
	std::function<T()> Getter;
	std::function<void(const T& value)> Setter;
	Property(
		const std::function<T()>& Getter,
		const std::function<void(const T&)>& Setter)
		: Getter(Getter)
		, Setter(Setter)
	{}
	void operator=(const T& value) { Setter(value); }
	operator T() const { return Getter(); }
	void operator+=(const T& value) { Setter(Getter() + value); }
	void operator-=(const T& value) { Setter(Getter() - value); }
	void operator*=(const T& value) { Setter(Getter() * value); }
	void operator/=(const T& value) { Setter(Getter() / value); }
	void operator%=(const T& value) { Setter(Getter() % value); }
	T operator*() const { return Getter(); }
	T operator->() const { return Getter(); }
};


