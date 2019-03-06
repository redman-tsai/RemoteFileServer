#ifndef SERVER_H
#define SERVER_H
/////////////////////////////////////////////////////////////////////////
// Server.h -building server for socket communication                  //
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
This package has server class for build a server;
it has a sender and receiver
*/
#include <string>
#include <vector>
#include <iostream>
#include <set>
#include "../../MPC/Sender/Sender.h"
#include "../../MPC/Receiver/Receiver.h"
#include "../../MPC/Sockets/Sockets.h"
#include "../../MPC/MSG/MSG.h"
#include "../../XMLFileCataLoguer/XMLFileCataLoguer/FileManager/DataStore.h"
#include "../../XMLFileCataLoguer/XMLFileCataLoguer/FileManager/FileManager.h"
#include "../../XMLFileCataLoguer/XMLFileCataLoguer/XmlElement/XmlElement.h"
class server
{

public:
	using sPtr = DataStore::sPtr;
	server(const std::string& ip_, const size_t port_) : S(), RQ(), R(port_), ip(ip_), port(port_)
	{
		homedir = "./serverend" + ApplicationHelpers::toString(port_);
		if (!FileSystem::Directory::exists(homedir))
			FileSystem::Directory::create(homedir);
		S.setdir(homedir);
		msgr.setdir(homedir);
		R.setdir(homedir);
		RQ.setdir(homedir);
		RQ.linkreceiver(R);
		FileSystem::Directory::setCurrentDirectory(homedir);
		srcip = "localhost";
		root = XmlProcessing::makeTaggedElement("root");
		FileManager::BaseBuilder(Base, root, ".");
		FL = FileManager::FileList(Base);
	}

	void startusermode()
	{
		S.startsender();
		R.start(RQ);
		processmessage();
		std::string cmd;
		std::getline(std::cin, cmd);
		while (cmd != "stop" || ! terminated)
		{
			std::getline(std::cin, cmd);
		}
		terminate();
	}
	void start()
	{
		S.startsender();
		R.start(RQ);
		startprocess();
	}
	void quit()
	{
		while (true)
		{
			if (R.bytesWaiting() == 0)
				break;
		}
		S.postmessage(msgr.msgbuilder("connect", ip, port, ip, port));
		S.postmessage(msgr.msgbuilder("stop", ip, port, ip, port));
	}
	Receiver R;
	RequestHandler RQ;
private:
	void startprocess(){ t = std::thread([this] { this->processmessage(); }); t.detach(); }
	bool  nomessage;
	messenger msgr;
	std::string homedir;
	Sender S;
	std::string ip;
	size_t port;
	std::string srcip;
	size_t srcport;
	bool terminated;
	std::thread t;
	std::thread r;
	DataStore Base;
	sPtr root;
	FileManager::fileslist FL;
	void textsearch(msg& m)
	{
		std::set<std::string> pattern;
		std::string text;
		std::string path;
		std::string filelist;
		std::string xmlstring;
		bool requirexml = false;
		msgr.bodyparser(pattern, text, path, m);
		if (msgr.getvalue(m, "RequireXml") == "true")
			requirexml = true;
		size_t thrdn = stoi(msgr.getvalue(m, "ThreadNumber"));
		filelist = FileManager::multithreadtextsearch(root, xmlstring, FL,  text, thrdn, Base, pattern);
		msgr.addattri(m, "filelist", filelist);
		msgr.addattri(m, "filelistend", "here");
		if (requirexml)
		{
			msgr.addattri(m, "XmlList", xmlstring);
			msgr.addattri(m, "XmlListend", "here");
		}
	}

	void filesearch(msg& m)
	{
		std::set<std::string> pattern;
		std::string text;
		std::string path;
		std::string filelist;
		std::string folderlist;
		std::string xmlstring;
		bool requirexml = false;
		msgr.bodyparser(pattern, text, path, m);
		if (msgr.getvalue(m, "RequireXml") == "true")
			requirexml = true;
		FileManager::FileSearch(filelist, folderlist, xmlstring, pattern, path);
		msgr.addattri(m, "filelist", filelist);
		msgr.addattri(m, "filelistend", "here");
		msgr.addattri(m, "folderlist", folderlist);
		msgr.addattri(m, "folderlistend", "here");
		if (requirexml)
		{
			msgr.addattri(m, "XmlList", xmlstring);
			msgr.addattri(m, "XmlListend", "here");
		}
	}

	void processmessage()
	{
		while (true)
		{
			msg m_;
			m_ = R.popmessage();
			if (m_.header.size() == 0)
				continue;
			msg mm = msgr.responseMsg(m_);
			if (msgr.getdstip(mm) != srcip || msgr.getdstport(mm) != srcport)
			{
				srcip = msgr.getdstip(mm);
				srcport = msgr.getdstport(mm);
				msg m = msgr.msgbuilder("connect", srcip, srcport, ip, port);
				S.postmessage(m);
			}
			if (msgr.getvalue(mm, "command") == "filequery")
				filesearch(mm);
			if (msgr.getvalue(mm, "command") == "textsearch")
				textsearch(mm);
			S.postmessage(mm);
			if (msgr.getvalue(m_, "command") == "stop")
				break;
		}
	}
};

#endif