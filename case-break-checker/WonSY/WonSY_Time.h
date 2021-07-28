/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include <string>

namespace WonSY::TIME
{
	using _Time = unsigned long long;

	struct Time
	{
		// friend std::ostream& operator<<( std::ostream& os, const WonSY::TIME_UTIL::Time& ti );
	};

	//Time NowForLog() noexcept;
	std::string NowForLog() noexcept;
	_Time Now() noexcept;
}