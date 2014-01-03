//XMLConfig.cpp
#include "XMLConfig.h"
#include <iostream>
//#include "../comm/Error.h"
//#include "../DevLog/DevLog.h"
//extern DevLog *g_pDevLog;

using namespace std;
XMLConfig::XMLConfig() :
        m_pDoc(NULL),
        m_pRoot(NULL),
        m_pBuf(NULL),
        m_XmlType(UNINIT)
{
}

XMLConfig::XMLConfig(const char *fileName) :
        m_pDoc(NULL),
        m_pRoot(NULL),
        m_pBuf(NULL),
        m_XmlType(UNINIT)
{
    if (NULL == loadFile(fileName))
    {
        //DEV_LOG(LERROR, OUT_SCR, "loadFile error: " + string(fileName));
    }
}

XMLConfig::~XMLConfig()
{
    try
    {
        if (m_pDoc != NULL)
            xmlFreeDoc(m_pDoc);
        if (m_pBuf != NULL)
        {
            xmlBufferFree(m_pBuf);
        }
        vector<xmlChar*>::iterator vit;
        for (vit=m_Attrs.begin(); vit!=m_Attrs.end(); ++vit)
            xmlFree(*vit);
    }
    catch (...)
    {
        //DEV_LOG(LERROR, OUT_SCR, "~XMLConfig exception");
    }
}

XMLNode* XMLConfig::loadMemory(const char *buf, unsigned int size)
{
    //free doc handle
    if (m_pDoc != NULL)
        xmlFreeDoc(m_pDoc);

    m_XmlType = XMLMEM;
    m_pDoc = xmlReadMemory(buf, size, NULL, "UTF-8", int(XML_PARSE_RECOVER));
    if (NULL == m_pDoc)
    {
        cerr<<"read file error"<<endl;
        return NULL;
    }
    xmlNodePtr node = xmlDocGetRootElement(m_pDoc);
    if (NULL == node)
    {
        cerr<<"empty file!"<<endl;
        return NULL;
    }
    m_pRoot = node;
    return m_pRoot;
}

char* XMLConfig::getXMLBuffer()
{
    return getXMLBuffer(m_pRoot);
}

char* XMLConfig::getXMLBuffer(XMLNode *node)
{
    if (m_pBuf != NULL)
    {
        xmlBufferFree(m_pBuf);
        m_pBuf = NULL;
    }
    //xmlOutputBufferPtr buf = (xmlOutputBufferPtr)xmlMalloc(sizeof(xmlOutputBuffer));
    //buf->buffer = xmlBufferCreate();
    m_pBuf = xmlBufferCreate();
    if (m_pBuf == NULL)
    {
        //DEV_LOG_ERROR("xmlBufferCreate error");
        return NULL;
    }

    //int ret = xmlSaveFileTo(buf, m_pDoc, NULL);
    //cout << "XML: " << buf->buffer->content;
    //return (char*)buf->buffer->content;


    int bytes = xmlNodeDump(m_pBuf, m_pDoc, node, 0, 1);
    if (bytes < 0)
    {
        //DEV_LOG(LERROR, OUT_SCR, "xmlNodeDump failed");
        return NULL;
    }

    return (char*)m_pBuf->content;
}

XMLNode* XMLConfig::loadFile(const char *xname)
{
    //free doc handle
    if (m_pDoc != NULL)
        xmlFreeDoc(m_pDoc);

    m_FileName = xname;
    m_XmlType = XMLFILE;
    m_pDoc = xmlReadFile(xname, "UTF-8", int(XML_PARSE_RECOVER));
    if (NULL == m_pDoc)
    {
        cerr<<"read file error"<<endl;
        return NULL;
    }
    xmlNodePtr node = xmlDocGetRootElement(m_pDoc);
    if (NULL == node)
    {
        cerr<<"empty file!"<<endl;
        return NULL;
    }
    m_pRoot = node;
    return m_pRoot;
}

XMLNode* XMLConfig::getParent(XMLNode *curnode)
{
    return curnode->parent;
}

int XMLConfig::getFirstNode(const char *xpath, XMLNode *&node)
{
    xmlXPathContextPtr context = xmlXPathNewContext(m_pDoc);
    if (NULL == context)
    {
        cerr<<"content is NULL"<<endl;
        return -1;
    }
    xmlXPathObjectPtr result = xmlXPathEvalExpression(BAD_CAST xpath, context);
    xmlXPathFreeContext(context);
    if (NULL == result)
    {
        cerr<<"xmlXPathEvalExpression return NULL"<<endl;
        xmlXPathFreeObject(result);
        return -1;
    }

    xmlNodeSetPtr pNodeset = result->nodesetval;
    if (pNodeset->nodeNr >= 1)
    {
        node = pNodeset->nodeTab[0];
    }
    else
    {
        cerr << "no such path: " << xpath << endl;
        return -1;
    }
    xmlXPathFreeObject(result);//new add
    return 0;
}

int XMLConfig::getNodeSet(const char *xpath, vector<XMLNode*> &nodes)
{
    xmlXPathContextPtr context = xmlXPathNewContext(m_pDoc);
    if (NULL == context)
    {
        cerr<<"content is NULL"<<endl;
        return -1;
    }
    xmlXPathObjectPtr result = xmlXPathEvalExpression(BAD_CAST xpath, context);
    xmlXPathFreeContext(context);
    if (NULL == result)
    {
        cerr<<"xmlXPathEvalExpression return NULL"<<endl;
        xmlXPathFreeObject(result);
        return -1;
    }

    xmlNodeSetPtr pNodeset = result->nodesetval;
    for (int i=0; i!=pNodeset->nodeNr; ++i)
    {
        /*  lint -e(534) */
        nodes.push_back(pNodeset->nodeTab[i]);//no return value
    }
    xmlXPathFreeObject(result);//new add
    return 0;
}

XMLNode* XMLConfig::getFirstChild(XMLNode *curnode)
{
    vector<XMLNode*> nodeset;
    XMLNode *node = NULL;
    if (getChildren(curnode, nodeset) < 0)
    {
        //DEV_LOG(LERROR, OUT_SCR, "getChildren error");
        return NULL;
    }

    if (nodeset.size() > 0)
    {
        node = *(nodeset.begin());
    }
    else
    {
        cerr << "this node has no child" << endl;
        node = NULL;
    }
    return node;
}


XMLNode* XMLConfig::getSpecificChild(XMLNode *curnode, const char* nodeName)
{
    vector<XMLNode*> nodeset;
    if (getChildren(curnode, nodeset) < 0)
    {
        //DEV_LOG(LERROR, OUT_SCR, "getChildren error");
        return NULL;
    }

    vector<XMLNode*>::iterator vit;
    for (vit=nodeset.begin(); vit!=nodeset.end(); ++vit)
    {
        if (!xmlStrcmp((*vit)->name, BAD_CAST nodeName))
        {
            return *vit;
        }
    }
    cerr << "this node has no child named " << nodeName << endl;
    return NULL;
}

int XMLConfig::getSpecificChildren(XMLNode *curnode, const char* nodeName, std::vector<XMLNode*> &nodes)
{
    vector<XMLNode*> children;
   
    getChildren(curnode, children);
    /*
        if (getChildren(curnode, children))// != SUCCESSFUL)
    {
        //DEV_LOG(LERROR, OUT_SCR, "getChildren error");
        return -1;
    }
*/
    vector<XMLNode*>::iterator vit;
    for (vit=children.begin(); vit!=children.end(); ++vit)
    {
        if (!xmlStrcmp((*vit)->name, BAD_CAST nodeName))
        {
            /*  lint -e(534) */
            nodes.push_back(*vit);//no return value
        }
    }
    return 0;
}

int XMLConfig::getChildren(XMLNode *curnode, vector<XMLNode*> &children)
{
    if (NULL == curnode)
    {
        return -1;
    }
    xmlNodePtr child = NULL;
    child = curnode->xmlChildrenNode;
    while (child != NULL)
    {
        if (xmlStrcmp(child->name, BAD_CAST "text"))//如果不是text
        {
            children.push_back(child);
        }
        child = child->next;
    }
    return 0;
}

int XMLConfig::getProperty(XMLNode *node, const char *property, std::string &value)//new
{
    xmlChar *attr = NULL;
    if (xmlHasProp(node, BAD_CAST property))
    {
        attr = xmlGetProp(node, BAD_CAST property);
        value = (const char*)attr;
        xmlFree(attr);
    }
    else
    {
        cerr<<"no such property: "<<property<<endl;
        return -1;
    }
    return 0;
    //need to xmlFree()
}
const char* XMLConfig::getProperty(XMLNode *node, const char *property)
{
    xmlChar *attr = NULL;
    if (xmlHasProp(node, BAD_CAST property))
    {
        attr = xmlGetProp(node, BAD_CAST property);
        /*  lint -e(534) */
        m_Attrs.push_back(attr);//no return value
    }
    else
    {
        cerr<<"no such property: "<<property<<endl;
        attr = NULL;
    }
    return (const char*)attr;
    //need to xmlFree()
}

int XMLConfig::setProperty(XMLNode *node, const char *property, const char *value)
{
    //m_changed = true;
    if (xmlHasProp(node, BAD_CAST property))
    {
        if (NULL == xmlSetProp(node, BAD_CAST property, BAD_CAST value))
        {
            //DEV_LOG(LERROR, OUT_SCR, "xmlSetProp error");
        }
    }
    else
    {
        if (NULL == xmlNewProp(node, BAD_CAST property, BAD_CAST value))
        {
            //DEV_LOG(LERROR, OUT_SCR, "xmlNewProp error");
        }
    }

    return 0;
}

int XMLConfig::delProperty(XMLNode *node, const char *property)
{
    xmlAttrPtr attr = NULL;
    if ((attr=xmlHasProp(node, BAD_CAST property)) != NULL)
    {
        return xmlRemoveProp(attr);//0 if success and -1 in case of error
    }
    else
    {
        cerr<<"no such property: "<<property<<endl;
        return -1;
    }
}

XMLNode* XMLConfig::createXMLBuffer(const char *rootname)
{
    m_XmlType = XMLMEM;
    m_pDoc = xmlNewDoc(BAD_CAST "1.0");
    m_pRoot = xmlNewNode(NULL, BAD_CAST rootname);
    /* lint -e(534) */
    xmlDocSetRootElement(m_pDoc, m_pRoot);//return the old root element if any was found, NULL if root was NULL
    return m_pRoot;
}

XMLNode* XMLConfig::createFile(const char *filename, const char *rootname)
{
    m_XmlType = XMLFILE;
    //m_changed = true;
    m_FileName = filename;
    m_pDoc = xmlNewDoc(BAD_CAST "1.0");
    m_pRoot = xmlNewNode(NULL, BAD_CAST rootname);
    /*  lint -e(534) */
    xmlDocSetRootElement(m_pDoc, m_pRoot);//return the old root element if any was found, NULL if root was NULL
    return m_pRoot;
}

int XMLConfig::saveFile()
{
    return saveFile(m_FileName.c_str());
}

int XMLConfig::saveFile(const char *filename)
{
    int nwrite = xmlSaveFormatFileEnc(filename, m_pDoc, "UTF-8", 1);
    if (nwrite != -1)
    {
        //stringstream logStream;
        //logStream << "write " << nwrite << " bytes into " << filename;
        //DEV_LOG(LINFO, OUT_FILE, logStream.str());
    }
    else
    {
        cerr<<"saveFile error."<<endl;
    }
    return nwrite;
}

XMLNode* XMLConfig::addSimpleChild(XMLNode *parent, const char *nodename, \
                                   const char *value)
{
    //m_changed = true;
    return xmlNewTextChild(parent, NULL, BAD_CAST nodename, BAD_CAST value);
}

XMLNode* XMLConfig::addChild(XMLNode *parent, const char *nodename)
{
    //m_changed = true;
    xmlNodePtr node = xmlNewNode (NULL, BAD_CAST nodename);
    return xmlAddChild(parent, node);
    //return node;
}

XMLNode* XMLConfig::addChild(XMLNode *parent, XMLNode *cur)
{
    xmlNodePtr node = xmlCopyNode(cur, 1);//if 1 do a recursive copy , if 2 copy properties and namespaces
    if (node != NULL)
        return xmlAddChild(parent, node);//return the child or NULL in case of error
    else
    {
        cerr << "copy node error" << endl;
        return NULL;
    }
}
//int XMLConfig::addChildNode(XMLNode *parent, const char *nodename, const char *value)
//{
//    xmlNodePtr node = xmlNewNode (NULL, BAD_CAST nodename);
//    xmlNodePtr content = xmlNewText(BAD_CAST value);
//    xmlAddChild(parent, node);
//    xmlAddChild(node, content);
//    return 0;
//}

//int XMLConfig::moveToNode(XMLNode *node)
//{
//    //if(xmlTextReaderMoveToElement(...) < 0)
//    //{
//    //    return -1;
//    //}
//    return 0;
//}


int XMLConfig::getValue(XMLNode *node, std::string &value)//new
{
    xmlChar *pValue = xmlNodeGetContent(node);
    if ( pValue != NULL)
    {
        value = (const char*)pValue;
        xmlFree(pValue);
        return 0;
    }
    else
    {
        return -1;
    }
}
const char* XMLConfig::getValue(XMLNode *node)
{
    xmlChar *p = xmlNodeGetContent(node);
    /*  lint -e(534) */
    m_Attrs.push_back(p);//no return value
    return (const char*)p;
}

int XMLConfig::setValue(XMLNode *node, const char *value)
{
    xmlNodeSetContent(node, BAD_CAST value);//no return
    //xmlNodePtr content = xmlNewText(BAD_CAST value);
    //xmlAddChild(node, content);
    return 0;
}


int XMLConfig::getFirstNodeValue(const char *xpath, std::string &value)//new
{
    XMLNode *node = NULL;
    if (getFirstNode(xpath, node) < 0)
    {
        //DEV_LOG(LERROR, OUT_SCR, "getFirstNode error");
        return -1;
    }
    if (node != NULL)
        return getValue(node, value);
    else
        return -1;
}

const char* XMLConfig::getFirstNodeValue(const char *xpath)
{
    XMLNode *node = NULL;
    getFirstNode(xpath, node);
    /*
    if (getFirstNode(xpath, node) != SUCCESSFUL)
    {
        //DEV_LOG(LERROR, OUT_SCR, "getFirstNode error");
        return NULL;
    }
    */
    return getValue(node);
}

//void XMLConfig::printProperties(XMLNode *node)
//{
//}

XMLNode* XMLConfig::getRootNode()
{
    return m_pRoot;
}
