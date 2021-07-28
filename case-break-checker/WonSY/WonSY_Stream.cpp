/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "WonSY_Stream.h"

#include <memory>
#include <cstdlib>

#pragma region [ BaseMemoryStream ]

//WonSY::Stream::BaseMemoryStream::BaseMemoryStream()
//	: m_buffer     ( nullptr )
//	, m_loadedSize ( 0       )
//	, m_unloadedPos( 0       )
//	, m_maxSize    ( 0       )
//{
//	_Realloc( DEFAULT_SIZE );
//}

//WonSY::Stream::BaseMemoryStream::~BaseMemoryStream()
//{
//	std::free( m_buffer );
//}

#pragma endregion

#pragma region [ StreamWriter ]

WonSY::Stream::StreamWriter::StreamWriter()
	: m_buffer     ( nullptr )
	, m_loadedSize ( 0       )
	, m_maxSize    ( 0       )
{
	_Realloc( DEFAULT_SIZE );
}

WonSY::Stream::StreamWriter::~StreamWriter()
{
	std::free( m_buffer );
}

void WonSY::Stream::StreamWriter::_Realloc( const SizeType newMaxSize )
{
	if ( newMaxSize <= m_maxSize ) UNLIKELY
		return;

	if ( auto reallocResult = std::realloc( m_buffer, newMaxSize );
		reallocResult != nullptr ) LIKELY
	{
		m_buffer  = static_cast< BufferType >( reallocResult );
		m_maxSize = newMaxSize;
	}
}

void WonSY::Stream::StreamWriter::_CheckAndRealloc( const SizeType requiredSize )
{
	if constexpr ( !IsCheckRealloc() )
		return;
	
	const SizeType reaminSize = _GetReaminSize();

	if ( requiredSize < reaminSize ) LIKELY
		return;

	if ( requiredSize == reaminSize ) UNLIKELY
		return;

	_Realloc( requiredSize > m_maxSize ? requiredSize * _GetReallocMultiples() : m_maxSize * _GetReallocMultiples() );
}

#define PRIMITIVE_COMMON_WRITE                                                                \
_CheckAndRealloc( sizeof( value ) );                                                          \
memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &value ), sizeof( value ) ); \
                                                                                              \
m_loadedSize += sizeof( value );                                                              \

void  WonSY::Stream::StreamWriter::_Write( bool               value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( char               value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( unsigned char      value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( short              value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( unsigned short     value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( int                value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( unsigned int       value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( long               value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( unsigned long      value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( long long          value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( unsigned long long value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( wchar_t            value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( float              value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( double             value ){ PRIMITIVE_COMMON_WRITE };
void  WonSY::Stream::StreamWriter::_Write( long double        value ){ PRIMITIVE_COMMON_WRITE };

#pragma endregion

#pragma region [ StreamReader ]

WonSY::Stream::StreamReader::StreamReader( const BufferType bufferPtr )
	: m_buffer     ( bufferPtr )
	, m_unloadedPos( 0         )
{
}

WonSY::Stream::StreamReader::~StreamReader()
{
	// not owned buffer
}

#define PRIMITIVE_COMMON_READ                              \
memcpy( &dest, m_buffer + m_unloadedPos, sizeof( dest ) ); \
m_unloadedPos += sizeof( dest );                           \

void WonSY::Stream::StreamReader::_Read( bool               &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( char               &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( unsigned char      &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( short              &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( unsigned short     &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( int                &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( unsigned int       &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( long               &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( unsigned long      &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( long long          &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( unsigned long long &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( wchar_t            &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( float              &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( double             &dest ){ PRIMITIVE_COMMON_READ };
void WonSY::Stream::StreamReader::_Read( long double        &dest ){ PRIMITIVE_COMMON_READ };

#pragma endregion
