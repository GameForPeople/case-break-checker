/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#define WONSY_MACRO

// Macros
#define WonInterface class

// OS
#ifdef _WIN32
	#define WONSY_WINDOW
#endif

#ifdef __linux__
	#define WONSY_LINUX
#endif

#ifndef WONSY_WINDOW
#ifndef WONSY_LINUX
static_assert( false , "not supported os");
#endif
#endif
// C++
// 프로젝트 - 속성 - 구성 - c/c++ - 명령줄에 추가옵션에 다음을 추가해주세요. /Zc:__cplusplus
#if __cplusplus > 201703L
	#define WONSY_CPP    4
	#define WONSY_CPP_20 4
#elif __cplusplus > 201402L
	#define WONSY_CPP    3
	#define WONSY_CPP_17 3
#elif __cplusplus > 201103L
	#define WONSY_CPP    2
	#define WONSY_CPP_14 2
#elif __cplusplus > 201103L
	#define WONSY_CPP    1
	#define WONSY_CPP_11 1
#else
	static_assert( false, "--min cpp11 or 프로젝트 - 속성 - 구성 - c/c++ - 명령줄에 추가옵션에 다음을 추가해주세요. /Zc:__cplusplus" );
#endif
