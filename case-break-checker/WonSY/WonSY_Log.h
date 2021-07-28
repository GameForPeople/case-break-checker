/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#include "WonSY.hh"

#define WONSY_LOG

#include <ostream>
#include <string>
#include <sstream>

namespace WonSY::LOG
{
#define LOG_LEVEL_ALL        0
#define LOG_LEVEL_NOT_INFO   1
#define LOG_LEVEL_NOT_NOTICE 2
#define LOG_LEVEL_NOT_WARN   3
#define LOG_LEVEL_NOT_ERROR  4
#define LOG_LEVEL_NOT_ALL    5

#ifdef _DEBUG
	static constexpr int LOG_LEVEL = LOG_LEVEL_ALL;
#else
	static constexpr int LOG_LEVEL = LOG_LEVEL_NOT_INFO;
#endif

#if LOG_LEVEL < LOG_LEVEL_NOT_ALL

#define SAFE_LOG_PRINT    true
#define __FUNCTION_NAME__ __FUNCTION__
#define __FILENAME__      (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define SOURCE_LOCATION   {__LINE__, __FILENAME__, __FUNCTION_NAME__}

// #define ERROR_	SOURCE_LOCATION,	LOG_TYPE::LT_ERROR
// #define WARN_	SOURCE_LOCATION,	LOG_TYPE::LT_WARN
// #define INFO_	SOURCE_LOCATION,	LOG_TYPE::LT_INFO
// #define OTHER_	SOURCE_LOCATION,	LOG_TYPE::LT_OTHER_LOG

	struct SourceLocation
	{
		int         fileLine;
		const char* fileName;
		const char* functionName;
		
		// std::wstring outputString;
		// std::string	outputString;

		SourceLocation( const int fileLine, const char* fileName,  const char* functionName );
		
		std::string GetString() const;
		
		friend std::ostream& operator<<( std::ostream& os, const SourceLocation& sl );
	};

	enum class LOG_TYPE : unsigned char
	{
		LT_ERROR,
		LT_WARN,
		LT_INFO,
		// LT_NOTICE,
		LT_OTHER_LOG,
	};

	void SetLogColor( const unsigned short COLOR );

	DEPRECATED void Log( const SourceLocation& sourceLocation, const LOG_TYPE logType, const std::string& message );

	void ErrorLog ( const SourceLocation& sourceLocation, const std::string& message, bool isTerminate = true );
	void WarnLog  ( const SourceLocation& sourceLocation, const std::string& message );
	void NoticeLog( const SourceLocation& sourceLocation, const std::string& message );
	void InfoLog  ( const SourceLocation& sourceLocation, const std::string& message );
	void OtherLog ( const SourceLocation& sourceLocation, const std::string& message );

#define ERROR_LOG( logMessage )               ErrorLog ( SOURCE_LOCATION, logMessage, true );
#define ERROR_LOG_NOT_TERMINATE( logMessage ) ErrorLog ( SOURCE_LOCATION, logMessage, false );
#define WARN_LOG( logMessage )                WarnLog  ( SOURCE_LOCATION, logMessage );
#define NOTICE_LOG( logMessage )              NoticeLog( SOURCE_LOCATION, logMessage );
#define INFO_LOG( logMessage )                InfoLog  ( SOURCE_LOCATION, logMessage );
#define OTHER_LOG( logMessage )               OtherLog ( SOURCE_LOCATION, logMessage );

#else

#define SOURCE_LOCATION 0
#define Log(x, y) {}

#endif

} using namespace WonSY::LOG;
