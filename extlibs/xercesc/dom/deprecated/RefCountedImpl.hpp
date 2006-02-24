#ifndef RefCountedImpl_HEADER_GUARD_
#define RefCountedImpl_HEADER_GUARD_
/*
 * Copyright 1999-2002,2004 The Apache Software Foundation.
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

/*
 * $Id: RefCountedImpl.hpp,v 1.1.1.1 2005/07/29 01:44:53 openns Exp $
 */

//
//  This file is part of the internal implementation of the C++ XML DOM.
//  It should NOT be included or used directly by application programs.
//
//  Applications should include the file <xercesc/dom/deprecated/DOM.hpp> for the entire
//  DOM API, or DOM_*.hpp for individual DOM classes, where the class
//  name is substituded for the *.
//



#include <xercesc/util/XMemory.hpp>

XERCES_CPP_NAMESPACE_BEGIN


class DEPRECATED_DOM_EXPORT RefCountedImpl : public XMemory
{
public:
    int			    nodeRefCount;

                    RefCountedImpl();
    virtual         ~RefCountedImpl();

    static void     addRef(RefCountedImpl *thisNode);
    static void     removeRef(RefCountedImpl *thisNode);

    virtual void    referenced();   // This function will be called by
                                    //  the reference counting implementation
                                    //  whenever the reference count transitions
                                    //  from 0 to 1.

    virtual void    unreferenced(); // unreferenced() is called whenever the
                                    //  the ref count goes from 1 to 0.  (Nodes are
                                    //  not deleted when the ref count goes to zero
                                    //  if they are in the doc tree and the tree
                                    //  is still referenced, so a nodes referenced /
                                    //  unreferenced state may switch many times
                                    //  over its life time.)
};

XERCES_CPP_NAMESPACE_END

#endif
