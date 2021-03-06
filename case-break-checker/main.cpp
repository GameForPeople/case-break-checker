/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

/*
	swtich-case 구문에서, break가 누락되는 경우를 확인합니다.

	다만 if-else 등의 분기 내에서 한 곳의 break 시 등의 경우는 아직...(그리고 영원히..) 탐색하지 못할 듯 보입니다.
	단순 break 누락 검사용으로 사용부탁드립니다.

	감사합니다.
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
	// 테스트 코드
	{
		if constexpr ( false )
		{
			return 1;
		}
	}

	// UI 출력
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

	// 명령형 인수에 따라, 적법한 프로젝트 경로를 구합니다.
	const auto rootPathCont = [ argc, &argv ]() mutable -> std::vector < std::filesystem::path >
		{
			if ( argc < 2 )
			{
				argc      = 2;
				argv[ 1 ] = (char *)("test/");
				NOTICE_LOG( "주어진 명령형 인수가 없어, 하드 코딩된 default 경로를 사용하였습니다." );
			}

			std::vector < std::filesystem::path > retPathCont;
			retPathCont.reserve( argc - 1 );

			for ( int pathCount = 1; pathCount < argc; ++pathCount )
			{
				const std::filesystem::path tempPath{ static_cast< std::string >( argv[ pathCount ] ) };

				// 소스 링크 검사
				if ( !std::filesystem::exists( tempPath ) )
				{
					ERROR_LOG( "해당 경로가 유효하지 않아 종료합니다. : " + tempPath.generic_string() );
				}

				retPathCont.emplace_back( tempPath );
			}
			
			return retPathCont;
		}();

	// 멀티쓰레드 해야지?
	const auto threadCount = static_cast< int >( std::thread::hardware_concurrency() );
	NOTICE_LOG( "hardware_concurrency is : " + std::to_string( threadCount ) );

	std::vector< std::queue< std::filesystem::path > > pathCont;
	std::vector< std::queue< std::string > >           logCont;
	pathCont.resize( threadCount );
	logCont.resize( threadCount );

	// 일단은 단일 쓰레드에서, 파일을 돌아다니면서, 모든 소스파일을 찾아 넣습니다.
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

	// 각각의 쓰레드에서 자신이 할당받은 파일들에 대하여, 검사합니다.
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
								BEFORE_CASE,        // CASE 읽기 전
								AFTER_CASE_DIRECT,  // CASE 읽은 직후 ~ 다음 라인까지
								AFTER_CASE,         // CASE 읽고 다다음 라인 부터, Break, Return 등의 문자 나오기 전까지. 
							};

							// after Check
							bool       isExistSwitch = false; // "switch" 가 해당파일에 존재했었는지 여부
							bool       isOnComment   = false; // 주석 상태인지 여부 체크

							// Check
							READ_STATE                                 readState       = READ_STATE::BEFORE_CASE;
							int                                        prevIfLineIndex = 0; // 이전에 If문이 존재했던 라인
							int                                        depthLevel      = 0; // 뎁스
							// int                                        depthLevel = 0; // 뎁스
							// std::stack< std::pair< READ_STATE, int > > stateCont;      // 이중 Switch문 처리.

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

									// 해당 라인이 주석인지 체크합니다.
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

									// 빈칸 일 시, 주석일 시, 전처리기 일 시 등의 여부를 확인하여 넘깁니다.
									{
										if ( !tokenCont.size() ) UNLIKELY
										{
											// 빈칸만 있을 경우 제거해줍니다.
											continue;
										}
										else if ( tokenCont[ 0 ].size() > 1 && tokenCont[ 0 ][ 0 ] == '/' && tokenCont[ 0 ][ 1 ] == '/' ) UNLIKELY
										{
											// 앞부분의 탭과 공백을 모두 제거한 이후, 첫번쨰 글자를 확인하여, 주석인지 여부를 체크합니다.
											continue;
										}
										else if ( tokenCont[ 0 ].size() > 1 && tokenCont[ 0 ][ 0 ] == '#' ) UNLIKELY
										{
											// 전처리기 지시자를 제거해줍니다. ( #ifdef, #endif )
											continue;
										}
									}

									// 현재 파일에서 지금까지, Switch가 존재했었는지 여부를 확인합니다.
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

									// case를 탐색하고, break - return 등을 확인하여 처리한다.
									{
										if ( READ_STATE::BEFORE_CASE == readState ) LIKELY
										{
											if ( WonSY::File::FindString( tokenCont, "case" ) )
											{
												readState = READ_STATE::AFTER_CASE_DIRECT;
											}

											// 같은 줄에 바로 break, return 등 배치 시, 바로 푼다.
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
												// 회사 전용 매크로
												if (
													line.find( "PROCESS_STAT_VALUE" ) != std::string::npos ) UNLIKELY
												{
													initFunc();
												}
											}
										}
										else if ( READ_STATE::AFTER_CASE_DIRECT == readState ) UNLIKELY
										{
											// Case를 읽은 후 바로 다음줄을 처리하여줍니다.

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
											// 마지막 case는 이후 break나 return이 없더라도 큰 문제가 되지 않는다고 판단하였습니다.
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
												// 이전 라인에서 if문이 있었다면,
												if ( prevIfLineIndex + 1 == checkFile.GetCurLineIndex() )
												{
													++depthLevel;
													
													// 현재 라인에서, ( '{'이 있다면, '}'가 있을 떄 까지, 뎁스 상승 범위가 해당 범위이고, 없을 경우 현재 라인까지이다.
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
													WonSY::File::FindString( tokenCont, "if" )  ||
													WonSY::File::FindString( tokenCont, "if(" ) ||
													WonSY::File::FindString( tokenCont, "else" ) )
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