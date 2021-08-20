/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#include "WonSY.h"

namespace WonSY::File
{
#pragma region [Base File]
	BaseFile::BaseFile(	const std::filesystem::path& filePathString )
		: m_filePath( filePathString )
	{
	}

	std::filesystem::path BaseFile::GetFilePath()
	{
		return m_filePath;
	}

	std::string BaseFile::GetFileName()
	{
		return m_filePath.filename().generic_string();
	}
#pragma endregion

#pragma region [WsyFileReader]
	WsyFileReader::WsyFileReader(
		const std::filesystem::path& filePathString,
		const READ_MODE              readMode /* = READ_MODE::NONE */ )
		: BaseFile         ( filePathString )
		, m_readMode       ( readMode       )
		, m_fileStream     ( m_filePath     )
		, m_stringCont     ()
		, m_nextStringIndex()
	{
		_Load();
	}

	WsyFileReader::~WsyFileReader()
	{
	}

	void WsyFileReader::Reload()
	{
		// ㄴㄷ
		if ( m_readMode == READ_MODE::NONE )
		{
			ERROR_LOG( "에러! READ_MODE::NONE 타입은 Reload가 불가능함... 맞아..?" + m_filePath.generic_string() );
		}
		else if ( m_readMode == READ_MODE::FULL_LOAD )
		{
			m_nextStringIndex = 0;
		}
	}

	std::optional< std::string > WsyFileReader::GetNextLine()
	{
		// 하 빨리 템플릿으로 뿌셔놓자.. 뿌셔뿌셔~~~~
		if ( m_readMode == READ_MODE::NONE )
		{
			if ( m_fileStream.eof() ) UNLIKELY
				return std::nullopt;

			std::string lineBuffer;
			std::getline( m_fileStream, lineBuffer );

			++m_nextStringIndex;

			return lineBuffer;
		}
		else /* if ( m_readMode == READ_MODE::FULL_LOAD ) */
		{
			if ( m_stringCont.size() <= m_nextStringIndex ) UNLIKELY
				return std::nullopt;

			return m_stringCont[ m_nextStringIndex++ ];
		}
	}

	int WsyFileReader::GetCurLineIndex()
	{
		return m_nextStringIndex;
	}

	void WsyFileReader::_Load()
	{
		// 소스 링크 검사
		if ( !std::filesystem::exists( m_filePath ) )
		{
			ERROR_LOG( "에러! 해당 링크가 존재하지 않음! " + m_filePath.generic_string() );
		}

		// 파일 오픈 검사
		if ( !m_fileStream.is_open() )
		{
			ERROR_LOG( "?? 죽어야지?" + m_filePath.generic_string() );
		}

		if ( m_readMode == READ_MODE::NONE )
			return;

		// Full Load일 경우에는 파일을 모두 읽어저장한다.
		while ( m_fileStream )
		{
			if ( m_fileStream.eof() ) UNLIKELY
				break;

			std::string lineBuffer;
			std::getline( m_fileStream, lineBuffer );
			m_stringCont.emplace_back( lineBuffer );
		}
	}
#pragma endregion

#pragma region [WsyFileWriter]
	WsyFileWriter::WsyFileWriter(
		const std::string& filePathString )
		: BaseFile( filePathString )
	{
		ERROR_LOG( "언제만들지 귀찮다. 게임 ㄱ" );
	}

	WsyFileWriter::~WsyFileWriter()
	{
	}

#pragma endregion

#pragma region [Others]

	bool FindString( const std::vector< std::string >& cont, const std::string& checkString )
	{
		for ( const auto& ele : cont )
		{
			if ( ele == checkString )
				return true;
		}

		return false;
	}

	void EraseFirstTab( std::string& retString )
	{
		FIRST:
		if ( retString.size() && retString[ 0 ] == '\t' )
		{
			retString.erase( retString.begin() );
			goto FIRST;
		}
	}

	// "delimiter"를 기준으로 토크나이즈한 결과를 리턴한다.
	std::vector< std::string > DoTokenize( const std::string& stringValue, const char delimiter /* = ',' */ )
	{
		std::vector< std::string > retCont;
		std::string                token;
		std::stringstream          stringStream( stringValue );

		while ( std::getline( stringStream, token, delimiter ) )
		{
			retCont.emplace_back( token );
		}

		return retCont;
	}

	void Replace( std::string& targetString, const std::string& oldString, const std::string& newString /* == ""*/ )
	{
		// 으악 잘못하면 무한 루프에요!

		size_t index = 0;

		HEAD:
		if ( index = targetString.find( oldString, index + 1 );
			index != std::string::npos )
		{
			targetString.erase( index, oldString.size() );

			if ( newString.size() )
				targetString.insert( index, newString );

			goto HEAD;
		}
	}
#pragma endregion
}