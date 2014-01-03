/* =============================================================================
 *
 * File: XML.cpp
 * Desc: 封装XML DOM操作
 * Author: sw
 * Email: luochen2004@gmail.com
 * Created on: 2010-10-08
 *
 * =============================================================================
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "XML.h"

#include <cstdlib>

#include <string>

#include "log/log.h"


DOMImplementation* XML::m_pImpl = NULL;

// -----------------------------------------------------------------------------
// Constructors
// -----------------------------------------------------------------------------
XML::XML()
{
    init();
}

XML::XML(const char *fileName)
{
    init();
    loadFile(fileName);
}

XML::XML(const char *xbuf, size_t size)
{
    init();
    loadMemory(xbuf, size);
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
XML::~XML()
{
    release();
}    

// -----------------------------------------------------------------------------
// Initialize Xerces (per-process)
// -----------------------------------------------------------------------------
int XML::initialize() 
{
    try {
        XMLPlatformUtils::Initialize();

        m_pImpl = DOMImplementationRegistry::getDOMImplementation(X("Core"));
        if (NULL == m_pImpl) {
            FATAL_LOG("Initialize Xml environment error. Program will exit.");
            return -1;
        }
    } catch (const XMLException &e) {
        ERROR_LOG(S(e.getMessage()));
        return -1;
    } catch (const DOMException &e) {
        ERROR_LOG(S(e.getMessage()));
        return -1;
    }
    
    return 0;
}

// -----------------------------------------------------------------------------
// Terminate Xerces (per-process)
// -----------------------------------------------------------------------------
int XML::terminate() 
{
    try {
        XMLPlatformUtils::Terminate();
    } catch (const XMLException &e) {
        ERROR_LOG(S(e.getMessage()));
        return -1;
    } catch (const DOMException &e) {
        ERROR_LOG(S(e.getMessage()));
        return -1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// Init this object
// -----------------------------------------------------------------------------
void XML::init() 
{
    m_XmlType = UNINIT;
    m_pDoc = NULL;
    m_pRoot = NULL;
    m_pLSParser = NULL;
    m_pLSInput = NULL;
    m_pInputSource = NULL;
    m_pLSSerializer = NULL;
    m_pLSOutput = NULL;
    m_pFormatTarget = NULL;
}

// -----------------------------------------------------------------------------
// Release resources taken by this object
// -----------------------------------------------------------------------------
void XML::release() 
{
    if (NULL != m_pDoc && XMLNEW == m_XmlType) {
        m_pDoc->release();
        m_pDoc = NULL;
    }
    if (NULL != m_pInputSource) {
        delete m_pInputSource;
        m_pInputSource = NULL;
    }
    if (NULL != m_pLSInput) {
        m_pLSInput->release();
        m_pLSInput = NULL;
    }
    if (NULL != m_pLSParser) {
        m_pLSParser->release();
        m_pLSParser = NULL;
    }
    if (NULL != m_pFormatTarget) {
        delete m_pFormatTarget;
        m_pFormatTarget = NULL;
    }
    if (NULL != m_pLSOutput) {
        m_pLSOutput->release();
        m_pLSOutput = NULL;
    }
    if (NULL != m_pLSSerializer) {
        m_pLSSerializer->release();
        m_pLSSerializer = NULL;
    }
}

// -----------------------------------------------------------------------------
// Parse XML.
// -----------------------------------------------------------------------------
DOMNode* XML::parse(InputSource *inputSource)
{
    m_pInputSource = inputSource;
    m_pLSParser = ((DOMImplementationLS *) m_pImpl)->createLSParser(
            DOMImplementationLS::MODE_SYNCHRONOUS, NULL);
    m_pLSInput = ((DOMImplementationLS *) m_pImpl)->createLSInput();
    m_pLSInput->setByteStream(m_pInputSource);
    m_pLSInput->setEncoding(X("UTF-8"));
    m_pDoc = m_pLSParser->parse(m_pLSInput);
    if (NULL == m_pDoc) {
        ERROR_LOG("Parse Xml error.");
        return NULL;
    }
    m_pRoot = m_pDoc->getDocumentElement();
    return m_pRoot;
}

// -----------------------------------------------------------------------------
// Parse XML from memory buffer.
// -----------------------------------------------------------------------------
DOMNode* XML::loadMemory(const char *xbuf, size_t size)
{
    m_XmlType = XMLMEM;   
    m_pInputSource = new MemBufInputSource(
            (const XMLByte *) xbuf,
            size,
            X("xbuf"));
    return parse(m_pInputSource);
}

// -----------------------------------------------------------------------------
// Parse XML from local file.
// -----------------------------------------------------------------------------
DOMNode* XML::loadFile(const char *fileName)
{
    m_XmlType = XMLFILE;
    m_FileName = fileName;
    m_pInputSource = new LocalFileInputSource(X(fileName));
    return parse(m_pInputSource);
}


// -----------------------------------------------------------------------------
// Create an XML in memory.
// -----------------------------------------------------------------------------
DOMNode* XML::createDocument(const char *rootName)
{
    m_XmlType = XMLNEW;
    m_pDoc = m_pImpl->createDocument(
            0,                      // root element namespace URI
            X(rootName),            // root element name
            0);                     // document type object(DTD)
    m_pRoot = m_pDoc->getDocumentElement();
    return m_pRoot;
}


// -----------------------------------------------------------------------------
// Serialize XML Document to specified destination.
// -----------------------------------------------------------------------------
void XML::serialize(XMLFormatTarget *target)
{
    if (NULL != m_pFormatTarget) {
        delete m_pFormatTarget;
    }
    m_pFormatTarget = target;
    m_pLSSerializer = ((DOMImplementationLS *) m_pImpl)->createLSSerializer();
    m_pLSOutput = ((DOMImplementation *) m_pImpl)->createLSOutput();
    m_pLSOutput->setByteStream(m_pFormatTarget);
    m_pLSOutput->setEncoding(X("UTF-8"));
    m_pLSSerializer->write(m_pDoc, m_pLSOutput);
}


// -----------------------------------------------------------------------------
// Get raw buffer of XML Document created in memory.
// -----------------------------------------------------------------------------
char* XML::getXMLBuffer()
{
    serialize(new MemBufFormatTarget());
    return (char *) ((MemBufFormatTarget *) m_pFormatTarget)->getRawBuffer();
}

// -----------------------------------------------------------------------------
// Save after modifying an xml file.
// -----------------------------------------------------------------------------
void XML::save()
{
    if (XMLFILE == m_XmlType) {
        serialize(new LocalFileFormatTarget(m_FileName.c_str()));
    }
}

// -----------------------------------------------------------------------------
// Save Xml Document to specified file.
// -----------------------------------------------------------------------------
void XML::saveToFile(const char *fileName)
{
    serialize(new LocalFileFormatTarget(fileName));
}


// -----------------------------------------------------------------------------
// Get Root Element.
// -----------------------------------------------------------------------------
DOMNode* XML::getRoot() const
{
    return m_pRoot;
}

// -----------------------------------------------------------------------------
// Get elements by tag name.
// -----------------------------------------------------------------------------
DOMNodeList* XML::getElementsByTagName(
        const DOMNode *curnode, 
        const char *tagName) const
{
    return static_cast<const DOMElement *>(curnode)->getElementsByTagName(X(tagName));
}

// -----------------------------------------------------------------------------
// Get first Child.
// -----------------------------------------------------------------------------
DOMNode* XML::getFirstChild(const DOMNode *curnode) 
{
    return curnode->getFirstChild();
}

// -----------------------------------------------------------------------------
// Get child nodes.
// -----------------------------------------------------------------------------
DOMNodeList* XML::getChildNodes(const DOMNode *curnode)
{
    return curnode->getChildNodes();
}

// -----------------------------------------------------------------------------
// Get child node by tag name.
// -----------------------------------------------------------------------------
DOMNode* XML::getChildNodeByTagName(
        const DOMNode *curnode,
        const char *tagName) const
{
    return static_cast<const DOMElement *>(curnode)->getElementsByTagName(X(tagName))->item(0);
}

// -----------------------------------------------------------------------------
// Get value of this node.
// -----------------------------------------------------------------------------
std::string XML::getText(const DOMNode *curnode) const
{
    std::string text = "";
    if (DOMNode::ELEMENT_NODE == curnode->getNodeType()) {
        DOMNode *textNode = curnode->getFirstChild();
        if ( ( NULL != textNode )
             &&
             ( DOMNode::TEXT_NODE == textNode->getNodeType() ) ) {
            text = S(textNode->getNodeValue());
        }
    }
    return text;
}


// -----------------------------------------------------------------------------
// Get value by specified tag name.
// -----------------------------------------------------------------------------
std::string XML::getTextByTagName(
        const DOMNode *curnode, 
        const char *tagname) const
{
    std::string text = "";
    if (DOMNode::ELEMENT_NODE == curnode->getNodeType()) {
        DOMNode *node = ((DOMElement *) curnode)
            ->getElementsByTagName(X(tagname))->item(0);
        text = getText(node);
    }
    return text;
}


// -----------------------------------------------------------------------------
// Get value by specified tag name. The result is value of child node whose
// parent is specified by the last tagname parameter.
// -----------------------------------------------------------------------------
std::string XML::getText2ByTagName(
        const DOMNode *curnode,
        const char *ctagname,
        const char *gtagname) const
{
    std::string text = "";
    if (DOMNode::ELEMENT_NODE == curnode->getNodeType()) {
        DOMNode *cnode = ((DOMElement *) curnode)
            ->getElementsByTagName(X(ctagname))->item(0);
        text = getTextByTagName(cnode, gtagname);
    }
    return text;
}
    
// -----------------------------------------------------------------------------
// Get value of specified node
// -----------------------------------------------------------------------------
void XML::setText(DOMNode *curnode, const char *text)
{
    if (DOMNode::ELEMENT_NODE == curnode->getNodeType()) {
        DOMNode *textNode = curnode->getFirstChild();
        if ( ( NULL != textNode )
             &&
             ( DOMNode::TEXT_NODE == textNode->getNodeType() ) ) {
             textNode->setNodeValue(X(text));
        }
    }
}
    

// -----------------------------------------------------------------------------
// Add a child node given tagname of child node and the text value.
// -----------------------------------------------------------------------------
DOMNode* XML::addChildNode(
            DOMNode *parent,
            const char *childname,
            const char *textVal)
{
    DOMElement *child = m_pDoc->createElement(X(childname));
    parent->appendChild(child);
    DOMText *textNode = m_pDoc->createTextNode(X(textVal));
    child->appendChild(textNode);
    return textNode;
}
 

// -----------------------------------------------------------------------------
// Add a child node given the child node.
// -----------------------------------------------------------------------------
DOMNode* XML::addChildNode(DOMNode *parent, const char *childname)
{
    DOMElement *child = m_pDoc->createElement(X(childname));
    parent->appendChild(child);
    return child;
}
