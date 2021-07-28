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
	// �׽�Ʈ
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
	const auto rootPath = [ argc, &argv ]() -> std::filesystem::path
		{
			if ( argc < 2 )
			{
				argv[ 1 ] = (char *)("test/");
				NOTICE_LOG( "�־��� ����� �μ��� ����, �ϵ� �ڵ��� default ��θ� ����Ͽ����ϴ�." );
			}

			const std::filesystem::path retPath{ static_cast< std::string >( argv[ 1 ] ) };

			// �ҽ� ��ũ �˻�
			if ( !std::filesystem::exists( retPath ) )
			{
				ERROR_LOG( "�ش� ��ΰ� ��ȿ���� �ʾ� �����մϴ�. : " + retPath.generic_string() );
			}
			
			return retPath;
		}();

	// ��Ƽ������ �ؾ���?
	const auto threadCount = static_cast< int >( std::thread::hardware_concurrency() );
	NOTICE_LOG( "hardware_concurrency is : " + std::to_string( threadCount ) );

	std::vector< std::queue< std::filesystem::path > > pathCont;
	pathCont.resize( threadCount );

	// �ϴ��� ���� �����忡��, ������ ���ƴٴϸ鼭, ��� �ҽ������� ã�� �ֽ��ϴ�.
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

	// ������ �����忡�� �ڽ��� �Ҵ���� ���ϵ鿡 ���Ͽ�, �˻��մϴ�.
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

							//bool isReadedSwitch   = false; // "switch" �� �о����� ����
							bool isReadedCase     = false; // "case" �� �о����� ���� 
							bool isPrevReadedCase = false; // "case" �� �ٷ� ������ �о����� ���� 

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

									// ���� �ٿ� break ��ġ ��, �ٷ� Ǭ��.
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
									// �ٷ� �������� case�� �����߰�,
									if ( isPrevReadedCase )
									{
										if ( line.find( "case" ) != std::string::npos )
										{
											isReadedCase     = true;
											isPrevReadedCase = true;
										}
										// ���� �ٿ� break ��ġ ��, �ٷ� Ǭ��.
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
											WARN_LOG( " Switch - Case ���� Ȯ���� �ʿ��մϴ�.  [ File : " + 
												checkFile.GetFileName() + ", Line : " + std::to_string( checkFile.GetCurLineIndex() ) );

											isReadedCase     = false;
											isPrevReadedCase = false;
										}
										else if ( line.find( "switch" ) != std::string::npos )
										{
											// �ϳ��� ���Ͽ����� ���� switch������, ���� switch���� ������case�� return�̳� break�� ������.
											isReadedCase     = false;
											isPrevReadedCase = false;
										}
										// ������ case�� ���� break�� return�� ������ ū ������ ���� �ʴ´�.
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