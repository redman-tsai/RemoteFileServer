#ifndef RECEIVER_H
#define RECEIVER_H
/////////////////////////////////////////////////////////////////////////
// Receiver.h -easy to use package t receive socket                    //
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
Receiver has queue for incoming message
Requesthandler is the function to handle request
*/
#include<vector>
#include<thread>
#include<functional>
#include<map>
#include"../Sockets/Sockets.h"
#include<queue>
#include<string>
#include"../ApplicationHelpers/AppHelpers.h"
#include "../FileSystem/FileSystem.h"
#include"../MSG/MSG.h"
#include"../BlockingQueue/Cpp11-BlockingQueue.h"

class Receiver : public SocketListener
{
public:
	Receiver(size_t prt, Socket::IpVer ip = Socket::IP6) :ss(), msgr(), msgqueue(), SocketListener(prt, ip){ }
	Receiver(const Receiver& R) = delete;
	Receiver(Receiver&& R) : ss(), msgr(), msgqueue(), SocketListener(std::move(R)){ }
	virtual ~Receiver(){}
	size_t sizewaiting(){ return msgqueue.size(); }
	msg popmessage()
	{
		if (msgqueue.size() != 0)
			return (msgqueue.deQ()); 
		msg m; 
		msgr.clear(m); 
		return m; 
	}
	void pushmessage(msg& m_){ msgqueue.enQ(m_); }
	void setdir(const std::string& path)
	{
		homedir = path;
		msgr.setdir(path);
	}
private:
	std::string homedir;
	messenger msgr;
	SocketSystem ss;
	BlockingQueue<msg> msgqueue;
	std::thread t;
};


class RequestHandler
{
	typedef bool(RequestHandler::*func)(msg m, Socket& R_);
public:
	RequestHandler()
	{

		funcMap["upload"] = &RequestHandler::uploadHD;
		funcMap["sendmessage"] = &RequestHandler::messageHD;
		funcMap["default"] = &RequestHandler::messageHD;
		funcMap["filequery"] = &RequestHandler::messageHD;
		funcMap["textsearch"] = &RequestHandler::messageHD;
	}
	void operator()(Socket& R_);
	void linkreceiver(Receiver& R_){ r = &R_; }
	void setdir(const std::string& path)
	{
		homedir = path;
		msgr.setdir(path);
	}
private:
	bool uploadHD(msg m, Socket& R_);
	bool messageHD(msg m, Socket& R_);

	bool call(const std::string& s, msg& m, Socket& R_)
	{
		func fp = funcMap[s];
		return ((this->*fp)(m, R_));
			
	}
	Receiver* r;
	messenger msgr;
	std::string homedir;
	msg m_;
	std::mutex meslock;
	std::map<std::string, func>  funcMap;
	std::map<std::string, std::vector<std::pair<Socket::byte*, size_t>>> filemap;
};




#endif

