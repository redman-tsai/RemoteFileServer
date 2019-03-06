/////////////////////////////////////////////////////////////////////////////
// FileManager.cpp - file and directory search functions                     //
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
#include <stack>
#include <iostream>
#include <vector>
#include "FileManager.h"
#include "../FileSystem/FileSystem.h"
#include "../XmlElement/XmlElement.h"
#include "../XmlDocument/XmlDocument.h"

// if file meets patterns requirements (for refine search in catalogue)
bool FileManager::patternsearch(const std::string& name, const std::set<std::string>& pattern = { "*.*" })
{
	if (pattern.find("*.*") != pattern.end())
		return true;
	else
	{
		std::string fr = "*.";
		std::string ext;
		ext = FileSystem::Path::getExt(name);
		if (pattern.find(fr.append(ext)) != pattern.end())
		{
			return true;
		}
		return false;
	}
}
//if file meets txt search reqs
bool FileManager::txtsearch(const std::string& name, const std::string& txt = "", const std::string& path = ".")
{
	if (txt.empty())
		return true;
	else
	{
		std::string filespec;
		filespec = FileSystem::Path::getFullFileSpec(FileSystem::Path::fileSpec(path, name));
		FileSystem::File F = FileSystem::File::File(filespec);
		F.open(F.in,F.text);
		while (true)
		{
			if (!F.isGood())
				return false;
			if (F.getLine(true).find(txt) != std::string::npos)
				return true;
		}
	}
}

bool FileManager::textsearch(const std::string& filespec, const std::string& txt)
{
	if (txt.empty())
		return true;
	else
	{
		FileSystem::File F = FileSystem::File::File(filespec);
		if (F.open(F.in, F.text))
		{
			while (true)
			{
				if (!F.isGood())
					return false;
				if (F.getLine(true).find(txt) != std::string::npos)
					return true;
			}
		}
	}
	return false;
}
//file search and save//
void FileManager::filesearch(DataStore& DS, const std::string& path = ".", const std::string& txt = "", const std::set<std::string>& pattern = { "*.*" }, const bool rooted = false)
{
	std::vector<std::string> DIRorFILE;
	std::string dir = path;
	std::string currentpath;
	std::stack<std::string> dirs;//use a stack to store unvisited dirs if /s provided
	dirs.push(dir);
	while (!dirs.empty())
	{
		dir = dirs.top();
		for (std::set<std::string>::iterator it = pattern.begin(); it != pattern.end(); ++it)
		{
			DIRorFILE = FileSystem::Directory::getFiles(dir, *it);
			for (size_t i = 0; i < DIRorFILE.size(); i++)
			{
				if (txtsearch(DIRorFILE[i], txt, dir) == true)
					DS.save(DIRorFILE[i], dir);
			}
		}
		DIRorFILE = FileSystem::Directory::getDirectories(dir);
		dirs.pop();
		if (rooted == true)
		{
			if (DIRorFILE.size()>2)
			{
				for (size_t i = 2; i < DIRorFILE.size(); i++)
				{
					currentpath = dir;
					if (currentpath.find_last_of("/") != currentpath.size() - 1)
						currentpath.append("/");
					dirs.push(currentpath.append(DIRorFILE[i]));
				}
			}
		}
	}
}

//refine search and display for refine search upon catalogue
void FileManager::refine(const store::iterator& bgn, const store::iterator& edn, const std::string& txt, const std::set<std::string>& pattern)
{
	bool found = false;
	store::iterator nameit;
	std::list < std::set< std::string >::iterator > ::iterator refsit;
	for (nameit = bgn; nameit != edn; ++nameit)
	{
		for (refsit = nameit->second.begin(); refsit != nameit->second.end(); ++refsit)
		{
			if ((txt.empty()) || (!txt.empty() && (txtsearch(nameit->first, txt, **refsit))))
			{
				if ((pattern.find("*.*") != pattern.end()) || ((pattern.find("*.*") == pattern.end()) && (patternsearch(nameit->first, pattern))))
				{
					std::cout << "\n" << FileSystem::Path::getFullFileSpec(FileSystem::Path::fileSpec(**refsit, nameit->first));
					found = true;
				}
			}
		}
	}
	if (found = false)
		std::cout << "No File Found";
	std::cout << std::endl;
}


#ifdef TEST_FILEMANAGER
//----< test stub >--------------------------------------------------------
void title(const std::string& title, char ch = '=')
{
	std::cout << "\n  " << title;
	std::cout << "\n " << std::string(title.size() + 2, ch);
}
void show(DataStore& DS)
{
	for (std::map<std::string, std::list<std::set<std::string>::iterator>>::iterator nit = DS.begin(); nit != DS.end(); ++nit)
	{
		std::cout << "\n\nFile name is\n" << nit->first << "\nPaths are";
		for (std::list<std::set<std::string>::iterator>::iterator pit = nit->second.begin(); pit != nit->second.end(); ++pit)
			std::cout << "\n" << **pit;
	}
	std::cout << std::endl;
}
int main(int argc, char* argv[])
{
	/*title("DEMONSTRATE FILEMANAGER");
	DataStore DS;
	FileManager::filesearch(DS, ".", "", { "*.*" }, true);
	std::cout << "\nlisting all files rooted in current folder" << std::endl;
	show(DS);
	title("refine function test for all files containing line");
	FileManager::refine(DS.begin(),DS.end(), "line", { "*.*" });
	system("pause");
	FileManager::fileslist temp;
	std::string x;
	temp = FileManager::FileList();
	x=FileManager::multithreadtextsearch(temp, "", 5);
	std::cout << x;
	system("pause");*/
	FileManager::fileslist temp1;
	std::string x;
	std::string xmlstring;
	DataStore temp;
	DataStore::sPtr node = XmlProcessing::makeTaggedElement("Root");
	FileManager::BaseBuilder(temp, node);
	temp1 = FileManager::FileList(temp);
	x = FileManager::multithreadtextsearch(node, xmlstring, temp1, "hello", 5, temp, {"*.txt*"});
	
	
	std::cout << node->toString();
	std::cout << "\n\n\n";
	std::string fl, ffl;
	FileManager::FileSearch(fl, ffl, xmlstring, {"*.txt"}, "./Test");
	std::cout << xmlstring;
	system("pause");
}
#endif


