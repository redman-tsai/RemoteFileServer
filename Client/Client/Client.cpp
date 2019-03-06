#include "Client.h"
/////////////////////////////////////////////////////////////////////////
// Client.cpp - demo client                                            //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Ruizhe CAI (c) copyright 2015                                       //
//354520026 rcai100@syr.edu                                            //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Parallel Desktop, MBP15  Win 8.1 pro                   //
/////////////////////////////////////////////////////////////////////////

/*int clientcontrolmode(int argc, char* argv[])
{
	if (argc < 4)
	{
		std::cout << "\n too less arguments\n" << std::endl;
		return 1;
	}
	std::string sip = argv[1];
	size_t port = atoi(argv[2]);
	bool inputmode = (atoi(argv[3]) != 0);
	client C(sip, port)
	C.start(inputmode);
	return 0;
}*/

#ifdef TEST_CLIENT
int main(int argc, char* argv[])
{
	ApplicationHelpers::Verbose v(true);

	if (argc < 4)
	{
		std::cout << "\n too less arguments\n" << std::endl;
		return 1;
	}
	std::string sip = argv[1];
	size_t port = atoi(argv[2]);
	bool inputmode  = (atoi(argv[3]) != 0);
	client client1(sip, port);
	client1.start(inputmode);
	std::string cmd;
	cmd = "command:connect\ndstaddr:localhost+9080\nsrcaddr:localhost+9050";
	client1.postGuiCmd(cmd);
	cmd = "command:upload\ndstaddr:localhost+9080\nsrcaddr:localhost+9050\nfilename:./aa.txt";
	client1.postGuiCmd(cmd);
	cmd = "command:filequery\ndstaddr:localhost+9080\nsrcaddr:localhost+9050\nbody:<Filter></Filter>";
	client1.postGuiCmd(cmd);
	std::cout << client1.popResult();
	system("pause");
}
#endif
