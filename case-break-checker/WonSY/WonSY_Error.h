/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include "WonSY.hh"

#define WONSY_ERROR

#include <string>
#include <functional>

namespace WonSY::Error
{
	NORETURN void Terminate( const std::string& terminateMessage    );
	NORETURN void Terminate( std::function< void() > func = nullptr );

	// /*_NORETURN*/ void ErrorRecv();
	// /*_NORETURN*/ void ErrorSend();
	// /*_NORETURN*/ void ErrorAccept();
};