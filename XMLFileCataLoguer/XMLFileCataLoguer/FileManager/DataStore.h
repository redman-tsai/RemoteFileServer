#ifndef DATASTORE_H
#define DATASTORE_H
/////////////////////////////////////////////////////////////////////////////
// DataStore.h - Storage class for file name and path storage              //
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
Storage class for path and name data storage, with certain functions.
*/
#include <set>
#include <map>
#include <list>
#include <string>
#include "../XmlElement/XmlElement.h"
#include "../XmlDocument/XmlDocument.h"
class DataStore
{
public:
	using Path = std::string;
	using Paths = std::set < Path >;
	using PathIter = Paths::iterator;
	using ListOfIters = std::list < PathIter >;
	using File = std::string;
	using Store = std::map < File, ListOfIters >;
	using iterator = Store::iterator;
	using sPtr = XmlProcessing::XmlDocument::sPtr;
	using PXml = std::map < Path, sPtr > ;


	void save(const std::string& filesspec);
	void save(const std::string& filename, const std::string& path);
	void add(const std::string& path, sPtr node)
	{
		Pmap[path] = node;
	}
	sPtr getnode(std::string& path)
	{
		if (Pmap.find(path)!=Pmap.end())
			return Pmap[path];
		return nullptr;
	}
	iterator begin() { return store.begin(); }
	iterator end() { return store.end(); }
private:
	Store store;
	Paths paths;
	PXml Pmap;
};
#endif
