/*
 * ============================================================================
 * @file XML.h
 * @brief 封装xml DOM操作
 *
 * <pre>
 * 
 *  Created on: 2010-09-17
 *      Author: sw 
 * </pre>
 *
 * ============================================================================
 */

#ifndef DFS_COMMON_XML_DOM_H_
#define DFS_COMMON_XML_DOM_H_

#include <iostream>
#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

// convert string from char* to XMLCh*
class Str2Xml {
public:
    Str2Xml(const char *toTranscode) {
        m_pUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~Str2Xml() {
        XMLString::release(&m_pUnicodeForm);
    }

    const XMLCh* unicodeForm() const {
        return m_pUnicodeForm;
    }
private:
    XMLCh *m_pUnicodeForm;
};

// convert string from XMLCh* to char*
class Xml2Str {
public:
    Xml2Str(const XMLCh *toTranscode) {
        m_pLocalForm = XMLString::transcode(toTranscode);
    }

    ~Xml2Str() {
        XMLString::release(&m_pLocalForm);
    }

    const char* localForm() {
        return m_pLocalForm;
    }
private:
    char *m_pLocalForm;
};

#define X(str) Str2Xml(str).unicodeForm()
#define S(str) Xml2Str(str).localForm()


/**
 * @brief 将xml文件或内存xml封装成一个xml对象
 */
class XML {
public:
    // the type of XML object, file or memory or create
    enum XMLType {
        UNINIT = 0,
        XMLFILE,
        XMLMEM,
        XMLNEW
    };
public:
    XML();
    XML(const char *fileName);
    XML(const char *xbuf, size_t size);
    
    virtual ~XML();

    static int initialize();
    static int terminate();

    // for xml file
    DOMNode* loadFile(const char *fileName);
    void save();  // save to file after modifying an xml file
    void saveToFile(const char *fileName);

    
    // for xml memory
    DOMNode* loadMemory(const char *xbuf, size_t size);
    char* getXMLBuffer();
    
    // create an Xml Document in memory
    DOMNode* createDocument(const char *rootName);

    // for reading from xml
    DOMNode* getRoot() const;
    DOMNodeList* getElementsByTagName(
            const DOMNode *curnode, 
            const char *tagName) const;
    DOMNode* getFirstChild(const DOMNode *curnode);
    DOMNodeList* getChildNodes(const DOMNode *curnode);
    DOMNode* getChildNodeByTagName(
            const DOMNode *curnode,
            const char *tagName) const;
    std::string getText(const DOMNode *curnode) const;
    std::string getTextByTagName(
            const DOMNode *curnode, 
            const char *tagname) const;
    std::string getText2ByTagName(
            const DOMNode *curnode,
            const char *ctagname,
            const char *gtagname) const;

    // for modify an xml
    void setText(DOMNode *curnode, const char *text);

    // for creating an xml
    DOMNode* addChildNode(
            DOMNode *parent,
            const char *childname,
            const char *textVal);
    DOMNode* addChildNode(DOMNode *parent, const char *childname);
    


private:
    void init();
    void release();
    void serialize(XMLFormatTarget *target);
    DOMNode* parse(InputSource *inputSource);

private:
    enum XMLType m_XmlType;
    std::string m_FileName;  // file name when parse a xml file
    static DOMImplementation *m_pImpl;  // only one instance
    DOMDocument *m_pDoc;
    DOMNode *m_pRoot;
    // for parse xml 
    DOMLSParser *m_pLSParser;
    DOMLSInput *m_pLSInput;
    InputSource *m_pInputSource;
    // for serialze xml
    DOMLSSerializer *m_pLSSerializer;
    DOMLSOutput *m_pLSOutput;
    XMLFormatTarget *m_pFormatTarget;

};

#endif  /* DFS_COMMON_XML_DOM_H_ */
