/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "WonSY_Time.h"

#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>

std::string WonSY::TIME::NowForLog() noexcept
{
	const auto time      = std::time( nullptr );
	const auto localTime = *std::localtime( &time );

	std::ostringstream os;
	os << std::put_time( &localTime, "%y-%m-%d %H:%M:%S" );

	return os.str();
}

WonSY::TIME::_Time WonSY::TIME::Now() noexcept
{
	return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::high_resolution_clock::now().time_since_epoch() ).count();
}