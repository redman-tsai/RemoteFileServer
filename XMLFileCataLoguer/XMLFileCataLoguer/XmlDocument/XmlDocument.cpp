///////////////////////////////////////////////////////////////////
// XmlDocument.cpp - a container of XmlElement nodes             //
// Ver 1.3                                                       //
// Application: CSE687 Pr#2, Spring 2015                         //
// Platform:    ParallelDesktop10, Win8, MBP15                   //
// Author:      Ruizhe CAI, 354520026 ,                          //
//               rcai100@syr.edu                                 //
///////////////////////////////////////////////////////////////////

#include <iostream>
#include <stack>
#include "../XmlParsing/xmlElementParts.h"
#include "../XmlReader/XmlReader.h"
#include "../FileSystem/FileSystem.h"
#include "XmlDocument.h"

using namespace XmlProcessing;

// default consturctor
XmlProcessing::XmlDocument::XmlDocument()
{
	pDocElement_ = XmlProcessing::makeDocElement();
}

// constructor given xml file or string
XmlProcessing::XmlDocument::XmlDocument(const std::string& src, sourceType srcType)
{
	std::stack<sPtr> parentnodes;
	pDocElement_ = XmlProcessing::makeDocElement();
	parentnodes.push(pDocElement_);
	std::string xmlstring;
	Toker toker(src, (srcType==filename));
	toker.setMode(Toker::xml);
	XmlParts parts(&toker);
	while (parts.get())
	{
		xmlstring = parts.show().c_str();
		elementType tp = elementfilter(xmlstring);
		if (tp != closingtag)
		{
			sPtr child = makeXmlElement(xmlstring);
			parentnodes.top()->addChild(child);
			if (tp == Tagged)
				parentnodes.push(child);
		}
		else
			parentnodes.pop();
	}
}

//move constructor
XmlProcessing::XmlDocument::XmlDocument(XmlDocument &&doc)
{
	std::shared_ptr<DocElement> root_ = std::dynamic_pointer_cast<DocElement>(doc.pDocElement_);
	std::shared_ptr<AbstractXmlElement> temp(new DocElement(std::move(*root_)));
	pDocElement_ = temp;
}

// move assignment
XmlDocument&  XmlProcessing::XmlDocument::operator=(XmlDocument &&doc)
{
	sPtr temp(new DocElement(std::move(doc.pDocElement_)));
	pDocElement_ = temp;
	return *this;
}

//return xml element type based on xml string
XmlDocument::elementType XmlDocument::elementfilter(const std::string& xmlparts)
{
	elementType type;
	if (xmlparts.find("<") == xmlparts.npos)
		type = Text;
	else
	{
		if (xmlparts.find("<!--") != xmlparts.npos || xmlparts.find("< ! --") != xmlparts.npos)
			type = comment;
		else
		{
			if (xmlparts.find("<?") != xmlparts.npos || xmlparts.find("< ?") != xmlparts.npos)
			{
				if (xmlparts.find("xml version") != xmlparts.npos)
					type = Declare;
				else
					type = Proc;
			}
			else
			{
				if (xmlparts.find("</") != xmlparts.npos || xmlparts.find("< /")!= xmlparts.npos)
					type = closingtag;
				else
				{
					if (xmlparts.find("/>") != xmlparts.npos || xmlparts.find("/ >") != xmlparts.npos)
						type = TaggedSelfColosed;
					else
						type = Tagged;
				}
			}
				
		}

	}
	return type;
}

// return found_
std::vector<XmlDocument::sPtr> XmlDocument::select()
{
	return (std::move(found_));
}

//add child given parent and child sPtr
bool XmlDocument::addChild(const sPtr parent, const sPtr child)
{
	if (parent == pDocElement_)
		return pDocElement_->addChild(child);
	std::shared_ptr<TaggedElement> tagNode = std::dynamic_pointer_cast<TaggedElement>(parent);
	if (tagNode != nullptr)
		return tagNode->addChild(child);
	return false;
}

//remove child given parent and child sPtr
bool XmlDocument::removeChild(const sPtr parent, const sPtr child)
{
	if (parent == pDocElement_)
		return pDocElement_->removeChild(child);
	std::shared_ptr<TaggedElement> tagNode = std::dynamic_pointer_cast<TaggedElement>(parent);
	if (tagNode != nullptr)
		return tagNode->removeChild(child);
	return false;
}

//add child given parent sPtr and child xml string
bool XmlDocument::addChild(const sPtr parent, std::string& child)
{
	if (parent == pDocElement_)
		return pDocElement_->addChild(makeXmlElement(child));
	std::shared_ptr<TaggedElement> tagNode = std::dynamic_pointer_cast<TaggedElement>(parent);
	if (tagNode != nullptr)
		return tagNode->addChild(makeXmlElement(child));
	return false;
}

// add child given parent tag(first node with this tag) and child sPtr
bool XmlDocument::addChild(const std::string& parenttag, std::string& child)
{
	std::vector<sPtr> found = DFS(parenttag, first);
	if (found.empty())
		return false;
	return (found[0]->addChild(makeXmlElement(child)));
}

//remove child based on parent and child tag
bool XmlDocument::removeChild(const std::string& parenttag, const std::string& childtag)
{
	std::vector<sPtr> found = DFS(parenttag, first);
	if (found.empty())
		return false;
	std::vector<sPtr> child = DFS(childtag, first, std::pair<std::string, std::string>(), found[0]);
	if (child.empty())
		return false;
	return (found[0]->removeChild(child[0]));
}

//add child given parent tag and child sPtr
bool XmlDocument::addChild(const std::string& parenttag, const sPtr child)
{
	std::vector<sPtr> found = DFS(parenttag, first);
	if (found.empty())
		return false;
	return (found[0]->addChild(child));
}

// remove child given parent tag and sPtr
bool XmlDocument::removeChild(const std::string& parenttag, const sPtr child)
{
	std::vector<sPtr> found = DFS(parenttag, first);
	if (found.empty())
		return false;
	return (found[0]->removeChild(child));
}

//add attribute pair given parent sPtr and attribute pair
bool XmlDocument::addAttrib(const sPtr parent, const std::string& attr, const std::string& value)
{
	std::shared_ptr<TaggedElement> tagNode = std::dynamic_pointer_cast<TaggedElement>(parent);
	std::shared_ptr<ProcInstrElement> procnode = std::dynamic_pointer_cast<ProcInstrElement>(parent);
	if (tagNode != nullptr)
		return tagNode->addAttrib(attr, value);
	if (procnode != nullptr)
		return procnode->addAttrib(attr, value);
	return false;
}

// add attribute pair given parent tag and arrtibute pair
bool XmlDocument::addAttrib(const std::string parenttag, const std::string& attr, const std::string& value)
{
	std::vector<sPtr> found = DFS(parenttag, first);
	if (found.empty())
		return false;
	std::shared_ptr<TaggedElement> tagNode = std::dynamic_pointer_cast<TaggedElement>(found[0]);
	if (tagNode != nullptr)
		return tagNode->addAttrib(attr, value);
	return false;
}

//remove attribute pair given parent sPtr and attribute pair
bool XmlDocument::removeAttrib(const sPtr parent, const std::string& attr)
{
	std::shared_ptr<TaggedElement> tagNode = std::dynamic_pointer_cast<TaggedElement>(parent);
	std::shared_ptr<ProcInstrElement> procnode = std::dynamic_pointer_cast<ProcInstrElement>(parent);
	if (tagNode != nullptr)
		return tagNode->removeAttrib(attr);
	if (procnode != nullptr)
		return procnode->removeAttrib(attr);
	return false;
}

// add attribute pair given parent tag and arrtibute pair
bool XmlDocument::removeAttrib(const std::string parenttag, const std::string& attr)
{
	std::vector<sPtr> found = DFS(parenttag, first);
	if (found.empty())
		return false;
	std::shared_ptr<TaggedElement> tagNode = std::dynamic_pointer_cast<TaggedElement>(found[0]);
	if (tagNode != nullptr)
		return tagNode->removeAttrib(attr);
	return false;
}

// get comment form xml string
std::string XmlDocument::getComment(const std::string& xmlpart)
{
	size_t begin;
	size_t end;
	begin = xmlpart.find_first_of("--") + 3;
	end = xmlpart.find_last_of("--") - 3;
	return (xmlpart.substr(begin, end - begin + 1));
}

// get declare xmlstring as tag in order to use xmlreader to obtain tag and attriutes
std::string XmlDocument::getDeclare(const std::string& xmlpart)
{
	size_t begin;
	size_t end;
	begin = xmlpart.find_first_of("?") + 1;
	end = xmlpart.find_last_of("?")-1;
	std::string x = "<" + xmlpart.substr(begin, end - begin) + ">";
	return ("<" + xmlpart.substr(begin, end - begin + 1) + ">");
}

//find first element with tag
XmlDocument&  XmlProcessing::XmlDocument::element(const std::string& tag)
{
	std::vector<sPtr> found=std::move(DFS(tag, first));
	sPtr root = pDocElement_;
	XmlDocument *result = new XmlDocument(root, std::move(found));
	return *result;
}

//find all elemetns with tag
XmlDocument&  XmlProcessing::XmlDocument::elements(const std::string& tag)
{
	std::vector<sPtr> found = std::move(DFS(tag, all));
	sPtr root = found[0];
	XmlDocument *result = new XmlDocument(root, std::move(found));
	return *result;
}

// DFS search given tag or attribute pair, with selected root
std::vector<XmlDocument::sPtr>& XmlDocument::DFS(const std::string& tag, const returntype& tpe, const std::pair<std::string, std::string>& attri, const sPtr root_)
{
	std::stack<sPtr> parentsnode;
	std::vector<sPtr> *found =  new std::vector<sPtr>;
	sPtr top;
	sPtr root;
	if (found_.empty())
		root = pDocElement_;
	else
		root = found_[0];
	if (root_ != nullptr)
		root = root_;
	parentsnode.push(root);
	while (!parentsnode.empty())
	{
		top = parentsnode.top();
		parentsnode.pop();
		if ((top->tag() == tag || tag == " ") && (std::find(top->attributes().begin(), top->attributes().end(), attri) != top->attributes().end() ||(attri.first.empty()) && (attri.second.empty()) ))
		{
			found->push_back(top);
			if (tpe == first)
				break;
		}
		if (top->children().size() != 0)
		{
			for (auto chld : top->children())
				parentsnode.push(chld);
		}
	}
	return *found;
}

// find element by attribute pair
XmlDocument::sPtr  XmlDocument::findByID(const std::pair<std::string, std::string>& attr)
{
	std::vector<sPtr> found=DFS(" ", first, attr);
	if (found.empty())
		return nullptr;
	return found[0];
}

// find children given by a tag
XmlDocument& XmlDocument::children(const std::string& tag)
{
	std::vector<sPtr> found;
	sPtr root;
	if (found_.empty())
		root = pDocElement_;
	else
		root = found_[0];
	for (auto child : root->children())
	{
		if (child->tag() == tag|| tag == " ")
			found.push_back(child);
	}
	XmlDocument *result = new XmlDocument(pDocElement_, std::move(found));
	return *result;
}

//make corresponding xmlemement based on xmlstring
XmlDocument::sPtr XmlDocument::makeXmlElement(std::string& xmlstring)
{
	elementType tp = elementfilter(xmlstring);
	if (tp == Text)
		return makeTextElement(xmlstring);
	else
	{
		if (tp == comment)
			return makeCommentElement(getComment(xmlstring));
		else
		{
			if (tp == Declare)
			{

				XmlReader reader(getDeclare(xmlstring));
				sPtr dclrnode = makeXmlDeclarElement();
				for (auto at : reader.attributes())
				{
					dclrnode->addAttrib(at.first, at.second);
				}
				return dclrnode;
			}
			else
			{
				if (tp == Proc)
				{
					XmlReader reader(getDeclare(xmlstring));
					sPtr procnode = makeProcInstrElement(reader.tag());
					for (auto at : reader.attributes())
					{
						procnode->addAttrib(at.first, at.second);
					}
					return procnode;
				}
				else
				{
					if (tp == Tagged || tp == TaggedSelfColosed)
					{
						XmlReader reader(xmlstring);
						sPtr Tagnode = makeTaggedElement(reader.tag());
						for (auto id : reader.attributes())
							Tagnode->addAttrib(id.first, id.second);
						return Tagnode;
					}
					else
						return nullptr;
				}
			}

		}
	}
}

// add root
bool XmlDocument::settagroot(const sPtr root_)
{
	std::shared_ptr<TaggedElement> tagg = std::dynamic_pointer_cast<TaggedElement>(root_);
	if (tagg != nullptr)
	{
		return(pDocElement_->addChild(root_));
	}
	return false;
}

// return descendents
XmlDocument& XmlDocument::descendents(const std::string& tag)
{
	std::vector<sPtr> fd = elements(tag).select();
	std::vector<sPtr>::iterator first = fd.begin() + 1;
	std::vector<sPtr> found(first, fd.end());
	XmlDocument *result = new XmlDocument(pDocElement_, std::move(found));
	return *result;
}

// writetostring
std::string XmlDocument::WriteToString()
{
	std::string xml;
	xml = pDocElement_->toString();
	return xml;
}

//writeto file
void XmlDocument::WriteToFile(const std::string& path)
{
	std::string content = WriteToString();
	std::string fullPath = FileSystem::Path::getFullFileSpec(path);
	FileSystem::File fileStream = FileSystem::File::File(fullPath);
	if (fileStream.open(fileStream.out, fileStream.text))
	{
		fileStream.putLine(content, true);
		fileStream.flush();
		fileStream.close();
	}
}

#ifdef TEST_XMLDOCUMENT
int main()
{
  title("Testing XmlDocument class");
  title("xml tree from lecturenote.xml");
  XmlDocument doc("LectureNote.xml",XmlDocument::filename);
  std::cout << doc.WriteToString() << std::endl;
  title("move ctor");
  XmlDocument X(std::move(doc));
  std::cout << X.WriteToString() << std::endl;
  title("find elements with reference tag");
  std::vector<XmlDocument::sPtr> finds = X.elements("reference").select();
  for (auto item : finds)
  {
	  std::cout << "\n" << item->toString();
  }
  std::cout << std::endl;
  title("find elements with attr course");
  std::pair<std::string, std::string> F  ;
  F.first = "course";
  F.second = "CSE681";
  std::cout << "\n" << X.findByID(F)->toString();
  std::string  tg = "reference";
  std::string xx = "< B C=\"D\" / >";
  title("add child");
  X.addChild(tg, xx);
  std::cout << X.WriteToString() << std::endl;
  xx = "B";
  title("remove child");
  X.removeChild(tg, xx);
  std::cout << X.WriteToString() << std::endl;
  X.WriteToFile();

  return 0;
}
#endif