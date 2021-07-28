/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#define WONSY_STREAM

#pragma region [ head ]

#include "WonSY_Attributes.hh"


#define WRITE_AND_READ_WITH_IF_CONSTEXPR 0 // �ɷ���
#define WRITE_COMMON_CONTAINER  1 // �����̳� �ϳ��� ������
#define READ_COMMON_CONTAINER   0 // �����̳� �ϳ��� ������

#include <cstdint>
#include <string>

#if WRITE_AND_READ_WITH_IF_CONSTEXPR == 1
#include <type_traits>
#endif

// container
#include <utility> // for pair
#include <list>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

// �ֳ� ��Ŷ���� �������� ������ ������..
//#include <tuple>
//#include <queue>
//#include <deque>

#pragma endregion

namespace WonSY::Stream
{
	inline namespace
	{
		using BufferType  = char*;
		using SizeType    = uint8_t;

		static constexpr SizeType DEFAULT_SIZE      = 128; // �⺻ �޸� �Ҵ� �������Դϴ�.
		static constexpr SizeType REALLOC_MULTIPLES = 2;   // ���Ҵ� ��, �Ҵ��� �޸� ������ ����Դϴ�.
	};

	class StreamWriter
	{
	#pragma region [ Func ]
	public:
		StreamWriter();
		~StreamWriter();

		static constexpr bool IsCheckRealloc() { return true; }; // ���Ҵ� ó�������Դϴ�.

		BufferType GetBufferPtr() { return m_buffer; };

	private:
		void _Realloc( const SizeType newMaxSize );              // �޸𸮸� ���Ҵ��մϴ�.
		void _CheckAndRealloc( const SizeType requiredSize );    // üũ �� �޸𸮸� ���Ҵ��մϴ�.
		
		SizeType _GetReallocMultiples() const { return REALLOC_MULTIPLES;        }; // ���Ҵ� ����� ��ȯ�մϴ�.
		SizeType _GetReaminSize()       const { return m_maxSize - m_loadedSize; };	// ���� �޸��� ũ�⸦ ��ȯ�մϴ�.

	public:
		template < typename Type >
		void operator<< ( const Type& value )
		{
			_Write( value );
		}

	private:
#if WRITE_AND_READ_WITH_IF_CONSTEXPR == 1
		template < typename Type >
		void _Write( const Type& source )
		{
			if constexpr (
				std::is_same< std::remove_cv< Type >, bool >::value               ||
				std::is_same< std::remove_cv< Type >, char >::value               ||
				std::is_same< std::remove_cv< Type >, short >::value              ||
				std::is_same< std::remove_cv< Type >, unsigned short >::value     ||
				std::is_same< std::remove_cv< Type >, int >::value                ||
				std::is_same< std::remove_cv< Type >, unsigned int >::value       ||
				std::is_same< std::remove_cv< Type >, long >::value               ||
				std::is_same< std::remove_cv< Type >, unsigned long >::value      ||
				std::is_same< std::remove_cv< Type >, long long >::value          ||
				std::is_same< std::remove_cv< Type >, unsigned long long >::value )
			{
				constexpr SizeType requiredSize = sizeof( source );
				_CheckAndRealloc( requiredSize );

				std::memcpy( m_buffer + m_loadedSize, &source, requiredSize );
				m_loadedSize += requiredSize;
			}
			else if constexpr ( 
				std::is_same< Type, std::string  >::value ||
				std::is_same< Type, std::wstring >::value )
			{
				constexpr SizeType valueTypeSize = sizeof( Type::value_type );
				const     SizeType stringSize    = source.size();
				const     SizeType requiredSize  = sizeof( SizeType ) + valueTypeSize * stringSize;

				_CheckAndRealloc( requiredSize );

				std::memcpy( m_buffer + m_loadedSize,                      &stringSize,    sizeof( SizeType )                );
				std::memcpy( m_buffer + m_loadedSize + sizeof( SizeType ), source.c_str(), requiredSize - sizeof( SizeType ) );
				m_loadedSize += requiredSize;
			}
			else // if constexpr ( �����̳�? )
			{
				const SizeType count = source.size();
				_CheckAndRealloc( sizeof( SizeType ) );

				std::memcpy( m_buffer + m_loadedSize, &count, sizeof( SizeType ) );
				m_loadedSize += sizeof( SizeType );

				for ( const auto& ele : source )
				{
					(*this) << ele;
				}
			}
		}
#else
		void _Write( bool               value );
		void _Write( char               value );
		void _Write( unsigned char      value );
		void _Write( short              value );
		void _Write( unsigned short     value );
		void _Write( int                value );
		void _Write( unsigned int       value );
		void _Write( long               value );
		void _Write( unsigned long      value );
		void _Write( long long          value );
		void _Write( unsigned long long value );
		void _Write( wchar_t            value );
		void _Write( float              value );
		void _Write( double             value );
		void _Write( long double        value );

		template < typename Type1, typename Type2 >
		void _Write( const std::pair< Type1, Type2 >& source )
		{
			(*this) << source.first;
			(*this) << source.second;
		};

		template < typename Type, SizeType eleCount >
		void _Write( const std::array< Type, eleCount >& cont )
		{
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );
			
			for ( const auto& ele : cont ) { (*this) << ele; }
		};

		#if WRITE_COMMON_CONTAINER == 1

		template < 
			typename Type,
			typename = std::void_t< 
				decltype( std::declval< Type >().size()  ),
				decltype( std::declval< Type >().begin() ),
				decltype( std::declval< Type >().end()   ) > >
		void _Write( const Type& cont )
		{
			SizeType eleCount = cont.size();
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );
		
			for ( const auto& ele : cont ) { (*this) << ele; }
		};
		#else
		template < typename Type >
		void _Write( const std::list< Type >& cont )
		{
			const SizeType eleCount = cont.size();
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );
		
			for ( const auto& ele : cont ) { (*this) << ele; }
		};

		template < typename Type >
		void _Write( const std::vector< Type >& cont )
		{
			const SizeType eleCount = cont.size();
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );

			for ( const auto& ele : cont ) { (*this) << ele; }
		};

		template < typename KeyType, typename ValueType >
		void _Write( const std::map< KeyType, ValueType >& cont )
		{
			const SizeType eleCount = cont.size();
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );

			for ( const auto& ele : cont ) { (*this) << ele; }
		};

		template < typename Type >
		void _Write( const std::set< Type >& cont )
		{
			const SizeType eleCount = cont.size();
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );

			for ( const auto& ele : cont ) { (*this) << ele; }
		};

		template < typename KeyType, typename ValueType >
		void _Write( const std::unordered_map< KeyType, ValueType >& cont )
		{
			const SizeType eleCount = cont.size();
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );

			for ( const auto& ele : cont ) { (*this) << ele; }
		};

		template < typename Type >
		void _Write( const std::unordered_set< Type >& cont )
		{
			const SizeType eleCount = cont.size();
			_CheckAndRealloc( sizeof( SizeType ) );

			memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
			m_loadedSize += sizeof( SizeType );

			for ( const auto& ele : cont ) { (*this) << ele; }
		};
		#endif
#endif
	#pragma endregion
	#pragma region [ Var ]
	protected:
		BufferType m_buffer      = nullptr; // �����Դϴ�. 
		SizeType   m_loadedSize  = 0;       // ������ �������Դϴ�.
		SizeType   m_maxSize     = 0;       // �ִ� �������Դϴ�.
	#pragma endregion
	};

	class StreamReader
	{
	#pragma region [ Func ]
	public:
		StreamReader( const BufferType bufferPtr );
		~StreamReader();

		template < typename Type >
		void operator>> ( Type& dest )
		{
			_Read( dest );
		};

	private:
#if WRITE_AND_READ_WITH_IF_CONSTEXPR == 1
		template < typename Type >
		void _Read( Type& dest )
		{
			if constexpr (
				std::is_same< std::remove_cv< Type >, bool >::value               ||
				std::is_same< std::remove_cv< Type >, char >::value               ||
				std::is_same< std::remove_cv< Type >, short >::value              ||
				std::is_same< std::remove_cv< Type >, unsigned short >::value     ||
				std::is_same< std::remove_cv< Type >, int >::value                ||
				std::is_same< std::remove_cv< Type >, unsigned int >::value       ||
				std::is_same< std::remove_cv< Type >, long >::value               ||
				std::is_same< std::remove_cv< Type >, unsigned long >::value      ||
				std::is_same< std::remove_cv< Type >, long long >::value          ||
				std::is_same< std::remove_cv< Type >, unsigned long long >::value )
			{
				constexpr SizeType requiredSize = sizeof( dest );
				std::memcpy( &dest, m_buffer + m_loadedSize, requiredSize );

				m_loadedSize += requiredSize;
			}
			else if constexpr ( 
				std::is_same< Type, std::string  >::value ||
				std::is_same< Type, std::wstring >::value )
			{
			}
			else // if constexpr ( �����̳�? )
			{
				using ValueType = Type::value_type;
				SizeType eleCount = 0;
				std::memcpy( &eleCount, m_buffer + m_loadedSize, sizeof( SizeType ) );
				m_loadedSize += sizeof( SizeType );

				for ( int i = 0; i < eleCount; ++i )
				{
					ValueType ele;
					(*this) >> ele;
					dest.push_back( ele );
				}
			}
		}
#else
		void _Read( bool               &dest );
		void _Read( char               &dest );
		void _Read( unsigned char      &dest );
		void _Read( short              &dest );
		void _Read( unsigned short     &dest );
		void _Read( int                &dest );
		void _Read( unsigned int       &dest );
		void _Read( long               &dest );
		void _Read( unsigned long      &dest );
		void _Read( long long          &dest );
		void _Read( unsigned long long &dest );
		void _Read( wchar_t            &dest );
		void _Read( float              &dest );
		void _Read( double             &dest );
		void _Read( long double        &dest );

		template < typename Type1, typename Type2 >
		void _Read( std::pair< Type1, Type2 >& dest )
		{
			(*this) >> dest.first;
			(*this) >> dest.second;
		};

		// ???? ���� �迭 ���� ������ ������
		// template < typename Type, SizeType eleCount >
		// void _Read( std::array< Type, eleCount >& dest )
		// {
		// 	memcpy( m_buffer + m_loadedSize, reinterpret_cast< BufferType >( &eleCount ), sizeof( SizeType ) );
		// 	m_loadedSize += sizeof( SizeType );
		// 	
		// 	for ( const auto& ele : cont ) { (*this) << ele; }
		// };

		#if READ_COMMON_CONTAINER == 1
		
		template < 
			typename Type,
			typename = std::void_t< 
				decltype( Type::value_type                 ),
				decltype( std::declval< Type >().begin()   ),
				decltype( std::declval< Type >().end()     ) > >
				// decltype( std::declval< Type >().emplace() ),
				// decltype( std::declval< Type >().reserve() ) > >
		void _Read( Type& dest )
		{
			SizeType eleCount{};
			memcpy( &eleCount, m_buffer + m_unloadedPos, sizeof( SizeType ) );

			m_unloadedPos += sizeof( SizeType );

			for ( int i = 0; i < eleCount; ++i )
			{
				typename Type::value_type temp;
				(*this) >> temp;

				dest.emplace( temp );
			}
		};
		#else

		template < typename Type >
		void _Read( std::vector< Type >& dest )
		{
			SizeType eleCount{};
			memcpy( &eleCount, m_buffer + m_unloadedPos, sizeof( SizeType ) );

			m_unloadedPos += sizeof( SizeType );

			dest.reserve( eleCount );

			for ( int i = 0; i < eleCount; ++i )
			{
				Type temp;
				(*this) >> temp;

				dest.emplace_back( temp );
			}
		};

		template < typename Type >
		void _Read( std::list< Type >& dest )
		{
			SizeType eleCount{};
			memcpy( &eleCount, m_buffer + m_unloadedPos, sizeof( SizeType ) );

			m_unloadedPos += sizeof( SizeType );

			for ( int i = 0; i < eleCount; ++i )
			{
				Type temp;
				(*this) >> temp;

				dest.emplace( temp );
			}
		};

		template < typename Type1, typename Type2 >
		void _Read( std::map< Type1, Type2 >& dest )
		{
			SizeType eleCount{};
			memcpy( &eleCount, m_buffer + m_unloadedPos, sizeof( SizeType ) );

			m_unloadedPos += sizeof( SizeType );

			for ( int i = 0; i < eleCount; ++i )
			{
				Type1 key;
				(*this) >> key;

				Type2 value;
				(*this) >> value;

				dest.emplace( std::make_pair( key, value ) );
			}
		};

		template < typename Type >
		void _Read( std::set< Type >& dest )
		{
			SizeType eleCount{};
			memcpy( &eleCount, m_buffer + m_unloadedPos, sizeof( SizeType ) );

			m_unloadedPos += sizeof( SizeType );

			for ( int i = 0; i < eleCount; ++i )
			{
				Type temp;
				(*this) >> temp;

				dest.emplace( temp );
			}
		};

		template < typename Type1, typename Type2 >
		void _Read( std::unordered_map< Type1, Type2 >& dest )
		{
			SizeType eleCount{};
			memcpy( &eleCount, m_buffer + m_unloadedPos, sizeof( SizeType ) );

			m_unloadedPos += sizeof( SizeType );

			for ( int i = 0; i < eleCount; ++i )
			{
				Type1 key;
				(*this) >> key;

				Type2 value;
				(*this) >> value;

				dest.emplace( std::make_pair( key, value ) );
			}
		};

		template < typename Type >
		void _Read( std::unordered_set< Type >& dest )
		{
			SizeType eleCount{};
			memcpy( &eleCount, m_buffer + m_unloadedPos, sizeof( SizeType ) );

			m_unloadedPos += sizeof( SizeType );

			for ( int i = 0; i < eleCount; ++i )
			{
				Type temp;
				(*this) >> temp;

				dest.emplace( temp );
			}
		};

		#endif
#endif
	#pragma endregion
	#pragma region [ Var ]
	protected:
		const BufferType& m_buffer;      // �ܺο��� ���޹��� �޸� ��Ʈ�� �����Դϴ�. 
		      SizeType    m_unloadedPos; // ������ ��ġ�Դϴ�.
	#pragma endregion
	};
}