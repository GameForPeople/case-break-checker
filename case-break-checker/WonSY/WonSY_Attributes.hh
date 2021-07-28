/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#define WONSY_ATTRIBUTES

#include "WonSY_Macro.hh"

#if WONSY_CPP >= WONSY_CPP_20

	#define NORETURN           [[noreturn]]
	#define CARRIES_DEPENDENCY [[carries_dependency]]

	#define DEPRECATED         [[deprecated]]

	#define FALLTHROUGH        [[fallthrough]]
	#define NODISCARD          [[nodiscard]]
	#define MAYBE_UNUSED       [[maybe_unused]]
	#define NO_UNIQUE_ADDRESS  [[no_unique_address]]

	#define LIKELY             [[likely]]
	#define UNLIKELY           [[unlikely]]

#elif WONSY_CPP >= WONSY_CPP_17

	#define NORETURN           [[noreturn]]
	#define CARRIES_DEPENDENCY [[carries_dependency]]

	#define DEPRECATED         [[deprecated]]

	#define FALLTHROUGH        [[fallthrough]]
	#define NODISCARD          [[nodiscard]]
	#define MAYBE_UNUSED       [[maybe_unused]]

	#define NO_UNIQUE_ADDRESS  
	#define LIKELY             
	#define UNLIKELY           

#elif WONSY_CPP >= WONSY_CPP_14

	#define NORETURN           [[noreturn]]
	#define CARRIES_DEPENDENCY [[carries_dependency]]

	#define DEPRECATED         [[deprecated]]

	#define FALLTHROUGH        
	#define NODISCARD          
	#define MAYBE_UNUSED       

	#define NO_UNIQUE_ADDRESS  
	#define LIKELY             
	#define UNLIKELY           

#elif WONSY_CPP >= WONSY_CPP_11

	#define NORETURN           [[noreturn]]
	#define CARRIES_DEPENDENCY [[carries_dependency]]

	#define DEPRECATED         

	#define FALLTHROUGH        
	#define NODISCARD          
	#define MAYBE_UNUSED       

	#define NO_UNIQUE_ADDRESS  
	#define LIKELY             
	#define UNLIKELY           

#endif