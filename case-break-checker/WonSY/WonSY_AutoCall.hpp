/*
	Copyright 2019, Won Seong-Yeon, All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#define WONSY_AUTOCALL

#include <functional>

namespace WonSY 
{
	inline namespace BASE 
	{
		class AutoCall
		{
			const std::function< void() > func;
		
		public:
			AutoCall( const std::function< void() >& func ) noexcept
				: func( func )
			{
			}
		
			~AutoCall()
			{
				if ( func ) { func(); }
			}
		};

		class AutoCallList
		{
			std::list< std::function< void() > > funcList;

		public:
			AutoCallList() = default;
			~AutoCallList()
			{
				for ( auto iter = funcList.begin(); iter != funcList.end(); ++iter )
				{
					( *iter )();
				}
			}

			void Add( const std::function< void() >& func )
			{
				funcList.push_back( func );
			}

			void Clear()
			{
				funcList.clear();
			}
		};
	} 
}

#define AUTO_CALL( x )  WonSY::BASE::AutoCall AUTO_CALL_PREVENT_MISTAKE( x )
#define DEFER( x )       AUTO_CALL( x )