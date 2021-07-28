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
		// ����
		if ( m_readMode == READ_MODE::NONE )
		{
			ERROR_LOG( "����! READ_MODE::NONE Ÿ���� Reload�� �Ұ�����... �¾�..?" + m_filePath.generic_string() );
		}
		else if ( m_readMode == READ_MODE::FULL_LOAD )
		{
			m_nextStringIndex = 0;
		}
	}

	std::optional< std::string > WsyFileReader::GetNextLine()
	{
		// �� ���� ���ø����� �Ѽų���.. �ѼŻѼ�~~~~
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
		// �ҽ� ��ũ �˻�
		if ( !std::filesystem::exists( m_filePath ) )
		{
			ERROR_LOG( "����! �ش� ��ũ�� �������� ����! " + m_filePath.generic_string() );
		}

		// ���� ���� �˻�
		if ( !m_fileStream.is_open() )
		{
			ERROR_LOG( "?? �׾����?" + m_filePath.generic_string() );
		}

		if ( m_readMode == READ_MODE::NONE )
			return;

		// Full Load�� ��쿡�� ������ ��� �о������Ѵ�.
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
		ERROR_LOG( "���������� ������. ���� ��" );
	}

	WsyFileWriter::~WsyFileWriter()
	{
	}

#pragma endregion
}