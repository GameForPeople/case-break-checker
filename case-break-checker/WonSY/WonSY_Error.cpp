/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "WonSY.h"

#include <thread>

void WonSY::Error::Terminate( const std::string& terminateMessage )
{
	ERROR_LOG( terminateMessage );
}

void WonSY::Error::Terminate( std::function< void() > func )
{
	using namespace std::chrono_literals;

	if ( func ) func();
	std::cout << " Call Terminate ################################################# ";

	for( int i = 5; i > 0; --i )
	{
		std::cout << " " << i;
		std::this_thread::sleep_for( 1s );
	}

	std::function< void() > HelloMyNameIsCrash = nullptr;
	HelloMyNameIsCrash();
}

/*
void WonSY::ERROR_UTIL::ErrorRecv()
{
	if ( 
		int errorCode = WSAGetLastError();	
		errorCode != ERROR_IO_PENDING	&&
		errorCode != WSAENOTSOCK		&& 
		errorCode != WSAECONNRESET )
	{
		LPVOID lpMsgBuf;

		FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errorCode,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );

		WARN_LOG( "[ RECV ( " + std::to_string( errorCode ) + ") ] "
		+ ((LPCTSTR)lpMsgBuf) );
	}
}

void WonSY::ERROR_UTIL::ErrorSend()
{
	if ( int errorCode = WSAGetLastError();
		errorCode != ERROR_IO_PENDING	&&
		errorCode != WSAENOTSOCK		&&
		//errorCode != WSAECONNABORTED	&&
		errorCode != WSAECONNRESET )
	{
		LPVOID lpMsgBuf;

		FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errorCode,
		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		(LPTSTR)&lpMsgBuf, 0, NULL );

		WARN_LOG( "[ SEND ( " + std::to_string(errorCode) + ") ] "
		+ ((LPCTSTR)lpMsgBuf) );
	}
}

void WonSY::ERROR_UTIL::ErrorAccept()
{
	if (int errorCode = WSAGetLastError()
		; errorCode != ERROR_IO_PENDING )
	{
		LPVOID lpMsgBuf;

		FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL );

		WARN_LOG( "[ ACCEPT ( " + std::to_string(errorCode) + ") ] "
		+ ((LPCTSTR)lpMsgBuf)  );
	}
}
*/