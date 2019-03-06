#include"Sender.h"
/////////////////////////////////////////////////////////////////////////
// Sender.cpp -functions in sender                                     //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Ruizhe CAI (c) copyright 2015                                       //
//354520026 rcai100@syr.edu                                            //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Parallel Desktop, MBP15  Win 8.1 pro                   //
/////////////////////////////////////////////////////////////////////////
void Sender::processmsg()
{
	msg m;
	do
	{
			m = msgqueue.deQ();
			{
				std::lock_guard<std::mutex> l(ioLock);
				sendmsg(m);
			}
	} while (msgr.getvalue(m, "command") != "close" );
	sendmsg(m);
}

void Sender::sendmsg(msg& m_)
{ 
	std::string command = "default";
	if (funcMap.find(msgr.getvalue(m_, "command")) == funcMap.end())
		std::cout << "\nCommand:" << msgr.getvalue(m_, "command")<<" not found, processs as sendmessage" << std::endl;
	else
		command = msgr.getvalue(m_, "command");
	call(command, m_);
}

void Sender::upload(msg& m_)
{
	std::string filespec = homedir + "/" + msgr.getvalue(m_, "filename");
	if (msgr.getvalue(m_, "HasPath")=="true")
		filespec = msgr.getvalue(m_, "filename");
	size_t size = msgr.getfilesize(m_);
	const size_t BufLen = 1024;
	Socket::byte buffer[BufLen];
	FileSystem::File f(filespec);
	if (msgr.getvalue(m_, "filetype") == "binary")
	{
		f.open(FileSystem::File::in, FileSystem::File::binary);
		while (size != 0)
		{
			ApplicationHelpers::Verbose::show("\nSender sent part binary file\n");
			f.getBuffer(min(BufLen, size), buffer);
			msgr.replacefilesize(m_, min(BufLen, size));
			if (!sendString(msgr.endoder(m_))){ std::cout << "\nNot sent\n"; }
			ApplicationHelpers::Verbose::show("\nSender sent\n" + msgr.endoder(m_));
			if (!send(min(BufLen, size), buffer)) { std::cout << "\nNot sent\n"; }
			size = size - min(size, BufLen);
			if (size == 0)
				break;
		}
		ApplicationHelpers::Verbose::show("\nSender sent binary file\n");
		f.close();
	}
	else
	{
		f.open(FileSystem::File::in, FileSystem::File::text);
		ApplicationHelpers::Verbose::show("\nSender sent\n" + msgr.endoder(m_));
		if (!sendString(msgr.endoder(m_))){ std::cout << "\nNot sent\n"; }
		ApplicationHelpers::Verbose::show("\nSender sent txt file\n");
		if(!sendString(f.readAll(true))) { ApplicationHelpers::Verbose::show("not sent", ApplicationHelpers::always); }
		f.close();
	}
}

void Sender::cnt(msg& m_)
{
	std::string ip = msgr.getdstip(m_);
	size_t port = msgr.getdstport(m_);
	if (!connectto(ip, port)){ std::cout << "\nnot connected " << ip << port << '\n'; }
	else
		std::cout << "\nconnected to " << ip << port << '\n';
	sendmsgbasic(m_);
}
		


void Sender::sendmsgbasic(msg& m_)
{
	while (!connected)
	{
		size_t port = msgr.getsdrport(m_);
		std::string ip = msgr.getsdrip(m_);
		while (!connectto(ip, port))
		{
			ApplicationHelpers::Verbose::show("Waiting for connection", ApplicationHelpers::always);
			Sleep(100);
		}
	}
	if (!sendString(msgr.endoder(m_))){ ApplicationHelpers::Verbose::show("not send", ApplicationHelpers::always); }
}

#ifdef TEST_SENDER
class ClientHandler
{
public:
	void operator()(Socket& socket_);
private:
	messenger msgr;
	void show(msg& m_)
	{
		std::vector<std::string> temp;
		for (auto item : m_.header)
			std::cout << '\n' << item;
		std::cout << '\n' << m_.body << std::endl;
	}
};
//----< Client Handler thread starts running this function >-----------------

void ClientHandler::operator()(Socket& socket_)
{
	
	msgr.setdir("./test");
	const size_t BufLen = 1024;
	Socket::byte buffer[BufLen];
	while (true)
	{
		// interpret test command

		std::string m = socket_.recvString();
		msg x = msgr.decoder(m);
		std::string command = msgr.getvalue(x, "command");
		if (command.size() == 0)
			break;
		ApplicationHelpers::Verbose::show("\nserver received string\n"+m);

		if (command == "upload")
		{
			std::string filetype = msgr.getvalue(x, "filetype");
			if (filetype == "binary")
			{
				size_t size = msgr.getfilesize(x);
				socket_.recv(size, buffer);
				std::cout << "\n file received";
			}
			else
			{
				std::string w = socket_.recvString();
				std::cout << "\nserver processed tex file string\n" << w;
			}
			std::cout << "\nserver pocessed command string\n" << m;
			continue; // go back and get another command
		}
		else if (command == "stop")
			break;
		else
		{
			std::cout << "\nserve processed command string\n" << m;
			continue;  // get another command
		}
	}

	// we get here if command isn't requesting a test, e.g., "TEST_STOP"

	ApplicationHelpers::Verbose::show("ClientHandler socket connection closing");
	socket_.shutDown();
	socket_.close();
	ApplicationHelpers::Verbose::show("ClientHandler thread terminating");
}

struct Cosmetic
{
	~Cosmetic()
	{
		std::cout << "\n  press key to exit: ";
		std::cin.get();
		std::cout << "\n\n";
	}
} aGlobalForCosmeticAction;



int main()
{
	try
	{
		ApplicationHelpers::Verbose v(true);
		Sender si, si2;
		SocketListener sl(9070, Socket::IP6);
		ClientHandler cp;
		sl.start(cp);
		messenger msgr;
		msg x, y, x2, y2;
		x = msgr.msgbuilder("upload", "localhost", 9070, "localhost", 9060);
		x2 = msgr.msgbuilder("upload", "localhost", 9070, "localhost", 9080);
		msgr.addattri(x, "filename", "jjj.txt");
		msgr.addattri(x2, "filename", "jjj.txt");
		y = msgr.msgbuilder("upload", "localhost", 9070, "localhost", 9060);
		y2 = msgr.msgbuilder("upload", "localhost", 9070, "localhost", 9080);
		msgr.addattri(y, "filename", "a.png");
		msgr.addattri(y2, "filename", "a.png");
		si.connectto("localhost", 9070);
		si2.connectto("localhost", 9070);
		si2.startsender();
		si.startsender();
		ApplicationHelpers::title("Starting sending txt file from different servers at the same time");
		si.postmessage(x);
		si2.postmessage(x2);
		ApplicationHelpers::title("Starting sending png");
		std::cout << "\n  press key to continue: ";
		std::cin.get();
		std::cout << "\n\n";
		si.postmessage(y);
		x = msgr.msgbuilder("stop", "localhost", 9070, "localhost", 9060);
		x2 = msgr.msgbuilder("stop", "localhost", 9070, "localhost", 9080);
		si.postmessage(x);
		si2.postmessage(x2);
		x = msgr.msgbuilder("close", "localhost", 9070, "localhost", 9060);
		x2 = msgr.msgbuilder("close", "localhost", 9070, "localhost", 9080);
		si.postmessage(x);
		si2.postmessage(x2);

		std::cin.get();
		ApplicationHelpers::Verbose::show("\n  client calling send shutdown\n");
		si.shutDownSend();
	}
	catch (std::exception& ex)
	{
		ApplicationHelpers::Verbose::show("  Exception caught:", ApplicationHelpers::always);
		ApplicationHelpers::Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
}
#endif