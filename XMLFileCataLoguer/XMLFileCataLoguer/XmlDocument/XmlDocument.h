#ifndef XMLDOCUMENT_H
#define XMLDOCUMENT_H
///////////////////////////////////////////////////////////////////
// XmlDocument.h - a container of XmlElement nodes               //
// Ver 1.3                                                       //
// Application: CSE687 Pr#2, Spring 2015                         //
// Platform:    ParallelDesktop10, Win8, MBP15                   //
// Author:      Ruizhe CAI, 354520026 ,                          //
//               rcai100@syr.edu                                 //
///////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is intended to help students in CSE687 - Object Oriented Design
* get started with Project #2 - XML Document Model.  It uses C++11 constructs,
* most noteably std::shared_ptr.  The XML Document Model is essentially
* a program-friendly wrapper around an Abstract Syntax Tree (AST) used to
* contain the results of parsing XML markup.
*
* Abstract Syntax Trees, defined in this package, are unordered trees with 
* two types of nodes:
*   Terminal nodes     - nodes with no children
*   Non-Terminal nodes - nodes which may have a finite number of children
* They are often used to contain the results of parsing some language.
*
* The elements defined in the companion package, XmlElement, will be used in
* the AST defined in this package.  They are:
*   AbstractXmlElement - base for all the XML Element types
*   DocElement         - XML element with children designed to hold prologue, Xml root, and epilogue
*   TaggedElement      - XML element with tag, attributes, child elements
*   TextElement        - XML element with only text, no markup
*   CommentElement     - XML element with comment markup and text
*   ProcInstrElement   - XML element with markup and attributes but no children
*   XmlDeclarElement   - XML declaration element with attributes
*
* Required Files:
* ---------------
*   - XmlDocument.h, XmlDocument.cpp, 
*     XmlElement.h, XmlElement.cpp
*
* Build Process:
* --------------
*   devenv AST.sln /debug rebuild
*
* Maintenance History:
* --------------------
* ver 1.3 : 31 Mar 15
* - complete package
* ver 1.2 : 21 Feb 15
* - modified these comments
* ver 1.1 : 14 Feb 15
* - minor changes to comments, method arguments
* Ver 1.0 : 11 Feb 15
* - first release
*/

#include <memory>
#include <string>
#include "../XmlElement/XmlElement.h"

namespace XmlProcessing
{
  class XmlDocument
  {
  public:
    using sPtr = std::shared_ptr < AbstractXmlElement > ;

    enum sourceType { string, filename };
	enum elementType{ comment, Doc, Proc, Tagged, Text, Declare, TaggedSelfColosed, closingtag };
	enum returntype { first, all};

	XmlDocument();
    XmlDocument(const std::string& src, sourceType srcType=string);
	XmlDocument(const sPtr root_, const std::vector<sPtr>& fd=std::vector<sPtr>()){ pDocElement_ = root_; found_ = fd; }
	XmlDocument(const XmlDocument&) = delete;
	XmlDocument& operator=(const XmlDocument&) = delete;
	XmlDocument(XmlDocument &&doc);
	XmlDocument& operator=(XmlDocument &&doc);
    
    // queries return XmlDocument references so they can be chained, e.g., doc.element("foobar").descendents();

    XmlDocument& element(const std::string& tag);           // found_[0] contains first element (DFS order) with tag
    XmlDocument& elements(const std::string& tag);          // found_ contains all elements with tag
    XmlDocument& children(const std::string& tag = " ");     // found_ contains sPtrs to children of prior found_[0] 
    XmlDocument& descendents(const std::string& tag = " ");  // found_ contains sPtrs to descendents of prior found_[0]
    std::vector<sPtr> select();                            // return reference to found_.  Use std::move(found_) to clear found_
	sPtr findByID(const std::pair<std::string, std::string>& attr); // return sptr to element posses such id and value;
	std::vector<std::pair<std::string, std::string>> getAttri(sPtr element){ return element->attributes(); } // return  attributes;
	sPtr makeXmlElement(std::string& xmlpart);// make node based on xmlstring;
	std::string WriteToString();
	void WriteToFile(const std::string& path="../test.xml");
	bool settagroot(const sPtr root_);
	bool addChild(const sPtr parent, const sPtr child);
	bool addChild(const sPtr parent, std::string& child);
	bool addChild(const std::string& parenttag, std::string& child);
	bool addChild(const std::string& parenttag, const sPtr child);
	bool removeChild(const std::string& parenttag, const std::string& childtag);
	bool removeChild(const std::string& parenttag, const sPtr child);
	bool removeChild(const sPtr parent, const sPtr child);
	bool addAttrib(const std::string parenttag, const std::string& attr, const std::string& value);
	bool addAttrib(const sPtr parent,const std::string& attr, const std::string& value);
	bool removeAttrib(const sPtr parent, const std::string& attr);
	bool removeAttrib(const std::string parenttag,  const std::string& attr);

  private:
	elementType elementfilter(const std::string& xmlpart);// return elementtype when parsing each component
	std::vector<sPtr>& DFS(const std::string& tag = " ", const returntype& tpe = all, const std::pair<std::string, std::string>& attri = std::pair<std::string, std::string>(), const sPtr root_=nullptr);
	std::string getComment(const std::string& xmlpart);
	std::string getDeclare(const std::string& xmlpart); // helper function to transfrom "<?" to "<" in odrer to use xmlreader getting attributes
    sPtr pDocElement_;         // AST that holds procInstr, comments, XML root, and more comments
    std::vector<sPtr> found_;  // query results
  };
}
#endif
