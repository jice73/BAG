/*
 * Copyright 1999-2000,2004 The Apache Software Foundation.
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
 * $Log: HPCCDefs.cpp,v $
 * Revision 1.1.1.1  2005/07/29 01:44:54  openns
 * Checkin of library and support after second ONSWG meeting
 *
 * Revision 1.2  2004/09/08 13:56:32  peiyongz
 * Apache License Version 2.0
 *
 * Revision 1.1.1.1  2002/02/01 22:22:18  peiyongz
 * sane_include
 *
 * Revision 1.3  2000/03/02 19:55:08  roddey
 * This checkin includes many changes done while waiting for the
 * 1.1.0 code to be finished. I can't list them all here, but a list is
 * available elsewhere.
 *
 * Revision 1.2  2000/02/06 07:48:17  rahulj
 * Year 2K copyright swat.
 *
 * Revision 1.1.1.1  1999/11/09 01:07:34  twl
 * Initial checkin
 *
 * Revision 1.2  1999/11/08 20:45:23  rahul
 * Swat for adding in Product name and CVS comment log variable.
 *
 */


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/Compilers/HPCCDefs.hpp>
#include <strings.h>



int stricmp(const char* const str1, const char* const  str2) 
{
    return strcasecmp(str1, str2);
}

int strnicmp(const char* const str1, const char* const  str2, const unsigned int count)
{
    if (count == 0)
        return 0;

    return strncasecmp( str1, str2, (size_t)count);
}

