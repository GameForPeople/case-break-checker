/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "WonSY.h"

#include <iostream>

#if SAFE_LOG_PRINT == true
#include <mutex>
	static std::mutex logLock;
#endif

#ifdef WONSY_WINDOW
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

#if LOG_LEVEL < LOG_LEVEL_NOT_ALL

WonSY::LOG::SourceLocation::SourceLocation( const int fileLine, const char* fileName,  const char* functionName )
	: fileLine    ( fileLine     )
	, fileName    ( fileName     )
	, functionName( functionName )
	//, outputString()
{
}

std::string WonSY::LOG::SourceLocation::GetString() const
{
	std::ostringstream os;

	os
	//<< std::string( functionName )
	//<< " -> "
	<< std::string( fileName )
	<< " ("
	<< std::to_string( fileLine )
	<< ") ]";

	return os.str();
}

std::ostream& operator<<( std::ostream& os, const SourceLocation& sl ) 
{
	os 
	//<< std::string( sl.functionName )
	//<< " -> "
	<< std::string( sl.fileName )
	<< " ("
	<< std::to_string( sl.fileLine )
	<< ") ]";

	return os;
}

void WonSY::LOG::SetLogColor( const unsigned short COLOR )
{
#ifdef WONSY_WINDOW
	SetConsoleTextAttribute ( GetStdHandle( (DWORD)( -11 ) ), COLOR );
#endif
}

void WonSY::LOG::Log( const SourceLocation& sourceLocation, const LOG_TYPE logType, const std::string& log = "" )
{
	//TIME_UTIL::Time time;
#if SAFE_LOG_PRINT == true
	AutoCall autoCall {
		[ & ]() noexcept -> void 
		{
			SetLogColor( 0x0F | ( 0 << 4 )  );
			logLock.unlock();
		} };

	logLock.lock();
#else
	AutoCall autoCall { [&]()
	{
		SetConsoleTextAttribute ( GetStdHandle(STD_OUTPUT_HANDLE), 0x0F | (0 << 4) );
	}};
#endif
	switch( logType )
	{
	case LOG_TYPE::LT_ERROR:
		SetLogColor( 0x0F | ( 0x0C << 4 ) );
		std::cout << "[ ERROR | " << WonSY::TIME::NowForLog() << " | " << log << " | " << sourceLocation.GetString() << std::endl;
		return;
	case LOG_TYPE::LT_WARN:
		if constexpr ( LOG_LEVEL < LOG_LEVEL_NOT_WARN )
		{
			SetLogColor( 0x0F | ( 0x06 << 4 ) );
			std::cout << "[ WARN  | " << TIME::NowForLog() << " | " << log << " | " << sourceLocation.GetString() << std::endl;
		}
		return;
	case LOG_TYPE::LT_INFO:
		if constexpr ( LOG_LEVEL < LOG_LEVEL_NOT_INFO )
		{
			SetLogColor( 0x08 | ( 0x00 << 4 ) );
			std::cout << "[ INFO  | " << TIME::NowForLog()  << " | " << log << " | " << sourceLocation.GetString() << std::endl;
		}
		return;
	case LOG_TYPE::LT_OTHER_LOG:
		if constexpr ( LOG_LEVEL < LOG_LEVEL_NOT_INFO )
		{
			// SetLogColor( 0x0F | (0 << 4) );
			std::cout << "[ OTHER | " << TIME::NowForLog() << " | " << log << " | " << sourceLocation.GetString() << std::endl;
		}
		return;
	default:
		return;
	}
}

void WonSY::LOG::ErrorLog( const SourceLocation& sourceLocation, const std::string& message, bool isTerminate )
{
#if SAFE_LOG_PRINT == true
	logLock.lock();
#else
#endif
	SetLogColor( 0x0F | ( 0x0C << 4 ) );
	std::cout << "[ ERROR  | " << WonSY::TIME::NowForLog() << " | " << sourceLocation.GetString() << " " << message << std::endl;
	
	if( !isTerminate )
	{
		SetLogColor( 0x0F | ( 0 << 4 ) );
		logLock.unlock();
		return;
	}
	else
		WonSY::Error::Terminate();
}

void WonSY::LOG::WarnLog( const SourceLocation& sourceLocation, const std::string& message )
{
	if constexpr ( LOG_LEVEL < LOG_LEVEL_NOT_WARN )
	{
		#if SAFE_LOG_PRINT == true
			AutoCall autoCall { 
				[ & ]() noexcept -> void 
				{
					SetLogColor( 0x0F | ( 0 << 4 ) );
					logLock.unlock();
				} };
		
			logLock.lock();
		#else
		#endif
			SetLogColor( 0x0F | ( 0x06 << 4 ) );
			std::cout << "[ WARN   | " << TIME::NowForLog() <<" | " << sourceLocation.GetString() << " " << message << std::endl;
	}
}

void WonSY::LOG::NoticeLog( const SourceLocation& sourceLocation, const std::string& message )
{
	if constexpr ( LOG_LEVEL < LOG_LEVEL_NOT_NOTICE )
	{
		#if SAFE_LOG_PRINT == true
			AutoCall autoCall { 
				[&]() noexcept -> void 
				{
					logLock.unlock();
				} };
		
			logLock.lock();
		#else
		#endif
			std::cout << "[ NOTICE | " << TIME::NowForLog()  << " | " << sourceLocation.GetString() << " " << message << std::endl;
	}
}

void WonSY::LOG::InfoLog( const SourceLocation& sourceLocation, const std::string& message )
{
	if constexpr ( LOG_LEVEL < LOG_LEVEL_NOT_INFO )
	{
		#if SAFE_LOG_PRINT == true
			AutoCall autoCall {
				[ & ]() noexcept -> void 
				{
					SetLogColor( 0x0F | ( 0 << 4 ) );
					logLock.unlock();
				} };
		
			logLock.lock();
		#else
		#endif
			SetLogColor( 0x08 | ( 0x00 << 4 ) );
			std::cout << "[ INFO   | " << TIME::NowForLog()  << " | " << sourceLocation.GetString() << " " << message << std::endl;
	}
}

void WonSY::LOG::OtherLog( const SourceLocation& sourceLocation, const std::string& message )
{
	if constexpr ( LOG_LEVEL < LOG_LEVEL_NOT_INFO )
	{
		#if SAFE_LOG_PRINT == true
			AutoCall autoCall {
				[&]() noexcept -> void 
				{
					logLock.unlock();
				} };
		
			logLock.lock();
		#else
		#endif
			std::cout << "[ OTHER   | " << TIME::NowForLog()  << " | " << sourceLocation.GetString() << " " << message << std::endl;
	}
}

#endif
