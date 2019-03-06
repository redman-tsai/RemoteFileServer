#ifndef MSG_H
#define MSG_H
/////////////////////////////////////////////////////////////////////////
// MSG.h -messege struct and messenger class                           //
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
msg is the struct for meessage,
messenger is the interupter and analyser class
*/
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include "../../XMLFileCataLoguer/XMLFileCataLoguer/XmlDocument/XmlDocument.h"
//#define TEST_MSG
struct msg
{
	std::vector<std::string> header;
	std::string body;
};

class messenger
{
public:
	messenger()
	{
		response["default"] = "sendmessage";
		response["sendmessage"] = "receivemessage";
		response["upload"] = "receivefile";
		response["download"] = "upload";
		response["connect"] = "connect";
		response["close"] = "stop";
		response["stop"] = "close";
		response["filequery"] = "filequery";
		response["textsearch"] = "textsearch";
		dir = "./";
	}
	msg msgbuilder(const std::string& command, const std::string& dstip, int dstport, const std::string& srcip, int srcport)
	{
		msg m;
		m.header.push_back("command:" + command);
		m.header.push_back("dstaddr:" + dstip + '+' + toString(dstport));
		m.header.push_back("srcaddr:" + srcip + '+' + toString(srcport));
		return m;
	}
	bool addattri(msg& m_, const std::string& id, const std::string& value);
	std::string endoder(const msg& m_);
	void addbody(msg&m_, const std::string body){ m_.body = body; addattri(m_, "bodysize", toString(body.size())); }
	std::string getbody(const msg& m_){ return m_.body; }
	msg decoder(const std::string& str_);
	std::string getvalue(const msg& m_, const std::string& id);
	std::string getsdrip(const msg& m_);
	std::string getdstip(const msg& m_);
	void replacefilesize(msg& m_, size_t newsize);
	size_t getsdrport(const msg& m_);
	size_t getdstport(const msg& m_);
	size_t getfilesize(const msg& m_);
	size_t getbodysize(const msg& m_);
	msg responseMsg(const msg& m_);
	void bodyparser(std::set<std::string>& pattern, std::string& text, std::string& path,const msg& m_)
	{
		std::string body = m_.body;
		if (!body.empty())
		{
			XmlProcessing::XmlDocument x(body);
			XmlProcessing::XmlDocument::sPtr filter;
			XmlProcessing::XmlDocument y;
			std::vector<XmlProcessing::XmlDocument::sPtr> temp;
			temp = x.element("Filter").select();
			if (temp.size() > 0)
			{
				filter = temp.at(0);
				temp = filter->children();
			}
			if (temp.size() > 0)
			{
				for (auto item : temp)
				{
					if (item->tag() == "pattern")
					{
						std::shared_ptr<XmlProcessing::TextElement> comNode = std::dynamic_pointer_cast<XmlProcessing::TextElement>(item->children().at(0));
						if (comNode != nullptr)
							pattern.insert(comNode->value().substr(comNode->value().find_first_not_of(" \""), comNode->value().size() - 4));
						continue;
					}
					if (item->tag() == "text")
					{
						std::shared_ptr<XmlProcessing::TextElement> comNode = std::dynamic_pointer_cast<XmlProcessing::TextElement>(item->children().at(0));
						if (comNode != nullptr)
							text = comNode->value().substr(comNode->value().find_first_not_of(" \""), comNode->value().size() - 4);
						continue;
					}
					if (item->tag() == "path")
					{
						std::shared_ptr<XmlProcessing::TextElement> comNode = std::dynamic_pointer_cast<XmlProcessing::TextElement>(item->children().at(0));
						if (comNode != nullptr)
							path = comNode->value().substr(comNode->value().find_first_not_of(" \""), comNode->value().size() - 4);
						continue;
					}
				}
			}
		}
		if (pattern.empty())
			pattern.insert("*.*");
		if (path.empty())
			path = ".";
	}
	void clear(msg& m_)
	{
		m_.header.clear();
		m_.body.clear();
	}
	void setdir(const std::string& path)
	{
		dir = path;
	}
	void show(msg& m_)
	{
		std::vector<std::string> temp;
		for (auto item : m_.header)
			std::cout << '\n' << item;
		std::cout << '\n' << m_.body << std::endl;
	}

private:
	template<typename T>
	std::string toString(T t)
	{
		std::ostringstream out;
		out << t;
		return out.str();
	}
	bool fileexistance(const std::string& fn);
	bool textfile(const std::string& fn);
	void ipswap(msg& m_);
	std::string dir;
	std::vector<std::string> split(const std::string &s, char delim);
	std::map<std::string, std::string> response;
};

#endif
