#ifndef CLIENT_H
#define CLIENT_H

/////////////////////////////////////////////////////////////////////////
// Client.h - build a client and receive command                       //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Ruizhe CAI (c) copyright 2015                                       //
//354520026 rcai100@syr.edu                                            //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Parallel Desktop, MBP15  Win 8.1 pro                   //
/////////////////////////////////////////////////////////////////////////
/*
Prologue:
This package has client class for build a client;
it has a sender and receiver
*/
#include <string>
#include <vector>
#include <iostream>
#include "../../MPC/BlockingQueue/Cpp11-BlockingQueue.h"
#include "../../MPC/Sender/Sender.h"
#include "../../MPC/Receiver/Receiver.h"
#include "../../MPC/Sockets/Sockets.h"
#include "../../MPC/MSG/MSG.h"
#include <queue>
#include <map>
#include<set>

class client
{
public:
	client(const std::string& ip_, const size_t port_) : S(), RQ(), R(port_), ip(ip_), port(port_)
	{
		homedir = "./clientend" + ApplicationHelpers::toString(port_);
		if (!FileSystem::Directory::exists(homedir))
			FileSystem::Directory::create(homedir);
		S.setdir(homedir);
		msgr.setdir(homedir);
		R.setdir(homedir);
		RQ.linkreceiver(R);
		RQ.setdir(homedir);
		dstip = "localhost";
		stop_ = false;
	}
	size_t guismsgavail()
	{
		return GuiIn.size();
	}
	void messagebuilder(const std::string& command)
	{
		m_ = msgr.msgbuilder(command, dstip, dstport, ip, port);
		nomessage = false;
	}
	void connect(const std::string& port_)
	{
		dstport = stoi(port_);
		S.postmessage(msgr.msgbuilder("connect", dstip, dstport, ip, port));
	}
	void postmessage()
	{
		S.postmessage(m_);
		msgr.clear(m_);
		nomessage = true;
	}


	void addbody(const std::string& body)
	{
		msgr.addbody(m_, body);
	}
	void addatt(const std::string& id, const std::string& value)
	{
		msgr.addattri(m_, id, value);
	}
	void start(bool inputmode)
	{
		S.startsender();
		R.start(RQ);
		if (inputmode)
		{
			startguimode();
			startreplygui();
		}
	}

	void postGuiCmd(const std::string& cmd)
	{
		GuiOut.enQ(cmd);
	}

	std::string popResult( )
	{
		return(GuiIn.deQ());
	}
	void quit()
	{
		connect(ApplicationHelpers::toString(port));
		messagebuilder("stop");
		postmessage();
		messagebuilder("close");
		stop_ = true;
	}
private:
	std::string homedir;
	msg m_;
	bool  nomessage;
	messenger msgr;
	messenger guimsgr;
	messenger replyguimsgr;
	Sender S;
	Receiver R;
	RequestHandler RQ;
	std::string ip;
	size_t port;
	std::string dstip;
	size_t dstport;
	size_t guirequestedport;
	std::thread t;
	std::thread r;
	bool stop_;
	BlockingQueue<std::string> GuiIn;
	BlockingQueue<std::string> GuiOut;
	std::set< size_t > TextQuerySend;
	std::vector< std::string > TextQueryResult;
	std::vector <std::string> TextXml;


	std::vector<std::string> split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)){
			elems.push_back(item);
		}
		return elems; 
	}
	void startreplygui(){r = std::thread([this] { this->replygui(); }); r.detach();	}
	void startguimode(){ t = std::thread([this] { this->commandgui(); }); t.detach(); }

	void commandgui()
	{
		std::string guim;
		while (!stop_)
		{

			if (GuiOut.size() != 0)
			{
				guim = GuiOut.deQ();
				guicommandparser(guim);
			}
		}
	}

	void replygui()
	{
		msg rm;
		while (!stop_)
		{
			if (R.sizewaiting() != 0)
			{
				rm = R.popmessage();
				replyparser(rm);
			}
		}
	}

	void sendmsg(const msg& m)
	{
		m_ = m;
		postmessage();
	}

	void guicommandparser(const std::string& cmd)
	{
		msg m = guimsgr.decoder(cmd);
		if (guimsgr.getvalue(m, "command") == "download")
			downloadhelper(m);
		else
		{
			if (guimsgr.getvalue(m, "command") == "upload")
				uploadhelper(m);
			else
			{
				if (guimsgr.getvalue(m, "command") == "textsearch")
					textsearch(m);
				else 
				{
					if (guimsgr.getvalue(m, "command") == "connect")
					{
						guirequestedport = guimsgr.getdstport(m);
						dstport = guirequestedport;
						sendmsg(m);
					}
					else
						if (guimsgr.getvalue(m, "command") == "quit")
						{
							quit();
						}
						else
							sendmsg(m);
				}
			}
		}
	}

	void replyparser(const msg& m)
	{
		messenger rpm;
		std::string command = rpm.getvalue(m, "command");
		if (command == "receivefile")
			receivefilehelper(m);
		else
		{
			if (command == "textsearch")
				queryreplyhelper(m);
			else
			{
				if (command == "upload")
					downloadreplyhelper(m);
				else
					if (command == "connect")
						connecttedreplyhelper(m);
					else
					{
						if (command == "filequery")
						{
							GuiIn.enQ(rpm.endoder(m));
						}
					}
			}
		}
	}

	void textsearch(const msg& m)
	{
		std::vector<msg> multim;
		std::string cmd = guimsgr.getvalue(m, "command");
		std::vector<std::string> multiends;
		std::string body = guimsgr.getbody(m);
		std::string thr = guimsgr.getvalue(m, "ThreadNumber");
		std::string RequireXml = guimsgr.getvalue(m, "RequireXml");
		multiends = split(guimsgr.getvalue(m, "multiends"),'\n');
		for (size_t i = 0;  i < multiends.size() - 1; i++)
		{
			std::string item = multiends[i];
			msg newm = guimsgr.msgbuilder(cmd, dstip, stoi(item), ip, port);
			guimsgr.addattri(newm, "ThreadNumber", thr);
			guimsgr.addattri(newm, "RequireXml", RequireXml);
			connect(item);
			sendmsg(newm);
			TextQuerySend.insert(stoi(item));
		}
	}

	void uploadhelper(const msg& m)
	{
		std::string cmd = guimsgr.getvalue(m, "command");
		std::string filename = guimsgr.getvalue(m, "filename");
		msg newm = guimsgr.msgbuilder(cmd, dstip, dstport, ip, port);
		guimsgr.addattri(newm, "HasPath", "true");
		guimsgr.addattri(newm, "filename", filename);
		size_t filesz = guimsgr.getfilesize(newm);
		sendmsg(newm);
	}

	void downloadhelper(const msg& m)
	{
		size_t port_ = msgr.getdstport(m);
		if (port_ != dstport)
			connect(ApplicationHelpers::toString(port_));
		sendmsg(m);
	}
   
	void receivefilehelper(const msg& m)
	{
		size_t filesz = replyguimsgr.getfilesize(m);
		std::string filename = replyguimsgr.getvalue(m, "filename");
		if (filesz < 1024)
		{
			GuiIn.enQ(replyguimsgr.endoder(m));
		}
	}

	void queryreplyhelper(const msg& m)
	{
		size_t sender = replyguimsgr.getsdrport(m);
		TextQuerySend.erase(sender);
		std::vector<std::string> temp = split(replyguimsgr.getvalue(m, "filelist"), '\n');
		for (size_t i = 0; i < temp.size() - 1; i++)
		{
			TextQueryResult.push_back(sender + "@:" + temp[i]);
		}
		if (replyguimsgr.getvalue(m, "RequireXml") == "true")
		{
			TextXml.push_back(ApplicationHelpers::toString(sender) + "\n" + replyguimsgr.getvalue(m, "XmlList") + "\n");
		}
		if (TextQuerySend.empty())
		{
			std::string tmp = TextQueryResult[0];
			for (size_t i = 1; i < TextQueryResult.size(); i++)
				tmp += TextQueryResult[i];
			msg mm = replyguimsgr.msgbuilder("textsearch", ip, port, dstip, dstport);
			replyguimsgr.addattri(mm, "filelist", tmp);
			if (replyguimsgr.getvalue(m, "RequireXml") == "true")
			{
				tmp = "";
				for (auto items : TextXml)
					tmp += items;
				replyguimsgr.addattri(mm, "XmlList", tmp);
			}
			GuiIn.enQ(replyguimsgr.endoder(mm));
		}
	}

	void downloadreplyhelper(const msg& m)
	{
		size_t filesz = replyguimsgr.getfilesize(m);
		std::string filename = replyguimsgr.getvalue(m, "filename");
		if (filesz < 1024)
			GuiIn.enQ(replyguimsgr.endoder(m));
	}

	void connecttedreplyhelper(const msg& m)
	{
		size_t p = replyguimsgr.getsdrport(m);
		if (p == guirequestedport)
		{
			GuiIn.enQ(replyguimsgr.endoder(m));
			guirequestedport = 0000;
		}
			
	}


};

#endif
