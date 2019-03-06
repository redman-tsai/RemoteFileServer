#include "Receiver.h"
/////////////////////////////////////////////////////////////////////////
// Receiver.cpp -defining function for receiver package                //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Ruizhe CAI (c) copyright 2015                                       //
//354520026 rcai100@syr.edu                                            //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Parallel Desktop, MBP15  Win 8.1 pro                   //
/////////////////////////////////////////////////////////////////////////

void RequestHandler::operator()(Socket& R_)
{
	while (true)
	{
		std::string temp = R_.recvString();
		if (temp.size() != 0)
		{
			std::cout << "\nReceived Message\n" << temp << "\n";
			m_ = msgr.decoder(temp);
			if (m_.header.size() != 0)
			{
				r->pushmessage(m_);
			}
			std::string command = "default";
			if (funcMap.find(msgr.getvalue(m_, "command")) != funcMap.end())
				command = msgr.getvalue(m_, "command");
			if (command == "stop")
				break;
			if (call(command, m_,  R_))
				ApplicationHelpers::Verbose::show(command + " sucess", ApplicationHelpers::always);
			else
				ApplicationHelpers::Verbose::show(command + " failed", ApplicationHelpers::always);
		}
	}
}

bool RequestHandler::uploadHD(msg m, Socket& R_)
{
	std::string path = homedir+"/client" + ApplicationHelpers::toString(msgr.getsdrport(m));
	if (!FileSystem::Directory::exists(path))
		FileSystem::Directory::create(path);
	std::string filepath = path+"/"+msgr.getvalue(m, "filename");
	if (msgr.getvalue(m, "filetype") == "binary")
	{
		size_t  size = msgr.getfilesize(m);
		if (size == 0)
			return false;
		const size_t BufLen = 1024;
		Socket::byte* temp = new Socket::byte[BufLen];
		R_.recv(size, temp);
		if (size >= BufLen)
		{
			if (filemap.find(filepath) == filemap.end())
			{
				std::vector<std::pair<Socket::byte*, size_t>> x;
				x.push_back({ temp, size });
				filemap[filepath] = x;
			}
			else
				filemap[filepath].push_back({ temp, size });
		}
		else
		{
			FileSystem::File fileStream(filepath);
			if (fileStream.open(fileStream.out, fileStream.binary))
			{
				if (filemap.find(filepath) != filemap.end())
				{
					for (auto blocks : filemap[filepath])
					{
						fileStream.putBuffer(blocks.second, blocks.first);
						delete blocks.first;
					}
					filemap.erase(filepath);
				}
				fileStream.putBuffer(size, temp);
				delete temp;
				fileStream.flush();
				fileStream.close();
			}
		}
	}
	else
	{
		FileSystem::File fileStream(filepath);
		std::string content = R_.recvString();
		if (fileStream.open(fileStream.out, fileStream.text))
		{
			fileStream.putLine(content,false);
			fileStream.flush();
			fileStream.close();
		}
	}
	return true;
}

bool RequestHandler::messageHD(msg m, Socket& R_)
{
	return true;
}


#ifdef TEST_RECEIVER
#include"../Sender/Sender.h"
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
		Receiver sl(9070, Socket::IP6);
		RequestHandler hp;
		sl.start(hp);
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