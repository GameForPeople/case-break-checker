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
#include <stack>
// #include <mutex>

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
		std::cout << "Case-Break-Checker ver 0.2"                           << std::endl;
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

							enum class READ_STATE
							{
								BEFORE_CASE,        // CASE �б� ��
								AFTER_CASE_DIRECT,  // CASE ���� ���� ~ ���� ���α���
								AFTER_CASE,         // CASE �а� �ٴ��� ���� ����, Break, Return ���� ���� ������ ������. 
							};

							// after Check
							bool       isExistSwitch = false; // "switch" �� �ش����Ͽ� �����߾����� ����
							bool       isOnComment   = false; // �ּ� �������� ���� üũ

							// Check
							READ_STATE                                 readState       = READ_STATE::BEFORE_CASE;
							int                                        prevIfLineIndex = 0; // ������ If���� �����ߴ� ����
							int                                        depthLevel      = 0; // ����
							// int                                        depthLevel = 0; // ����
							// std::stack< std::pair< READ_STATE, int > > stateCont;      // ���� Switch�� ó��.

							const auto initFunc = [ & ](){ readState = READ_STATE::BEFORE_CASE; prevIfLineIndex = 0; depthLevel = 0; };
							initFunc();

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

									// �ش� ������ �ּ����� üũ�մϴ�.
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
									WonSY::File::Replace( deepCopyString, "\t", " " );
									auto tokenCont = WonSY::File::DoTokenize( deepCopyString, ' ' );
									WonSY::File::FindAndErase( tokenCont, "" );
									WonSY::File::FindAndErase( tokenCont, " " );

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

									// ���� ���Ͽ��� ���ݱ���, Switch�� �����߾����� ���θ� Ȯ���մϴ�.
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
										if ( READ_STATE::BEFORE_CASE == readState ) LIKELY
										{
											if ( WonSY::File::FindString( tokenCont, "case" ) )
											{
												readState = READ_STATE::AFTER_CASE_DIRECT;
											}

											// ���� �ٿ� �ٷ� break, return �� ��ġ ��, �ٷ� Ǭ��.
											if (
												line.find( "break;" ) != std::string::npos ||
												line.find( "return" ) != std::string::npos ||
												line.find( "fallthrough" ) != std::string::npos ) UNLIKELY
											{
												// ex ) case 1: ~~~~~~ break; or case 2: ~~~~~~ return;
												initFunc();
											}

											if constexpr ( FOR_L2SERVER )
											{
												// ȸ�� ���� ��ũ��
												if (
													line.find( "PROCESS_STAT_VALUE" ) != std::string::npos ) UNLIKELY
												{
													initFunc();
												}
											}
										}
										else if ( READ_STATE::AFTER_CASE_DIRECT == readState ) UNLIKELY
										{
											// Case�� ���� �� �ٷ� �������� ó���Ͽ��ݴϴ�.

											if ( WonSY::File::FindString( tokenCont, "case" ) ) UNLIKELY
											{
												// ex ) case 1:
												//      case 2:
												//      case 3:

												readState = READ_STATE::AFTER_CASE_DIRECT;
											}
											else if (
												line.find( "break;" ) != std::string::npos ||
												line.find( "return" ) != std::string::npos ||
												line.find( "switch" ) != std::string::npos ||
												line.find( "fallthrough" ) != std::string::npos ) UNLIKELY
											{
												initFunc();
											}
											else
											{
												readState = READ_STATE::AFTER_CASE;
												
												if constexpr ( FOR_L2SERVER )
												{
													if ( line.find( "PROCESS_STAT_VALUE" ) != std::string::npos ) UNLIKELY
													{
														initFunc();
														continue;
													}
												}
											}
										}
										else if ( READ_STATE::AFTER_CASE == readState ) UNLIKELY
										{
											// ������ case�� ���� break�� return�� ������ ū ������ ���� �ʴ´ٰ� �Ǵ��Ͽ����ϴ�.
											if constexpr ( FOR_L2SERVER )
											{
												if ( line.find( "PROCESS_STAT_VALUE" ) != std::string::npos )
												{
													initFunc();
													continue;
												}
											}

											if ( depthLevel )
											{
												if ( line.find( "}" ) != std::string::npos )
												{
													--depthLevel;
												}
											}

											WonSY::AutoCallList autoCallList;
											if ( prevIfLineIndex )
											{
												// ���� ���ο��� if���� �־��ٸ�,
												if ( prevIfLineIndex + 1 == checkFile.GetCurLineIndex() )
												{
													++depthLevel;
													
													// ���� ���ο���, ( '{'�� �ִٸ�, '}'�� ���� �� ����, ���� ��� ������ �ش� �����̰�, ���� ��� ���� ���α����̴�.
													if ( line.find( "{" ) != std::string::npos )
													{
													}
													else
													{
														autoCallList.Add( [ &depthLevel ](){ --depthLevel; } );
													}
												}

												prevIfLineIndex = 0;
											}

											if (
												line.find( "break;" ) != std::string::npos ||
												line.find( "return" ) != std::string::npos ||
												line.find( "switch" ) != std::string::npos ||
												line.find( "fallthrough" ) != std::string::npos )
											{
												if ( !depthLevel )
												{
													initFunc();
												}
											}
											else if (
												WonSY::File::FindString( tokenCont, "case" )     ||
												line.find( "default:" ) != std::string::npos     )
											{
												localLogCont.emplace( " Plz Check!!  [ File : " +
													checkFile.GetFileName() + ", Line : " + std::to_string( checkFile.GetCurLineIndex() ) + " ]" );

												initFunc();
											}
											else
											{
												if ( 
													line.find( "if" ) != std::string::npos  ||
													line.find( "else" ) != std::string::npos )
												{
													prevIfLineIndex = checkFile.GetCurLineIndex();
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