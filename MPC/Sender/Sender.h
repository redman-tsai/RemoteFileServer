#ifndef SENDER_H
#define SENDER_H
/////////////////////////////////////////////////////////////////////////
// Sender.h -easy to use package to send socket                        //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Ruizhe CAI (c) copyright 2015                                       //
//354520026 rcai100@syr.edu                                            //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Parallel Desktop, MBP15  Win 8.1 pro                   //
/////////////////////////////////////////////////////////////////////////
#include<vector>
#include<thread>
#include<map>
#include<functional>
#include"../Sockets/Sockets.h"
#include<queue>
#include<string>
#include"../ApplicationHelpers/AppHelpers.h"
#include "../FileSystem/FileSystem.h"
#include"../MSG/MSG.h"
#include"../BlockingQueue/Cpp11-BlockingQueue.h"

//#define TEST_SENDER
class Sender :public SocketConnecter
{
public:
	typedef  void (Sender::*func)(msg& m_);
	Sender() :ss(), msgr(), SocketConnecter()
	{
		funcMap["upload"] = &Sender::upload;
		funcMap["download"] = &Sender::sendmsgbasic;
		funcMap["connect"] = &Sender::cnt;
		funcMap["sendmessage"] = &Sender::sendmsgbasic;;
		funcMap["textsearch"] = &Sender::sendmsgbasic;
		funcMap["fileqiery"] = &Sender::sendmsgbasic;
		funcMap["default"] = &Sender::sendmsgbasic;
		funcMap["stop"] = &Sender::sendmsgbasic;
		funcMap["close"] = &Sender::sendmsgbasic;
		funcMap["receivefile"] = &Sender::sendmsgbasic;
		funcMap["receivemessage"] = &Sender::sendmsgbasic;
	}
	virtual ~Sender(){}
	void postmessage(msg& m_){ msgqueue.enQ(m_); }
	void closeconnection(){ close(); connected = false; }
	bool connectto(const std::string& ip, size_t port){ if (connect(ip, port)) connected = true; else connected = false; return connected; }
	bool connectionstatus(){ return connected; }
	size_t queuesize(){ return msgqueue.size(); }
	void startsender(){ t = std::thread([this] { this->processmsg(); }); t.detach(); }
	void setdir(const std::string& path){ homedir = path;  msgr.setdir(path); }
	bool commandexists(const std::string& cmd)
	{
		if (funcMap.find(cmd) != funcMap.end())
			return true;
		return false;
	}
private:
	void processmsg();
	bool connected;
	void sendmsg(msg& m_);
	void upload(msg& m_);
	void cnt(msg& m_);
	void sendmsgbasic(msg& m_);
	void call(const std::string& s, msg& m_)
	{
		func fp = funcMap[s];
		ApplicationHelpers::Verbose::show("\nSender processed\n" + msgr.endoder(m_));
		std::cout << "\nSender sent\n";
		msgr.show(m_);
		return (this->*fp)(m_);
	}

	std::string homedir;
	std::mutex ioLock;
	messenger msgr;
	SocketSystem ss;
	BlockingQueue<msg> msgqueue;
	std::thread t;
	std::map<std::string, func>  funcMap;
};

class CommandProcesser
{
public:
	void call(msg& m_);

};

#endif