/////////////////////////////////////////////////////////////////////////
// MSG.cpp -defining functions in messenger                           //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Ruizhe CAI (c) copyright 2015                                       //
//354520026 rcai100@syr.edu                                            //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Projects #3, #4                                    //
// Platform:    Parallel Desktop, MBP15  Win 8.1 pro                   //
/////////////////////////////////////////////////////////////////////////
#include "MSG.h"
#include <sstream>
#include <iostream>
#include "../FileSystem/FileSystem.h"
#include <stdio.h>

bool messenger::addattri(msg& m_, const std::string& id, const std::string& value)
{
	if (id == "filename" && getvalue(m_, "command") == "upload")
	{
		std::string filespec = dir + "/" + value;
		if (getvalue(m_, "HasPath") == "true")
			filespec = value;
		if (!fileexistance(filespec))
			return false;
		FileSystem::FileInfo f(filespec);
		if (!textfile(filespec))
		{
			m_.header.push_back(id + ":" + value);
			m_.header.push_back("filetype:binary");
			m_.header.push_back("filesize:" + toString(f.size()));
			return true;
		}
		m_.header.push_back(id + ":" + value);
		m_.header.push_back("filetype:text");
		m_.header.push_back("filesize:" + toString(f.size()));
		return true;
	}
	m_.header.push_back(id + ":" + value);
	return true;
}

bool messenger::fileexistance(const std::string& fn)
{
	FileSystem::File f(fn);
	return(f.exists(fn));
}

bool messenger::textfile(const std::string& fn)
{
	int c;
	std::ifstream a(fn.c_str());
	while ((c = a.get()) != EOF && c <= 127)
		;
	return (c == EOF);
}

std::vector<std::string> messenger::split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)){
		elems.push_back(item);
	}
	return elems;
}

std::string messenger::endoder(const msg& m_)
{
	std::string dm="";
	for (auto item : m_.header)
		dm = dm + item + "\n";
	if (m_.body.size() != 0)
		dm = dm + "body:" + m_.body + '\0';
	return dm;
}

msg messenger::decoder(const std::string& s)
{
	msg m_;
	m_.header = split(s, '\n');
	std::string temp = m_.header.at(m_.header.size() - 1);
	if (split(temp,  ':').at(0) != "body")
		return m_;
	m_.body = split(temp, ':').at(1);
	m_.header.erase(m_.header.end() - 1);
	return m_;
}

std::string messenger::getsdrip(const msg& m_)
{
	std::string ip = "";
	std::vector<std::string> temp;
	for (auto item : m_.header)
	{
		temp = split(item, ':');
		if (temp.size() == 2)
		{
			if (temp[0] == "srcaddr")
				ip = split(temp[1],'+').at(0);
		}
	}
	return ip;
}

std::string messenger::getdstip(const msg& m_)
{
	std::string ip = "";
	std::vector<std::string> temp;
	for (auto item : m_.header)
	{
		temp = split(item, ':');
		if (temp.size() == 2)
		{
			if (temp[0] == "dstaddr")
				ip = split(temp[1], '+').at(0);
		}
	}
	return ip;
}

size_t messenger::getsdrport(const msg& m_)
{
	size_t port = 0;
	std::vector<std::string> temp;
	for (auto item : m_.header)
	{
		temp = split(item, ':');
		if (temp.size() == 2)
		{
			if (temp[0] == "srcaddr")
				port = stoi(split(temp[1], '+').at(1));
		}
	}
	return port;
}

size_t messenger::getdstport(const msg& m_)
{
	size_t port = 0;
	std::vector<std::string> temp;
	for (auto item : m_.header)
	{
		temp = split(item, ':');
		if (temp.size() == 2)
		{
			if (temp[0] == "dstaddr")
				port = stoi(split(temp[1], '+').at(1));
		}
	}
	return port;
}


std::string messenger::getvalue(const msg& m_, const std::string& id)
{
	std::string t = " ";
	std::vector<std::string> temp;
	for (auto item : m_.header)
	{
		temp = split(item, ':');
		if (temp.size() == 2)
		{
			if (temp[0] == id)
			{
				t = temp[1];
				break;
			}
		}
	}
	if (id == "filesize"||id == "bodysize")
		std::cout << "\nReturning int in stirng, please use get size function" << std::endl;
	if (t == " ")
		std::cout << "\nReturning blank string, id not found" << std::endl;
	return t;
}

size_t messenger::getfilesize(const msg& m_)
{
	size_t size = 0;
	std::vector<std::string> temp;
	if (m_.header.size() == 0) return size;
	for (auto item : m_.header)
	{
		temp = split(item, ':');
		if (temp.size() == 2)
		{
			if (temp[0] == "filesize")
				size = stoi(temp[1]);
		}
	}
	return size;
}

size_t messenger::getbodysize(const msg& m_)
{
	size_t size = 0;
	std::vector<std::string> temp;
	for (auto item : m_.header)
	{
		temp = split(item, ':');
		if (temp.size() == 2)
		{
			if (temp[0] == "bodysize")
				size = stoi(temp[1]);
		}
	}
	return size;
}

void messenger::replacefilesize(msg& m_, size_t newsize)
{
	std::vector<std::string> temp;
	size_t i;
	for (i= 0; i < m_.header.size(); i++)
	{
		temp = split(m_.header.at(i), ':');
		if (temp.size() == 2)
		{
			if (temp[0] == "filesize")
				break;
		}
	}
	m_.header.at(i)= "filesize:" + toString(newsize);
}

msg messenger::responseMsg(const msg& m_)
{
	msg mm = m_;
	std::string tmp = mm.header[1];
	mm.header[1] = split(mm.header[1], ':').at(0) + ':' + split(mm.header[2], ':').at(1);
	mm.header[2] = split(mm.header[2], ':').at(0) + ':' + split(tmp, ':').at(1);
	std::string cmd = mm.header.at(0);
	std::vector<std::string> temp = split(cmd, ':');
	if (response.find(temp[1]) == response.end())
		temp[1] = "default";
	else
		temp[1] = response[temp[1]];
	mm.header[0] = temp[0] + ':' + temp[1];
	if (getvalue(mm, "command") == "upload")
	{
		size_t fileloc = 0;
		for (fileloc = 0; fileloc < mm.header.size(); fileloc++)
			if (split(mm.header.at(fileloc), ':').at(0) == "filename")
				break;
		if (fileloc != 0)
		{
			std::string filename = split(mm.header.at(fileloc), ':').at(1);
			mm.header.erase(mm.header.begin()+fileloc);
			addattri(mm, "filename", filename);
		}
	}
	return mm;
}

#ifdef TEST_MSG
int main()
{
	messenger msgr;
	msg x;
	x = msgr.msgbuilder("upload", "localhost", 9080, "localhost", 9060);
	msgr.addattri(x, "filename", "a.png");
	msgr.replacefilesize(x, 1024);
	msgr.show(x);
	std::cout << msgr.getvalue(x, "command")<<'\n';
	std::cout << msgr.getvalue(x, "filename") << '\n';
	std::cout << msgr.getfilesize(x) << '\n';
	std::cout << msgr.getdstport(x) << '\n';
	std::cout << msgr.endoder(x) << std::endl;
	msgr.show(msgr.decoder(msgr.endoder(x)));
	x = msgr.msgbuilder("upload", "localhost", 9080, "localhost", 9060);
	msgr.addattri(x, "filename", "jjj.txt");
	msgr.show(x);
	std::cout << msgr.getvalue(x, "command") << '\n';
	std::cout << msgr.getvalue(x, "filename") << '\n';
	std::cout << msgr.getfilesize(x) << '\n';
	std::cout << msgr.getdstport(x) << '\n';
	std::cout << msgr.endoder(x) << std::endl;
	msgr.show(msgr.decoder(msgr.endoder(x)));
	system("pause");
	return 0;
}
#endif

