///////////////////////////////////////////////////////////////////
// XmlElement.cpp - define XML Element types                     //
// ver 1.6                                                       //
// Application: CSE687 Pr#2, Spring 2015                         //
// Platform:    ParallelDesktop10, Win8, MBP15                   //
// Author:      Ruizhe CAI, 354520026 ,                          //
//               rcai100@syr.edu                                 //
///////////////////////////////////////////////////////////////////

#include "XmlElement.h"
#include <iostream>
#include <queue>

using namespace XmlProcessing;

size_t AbstractXmlElement::count = 0;
size_t AbstractXmlElement::tabSize = 2;
std::vector<std::shared_ptr<AbstractXmlElement>> AbstractXmlElement::nullchildren = std::vector<std::shared_ptr<AbstractXmlElement>>();
std::vector<std::pair<std::string, std::string>> AbstractXmlElement::nullattribs = std::vector<std::pair<std::string, std::string>>();

//----< factory for doc elements >----------------------------------------

std::shared_ptr<AbstractXmlElement> XmlProcessing::makeDocElement(std::shared_ptr<AbstractXmlElement> pRoot)
{
  std::shared_ptr<AbstractXmlElement> ptr(new DocElement(pRoot));
  return ptr;
}

//----< factory for tagged elements >----------------------------------------

std::shared_ptr<AbstractXmlElement> XmlProcessing::makeTaggedElement(const std::string& tag)
{
  std::shared_ptr<AbstractXmlElement> ptr(new TaggedElement(tag));
  return ptr;
}
//----< factory for text elements >------------------------------------------

std::shared_ptr<AbstractXmlElement> XmlProcessing::makeTextElement(const std::string& text)
{
  std::shared_ptr<AbstractXmlElement> ptr(new TextElement(text));
  return ptr;
}
//----< factory for comment elements >---------------------------------------

std::shared_ptr<AbstractXmlElement> XmlProcessing::makeCommentElement(const std::string& text)
{
  std::shared_ptr<AbstractXmlElement> ptr(new CommentElement(text));
  return ptr;
}
//----< factory for processing instruction elements >------------------------

std::shared_ptr<AbstractXmlElement> XmlProcessing::makeProcInstrElement(const std::string& text)
{
  std::shared_ptr<AbstractXmlElement> ptr(new ProcInstrElement(text));
  return ptr;
}
//----< factory for XML Declaration elements >-------------------------------

std::shared_ptr<AbstractXmlElement> XmlProcessing::makeXmlDeclarElement()
{
  std::shared_ptr<AbstractXmlElement> ptr(new XmlDeclarElement());
  return ptr;
}
//----< doc element constructor and destructor >-----------------------------

DocElement::DocElement(std::shared_ptr<AbstractXmlElement> pRoot) 
{
  std::shared_ptr<AbstractXmlElement> pDecl = makeXmlDeclarElement();
  pDecl->addAttrib("version", "1.0");
  children_.push_back(pDecl);
  if (pRoot != nullptr)
    children_.push_back(pRoot);
}
// ----< doc element move constructor >--------------------------------------

DocElement::DocElement(DocElement&& de)
{
	std::vector<std::shared_ptr<AbstractXmlElement>> temp = de.children_;
	if (!temp.empty())
	{
		for (auto chi : temp)
		{
			std::shared_ptr<TaggedElement> tagchild = std::dynamic_pointer_cast<TaggedElement>(chi);
			std::shared_ptr<XmlDeclarElement> dclrchild = std::dynamic_pointer_cast<XmlDeclarElement>(chi);
			std::shared_ptr<ProcInstrElement> procchild = std::dynamic_pointer_cast<ProcInstrElement>(chi);
			if (tagchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new TaggedElement(std::move(*tagchild)));
				addChild(newchild);
				continue;
			}
			if (dclrchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new XmlDeclarElement(std::move(*dclrchild)));
				addChild(newchild);
				continue;
			}
			if (procchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new ProcInstrElement(std::move(*procchild)));
				addChild(newchild);
				continue;
			}
			chi = nullptr;
		}
	}
}

// doc move operator
DocElement& DocElement::operator=(DocElement&& de)
{
	std::vector<std::shared_ptr<AbstractXmlElement>> temp = de.children_;
	if (!temp.empty())
	{
		for (auto chi : temp)
		{
			std::shared_ptr<TaggedElement> tagchild = std::dynamic_pointer_cast<TaggedElement>(chi);
			std::shared_ptr<XmlDeclarElement> dclrchild = std::dynamic_pointer_cast<XmlDeclarElement>(chi);
			std::shared_ptr<ProcInstrElement> procchild = std::dynamic_pointer_cast<ProcInstrElement>(chi);
			if (tagchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new TaggedElement(std::move(*tagchild)));
				addChild(newchild);
				continue;
			}
			if (dclrchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new XmlDeclarElement(std::move(*dclrchild)));
				addChild(newchild);
				continue;
			}
			if (procchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new ProcInstrElement(std::move(*procchild)));
				addChild(newchild);
				continue;
			}
			chi = nullptr;
		}
	}
	return *this;
}
//----< private helper for DocElement::addChild(...) >-----------------------

bool DocElement::hasXmlRoot()
{
  for (auto pElement : children_)
  {
    if (dynamic_cast<TaggedElement*>(pElement.get()) != nullptr)
      return true;
  }
  return false;
}
//----< add only one child to doc element using pointer to child >-----------

bool DocElement::addChild(std::shared_ptr<AbstractXmlElement> pChild)
{
  // don't add TextElements
  TextElement* pTxEl = dynamic_cast<TextElement*>(pChild.get());
  if (pTxEl != 0)  // don't add text elements to DocElement
    return false;

  // don't add another DocElement
  DocElement* pDcEl = dynamic_cast<DocElement*>(pChild.get());
  if (pDcEl != 0)  // don't add text elements to DocElement
    return false;

  // add ProcInstrElements and CommentElements 
  TaggedElement* te = dynamic_cast<TaggedElement*>(pChild.get());
  XmlDeclarElement* de = dynamic_cast<XmlDeclarElement*>(pChild.get());
  // add only one TaggedElement
  if (de == nullptr)
  {
	  if (te == nullptr) // is not a TaggedElement or dclrelement
	  {
		  children_.push_back(pChild);
		  return true;
	  }
	  if (!hasXmlRoot())
	  {
		  children_.push_back(pChild);
		  return true;
	  }
	  return false;
  }
  else // overwrite default declr element if xml file has dclr element
  {
	  std::shared_ptr<AbstractXmlElement> olddclr;
	  for (auto pElement : children_)
	  {
		  if (dynamic_cast<XmlDeclarElement*>(pElement.get()) != nullptr)
			  olddclr = pElement;
	  }
	  removeChild(olddclr);
	  children_.push_back(pChild);
	  return true;
  }
}
//----< remove child from doc element using pointer to child >---------------

bool DocElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild)
{
  auto iter = std::find(begin(children_), end(children_), pChild);
  if (iter != end(children_))
  {
    children_.erase(iter);
    return true;
  }
  return false;
}
//----< return value = tag from doc element >-----------------------------

std::string DocElement::value() { return std::string(""); }

//----< generate xml string for tagged element >-----------------------------

std::string DocElement::toString()
{
  std::string rtn;
  for (auto elem : children_)
    rtn += elem->toString();
  return rtn;
}
// move constructor for tagged element
TaggedElement::TaggedElement(TaggedElement&& te)
{
	tag_ = std::move(te.tag_);
	attribs_ = std::move(te.attribs_);
	std::vector<std::shared_ptr<AbstractXmlElement>> temp = te.children_;
	if (!temp.empty())
	{
		for (auto chi : temp)
		{
			std::shared_ptr<TaggedElement> tagchild = std::dynamic_pointer_cast<TaggedElement>(chi);
			std::shared_ptr<TextElement> textchild = std::dynamic_pointer_cast<TextElement>(chi);
			std::shared_ptr<CommentElement> commentchild = std::dynamic_pointer_cast<CommentElement>(chi);
			if (tagchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new TaggedElement(std::move(*tagchild)));
				addChild(newchild);
				continue;
			}
			if (textchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new TextElement(std::move(*textchild)));
				addChild(newchild);
				continue;
			}
			if (commentchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new CommentElement(std::move(*commentchild)));
				addChild(newchild);
				continue;
			}
			chi = nullptr;
		}
	}
}

// move operator for tagele
TaggedElement& TaggedElement::operator=(TaggedElement&& te)
{
	tag_ = std::move(te.tag_);
	attribs_ = std::move(te.attribs_);
	std::vector<std::shared_ptr<AbstractXmlElement>> temp = te.children_;
	if (!temp.empty())
	{
		for (auto chi : temp)
		{
			std::shared_ptr<TaggedElement> tagchild = std::dynamic_pointer_cast<TaggedElement>(chi);
			std::shared_ptr<TextElement> textchild = std::dynamic_pointer_cast<TextElement>(chi);
			std::shared_ptr<CommentElement> commentchild = std::dynamic_pointer_cast<CommentElement>(chi);
			if (tagchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new TaggedElement(std::move(*tagchild)));
				addChild(newchild);
				continue;
			}
			if (textchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new TextElement(std::move(*textchild)));
				addChild(newchild);
				continue;
			}
			if (commentchild != nullptr)
			{
				std::shared_ptr<AbstractXmlElement> newchild(new CommentElement(std::move(*commentchild)));
				addChild(newchild);
				continue;
			}
			chi = nullptr;
		}
	}
	return *this;
}
//----< add child to tagged element using pointer to child >-----------------

bool TaggedElement::addChild(std::shared_ptr<AbstractXmlElement> pChild)
{
  children_.push_back(pChild);
  return true;
}
//----< remove child from tagged element using pointer to child >------------

bool TaggedElement::removeChild(std::shared_ptr<AbstractXmlElement> pChild)
{
  auto iter = std::find(begin(children_), end(children_), pChild);
  if (iter != end(children_))
  {
    children_.erase(iter);
    return true;
  }
  return false;
}
//----< remove child from tagged element using child's value >---------------

bool TaggedElement::removeChild(const std::string& value)
{
  for (auto iter = begin(children_); iter != end(children_); ++iter)
  {
    if ((*iter)->value() == value)
    {
      children_.erase(iter);
      return true;
    }
  }
  return false;
}
//----< add attribute to tagged element >------------------------------------

bool TaggedElement::addAttrib(const std::string& name, const std::string& value)
{
  std::pair<std::string, std::string> atPair(name, value);
  attribs_.push_back(atPair);
  return true;
}
//----< remove attribute from tagged element >-------------------------------

bool TaggedElement::removeAttrib(const std::string& name)
{
  for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
  {
    if (iter->first == name)
    {
      attribs_.erase(iter);
      return true;
    }
  }
  return false;
}
//----< return value = tag from tagged element >-----------------------------

std::string TaggedElement::value() { return tag_; }

//----< generate xml string for tagged element >-----------------------------

std::string TaggedElement::toString()
{
  std::string spacer(tabSize*++count, ' ');
  std::string xml = "\n" + spacer + "<" + tag_;
  for (auto at : attribs_)
  {
    xml += " ";
    xml += at.first;
    xml += "=\"";
    xml += at.second;
    xml += "\"";
  }
  if (children_.empty())//self closing
	  xml += "/>";
  else
  {
	  xml += ">";
	  for (auto pChild : children_)
		  xml += pChild->toString();
	  xml += "\n" + spacer + "</" + tag_ + ">";
  }
  --count;
  return xml;
}

//----< move constructro fot te >--------------------------------------------
TextElement::TextElement(TextElement&& te)
{
	text_ = std::move(te.text_);
}

// move operator
TextElement& TextElement::operator=(TextElement&& te)
{
	text_ = std::move(te.text_);
	return *this;
}

//----< generate xml string for text element >-------------------------------

std::string TextElement::toString()
{
  std::string spacer(tabSize * ++count, ' ');
  std::string xml = "\n" + spacer + text_;
  --count;
  return xml;
}
// move constructor for ProcIns
ProcInstrElement::ProcInstrElement(ProcInstrElement&& pe)
{
	attribs_ = std::move(pe.attribs_);
	type_ = std::move(pe.type_);
	text = std::move(pe.text);
}

//move operator
ProcInstrElement& ProcInstrElement::operator=(ProcInstrElement&& pe)
{
	attribs_ = std::move(pe.attribs_);
	type_ = std::move(pe.type_);
	text = std::move(pe.text);
	return *this;
}

//----< add attribute to ProcInstElement >-----------------------------------

bool ProcInstrElement::addAttrib(const std::string& name, const std::string& value)
{
  std::pair<std::string, std::string> atPair(name, value);
  attribs_.push_back(atPair);
  return true;
}
//----< remove attribute from ProcInstElement >------------------------------

bool ProcInstrElement::removeAttrib(const std::string& name)
{
  for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
  {
    if (iter->first == name)
    {
      attribs_.erase(iter);
      return true;
    }
  }
  return false;
}

//----< generate xml strng for procinc>--------------------------------------
std::string ProcInstrElement::toString()
{
	std::string spacer(tabSize*++count, ' ');
	std::string xml = "\n" + spacer + "<?" + text;
	for (auto at : attribs_)
	{
		xml += " ";
		xml += at.first;
		xml += "=\"";
		xml += at.second;
		xml += "\"";
	}
	xml += "?>";
	--count;
	return xml;
}
// move constructor for comment
CommentElement::CommentElement(CommentElement&& ce)
{
	commentText_ = std::move(ce.commentText_);
}

// move operator
CommentElement& CommentElement::operator=(CommentElement&& ce)
{
	commentText_ = std::move(ce.commentText_);
	return *this;
}

//----< generate xml strng for comment>--------------------------------------
std::string CommentElement::toString()
{
	std::string spacer(tabSize * ++count, ' ');
	std::string xml = "\n" + spacer + "<!--" + commentText_ + "-->";
	--count;
	return xml;
}
// move constructor for declare
XmlDeclarElement::XmlDeclarElement(XmlDeclarElement&& xe)
{
	type_ = std::move(xe.type_);
	attribs_ = std::move(xe.attribs_);
}

//move operator for delcare
XmlDeclarElement& XmlDeclarElement::operator=(XmlDeclarElement&& xe)
{
	type_ = std::move(xe.type_);
	attribs_ = std::move(xe.attribs_);
	return *this;
}

//----< generate xml string for text element >-------------------------------

std::string XmlDeclarElement::toString()
{
  std::string spacer(tabSize * ++count, ' ');
  std::string xml = "\n" + spacer + "<?xml";
  for (auto at : attribs_)
  {
    xml += " ";
    xml += at.first;
    xml += "=\"";
    xml += at.second;
    xml += "\"";
  }
  xml += "?>";
  --count;
  return xml;
}
//----< add attribute to ProcInstElement >-----------------------------------

bool XmlDeclarElement::addAttrib(const std::string& name, const std::string& value)
{
  std::pair<std::string, std::string> atPair(name, value);
  attribs_.push_back(atPair);
  return true;
}
//----< remove attribute from ProcInstElement >------------------------------

bool XmlDeclarElement::removeAttrib(const std::string& name)
{
  for (auto iter = begin(attribs_); iter != end(attribs_); ++iter)
  {
    if (iter->first == name)
    {
      attribs_.erase(iter);
      return true;
    }
  }
  return false;
}
//----< helper function displays titles >------------------------------------

void XmlProcessing::title(const std::string& title, char underlineChar)
{
  std::cout << "\n  " << title;
  std::cout << "\n " << std::string(title.size() + 2, underlineChar);
}

//----< test stub >----------------------------------------------------------

#ifdef TEST_XMLELEMENT

int main()
{
  title("Testing XmlElement Package", '=');
  std::cout << "\n";

  using sPtr = std::shared_ptr < AbstractXmlElement > ;

  sPtr root = makeTaggedElement("root");
  root->addChild(makeTextElement("this is a test"));

  sPtr child = makeTaggedElement("child");
  child->addChild(makeTextElement("this is another test"));
  child->addAttrib("first", "test");
  root->addChild(child);

  sPtr docEl = makeDocElement(root);
  std::cout << "  " << docEl->toString();
  std::cout << "\n\n";
}

#endif
