/*
 * Copyright 2002,2004,2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
*  This file contains code to build the DOM tree. It registers a document
*  handler with the scanner. In these handler methods, appropriate DOM nodes
*  are created and added to the DOM tree.
*
* $Id: AbstractDOMParser.cpp,v 1.1.1.1 2005/07/29 01:44:53 openns Exp $
*
*/



// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/internal/XMLScannerResolver.hpp>
#include <xercesc/internal/ElemStack.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/framework/XMLNotationDecl.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/framework/psvi/PSVIElement.hpp>
#include <xercesc/framework/psvi/PSVIAttribute.hpp>
#include <xercesc/framework/psvi/PSVIAttributeList.hpp>
#include <xercesc/framework/psvi/XSElementDeclaration.hpp>
#include <xercesc/util/IOException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/impl/DOMAttrImpl.hpp>
#include <xercesc/dom/impl/DOMTypeInfoImpl.hpp>
#include <xercesc/dom/DOMCDATASection.hpp>
#include <xercesc/dom/DOMComment.hpp>
#include <xercesc/dom/impl/DOMTextImpl.hpp>
#include <xercesc/dom/impl/DOMDocumentImpl.hpp>
#include <xercesc/dom/impl/DOMDocumentTypeImpl.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/impl/DOMElementNSImpl.hpp>
#include <xercesc/dom/impl/DOMEntityImpl.hpp>
#include <xercesc/dom/impl/DOMEntityReferenceImpl.hpp>
#include <xercesc/dom/impl/DOMNotationImpl.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMProcessingInstruction.hpp>
#include <xercesc/dom/impl/DOMProcessingInstructionImpl.hpp>
#include <xercesc/dom/impl/DOMNodeIDMap.hpp>
#include <xercesc/validators/common/ContentSpecNode.hpp>
#include <xercesc/validators/common/GrammarResolver.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

XERCES_CPP_NAMESPACE_BEGIN


// ---------------------------------------------------------------------------
//  AbstractDOMParser: Constructors and Destructor
// ---------------------------------------------------------------------------
AbstractDOMParser::AbstractDOMParser( XMLValidator* const   valToAdopt
                                    , MemoryManager* const  manager
                                    , XMLGrammarPool* const gramPool) :

  fCreateEntityReferenceNodes(true)
, fIncludeIgnorableWhitespace(true)
, fWithinElement(false)
, fParseInProgress(false)
, fCreateCommentNodes(true)
, fDocumentAdoptedByUser(false)
, fCreateSchemaInfo(false)
, fScanner(0)
, fImplementationFeatures(0)
, fCurrentParent(0)
, fCurrentNode(0)
, fCurrentEntity(0)
, fDocument(0)
, fNodeStack(0)
, fDocumentType(0)
, fDocumentVector(0)
, fGrammarResolver(0)
, fURIStringPool(0)
, fValidator(valToAdopt)
, fMemoryManager(manager)
, fGrammarPool(gramPool)
, fBufMgr(manager)
, fInternalSubset(fBufMgr.bidOnBuffer())
, fPSVIHandler(0)
{
    try
    {
        initialize();
    }
    catch(const OutOfMemoryException&)
    {
        throw;
    }
    catch(...)
    {
       cleanUp();
       throw;
    }
}


AbstractDOMParser::~AbstractDOMParser()
{
    cleanUp();
}

// ---------------------------------------------------------------------------
//  AbstractDOMParser: Initialize/CleanUp methods
// ---------------------------------------------------------------------------
void AbstractDOMParser::initialize()
{
    //  Create grammar resolver and string pool to pass to the scanner
    fGrammarResolver = new (fMemoryManager) GrammarResolver(fGrammarPool, fMemoryManager);
    fURIStringPool = fGrammarResolver->getStringPool();

    //  Create a scanner and tell it what validator to use. Then set us
    //  as the document event handler so we can fill the DOM document.
    fScanner = XMLScannerResolver::getDefaultScanner(fValidator, fGrammarResolver, fMemoryManager);
    fScanner->setDocHandler(this);
    fScanner->setDocTypeHandler(this);
    fScanner->setURIStringPool(fURIStringPool);

    fNodeStack = new (fMemoryManager) ValueStackOf<DOMNode*>(64, fMemoryManager);
    this->reset();
}

void AbstractDOMParser::cleanUp()
{
    if (fDocumentVector)
        delete fDocumentVector;

    if (!fDocumentAdoptedByUser && fDocument)
        fDocument->release();

    delete fNodeStack;
    delete fScanner;
    delete fGrammarResolver;
    // grammar pool *always* owns this
    //delete fURIStringPool;
    fMemoryManager->deallocate(fImplementationFeatures);

    if (fValidator)
        delete fValidator;
}

// ---------------------------------------------------------------------------
//  AbstractDOMParser: Utilities
// ---------------------------------------------------------------------------
void AbstractDOMParser::reset()
{
    // if fDocument exists already, store the old pointer in the vector for deletion later
    if (fDocument && !fDocumentAdoptedByUser) {
        if (!fDocumentVector) {
            // allocate the vector if not exists yet
            fDocumentVector  = new (fMemoryManager) RefVectorOf<DOMDocumentImpl>(10, true, fMemoryManager) ;
        }
        fDocumentVector->addElement(fDocument);
    }

    fDocument = 0;
    resetDocType();
    fCurrentParent   = 0;
    fCurrentNode     = 0;
    fCurrentEntity   = 0;
    fParseInProgress = false;
    fWithinElement   = false;
    fDocumentAdoptedByUser = false;
    fNodeStack->removeAllElements();
    fInternalSubset.reset();
}


void AbstractDOMParser::resetPool()
{
    //  We cannot enter here while a regular parse is in progress.
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    if (fDocumentVector)
        fDocumentVector->removeAllElements();

    if (!fDocumentAdoptedByUser && fDocument)
        fDocument->release();

    fDocument = 0;
}

bool AbstractDOMParser::isDocumentAdopted() const
{
    return fDocumentAdoptedByUser;
}

DOMDocument* AbstractDOMParser::adoptDocument()
{
    fDocumentAdoptedByUser = true;
    return fDocument;
}


// ---------------------------------------------------------------------------
//  AbstractDOMParser: Getter methods
// ---------------------------------------------------------------------------
DOMDocument* AbstractDOMParser::getDocument()
{
    return fDocument;
}

const XMLValidator& AbstractDOMParser::getValidator() const
{
    return *fScanner->getValidator();
}

bool AbstractDOMParser::getDoNamespaces() const
{
    return fScanner->getDoNamespaces();
}

bool AbstractDOMParser::getGenerateSyntheticAnnotations() const
{
    return fScanner->getGenerateSyntheticAnnotations();
}

bool AbstractDOMParser::getValidateAnnotations() const
{
    return fScanner->getValidateAnnotations();
}

bool AbstractDOMParser::getExitOnFirstFatalError() const
{
    return fScanner->getExitOnFirstFatal();
}

bool AbstractDOMParser::getValidationConstraintFatal() const
{
    return fScanner->getValidationConstraintFatal();
}

AbstractDOMParser::ValSchemes AbstractDOMParser::getValidationScheme() const
{
    const XMLScanner::ValSchemes scheme = fScanner->getValidationScheme();

    if (scheme == XMLScanner::Val_Always)
        return Val_Always;
    else if (scheme == XMLScanner::Val_Never)
        return Val_Never;

    return Val_Auto;
}

bool AbstractDOMParser::getDoSchema() const
{
    return fScanner->getDoSchema();
}

bool AbstractDOMParser::getValidationSchemaFullChecking() const
{
    return fScanner->getValidationSchemaFullChecking();
}

bool AbstractDOMParser::getIdentityConstraintChecking() const
{
    return fScanner->getIdentityConstraintChecking();
}

int AbstractDOMParser::getErrorCount() const
{
    return fScanner->getErrorCount();
}

XMLCh* AbstractDOMParser::getExternalSchemaLocation() const
{
    return fScanner->getExternalSchemaLocation();
}

XMLCh* AbstractDOMParser::getExternalNoNamespaceSchemaLocation() const
{
    return fScanner->getExternalNoNamespaceSchemaLocation();
}

SecurityManager* AbstractDOMParser::getSecurityManager() const
{
    return fScanner->getSecurityManager();
}

bool AbstractDOMParser::getLoadExternalDTD() const
{
    return fScanner->getLoadExternalDTD();
}

bool AbstractDOMParser::getCalculateSrcOfs() const
{
    return fScanner->getCalculateSrcOfs();
}

bool AbstractDOMParser::getStandardUriConformant() const
{
    return fScanner->getStandardUriConformant();
}


// ---------------------------------------------------------------------------
//  AbstractDOMParser: Setter methods
// ---------------------------------------------------------------------------
void AbstractDOMParser::setPSVIHandler(PSVIHandler* const handler)
{
    fPSVIHandler = handler;
    if (fPSVIHandler) {
        fScanner->setPSVIHandler(this);
    }
    else if(!fCreateSchemaInfo) {
        fScanner->setPSVIHandler(0);        
    }
}


void AbstractDOMParser::setDoNamespaces(const bool newState)
{
    fScanner->setDoNamespaces(newState);
}

void AbstractDOMParser::setGenerateSyntheticAnnotations(const bool newState)
{
    fScanner->setGenerateSyntheticAnnotations(newState);
}

void AbstractDOMParser::setValidateAnnotations(const bool newState)
{
    fScanner->setValidateAnnotations(newState);
}

void AbstractDOMParser::setExitOnFirstFatalError(const bool newState)
{
    fScanner->setExitOnFirstFatal(newState);
}

void AbstractDOMParser::setValidationConstraintFatal(const bool newState)
{
    fScanner->setValidationConstraintFatal(newState);
}

void AbstractDOMParser::setValidationScheme(const ValSchemes newScheme)
{
    if (newScheme == Val_Never)
        fScanner->setValidationScheme(XMLScanner::Val_Never);
    else if (newScheme == Val_Always)
        fScanner->setValidationScheme(XMLScanner::Val_Always);
    else
        fScanner->setValidationScheme(XMLScanner::Val_Auto);
}

void AbstractDOMParser::setDoSchema(const bool newState)
{
    fScanner->setDoSchema(newState);
}

void AbstractDOMParser::setValidationSchemaFullChecking(const bool schemaFullChecking)
{
    fScanner->setValidationSchemaFullChecking(schemaFullChecking);
}

void AbstractDOMParser::setIdentityConstraintChecking(const bool identityConstraintChecking)
{
    fScanner->setIdentityConstraintChecking(identityConstraintChecking);
}

void AbstractDOMParser::setExternalSchemaLocation(const XMLCh* const schemaLocation)
{
    fScanner->setExternalSchemaLocation(schemaLocation);
}
void AbstractDOMParser::setExternalNoNamespaceSchemaLocation(const XMLCh* const noNamespaceSchemaLocation)
{
    fScanner->setExternalNoNamespaceSchemaLocation(noNamespaceSchemaLocation);
}

void AbstractDOMParser::setExternalSchemaLocation(const char* const schemaLocation)
{
    fScanner->setExternalSchemaLocation(schemaLocation);
}
void AbstractDOMParser::setExternalNoNamespaceSchemaLocation(const char* const noNamespaceSchemaLocation)
{
    fScanner->setExternalNoNamespaceSchemaLocation(noNamespaceSchemaLocation);
}

void AbstractDOMParser::setSecurityManager(SecurityManager* const securityManager)
{
    // since this could impact various components, don't permit it to change
    // during a parse
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    fScanner->setSecurityManager(securityManager);
}

void AbstractDOMParser::setLoadExternalDTD(const bool newState)
{
    fScanner->setLoadExternalDTD(newState);
}

void AbstractDOMParser::setCalculateSrcOfs(const bool newState)
{
    fScanner->setCalculateSrcOfs(newState);
}

void AbstractDOMParser::setStandardUriConformant(const bool newState)
{
    fScanner->setStandardUriConformant(newState);
}

void AbstractDOMParser::useScanner(const XMLCh* const scannerName)
{
    XMLScanner* tempScanner = XMLScannerResolver::resolveScanner
    (
        scannerName
        , fValidator
        , fGrammarResolver
        , fMemoryManager
    );

    if (tempScanner) {

        tempScanner->setParseSettings(fScanner);
        tempScanner->setURIStringPool(fURIStringPool);
        delete fScanner;
        fScanner = tempScanner;
    }
}

void AbstractDOMParser::setCreateSchemaInfo(const bool create)
{
    fCreateSchemaInfo = create;
    if(fCreateSchemaInfo)
        fScanner->setPSVIHandler(this);
    else if(!fPSVIHandler)
        fScanner->setPSVIHandler(0);
}

// ---------------------------------------------------------------------------
//  AbstractDOMParser: Parsing methods
// ---------------------------------------------------------------------------
void AbstractDOMParser::parse(const InputSource& source)
{
    // Avoid multiple entrance
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    try
    {
        fParseInProgress = true;
        fScanner->scanDocument(source);
        fParseInProgress = false;
    }
    catch(const OutOfMemoryException&)
    {
        throw;
    }    
    catch(...)
    {
        fParseInProgress = false;
        throw;
    }
}

void AbstractDOMParser::parse(const XMLCh* const systemId)
{
    // Avoid multiple entrance
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    try
    {
        fParseInProgress = true;
        fScanner->scanDocument(systemId);
        fParseInProgress = false;
    }
    catch(const OutOfMemoryException&)
    {
        throw;
    }
    catch(...)
    {
        fParseInProgress = false;
        throw;
    }
}

void AbstractDOMParser::parse(const char* const systemId)
{
    // Avoid multiple entrance
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    try
    {
        fParseInProgress = true;
        fScanner->scanDocument(systemId);
        fParseInProgress = false;
    }
    catch(const OutOfMemoryException&)
    {
        throw;
    }
    catch(...)
    {
        fParseInProgress = false;
        throw;
    }
}



// ---------------------------------------------------------------------------
//  AbstractDOMParser: Progressive parse methods
// ---------------------------------------------------------------------------
bool AbstractDOMParser::parseFirst( const XMLCh* const    systemId
                                   ,       XMLPScanToken&  toFill)
{
    //
    //  Avoid multiple entrance. We cannot enter here while a regular parse
    //  is in progress.
    //
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    return fScanner->scanFirst(systemId, toFill);
}

bool AbstractDOMParser::parseFirst( const char* const         systemId
                                   ,       XMLPScanToken&      toFill)
{
    //
    //  Avoid multiple entrance. We cannot enter here while a regular parse
    //  is in progress.
    //
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    return fScanner->scanFirst(systemId, toFill);
}

bool AbstractDOMParser::parseFirst( const InputSource& source
                                   ,       XMLPScanToken&  toFill)
{
    //
    //  Avoid multiple entrance. We cannot enter here while a regular parse
    //  is in progress.
    //
    if (fParseInProgress)
        ThrowXMLwithMemMgr(IOException, XMLExcepts::Gen_ParseInProgress, fMemoryManager);

    return fScanner->scanFirst(source, toFill);
}

bool AbstractDOMParser::parseNext(XMLPScanToken& token)
{
    return fScanner->scanNext(token);
}

void AbstractDOMParser::parseReset(XMLPScanToken& token)
{
    // Reset the scanner, and then reset the parser
    fScanner->scanReset(token);
    reset();
}


// ---------------------------------------------------------------------------
//  AbstractDOMParser: Implementation of PSVIHandler interface
// ---------------------------------------------------------------------------
void AbstractDOMParser::handleElementPSVI(const XMLCh* const            localName 
                                        , const XMLCh* const            uri
                                        ,       PSVIElement *           elementInfo)
{
    // associate the info now; if the user wants, she can override what we did
    if(fCreateSchemaInfo)
    {
        DOMTypeInfoImpl* typeInfo=new (getDocument()) DOMTypeInfoImpl();
        typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Validity, elementInfo->getValidity());
        typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Validitation_Attempted, elementInfo->getValidationAttempted());
        if(elementInfo->getTypeDefinition())
        {
            typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Type, elementInfo->getTypeDefinition()->getTypeCategory());
            typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Anonymous, elementInfo->getTypeDefinition()->getAnonymous());
            typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Namespace, 
                fDocument->getPooledString(elementInfo->getTypeDefinition()->getNamespace()));
            typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Name, 
                fDocument->getPooledString(elementInfo->getTypeDefinition()->getName()));
        }
        else if(elementInfo->getValidity()==PSVIItem::VALIDITY_VALID)
        {
            // if we are valid but we don't have a type validator, we are xs:anyType
            typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Type, XSTypeDefinition::COMPLEX_TYPE);
            typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Anonymous, false);
            typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Namespace, SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
            typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Name, SchemaSymbols::fgATTVAL_ANYTYPE);
        }
        if(elementInfo->getMemberTypeDefinition())
        {
            typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Anonymous, elementInfo->getMemberTypeDefinition()->getAnonymous());
            typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Namespace, 
                fDocument->getPooledString(elementInfo->getMemberTypeDefinition()->getNamespace()));
            typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Name, 
                fDocument->getPooledString(elementInfo->getMemberTypeDefinition()->getName()));
        }
        if(elementInfo->getElementDeclaration())
            typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Nil, elementInfo->getElementDeclaration()->getNillable());
        typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Schema_Default, fDocument->getPooledString(elementInfo->getSchemaDefault()));
        typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Schema_Normalized_Value, fDocument->getPooledString(elementInfo->getSchemaNormalizedValue()));
        typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Schema_Specified, true);
        ((DOMElementNSImpl*)fCurrentParent)->setTypeInfo(typeInfo);
    }
    if(fPSVIHandler)
        fPSVIHandler->handleElementPSVI(localName, uri, elementInfo);
}

void AbstractDOMParser::handlePartialElementPSVI(const XMLCh* const            localName 
                                               , const XMLCh* const            uri
                                               ,       PSVIElement *           elementInfo)
{
    if(fPSVIHandler)
        fPSVIHandler->handlePartialElementPSVI(localName, uri, elementInfo);
}

void AbstractDOMParser::handleAttributesPSVI( const XMLCh* const            localName 
                                            , const XMLCh* const            uri
                                            ,       PSVIAttributeList *     psviAttributes)
{
    if(fCreateSchemaInfo)
    {
        for (unsigned int index=0; index < psviAttributes->getLength(); index++) {
            XERCES_CPP_NAMESPACE_QUALIFIER PSVIAttribute *attrInfo=psviAttributes->getAttributePSVIAtIndex(index);
            XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* pAttrNode=fCurrentNode->getAttributes()->getNamedItemNS(psviAttributes->getAttributeNamespaceAtIndex(index),
                                                                                                            psviAttributes->getAttributeNameAtIndex(index));
            if(pAttrNode!=NULL)
            {
                DOMTypeInfoImpl* typeInfo=new (getDocument()) DOMTypeInfoImpl();
                typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Validity, attrInfo->getValidity());
                typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Validitation_Attempted, attrInfo->getValidationAttempted());
                if(attrInfo->getTypeDefinition())
                {
                    typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Type, XSTypeDefinition::SIMPLE_TYPE);
                    typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Anonymous, attrInfo->getTypeDefinition()->getAnonymous());
                    typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Namespace, 
                        fDocument->getPooledString(attrInfo->getTypeDefinition()->getNamespace()));
                    typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Name, 
                        fDocument->getPooledString(attrInfo->getTypeDefinition()->getName()));
                }
                else if(attrInfo->getValidity()==PSVIItem::VALIDITY_VALID)
                {
                    // if we are valid but we don't have a type validator, we are xs:anySimpleType
                    typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Type, XSTypeDefinition::SIMPLE_TYPE);
                    typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Anonymous, false);
                    typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Namespace, SchemaSymbols::fgURI_SCHEMAFORSCHEMA);
                    typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Type_Definition_Name, SchemaSymbols::fgDT_ANYSIMPLETYPE);
                }
                if(attrInfo->getMemberTypeDefinition())
                {
                    typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Anonymous, attrInfo->getMemberTypeDefinition()->getAnonymous());
                    typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Namespace, 
                        fDocument->getPooledString(attrInfo->getMemberTypeDefinition()->getNamespace()));
                    typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Member_Type_Definition_Name, 
                        fDocument->getPooledString(attrInfo->getMemberTypeDefinition()->getName()));
                }
                typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Schema_Default, fDocument->getPooledString(attrInfo->getSchemaDefault()));
                typeInfo->setStringProperty(DOMPSVITypeInfo::PSVI_Schema_Normalized_Value, fDocument->getPooledString(attrInfo->getSchemaNormalizedValue()));
                typeInfo->setNumericProperty(DOMPSVITypeInfo::PSVI_Schema_Specified, true);
                ((DOMAttrImpl*)pAttrNode)->setTypeInfo(typeInfo);
            }
        }
    }
    // associate the info now; if the user wants, she can override what we did
    if(fPSVIHandler)
        fPSVIHandler->handleAttributesPSVI(localName, uri, psviAttributes);
}

// ---------------------------------------------------------------------------
//  AbstractDOMParser: Implementation of XMLDocumentHandler interface
// ---------------------------------------------------------------------------
void AbstractDOMParser::docCharacters(  const   XMLCh* const    chars
                              , const unsigned int    length
                              , const bool cdataSection)
{
    // Ignore chars outside of content
    if (!fWithinElement)
        return;

    // revisit.  Is it really safe to null-terminate here?
    //                Does the scanner do it already?
    //                If scanner goes up to the very end of an unterminated
    //                buffer, we may be stepping on something bad here.
    //           Probably best to modify the scanner to null terminate.
    XMLCh savedChar = chars[length];
    XMLCh *ncChars  = (XMLCh *)chars;   // cast off const
    ncChars[length] = 0;
    if (cdataSection == true)
    {
        DOMCDATASection *node = fDocument->createCDATASection(chars);
        fCurrentParent->appendChild(node);
        fCurrentNode = node;
    }
    else
    {
        if (fCurrentNode->getNodeType() == DOMNode::TEXT_NODE)
        {
            DOMText *node = (DOMText *)fCurrentNode;
            node->appendData(chars);
        }
        else
        {
            DOMText *node = fDocument->createTextNode(chars);
            fCurrentParent->appendChild(node);
            fCurrentNode = node;
        }
    }
    ncChars[length] = savedChar;
    return;
}


void AbstractDOMParser::docComment(const XMLCh* const comment)
{
    if (fCreateCommentNodes) {
        DOMComment *dcom = fDocument->createComment(comment);
        fCurrentParent->appendChild(dcom);
        fCurrentNode = dcom;
    }
}


void AbstractDOMParser::docPI(  const   XMLCh* const    target
                      , const XMLCh* const    data)
{
    DOMProcessingInstruction *pi = fDocument->createProcessingInstruction
        (
        target
        , data
        );
    fCurrentParent->appendChild(pi);
    fCurrentNode = pi;
}


void AbstractDOMParser::endEntityReference(const XMLEntityDecl&)
{
    if (!fCreateEntityReferenceNodes) return;

    DOMEntityReferenceImpl *erImpl = 0;
    DOMNode* firstChild = 0;

    if (fCurrentParent->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
        erImpl = (DOMEntityReferenceImpl *) fCurrentParent;
        firstChild = erImpl->getFirstChild();
    }

    fCurrentParent = fNodeStack->pop();

    fCurrentNode   = fCurrentParent;

    if (erImpl)
        erImpl->setReadOnly(true, true);
}


void AbstractDOMParser::endElement( const   XMLElementDecl&
                           , const unsigned int
                           , const bool
                           , const XMLCh* const)
{
    fCurrentNode   = fCurrentParent;
    fCurrentParent = fNodeStack->pop();

    // If we've hit the end of content, clear the flag
    if (fNodeStack->empty())
        fWithinElement = false;
}

void AbstractDOMParser::elementTypeInfo( const   XMLCh * const  typeName
                           , const XMLCh *const                 typeURI)
{
}


void AbstractDOMParser::ignorableWhitespace(const   XMLCh* const    chars
                                    , const unsigned int    length
                                    , const bool)
{
    // Ignore chars before the root element
    if (!fWithinElement || !fIncludeIgnorableWhitespace)
        return;

    // revisit.  Not safe to slam in a null like this.
    XMLCh savedChar = chars[length];
    XMLCh *ncChars  = (XMLCh *)chars;   // cast off const
    ncChars[length] = chNull;

    if (fCurrentNode->getNodeType() == DOMNode::TEXT_NODE)
    {
        DOMText *node = (DOMText *)fCurrentNode;
        node->appendData(chars);
    }
    else
    {
        DOMTextImpl *node = (DOMTextImpl *)fDocument->createTextNode(chars);
        node->setIgnorableWhitespace(true);
        fCurrentParent->appendChild(node);

        fCurrentNode = node;
    }
    ncChars[length] = savedChar;
}


void AbstractDOMParser::resetDocument()
{
    //
    //  The reset methods are called before a new parse event occurs.
    //  Reset this parsers state to clear out anything that may be left
    //  from a previous use, in particular the DOM document itself.
    //
    this->reset();
}


void AbstractDOMParser::startDocument()
{
    if(fImplementationFeatures == 0)
        fDocument = (DOMDocumentImpl *)DOMImplementation::getImplementation()->createDocument(fMemoryManager);
    else 
        fDocument = (DOMDocumentImpl *)DOMImplementationRegistry::getDOMImplementation(fImplementationFeatures)->createDocument(fMemoryManager);

    // Just set the document as the current parent and current node
    fCurrentParent = fDocument;
    fCurrentNode   = fDocument;
    // set DOM error checking off
    fDocument->setErrorChecking(false);
    fDocument->setDocumentURI(fScanner->getLocator()->getSystemId());
    fDocument->setActualEncoding(fScanner->getReaderMgr()->getCurrentEncodingStr());
}


void AbstractDOMParser::endDocument()
{
    // set DOM error checking back on
    fDocument->setErrorChecking(true);

    // DOM L2 does not support editing DocumentType nodes
    if (fDocumentType && fScanner -> getDoNamespaces())
        fDocumentType->setReadOnly(true, true);
}


void AbstractDOMParser::startElement(const  XMLElementDecl&         elemDecl
                             , const unsigned int            urlId
                             , const XMLCh* const            elemPrefix
                             , const RefVectorOf<XMLAttr>&   attrList
                             , const unsigned int            attrCount
                             , const bool                    isEmpty
                             , const bool                    isRoot)
{
    DOMElement     *elem;
    DOMElementImpl *elemImpl;
    const XMLCh* namespaceURI = 0;

    //get the list for use in the loop
    XMLAttDefList* defAttrs = 0;
    if(elemDecl.hasAttDefs()) {
        defAttrs = &elemDecl.getAttDefList();
    }

    if (fScanner -> getDoNamespaces()) {    //DOM Level 2, doNamespaces on

        if (urlId != fScanner->getEmptyNamespaceId()) {  //TagName has a prefix

            namespaceURI = fScanner->getURIText(urlId); //get namespaceURI

            if (elemPrefix && *elemPrefix) {

                XMLBufBid elemQName(&fBufMgr);

                elemQName.set(elemPrefix);
                elemQName.append(chColon);
                elemQName.append(elemDecl.getBaseName());
                elem = createElementNSNode(namespaceURI, elemQName.getRawBuffer());
            }
            else { 
                elem = createElementNSNode(namespaceURI, elemDecl.getBaseName());
            }
        } 
        else {
            elem = createElementNSNode(namespaceURI, elemDecl.getBaseName());
        }

        elemImpl = (DOMElementImpl *) elem;
    }
    else {    //DOM Level 1
        elem = fDocument->createElement(elemDecl.getFullName());
        elemImpl = (DOMElementImpl *) elem;
    }
    for (unsigned int index = 0; index < attrCount; ++index) {
        const XMLAttr* oneAttrib = attrList.elementAt(index);
        DOMAttrImpl *attr = 0;
        DOMNode* remAttr = 0;

        //  revisit.  Optimize to init the named node map to the
        //            right size up front.
        if (fScanner -> getDoNamespaces()) {    //DOM Level 2, doNamespaces on
            unsigned int attrURIId = oneAttrib -> getURIId();
            namespaceURI = 0;            
            if (XMLString::equals(oneAttrib -> getName(), XMLUni::fgXMLNSString)) {   //for xmlns=...
                attrURIId = fScanner->getXMLNSNamespaceId();                
            }
            if (attrURIId != fScanner->getEmptyNamespaceId()) {  //TagName has a prefix
                namespaceURI = fScanner->getURIText(attrURIId);   //get namespaceURI
            }
            attr = (DOMAttrImpl *)fDocument->createAttributeNS(namespaceURI, oneAttrib->getQName());
            remAttr = elemImpl->setAttributeNodeNS(attr);
        }
        else {
            attr = (DOMAttrImpl *)fDocument->createAttribute(oneAttrib->getName());
            remAttr = elemImpl->setAttributeNode(attr);
        }
        attr->setValue(oneAttrib -> getValue());
        if (remAttr)
            remAttr->release();

        // Attributes of type ID.  If this is one, add it to the hashtable of IDs
        //   that is constructed for use by GetElementByID().
        //
        if (oneAttrib->getType()==XMLAttDef::ID)
        {
            if (fDocument->fNodeIDMap == 0)
                fDocument->fNodeIDMap = new (fDocument) DOMNodeIDMap(500, fDocument);
            fDocument->fNodeIDMap->add(attr);
            attr->fNode.isIdAttr(true);
        }

        attr->setSpecified(oneAttrib->getSpecified());

        // store DTD validation information
        if(fCreateSchemaInfo)
        {
            switch(oneAttrib->getType())
            {
            case XMLAttDef::CData:          attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedCDATAAttribute); break;
            case XMLAttDef::ID:             attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedIDAttribute); break;
            case XMLAttDef::IDRef:          attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedIDREFAttribute); break;
            case XMLAttDef::IDRefs:         attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedIDREFSAttribute); break;
            case XMLAttDef::Entity:         attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedENTITYAttribute); break;
            case XMLAttDef::Entities:       attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedENTITIESAttribute); break;
            case XMLAttDef::NmToken:        attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedNMTOKENAttribute); break;
            case XMLAttDef::NmTokens:       attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedNMTOKENSAttribute); break;
            case XMLAttDef::Notation:       attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedNOTATIONAttribute); break;
            case XMLAttDef::Enumeration:    attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedENUMERATIONAttribute); break;
            default:                        attr->setTypeInfo(&DOMTypeInfoImpl::g_DtdNotValidatedAttribute); break;
            }
        }
    }

    // set up the default attributes
	if (defAttrs != 0)
	{		
        XMLAttDef* attr = 0;

        DOMAttrImpl * insertAttr = 0;

        for(unsigned int i=0; i<defAttrs->getAttDefCount(); i++)
        {
            attr = &defAttrs->getAttDef(i);

            const XMLAttDef::DefAttTypes defType = attr->getDefaultType();
            if ((defType == XMLAttDef::Default)
            ||  (defType == XMLAttDef::Fixed))
            {

                if (fScanner->getDoNamespaces())
                {
                    // DOM Level 2 wants all namespace declaration attributes
                    // to be bound to "http://www.w3.org/2000/xmlns/"
                    // So as long as the XML parser doesn't do it, it needs to
                    // done here.
                    const XMLCh* qualifiedName = attr->getFullName();
                    XMLBufBid bbPrefixQName(&fBufMgr);
                    XMLBuffer& prefixBuf = bbPrefixQName.getBuffer();
                    int colonPos = -1;
                    unsigned int uriId = fScanner->resolveQName(qualifiedName, prefixBuf, ElemStack::Mode_Attribute, colonPos);

                    const XMLCh* namespaceURI = 0;
                    if (XMLString::equals(qualifiedName, XMLUni::fgXMLNSString))    //for xmlns=...
                        uriId = fScanner->getXMLNSNamespaceId();
                    if (uriId != fScanner->getEmptyNamespaceId()) {  //TagName has a prefix
                        namespaceURI = fScanner->getURIText(uriId);
                    }

                    insertAttr = (DOMAttrImpl *) fDocument->createAttributeNS(namespaceURI,     // NameSpaceURI
                                                                              qualifiedName);   // qualified name

                    DOMAttr* remAttr = elemImpl->setDefaultAttributeNodeNS(insertAttr);
                    if (remAttr)
                        remAttr->release();
                }
                else
                {
                    // Namespaces is turned off...
                    insertAttr = (DOMAttrImpl *) fDocument->createAttribute(attr->getFullName());

                    DOMNode* remAttr = elemImpl->setDefaultAttributeNode(insertAttr);
                    if (remAttr)
                        remAttr->release();
                }
                //need to do this before the get as otherwise we overwrite any value in the attr
                if (attr->getValue() != 0)
                {
                    insertAttr->setValue(attr->getValue());
                    insertAttr->setSpecified(false);
                }

                // store DTD validation information
                if(fCreateSchemaInfo)
                {
                    switch(attr->getType())
                    {
                    case XMLAttDef::CData:          insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedCDATAAttribute); break;
                    case XMLAttDef::ID:             insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedIDAttribute); break;
                    case XMLAttDef::IDRef:          insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedIDREFAttribute); break;
                    case XMLAttDef::IDRefs:         insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedIDREFSAttribute); break;
                    case XMLAttDef::Entity:         insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedENTITYAttribute); break;
                    case XMLAttDef::Entities:       insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedENTITIESAttribute); break;
                    case XMLAttDef::NmToken:        insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedNMTOKENAttribute); break;
                    case XMLAttDef::NmTokens:       insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedNMTOKENSAttribute); break;
                    case XMLAttDef::Notation:       insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedNOTATIONAttribute); break;
                    case XMLAttDef::Enumeration:    insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdValidatedENUMERATIONAttribute); break;
                    default:                        insertAttr->setTypeInfo(&DOMTypeInfoImpl::g_DtdNotValidatedAttribute); break;
                    }
                }
            }

            insertAttr = 0;
            attr->reset();
        }
    }


    fCurrentParent->appendChild(elem);

    fNodeStack->push(fCurrentParent);
    fCurrentParent = elem;
    fCurrentNode = elem;
    fWithinElement = true;

    // If an empty element, do end right now (no endElement() will be called)
    if (isEmpty)
        endElement(elemDecl, urlId, isRoot, elemPrefix);
}


void AbstractDOMParser::startEntityReference(const XMLEntityDecl& entDecl)
{
    const XMLCh * entName = entDecl.getName();
    DOMNamedNodeMap *entities = fDocumentType->getEntities();
    DOMEntityImpl* entity = (DOMEntityImpl*)entities->getNamedItem(entName);
    if (entity)
        entity->setActualEncoding(fScanner->getReaderMgr()->getCurrentEncodingStr());
    fCurrentEntity = entity;


    // Following line has been moved up so that erImpl is only declared
    // and used if create entity ref flag is true
    if (fCreateEntityReferenceNodes == true)    {
        DOMEntityReference *er = fDocument->createEntityReferenceByParser(entName);

        //set the readOnly flag to false before appending node, will be reset 
        // in endEntityReference
        DOMEntityReferenceImpl *erImpl = (DOMEntityReferenceImpl *) er;
        erImpl->setReadOnly(false, true);

        fCurrentParent->appendChild(er);

        fNodeStack->push(fCurrentParent);
        fCurrentParent = er;
        fCurrentNode = er;

    // this entityRef needs to be stored in Entity map too.
    // We'd decide later whether the entity nodes should be created by a
    // separated method in parser or not. For now just stick it in if
    // the ref nodes are created
        if (entity)
            entity->setEntityRef(er);
    }
}


void AbstractDOMParser::XMLDecl(const   XMLCh* const version
                                , const XMLCh* const encoding
                                , const XMLCh* const standalone
                                , const XMLCh* const actualEncStr)
{
    fDocument->setStandalone(XMLString::equals(XMLUni::fgYesString, standalone));

    fDocument->setVersion(version);
    fDocument->setEncoding(encoding);
    fDocument->setActualEncoding(actualEncStr);
}

// ---------------------------------------------------------------------------
//  AbstractDOMParser: Helper methods
// ---------------------------------------------------------------------------
DOMElement* AbstractDOMParser::createElementNSNode(const XMLCh *namespaceURI,
                                                   const XMLCh *qualifiedName)
{
    return fDocument->createElementNS(namespaceURI, qualifiedName);
}
// ---------------------------------------------------------------------------
//  AbstractDOMParser: Deprecated methods
// ---------------------------------------------------------------------------
bool AbstractDOMParser::getDoValidation() const
{
    //
    //  We don't want to tie the public parser classes to the enum used
    //  by the scanner, so we use a separate one and map.
    //
    //  DON'T mix the new and old methods!!
    //
    const XMLScanner::ValSchemes scheme = fScanner->getValidationScheme();
    if (scheme == XMLScanner::Val_Always)
        return true;
    return false;
}

void AbstractDOMParser::setDoValidation(const bool newState)
{
    fScanner->setDoValidation
    (
        newState ? XMLScanner::Val_Always : XMLScanner::Val_Never
    );
}

//doctypehandler interfaces
void AbstractDOMParser::attDef
(
    const   DTDElementDecl&     elemDecl
    , const DTDAttDef&          attDef
    , const bool
)
{	
    if (fDocumentType->isIntSubsetReading())
    {
        if (elemDecl.hasAttDefs())
        {
            fInternalSubset.append(attDef.getFullName());

            // Get the type and display it
            const XMLAttDef::AttTypes type = attDef.getType();
            switch(type)
            {
            case XMLAttDef::CData :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgCDATAString);
                break;
            case XMLAttDef::ID :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgIDString);
                break;
            case XMLAttDef::IDRef :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgIDRefString);
                break;
            case XMLAttDef::IDRefs :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgIDRefsString);
                break;
            case XMLAttDef::Entity :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgEntityString);
                break;
            case XMLAttDef::Entities :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgEntitiesString);
                break;
            case XMLAttDef::NmToken :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgNmTokenString);
                break;
            case XMLAttDef::NmTokens :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgNmTokensString);
                break;

            case XMLAttDef::Notation :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgNotationString);
                break;

            case XMLAttDef::Enumeration :
                fInternalSubset.append(chSpace);
                const XMLCh* enumString = attDef.getEnumeration();
                int length = XMLString::stringLen(enumString);
                if (length > 0) {

                    fInternalSubset.append(chOpenParen );
                    for(int i=0; i<length; i++) {
                        if (enumString[i] == chSpace)
                            fInternalSubset.append(chPipe);
                        else
                            fInternalSubset.append(enumString[i]);
                    }
                    fInternalSubset.append(chCloseParen);
                }
                break;
            }
            //get te default types of the attlist
            const XMLAttDef::DefAttTypes def = attDef.getDefaultType();
            switch(def)
            {
            case XMLAttDef::Required :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgRequiredString);
                break;
            case XMLAttDef::Implied :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgImpliedString);
                break;
            case XMLAttDef::Fixed :
                fInternalSubset.append(chSpace);
                fInternalSubset.append(XMLUni::fgFixedString);
                break;
            }

            const XMLCh* defaultValue = attDef.getValue();
            if (defaultValue != 0) {
                fInternalSubset.append(chSpace);
                fInternalSubset.append(chDoubleQuote);
                fInternalSubset.append(defaultValue);
                fInternalSubset.append(chDoubleQuote);
            }
        }
    }
}

void AbstractDOMParser::doctypeComment
(
    const   XMLCh* const    comment
)
{
    if (fDocumentType->isIntSubsetReading())
    {
        if (comment != 0)
        {
            fInternalSubset.append(XMLUni::fgCommentString);
            fInternalSubset.append(chSpace);
            fInternalSubset.append(comment);
            fInternalSubset.append(chSpace);
            fInternalSubset.append(chDash);
            fInternalSubset.append(chDash);
            fInternalSubset.append(chCloseAngle);
        }
    }
}

void AbstractDOMParser::doctypeDecl
(
    const   DTDElementDecl& elemDecl
    , const XMLCh* const    publicId
    , const XMLCh* const    systemId
    , const bool
    , const bool
)
{
    fDocumentType = (DOMDocumentTypeImpl *) fDocument->createDocumentType(elemDecl.getFullName(), publicId, systemId);
    fDocument->setDocumentType(fDocumentType);

}

void AbstractDOMParser::doctypePI
(
    const   XMLCh* const    target
    , const XMLCh* const    data
)
{
    if (fDocumentType->isIntSubsetReading())
    {
        //add these chars to internalSubset variable
        fInternalSubset.append(chOpenAngle);
        fInternalSubset.append(chQuestion);
        fInternalSubset.append(target);
        fInternalSubset.append(chSpace);
        fInternalSubset.append(data);
        fInternalSubset.append(chQuestion);
        fInternalSubset.append(chCloseAngle);
    }	
}


void AbstractDOMParser::doctypeWhitespace
(
    const   XMLCh* const    chars
    , const unsigned int
)
{
    if (fDocumentType->isIntSubsetReading())
        fInternalSubset.append(chars);
}

void AbstractDOMParser::elementDecl
(
    const   DTDElementDecl& decl
    , const bool
)
{
    if (fDocumentType->isIntSubsetReading())
    {
        fInternalSubset.append(chOpenAngle);
        fInternalSubset.append(chBang);
        fInternalSubset.append(XMLUni::fgElemString);
        fInternalSubset.append(chSpace);
        fInternalSubset.append(decl.getFullName());

        //get the ContentSpec information
        const XMLCh* contentModel = decl.getFormattedContentModel();
        if (contentModel != 0) {
            fInternalSubset.append(chSpace);
            fInternalSubset.append(contentModel);
        }

        fInternalSubset.append(chCloseAngle);
    }
}

void AbstractDOMParser::endAttList
(
    const   DTDElementDecl& elemDecl
)
{
    if (fDocumentType->isIntSubsetReading())
    {
        //print the closing angle
        fInternalSubset.append(chCloseAngle);
    }

	// this section sets up default attributes.
	// default attribute nodes are stored in a NamedNodeMap DocumentTypeImpl::elements
	// default attribute data attached to the document is used to conform to the
	// DOM spec regarding creating element nodes & removing attributes with default values
	// see DocumentTypeImpl
	if (elemDecl.hasAttDefs())
	{		
        XMLAttDefList* defAttrs = &elemDecl.getAttDefList();
        XMLAttDef* attr = 0;

        DOMAttrImpl * insertAttr = 0;
        DOMElement     *elem  = fDocument->createElement(elemDecl.getFullName());
        DOMElementImpl *elemImpl = (DOMElementImpl *) elem;

        for(unsigned int i=0; i<defAttrs->getAttDefCount(); i++)
        {
            attr = &defAttrs->getAttDef(i);
            if (attr->getValue() != 0)
            {
                if (fScanner->getDoNamespaces())
                {
                    // DOM Level 2 wants all namespace declaration attributes
                    // to be bound to "http://www.w3.org/2000/xmlns/"
                    // So as long as the XML parser doesn't do it, it needs to
                    // done here.
                    const XMLCh* qualifiedName = attr->getFullName();
                    int index = DOMDocumentImpl::indexofQualifiedName(qualifiedName);

                    XMLBufBid bbQName(&fBufMgr);
                    XMLBuffer& buf = bbQName.getBuffer();
                    static const XMLCh XMLNS[] = {
                        chLatin_x, chLatin_m, chLatin_l, chLatin_n, chLatin_s, chNull};

                    if (index > 0) {
                        // there is prefix
                        // map to XML URI for all cases except when prefix == "xmlns"
                        XMLCh* prefix;
                        XMLCh temp[1000];

                        if (index > 999)
                            prefix = (XMLCh*) fMemoryManager->allocate
                            (
                                (index + 1) * sizeof(XMLCh)
                            );//new XMLCh[index+1];
                        else
                            prefix = temp;

                        XMLString::subString(prefix ,qualifiedName, 0, index, fMemoryManager);

                        if (XMLString::equals(prefix,XMLNS))
                            buf.append(XMLUni::fgXMLNSURIName);
                        else
                            buf.append(XMLUni::fgXMLURIName);

                        if (index > 999)
                            fMemoryManager->deallocate(prefix);//delete [] prefix;
                    }
                    else {
                        //   No prefix
                        if (XMLString::equals(qualifiedName,XMLNS))
                            buf.append(XMLUni::fgXMLNSURIName);
                    }

                    insertAttr = (DOMAttrImpl *) fDocument->createAttributeNS(
                       buf.getRawBuffer(),     // NameSpaceURI
                       qualifiedName);   // qualified name

                    DOMNode* remAttr = elemImpl->setAttributeNodeNS(insertAttr);
                    if (remAttr)
                        remAttr->release();
                }
                else
                {
                    // Namespaces is turned off...
                    insertAttr = (DOMAttrImpl *) fDocument->createAttribute(attr->getFullName());
                    DOMNode* remAttr = elemImpl->setAttributeNode(insertAttr);
                    if (remAttr)
                        remAttr->release();
                }

                insertAttr->setValue(attr->getValue());
                insertAttr->setSpecified(false);
            }
        }
        DOMNode* rem = fDocumentType->getElements()->setNamedItem(elemImpl);
        if (rem)
            rem->release();
    }
}

void AbstractDOMParser::endIntSubset()
{
    fDocumentType->setInternalSubset(fInternalSubset.getRawBuffer());
    // the buffer shouldn't be released as it is reused in the next parse
    // fBufMgr.releaseBuffer(fInternalSubset);
    fDocumentType->fIntSubsetReading = false;
}

void AbstractDOMParser::endExtSubset()
{
}

void AbstractDOMParser::entityDecl
(
    const   DTDEntityDecl&  entityDecl
    , const bool
    , const bool
)
{
    DOMEntityImpl* entity = (DOMEntityImpl *) fDocument->createEntity(entityDecl.getName());

    entity->setPublicId(entityDecl.getPublicId());
    entity->setSystemId(entityDecl.getSystemId());
    entity->setNotationName(entityDecl.getNotationName());
    entity->setBaseURI(entityDecl.getBaseURI());

    DOMEntityImpl *previousDef = (DOMEntityImpl *)
	    fDocumentType->getEntities()->setNamedItem( entity );

    if (previousDef)
        previousDef->release();

    if (fDocumentType->isIntSubsetReading())
    {
        //add thes chars to internalSubset variable
        fInternalSubset.append(chOpenAngle);
        fInternalSubset.append(chBang);
        fInternalSubset.append(XMLUni::fgEntityString);
        fInternalSubset.append(chSpace);

        fInternalSubset.append(entityDecl.getName());

        const XMLCh* id = entity->getPublicId();
        if (id != 0) {
            fInternalSubset.append(chSpace);
            fInternalSubset.append(XMLUni::fgPubIDString);
            fInternalSubset.append(chSpace);
            fInternalSubset.append(chDoubleQuote);
            fInternalSubset.append(id);
            fInternalSubset.append(chDoubleQuote);
        }
        id = entity->getSystemId();
        if (id != 0) {
            fInternalSubset.append(chSpace);
            fInternalSubset.append(XMLUni::fgSysIDString);
            fInternalSubset.append(chSpace);
            fInternalSubset.append(chDoubleQuote);
            fInternalSubset.append(id);
            fInternalSubset.append(chDoubleQuote);

        }
        id = entity->getNotationName();
        if (id != 0) {
            fInternalSubset.append(chSpace);
            fInternalSubset.append(XMLUni::fgNDATAString);
            fInternalSubset.append(chSpace);
            fInternalSubset.append(id);
        }
        id = entityDecl.getValue();
        if (id !=0) {
            fInternalSubset.append(chSpace);
            fInternalSubset.append(chDoubleQuote);
            fInternalSubset.append(id);
            fInternalSubset.append(chDoubleQuote);
        }

        fInternalSubset.append(chCloseAngle);
    }

}

void AbstractDOMParser::resetDocType()
{
	fDocumentType = 0;
}

void AbstractDOMParser::notationDecl
(
    const   XMLNotationDecl&    notDecl
    , const bool
)
{
    DOMNotationImpl* notation = (DOMNotationImpl *)fDocument->createNotation(notDecl.getName());
    notation->setPublicId(notDecl.getPublicId());
    notation->setSystemId(notDecl.getSystemId());
    notation->setBaseURI(notDecl.getBaseURI());

    DOMNode* rem = fDocumentType->getNotations()->setNamedItem( notation );
    if (rem)
        rem->release();

    if (fDocumentType->isIntSubsetReading())
    {
        //add thes chars to internalSubset variable
        fInternalSubset.append(chOpenAngle);
        fInternalSubset.append(chBang);
        fInternalSubset.append(XMLUni::fgNotationString);
        fInternalSubset.append(chSpace);

        fInternalSubset.append(notDecl.getName());

        const XMLCh* id = notation->getPublicId();
        if (id != 0) {
            fInternalSubset.append(chSpace);
            fInternalSubset.append(XMLUni::fgPubIDString);
            fInternalSubset.append(chSpace);
            fInternalSubset.append(chDoubleQuote);
            fInternalSubset.append(id);
            fInternalSubset.append(chDoubleQuote);
        }
        id = notation->getSystemId();
        if (id != 0) {
            fInternalSubset.append(chSpace);
            fInternalSubset.append(XMLUni::fgSysIDString);
            fInternalSubset.append(chSpace);
            fInternalSubset.append(chDoubleQuote);
            fInternalSubset.append(id);
            fInternalSubset.append(chDoubleQuote);

        }
        fInternalSubset.append(chCloseAngle);
    }
}

void AbstractDOMParser::startAttList
(
    const   DTDElementDecl& elemDecl
)
{
    if (fDocumentType->isIntSubsetReading())
    {
        fInternalSubset.append(chOpenAngle);
        fInternalSubset.append(chBang);
        fInternalSubset.append(XMLUni::fgAttListString);
        fInternalSubset.append(chSpace);
        fInternalSubset.append(elemDecl.getFullName());
    }
}

void AbstractDOMParser::startIntSubset()
{
	fDocumentType->fIntSubsetReading = true;
}

void AbstractDOMParser::startExtSubset()
{
}

void AbstractDOMParser::TextDecl
(
    const   XMLCh* const    versionStr
    , const XMLCh* const    encodingStr
)
{
    if (fCurrentEntity) {
        fCurrentEntity->setVersion(versionStr);
        fCurrentEntity->setEncoding(encodingStr);
    }
}

XERCES_CPP_NAMESPACE_END
