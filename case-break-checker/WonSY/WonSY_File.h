/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

#pragma once

#define WONSY_FILE

#include "WonSY.hh"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <filesystem>

#include <optional>

namespace WonSY::File
{
	enum class READ_MODE
	{
		NONE,
		FULL_LOAD
	};

	class BaseFile
	{
	public:
		BaseFile( const std::filesystem::path& filePathString );
		virtual ~BaseFile() = default;

		std::string           GetFileName();
		std::filesystem::path GetFilePath();

		virtual int GetCurLineIndex() = 0;

	protected:
		const std::filesystem::path m_filePath;
	};

	// READ_MODE �̰� ���� ������, ���߿� ���ø����� �ѼŹ�����
	class WsyFileReader
		: public BaseFile
	{
	public:
		WsyFileReader( 
			const std::filesystem::path& filePathString,
			const READ_MODE              readMode = READ_MODE::NONE );

		~WsyFileReader();

		void                         Reload();
		std::optional< std::string > GetNextLine();

		virtual int GetCurLineIndex() final;
	private:
		void _Load();

	private:
		READ_MODE                  m_readMode;
		std::ifstream              m_fileStream;

		std::vector< std::string > m_stringCont;
		int                        m_nextStringIndex;
	};

	class WsyFileWriter
		: public BaseFile
	{
	public:
		WsyFileWriter( const std::string& filePathString );
		~WsyFileWriter();

		virtual int GetCurLineIndex() final { return 0; };
	};
}