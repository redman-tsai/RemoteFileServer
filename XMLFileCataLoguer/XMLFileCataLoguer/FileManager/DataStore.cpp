/////////////////////////////////////////////////////////////////////////////
// DataStore.cpp - Storage class for file name and path storage            //
// ver 1.0                                                                 //
// ----------------------------------------------------------------------- //
// copyright © Ruizhe CAI 2015                                             //
// All rights granted provided that this notice is retained                //
// ----------------------------------------------------------------------- //
// Language:    Visual C++, Visual Studio 2013                             //
// Platform:    ParallelDesktop10, Win8, MBP15                             //
// Application: Project1 OOD 2015                                          //
// Author:      Ruizhe CAI, 354520026,                                     //
//               rcai100@syr.edu     				                       //
/////////////////////////////////////////////////////////////////////////////

#include "DataStore.h"
#include "../FileSystem/FileSystem.h"
#include <map>
#include <list>
#include <set>
#include <string>
#include <iostream>
//#define TEST_DATASTORE
//save to storage based on filespec
void DataStore::save(const std::string& filespec)
{
	std::string path;
	std::string filename;
	path = FileSystem::Path::getPath(filespec).c_str();
	filename = FileSystem::Path::getName(filespec).c_str();
	PathIter Ptmp;
	Ptmp = paths.find(path);
	if (Ptmp == paths.end())
	{
		paths.insert(path);
		Ptmp = paths.find(path);
	}
	if (store.find(filename) == store.end())
	{
		ListOfIters tmp;
		store.insert(std::pair<File, ListOfIters>(filename, tmp));
	}
	store[filename].push_back(Ptmp);
}

//save to storage based on filename and path
void DataStore::save(const std::string& filename, const std::string& path)
{
	PathIter Ptmp;
	Ptmp = paths.find(path.c_str());
	if (Ptmp == paths.end())
	{
		paths.insert(path.c_str());
		Ptmp = paths.find(path.c_str());
	}
	if (store.find(filename.c_str()) == store.end())
	{
		ListOfIters tmp;
		store.insert(std::pair<File, ListOfIters>(filename.c_str(), tmp));
	}
	store[filename.c_str()].push_back(Ptmp);
}

//----< test stub >--------------------------------------------------------
#ifdef TEST_DATASTORE
void title(const std::string& title, char ch = '=')
{
	std::cout << "\n  " << title;
	std::cout << "\n " << std::string(title.size() + 2, ch);
}

void show(DataStore& DS)
{
	for(std::map<std::string, std::list<std::set<std::string>::iterator>>::iterator nit=DS.begin(); nit!=DS.end(); ++nit)
	{
		std::cout<<"\nFile name is\n"<<nit->first<<"\n\nPaths are";
		for (std::list<std::set<std::string>::iterator>::iterator pit=nit->second.begin(); pit!=nit->second.end(); ++pit)
			std::cout<<"\n"<<**pit;
	}
	std::cout << std::endl;
}

int main(int argc, char* argv[])
{
	DataStore DS;
	title ("Demonstrate Datastore CLASS");
	title("saving ./temp.exe");
	std::string path="./";
	std::string name="temp.exe";
	DS.save(name,path);
	show(DS);
	title("saving ./temp/temp.exe");
	std::string sp="./temp/temp.exe";
	DS.save(sp);
	show(DS);
	title("saving ./temp/a/temp.exe");
	path = "./temp/a";
	DS.save(name, path);
	show(DS);
	system("pause");
}
#endif
