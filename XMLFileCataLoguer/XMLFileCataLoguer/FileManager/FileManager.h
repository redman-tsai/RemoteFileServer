#ifndef FILEMANAGER_H
#define FILEMANAGER_H
/////////////////////////////////////////////////////////////////////////////
// FileManager.h - file and directory search functions                     //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// copyright © Ruizhe CAI 2015                                             //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013                             //
// Platform:    ParallelDesktop10, Win8, MBP15                             //
// Application: Project1 OOD 2015                                          //
// Author:      Ruizhe CAI, 354520026 ,                                    //
//               rcai100@syr.edu     				                       //
/////////////////////////////////////////////////////////////////////////////
/*
Searching files based on user's input 
*/
#include "../FileSystem/FileSystem.h"
#include "DataStore.h"
#include <thread>
#include <chrono>
#include <future>
#include <stack>
#include "../XmlElement/XmlElement.h"
#include "../XmlDocument/XmlDocument.h"

class FileManager
{
public:
	using sPtr = DataStore::sPtr;
	using store = std::map < std::string, std::list<std::set<std::string>::iterator> > ;
	using fileslist = std::vector < std::string > ;
	static void filesearch(DataStore& DS, const std::string& dir , const std::string& txt , const std::set<std::string>& pattern , const bool rooted);
	static void refine(const store::iterator& bgn,  const store::iterator& edn, const std::string& txt, const std::set<std::string>& pattern);
	static std::string multithreadtextsearch(sPtr root, std::string& xmlstring, fileslist& fl, const std::string& txt, size_t threadnumber, DataStore& DS, const std::set<std::string>& pattern)
	{
		size_t thn = threadnumber;
		if (fl.size() < thn)
			thn = fl.size();
		if (thn > maxthn)
			thn = maxthn;
		std::vector<std::string*> cl(thn);
		std::vector<std::vector<std::pair<sPtr, sPtr>>*> cm(thn);
		for (size_t i = 0; i < thn; i++)
		{
			std::string* tt = new std::string;
			cl[i] = tt;
			std::vector<std::pair<sPtr, sPtr>>* cc = new std::vector<std::pair<sPtr, sPtr>>;
			cm[i] = cc;
		}
		std::thread t[maxthn];
		size_t avg = fl.size() / thn;
		size_t i;
		std::vector<size_t> start(thn);
		start[0] = 0;
		std::vector<size_t> end(thn);
		for (i = 0; i < thn - 1; i++)
		{
			end[i] = start[i] + avg;
			start[i + 1] = end[i];
			t[i] = std::thread(thrtxtsearch, start[i], end[i], fl, cl[i], cm[i],  txt, DS, pattern);
		} 
		t[thn - 1] = std::thread(thrtxtsearch, start[thn-1], fl.size(), fl, cl[i], cm[i], txt, DS, pattern);
		for (i = 0; i < thn; i++)
			t[i].join();
		std::string filelist;
		for (auto item : cl)
		{
			filelist += item->c_str();
			delete item;
		}
		if (filelist.empty())
			filelist += "No File Found\n";
		filelist += "No File Found";
		xmlstring = root->toString();
		for (auto item : cm)
		{
			for (auto node : *item)
			{
				sPtr tmp = node.first;
				tmp->removeChild(node.second);
			}
			delete item;
		}
		return filelist;
	}
	static fileslist FileList(DataStore& DS)
	{
		fileslist fl;
		store::iterator sit;
		std::list<std::set<std::string>::iterator>::iterator pit;
		std::string temp;
		for (sit = DS.begin(); sit != DS.end(); ++sit)
			for (pit = sit->second.begin(); pit != sit->second.end(); ++pit)
			{
				temp = FileSystem::Path::fileSpec(**pit, sit->first);
				fl.push_back(temp);
			}
		return fl;
	}
	static void BaseBuilder(DataStore& DS, sPtr node , const std::string& path = ".")
	{
		std::vector<std::string> DIRorFILE;
		std::string dir = path;
		std::string currentpath;
		std::stack<sPtr> nodes;
		sPtr temp;
		temp = node;
		std::stack<std::string> dirs;//use a stack to store unvisited dirs if /s provided
		dirs.push(dir);
		nodes.push(temp);
		while (!dirs.empty())
		{
			dir = dirs.top();
			temp = nodes.top();
			temp->addAttrib("Name", FileSystem::Path::getName(dir));
			DS.add(dir+"/", temp);
			DIRorFILE = FileSystem::Directory::getFiles(dir, "*.*");
			for (size_t i = 0; i < DIRorFILE.size(); i++)
				DS.save(DIRorFILE[i], dir);
			DIRorFILE = FileSystem::Directory::getDirectories(dir);
			dirs.pop();
			nodes.pop();
			if (DIRorFILE.size()>2)
			{
				for (size_t i = 2; i < DIRorFILE.size(); i++)
				{
					currentpath = dir;
					sPtr PATH = XmlProcessing::makeTaggedElement("Directory");
					temp->addChild(PATH);
					nodes.push(PATH);
					if (currentpath.find_last_of("/") != currentpath.size() - 1)
						currentpath.append("/");
					dirs.push(currentpath.append(DIRorFILE[i]));
				}
			}
		}
	}
	static void FileSearch(std::string& filelist, std::string& folderlist, std::string& xmlstring, const std::set<std::string>& pattern, const std::string& path = ".")
	{
		std::vector<std::string> DIRorFILE;
		std::string dir = path;
		std::string currentpath;
		sPtr node = XmlProcessing::makeTaggedElement("root");
		std::stack<sPtr> nodes;
		sPtr temp;
		temp = node;
		std::stack<std::string> dirs;//use a stack to store unvisited dirs if /s provided
		dirs.push(dir);
		nodes.push(temp);
		while (!dirs.empty())
		{
			dir = dirs.top();
			temp = nodes.top();
			temp->addAttrib("Name", FileSystem::Path::getName(dir));
			folderlist += dir + "\n";
			DIRorFILE = FileSystem::Directory::getFiles(dir, "*.*");
			for (size_t i = 0; i < DIRorFILE.size(); i++)
			{	
				if (patternsearch(FileSystem::Path::getName(DIRorFILE[i]), pattern))
				{
					filelist+=FileSystem::Path::fileSpec(dir, FileSystem::Path::getName(DIRorFILE[i]))+"\n";
					sPtr FILE = XmlProcessing::makeTaggedElement("File");
					FILE->addAttrib("Name", FileSystem::Path::getName(DIRorFILE[i]));
					temp->addChild(FILE);
				}
			}
			DIRorFILE = FileSystem::Directory::getDirectories(dir);
			dirs.pop();
			nodes.pop();
			if (DIRorFILE.size()>2)
			{
				for (size_t i = 2; i < DIRorFILE.size(); i++)
				{
					currentpath = dir;
					sPtr PATH = XmlProcessing::makeTaggedElement("Directory");
					temp->addChild(PATH);
					nodes.push(PATH);
					if (currentpath.find_last_of("/") != currentpath.size() - 1)
						currentpath.append("/");
					dirs.push(currentpath.append(DIRorFILE[i]));
				}
			}
		}
		node->removeAttrib("Name");
		node->addAttrib("Name", path);
		xmlstring = node->toString();
		filelist += "No File Found";
		folderlist += "No Folder Found";
	}
	static void addFileNamestoXML(const std::string& item, std::vector<std::pair<sPtr, sPtr>>* hold, DataStore& DS)
	{
		std::string temp;
		std::string name;
		sPtr node;
		temp = FileSystem::Path::getPath(item);
		node = DS.getnode(temp);
		if (node != nullptr)
		{
			sPtr f = XmlProcessing::makeTaggedElement("File");
			name = temp = FileSystem::Path::getName(item);
			f->addAttrib("Name", name);
			node->addChild(f);
			hold->push_back(std::pair<sPtr, sPtr>(node, f));
		}
	}
private:
	static void thrtxtsearch(size_t start, size_t end, fileslist& fl, std::string* str, std::vector<std::pair<sPtr, sPtr>>* hold, std::string& txt, DataStore& DS, const std::set<std::string>& pattern)
	{
		for (size_t i = start; i < end; i++)
		{
			if (textsearch(fl[i], txt) && patternsearch(FileSystem::Path::getName(fl[i]), pattern))
			{
				*str += fl[i] + "\n";
				addFileNamestoXML(fl[i],hold,  DS);
			}
		}
	}
	static bool textsearch(const std::string& filespec, const std::string& txt);
	static bool txtsearch(const std::string& name, const std::string& txt, const std::string& path);
	static bool patternsearch(const std::string& name, const std::set<std::string>& pattern);
	static const size_t maxthn = 10;
};
#endif

