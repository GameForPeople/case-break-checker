/*
	Copyright 2021, Won Seong-Yeon. All Rights Reserved.
		KoreaGameMaker@gmail.com
		github.com/GameForPeople
*/

static constexpr bool FOR_NEO = true; 

#include "WonSY.h"

#include <thread>
#include <vector>
#include <queue>

#ifndef WONSY_CPP_20
	static_assert( false, "min cpp20" );
#endif

int main( int argc, char *argv[] )
{
	// 테스트
	{
		if constexpr ( false ) 
		{
			return 1;
		}
	}

	// UI 출력
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

	// 명령형 인수에 따라, 적법한 프로젝트 경로를 구합니다.
	const auto rootPath = [ argc, &argv ]() -> std::filesystem::path
		{
			if ( argc < 2 )
			{
				argv[ 1 ] = (char *)("test/");
				NOTICE_LOG( "주어진 명령형 인수가 없어, 하드 코딩된 default 경로를 사용하였습니다." );
			}

			const std::filesystem::path retPath{ static_cast< std::string >( argv[ 1 ] ) };

			// 소스 링크 검사
			if ( !std::filesystem::exists( retPath ) )
			{
				ERROR_LOG( "해당 경로가 유효하지 않아 종료합니다. : " + retPath.generic_string() );
			}
			
			return retPath;
		}();

	// 멀티쓰레드 해야지?
	const auto threadCount = static_cast< int >( std::thread::hardware_concurrency() );
	NOTICE_LOG( "hardware_concurrency is : " + std::to_string( threadCount ) );

	std::vector< std::queue< std::filesystem::path > > pathCont;
	pathCont.resize( threadCount );

	// 일단은 단일 쓰레드에서, 파일을 돌아다니면서, 모든 소스파일을 찾아 넣습니다.
	{
		NOTICE_LOG( "parsing start!");

		auto contIndex = 0;

		for ( const auto& pathIter : std::filesystem::recursive_directory_iterator( rootPath ) )
		{
			if ( pathIter.is_regular_file() ) LIKELY
			{
				if ( 
					pathIter.path().extension().generic_string() == ".cpp" ||
					pathIter.path().extension().generic_string() == ".h"   ) LIKELY
				{
					if constexpr ( FOR_NEO )
					{
						if ( pathIter.path().filename().generic_string().find( "template" ) != std::string::npos )
							continue;
					}

					pathCont[ contIndex++ ].push( pathIter.path() );

					if ( contIndex == pathCont.size() ) UNLIKELY
						contIndex = 0;
				}
			}
		}

		NOTICE_LOG( "parsing end!");
	}

	// 각각의 쓰레드에서 자신이 할당받은 파일들에 대하여, 검사합니다.
	{
		NOTICE_LOG( "check start!");

		std::vector< std::thread > threadCont;
		threadCont.reserve( threadCount );

		for ( int index = 0; index < threadCount; ++index )
		{
			threadCont.emplace_back( 
				static_cast< std::thread >( 
					[ index, &pathCont ]()
					{
						auto& localPathCont = pathCont[ index ];
						
						while ( !localPathCont.empty() )
						{
							WonSY::File::WsyFileReader checkFile( localPathCont.front(), WonSY::File::READ_MODE::NONE );
							localPathCont.pop();

							//bool isReadedSwitch   = false; // "switch" 를 읽었는지 여부
							bool isReadedCase     = false; // "case" 를 읽었는지 여부 
							bool isPrevReadedCase = false; // "case" 를 바로 이전에 읽었는지 여부 

							for ( const auto getResult = checkFile.GetNextLine(); getResult.has_value(); )
							{
								const auto& line = *getResult;

								if ( !isReadedCase ) LIKELY
								{
									if ( line.find( "case" ) != std::string::npos )
									{
										isReadedCase     = true;
										isPrevReadedCase = true;
									}

									// 같은 줄에 break 배치 시, 바로 푼다.
									if ( 
										line.find( "break" ) != std::string::npos || 
										line.find( "return" ) != std::string::npos )
									{
										isReadedCase     = false;
										isPrevReadedCase = false;
									}
								}
								else if ( isReadedCase ) UNLIKELY
								{
									// 바로 이전행이 case가 존재했고,
									if ( isPrevReadedCase )
									{
										if ( line.find( "case" ) != std::string::npos )
										{
											isReadedCase     = true;
											isPrevReadedCase = true;
										}
										// 같은 줄에 break 배치 시, 바로 푼다.
										else if ( 
											line.find( "break" ) != std::string::npos || 
											line.find( "return" ) != std::string::npos )
										{
											isReadedCase     = false;
											isPrevReadedCase = false;
										}
										else
										{
											isPrevReadedCase = false;
										}
									}
									else
									{
										if ( 
											line.find( "case" ) != std::string::npos    ||
											line.find( "default" ) != std::string::npos )
										{
											WARN_LOG( " Switch - Case 문의 확인이 필요합니다.  [ File : " + 
												checkFile.GetFileName() + ", Line : " + std::to_string( checkFile.GetCurLineIndex() ) );

											isReadedCase     = false;
											isPrevReadedCase = false;
										}
										else if ( line.find( "switch" ) != std::string::npos )
										{
											// 하나의 파일에서의 여러 switch문에서, 이전 switch문의 마지막case에 return이나 break가 없을떄.
											isReadedCase     = false;
											isPrevReadedCase = false;
										}
										// 마지막 case는 이후 break나 return이 없더라도 큰 문제가 되지 않는다.
									}
								}
							}
						}

					} ) );
		}

		for ( auto& th : threadCont )
			th.join();

		NOTICE_LOG( "check end! bye bye!");
	}

	std::system( "PAUSE" );
}