/////////////////////////////////////////////////////////////////////////
// Server.cpp-demo code                                                //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Ruizhe CAI (c) copyright 2015                                       //
//354520026 rcai100@syr.edu                                            //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Parallel Desktop, MBP15  Win 8.1 pro                   //
/////////////////////////////////////////////////////////////////////////
#include "Server.h"
#ifdef TEST_SERVER
int main(int argc, char* argv[]){
	ApplicationHelpers::Verbose v(true);
	if (argc < 3)
	{
		std::cout << "\n too less arguments\n" << std::endl;
		return 1;
	}
	std::string sip = argv[1];
	size_t port = atoi(argv[2]);
	server server1(sip, port);
	server1.start();
	std::string quit;

	while (quit != "quit")
	{
		std::cout << "\n  enter quit to quit server: ";
		std::getline(std::cin, quit);
		if (quit == "quit")
			server1.quit();
	}
	std::cout << "\n  press key to exit: ";
	std::cin.get();
	std::cout << "\n\n";

	system("pause");
	return 0;
}
#endif