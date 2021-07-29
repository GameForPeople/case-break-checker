/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

/*
	swtich-case ��������, break�� �����Ǵ� ��츦 Ȯ���մϴ�.

	�ٸ� if-else ���� �б� ������ �� ���� break �� ���� ���� ����...(�׸��� ������..) Ž������ ���� �� ���Դϴ�.
	�ܼ� break ���� �˻������ ����Ź�帳�ϴ�.

	�����մϴ�.
*/

static constexpr bool FOR_L2SERVER = true;

#include "WonSY.h"

#include <thread>
#include <vector>
#include <queue>
#include <mutex>

#ifndef WONSY_CPP_20
	static_assert( false, "need cpp20" );
#endif

int main( int argc, char *argv[] )
{
	// �׽�Ʈ �ڵ�
	{
		if constexpr ( false )
		{
			return 1;
		}
	}

	// UI ���
	{
		std::cout << ""                                                     << std::endl;
		std::cout << "Case-Break-Checker ver 0.1"                           << std::endl;
		std::cout << "Copyright 2021, Won Seong-Yeon. All Rights Reserved." << std::endl;
		std::cout << "		KoreaGameMaker@gmail.com"                       << std::endl;
		std::cout << "		github.com/GameForPeople"                       << std::endl;
		std::cout << "" << std::endl;
		std::cout << "" << std::endl;
		std::cout << "" << std::endl;
	}

	// ����� �μ��� ����, ������ ������Ʈ ��θ� ���մϴ�.
	const auto rootPathCont = [ argc, &argv ]() mutable -> std::vector < std::filesystem::path >
		{
			if ( argc < 2 )
			{
				argc      = 2;
				argv[ 1 ] = (char *)("test/");
				NOTICE_LOG( "�־��� ����� �μ��� ����, �ϵ� �ڵ��� default ��θ� ����Ͽ����ϴ�." );
			}

			std::vector < std::filesystem::path > retPathCont;
			retPathCont.reserve( argc - 1 );

			for ( int pathCount = 1; pathCount < argc; ++pathCount )
			{
				const std::filesystem::path tempPath{ static_cast< std::string >( argv[ pathCount ] ) };

				// �ҽ� ��ũ �˻�
				if ( !std::filesystem::exists( tempPath ) )
				{
					ERROR_LOG( "�ش� ��ΰ� ��ȿ���� �ʾ� �����մϴ�. : " + tempPath.generic_string() );
				}

				retPathCont.emplace_back( tempPath );
			}
			
			return retPathCont;
		}();

	// ��Ƽ������ �ؾ���?
	const auto threadCount = static_cast< int >( std::thread::hardware_concurrency() );
	NOTICE_LOG( "hardware_concurrency is : " + std::to_string( threadCount ) );

	std::vector< std::queue< std::filesystem::path > > pathCont;
	std::vector< std::queue< std::string > >           logCont;
	pathCont.resize( threadCount );
	logCont.resize( threadCount );

	// �ϴ��� ���� �����忡��, ������ ���ƴٴϸ鼭, ��� �ҽ������� ã�� �ֽ��ϴ�.
	{
		auto contIndex = 0;
		auto pathCount = static_cast< unsigned long long >( 0 );

		for ( const auto& rootPath : rootPathCont )
		{
			NOTICE_LOG( "parsing : " + rootPath.generic_string() );

			for ( const auto& pathIter : std::filesystem::recursive_directory_iterator( rootPath ) )
			{
				if ( pathIter.is_regular_file() ) LIKELY
				{
					if (
						pathIter.path().extension().generic_string() == ".cpp" ||
						pathIter.path().extension().generic_string() == ".h"   ) LIKELY
					{
						if constexpr ( FOR_L2SERVER )
						{
							if ( pathIter.path().filename().generic_string().find( "template" ) != std::string::npos )
								continue;
						}

						++pathCount;
						pathCont[ contIndex++ ].push( pathIter.path() );

						if ( contIndex == pathCont.size() ) UNLIKELY
							contIndex = 0;
					}
				}
			}
		}

		NOTICE_LOG( "parsing end! total file count : " + std::to_string( pathCount ) );
	}

	// ������ �����忡�� �ڽ��� �Ҵ���� ���ϵ鿡 ���Ͽ�, �˻��մϴ�.
	{
		NOTICE_LOG( "check start! ");

		std::vector< std::thread > threadCont;
		threadCont.reserve( threadCount );

		for ( int index = 0; index < threadCount; ++index )
		{
			threadCont.emplace_back(
				static_cast< std::thread >(
					[ index, &pathCont, &logCont ]()
					{
						auto& localPathCont = pathCont[ index ];
						auto& localLogCont  = logCont[ index ];
						
						while ( !localPathCont.empty() )
						{
							WonSY::File::WsyFileReader checkFile( localPathCont.front(), WonSY::File::READ_MODE::NONE );
							localPathCont.pop();

							bool isReadedCase     = false; // "case" �� �о����� ����
							bool isPrevReadedCase = false; // "case" �� �ٷ� ������ �о����� ����
							bool isExistSwitch    = false; // "switch" �� �ش����Ͽ� �����߾����� ����
							bool isOnComment      = false; // �ּ� �������� ���� üũ

							while ( 7 )
							{
								if ( const auto getResult = checkFile.GetNextLine();
									false == getResult.has_value() )
								{
									break;
								}
								else
								{
									const auto& line = *getResult;

									// �ش� ������ �ּ� ��Ȳ���� üũ�մϴ�.
									{
										if ( !isOnComment ) LIKELY
										{
											if ( line.find( "/*" ) != std::string::npos ) UNLIKELY
											{
												isOnComment = true;
											}

											if ( line.find( "*/" ) != std::string::npos ) UNLIKELY
											{
												isOnComment = false;
											}
										}

										if ( isOnComment ) UNLIKELY
											continue;
									}

									std::string deepCopyString = line;
									WonSY::File::EraseFirstTab( deepCopyString );
									const auto tokenCont = WonSY::File::DoTokenize( deepCopyString, ' ' );

									// ��ĭ �� ��, �ּ��� ��, ��ó���� �� �� ���� ���θ� Ȯ���Ͽ� �ѱ�ϴ�.
									{
										if ( !tokenCont.size() ) UNLIKELY
										{
											// ��ĭ�� ���� ��� �������ݴϴ�.
											continue;
										}
										else if ( tokenCont[ 0 ].size() > 1 && tokenCont[ 0 ][ 0 ] == '/' && tokenCont[ 0 ][ 1 ] == '/' ) UNLIKELY
										{
											// �պκ��� �ǰ� ������ ��� ������ ����, ù���� ���ڸ� Ȯ���Ͽ�, �ּ����� ���θ� üũ�մϴ�.
											continue;
										}
										else if ( tokenCont[ 0 ].size() > 1 && tokenCont[ 0 ][ 0 ] == '#' ) UNLIKELY
										{
											// ��ó���� �����ڸ� �������ݴϴ�. ( #ifdef, #endif )
											continue;
										}
									}

									// �ش� ���Ͽ� Switch�� �����߾����� ���θ� Ȯ���մϴ�.
									{
										if ( !isExistSwitch ) LIKELY
										{
											if ( WonSY::File::FindString( tokenCont, "switch" ) ) LIKELY
											{
												continue;
											}
											else UNLIKELY
											{
												isExistSwitch = true;
												continue;
											}
										}
									}

									// case�� Ž���ϰ�, break - return ���� Ȯ���Ͽ� ó���Ѵ�.
									{
										if ( !isReadedCase ) LIKELY
										{
											if ( WonSY::File::FindString( tokenCont, "case" ) )
											{
												isReadedCase     = true;
												isPrevReadedCase = true;
											}

											// ���� �ٿ� �ٷ� break, retunr �� ��ġ ��, �ٷ� Ǭ��.
											if (
												line.find( "break;" ) != std::string::npos ||
												line.find( "return" ) != std::string::npos ||
												line.find( "fallthrough" ) != std::string::npos ) UNLIKELY
											{
												// ex ) case 1: ~~~~~~ breal;
												isReadedCase     = false;
												isPrevReadedCase = false;
											}
											if constexpr ( FOR_L2SERVER )
											{
												if ( line.find( "PROCESS_STAT_VALUE" ) != std::string::npos )
												{
													isReadedCase     = false;
													isPrevReadedCase = false;
												}
											}
										}
										else if ( isReadedCase && isPrevReadedCase ) UNLIKELY
										{
											// Case�� ���� �� �ٷ� �������� ó���Ͽ��ݴϴ�.

											if ( WonSY::File::FindString( tokenCont, "case" ) )
											{
												// ex ) case 1:
												//      case 2:
												//      case 3:
												isReadedCase     = true;
												isPrevReadedCase = true;
											}
											else if (
												line.find( "break;" ) != std::string::npos ||
												line.find( "return" ) != std::string::npos ||
												line.find( "switch" ) != std::string::npos ||
												line.find( "fallthrough" ) != std::string::npos ) UNLIKELY
											{
												isReadedCase     = false;
												isPrevReadedCase = false;
											}
											else
											{
												if constexpr ( FOR_L2SERVER )
												{
													if ( line.find( "PROCESS_STAT_VALUE" ) != std::string::npos )
													{
														isReadedCase     = false;
														isPrevReadedCase = false;
													}
												}

												isPrevReadedCase = false;
											}
										}
										else if ( isReadedCase && !isPrevReadedCase ) UNLIKELY
										{
											if (
												line.find( "break;" ) != std::string::npos ||
												line.find( "return" ) != std::string::npos ||
												line.find( "switch" ) != std::string::npos ||
												line.find( "fallthrough" ) != std::string::npos )
											{
												isReadedCase     = false;
												isPrevReadedCase = false;
											}
											else if (
												WonSY::File::FindString( tokenCont, "case" )     ||
												line.find( "default:" ) != std::string::npos     )
											{
												localLogCont.emplace( " Plz Check!!  [ File : " +
													checkFile.GetFileName() + ", Line : " + std::to_string( checkFile.GetCurLineIndex() ) + " ]" );

												isReadedCase     = false;
												isPrevReadedCase = false;
											}

											// ������ case�� ���� break�� return�� ������ ū ������ ���� �ʴ´ٰ� �Ǵ��Ͽ����ϴ�.
											if constexpr ( FOR_L2SERVER )
											{
												if ( line.find( "PROCESS_STAT_VALUE" ) != std::string::npos )
												{
													isReadedCase     = false;
													isPrevReadedCase = false;
												}
											}
										}
									}
								}
							}
						}
					} ) );
		}

		for ( auto& th : threadCont )
			th.join();

		for ( auto& ele : logCont )
		{
			while ( !ele.empty() )
			{
				WARN_LOG( ele.front() );
				ele.pop();
			}
		}

		NOTICE_LOG( "check end! bye bye!");
	}

	// std::system( "PAUSE" );
}