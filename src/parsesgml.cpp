/*
PARSESGML - read SGML, HTML or XML

Copyright (C) 2012 Center for Sprogteknologi, University of Copenhagen

This file is part of PARSESGML.

PARSESGML is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

PARSESGML is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PARSESGML; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "parsesgml.h"

#if defined NOSGMLCALLBACKS
#define CallBackStartScript(a)
#define CallBackEndScript(a)
#define CallBackStartMarkUp(a)
#define CallBackEndMarkUp(a)
#define CallBackStartComment(a)
#define CallBackEndComment(a)
#define CallBackStartDOCTYPE(a)
#define CallBackEndDOCTYPE(a)
#define CallBackStartCDATA(a)
#define CallBackEndCDATA(a)
#define CallBackStartElementName(a)
#define CallBackEndElementName(a)
#define CallBackStartAttribute(a)
#define CallBackStartAttributeName(a)
#define CallBackEndAttributeName(a)
#define CallBackStartValue(a)
#define CallBackEndValue(a)
#define CallBackEmptyAttribute(a)
#define CallBackEndAttribute(a)
#define CallBackEndTag(a)
#define CallBackEmptyTag(a)
#define CallBackNoMoreAttributes(a)
#endif

static bool XML  = false;
static bool HTML = false;

estate (html_tag_class::*def)(wint_t kar);

extern void parseAsXml()   // processor instruction ends with ?>
    {
    XML  = true;
    }

extern void parseAsHtml()  // script and style elements take CDATA
    {
    HTML = true;
    }

void dummyCallBack(void *){}

estate (html_tag_class::*tagState)(wint_t kar);

estate html_tag_class::def_pcdata(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return tag;
        default:
            return notag;
        }
    }
    
estate html_tag_class::def_cdata(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt_cdata;
            return tag;
        default:
            return notag;
        }
    }

estate html_tag_class::lt(wint_t kar)
    {
    switch(kar)
        {
        case '>':
            tagState = def;
            return notag;
        case '!':
            tagState = &html_tag_class::markup;
            return tag;
        case '?':
            CallBackStartScript(arg);
            tagState = &html_tag_class::PI; // <?
            return tag;
        case '/':
            CallBackEndTag(arg);
            tagState = &html_tag_class::endtag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return tag;
        case 's':
        case 'S':
            if(HTML && !XML)
                {
                tagState = &html_tag_class::perhapsScriptOrStyle;
                CallBackStartElementName(arg);
                return tag;
                }
            /* fall through */
        default:
            if(('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                {
                tagState = &html_tag_class::element;
                CallBackStartElementName(arg);
                return tag;
                }
            else
                {
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::lt_cdata(wint_t kar)
    {
    switch(kar)
        {
        case '/':
            CallBackStartMarkUp(arg);
//            putOperatorChar('.');
            tagState = &html_tag_class::endtag;
            def = &html_tag_class::def_pcdata;
            return tag;
        default:
            tagState = &html_tag_class::def_cdata;
            return notag;            
        }
    }

static int scriptstylei = 0;

static void stillCdata()
    {
    if(scriptstylei > 0 && def == &html_tag_class::def_cdata)
        {
        def = &html_tag_class::def_pcdata;
        scriptstylei = 0;
        }
    }

estate html_tag_class::element(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackEndElementName(arg);
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndElementName(arg);
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case '-':
        case '_':
        case ':':
        case '.':
            stillCdata();
            return tag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            CallBackEndElementName(arg);
            tagState = &html_tag_class::atts;
            return tag;
        case '/':
            CallBackEndElementName(arg);
            CallBackEmptyTag(arg);
            CallBackNoMoreAttributes(arg);
            tagState = &html_tag_class::emptytag;
            return tag;
        default:
            if(('0' <= kar && kar <= '9') || ('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                {
                stillCdata();
                return tag;
                }
            else
                {
                stillCdata();
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::elementonly(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackEndElementName(arg);
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndElementName(arg);
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case '-':
        case '_':
        case ':':
        case '.':
            return tag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            CallBackEndElementName(arg);
            tagState = &html_tag_class::gt;
            return tag;
        default:
            if(('0' <= kar && kar <= '9') || ('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                return tag;
            else
                {
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::gt(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return tag;
        default:
            tagState = def;
            return notag;
        }
    }

estate html_tag_class::emptytag(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            //CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            //CallBackNoMoreAttributes(arg);
            return endoftag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return tag;
        default:
            tagState = def;
            return notag;
        }
    }

estate html_tag_class::atts(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return tag;
        case '/':
            CallBackEmptyTag(arg);
            CallBackNoMoreAttributes(arg);
            tagState = &html_tag_class::emptytag;
            return tag;
        default:
            if(('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                {
                CallBackStartAttributeName(arg);
                tagState = &html_tag_class::name;
                return tag;
                }
            else
                {
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::name(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackEndAttributeName(arg);
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndAttributeName(arg);
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case '-':
        case '_':
        case ':':
        case '.':
            return tag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            CallBackEndAttributeName(arg);
            tagState = &html_tag_class::atts_or_value;
            return tag;
        case '/':
            CallBackEndAttributeName(arg);
            CallBackEmptyTag(arg);
            tagState = &html_tag_class::emptytag;
            return tag;
        case '=':
            CallBackEndAttributeName(arg);
            tagState = &html_tag_class::value;
            return tag;
        default:
            if(('0' <= kar && kar <= '9') || ('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                return tag;
            else
                {
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::value(wint_t kar)
    {
    switch(kar)
        {
        case '>':
        case '/':
        case '=':
            tagState = def;
            return notag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return tag;
        case '\'':
            tagState = &html_tag_class::singlequotes;
            return tag;
        case '"':
            tagState = &html_tag_class::doublequotes;
            return tag;
        /*case '-':
        case '_':
        case ':':
        case '.':
            tagState = &html_tag_class::invalue;
            return tag;*/
        default:
            if(('0' <= kar && kar <= '9') || ('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                {
                CallBackStartValue(arg);
                tagState = &html_tag_class::invalue;
                return tag;
                }
            else
                {
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::atts_or_value(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case '-':
        case '_':
        case ':':
        case '.':
            tagState = def;
            return notag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return tag;
        case '/':
            CallBackEmptyTag(arg);
            CallBackNoMoreAttributes(arg);
            tagState = &html_tag_class::emptytag;
            return tag;
        case '=':
            tagState = &html_tag_class::value;
            return tag;
        default:
            if(('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                {
                CallBackStartAttributeName(arg);
                tagState = &html_tag_class::name;
                return tag;
                }
            else
                {
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::invalue(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackEndValue(arg);
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndValue(arg);
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            CallBackEndValue(arg);
            tagState = &html_tag_class::atts;
            return tag;
        default:
            if(('0' <= kar && kar <= '9') || ('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                {
                return tag;
                }
            else
                {
                tagState = def;
                return notag;
                }
        }
    }

estate html_tag_class::singlequotes(wint_t kar)
    {
    CallBackStartValue(arg);
    switch(kar)
        {
        case '\'':
            CallBackEndValue(arg);
            tagState = &html_tag_class::endvalue;
            return tag;
        default:
            tagState = &html_tag_class::insinglequotedvalue;
            return tag;
        }
    }

estate html_tag_class::doublequotes(wint_t kar)
    {
    CallBackStartValue(arg);
    switch(kar)
        {
        case '\"':
            CallBackEndValue(arg);
            tagState = &html_tag_class::endvalue;
            return tag;
        default:
            tagState = &html_tag_class::indoublequotedvalue;
            return tag;
        }
    }

estate html_tag_class::insinglequotedvalue(wint_t kar)
    {
    switch(kar)
        {
        case '\'':
            CallBackEndValue(arg);
            tagState = &html_tag_class::endvalue;
            return tag;
        default:
            return tag;
        }
    }

estate html_tag_class::indoublequotedvalue(wint_t kar)
    {
    switch(kar)
        {
        case '\"':
            CallBackEndValue(arg);
            tagState = &html_tag_class::endvalue;
            return tag;
        default:
            return tag;
        }
    }


estate html_tag_class::endvalue(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            tagState = &html_tag_class::atts;
            return tag;
        case '/': // 20090708
            CallBackEmptyTag(arg);
            CallBackNoMoreAttributes(arg);
            tagState = &html_tag_class::emptytag;
            return tag;
        default:
            tagState = def;
            return notag;
        }
    }

estate html_tag_class::markup(wint_t kar) // <!
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackEndMarkUp(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndMarkUp(arg);
            return endoftag;
        case '-':
            tagState = &html_tag_class::h1;
            return tag;
        case '[':
            tagState = &html_tag_class::CDATA1;
            return tag;
        case 'D':
            tagState = &html_tag_class::DOCTYPE1;
            return tag;
        default:
            tagState = &html_tag_class::unknownmarkup;
            return tag;
        }
    }

estate html_tag_class::unknownmarkup(wint_t kar) /* <! */
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackEndMarkUp(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndMarkUp(arg);
            return endoftag;
        default:
            return tag;
        }
    }

static int scriptstyleimax = 0;
static wint_t * elementNameLower;
static wint_t * elementNameUpper;
estate html_tag_class::scriptOrStyleElement(wint_t kar) /* <sc or <SC or <Sc or <sC or <st or <ST or <St or <sT */
    {
    if(kar == elementNameLower[scriptstylei] || kar == elementNameUpper[scriptstylei])
        {
        estate ret;
        if(scriptstylei == scriptstyleimax)
            {
            def = &html_tag_class::def_cdata;
            tagState = &html_tag_class::element;
            scriptstylei = 0;
            }
        else
            ++scriptstylei;
        ret = html_tag_class::element(kar);
        tagState = &html_tag_class::scriptOrStyleElement;
        return ret;
        }
    else
        {
        scriptstylei = 0;
        }
    return element(kar);
    }

estate html_tag_class::perhapsScriptOrStyle(wint_t kar) /* <s or <S */
    {
    static wint_t ript[] = {'r','i','p','t',0};
    static wint_t RIPT[] = {'R','I','P','T',0};
    static wint_t yle[] = {'y','l','e',0};
    static wint_t YLE[] = {'Y','L','E',0};
    estate ret;
    switch(kar)
        {
        case 'C':
        case 'c':
            elementNameLower = ript;
            elementNameUpper = RIPT;
            scriptstyleimax = sizeof(ript)/sizeof(ript[0]) - 2;
            ret = element(kar);
            tagState = &html_tag_class::scriptOrStyleElement;
            return ret;
        case 'T':
        case 't':
            elementNameLower = yle;
            elementNameUpper = YLE;
            scriptstyleimax = sizeof(yle)/sizeof(yle[0]) - 2;
            ret = element(kar);
            tagState = &html_tag_class::scriptOrStyleElement;
            return ret;
        default:
            tagState = &html_tag_class::element;
        }
    return element(kar);
    }

estate html_tag_class::PI(wint_t kar)
    {
    if(XML)
        switch(kar)
            {
            case '?':                       // <? ....... ?  If followed by >, XML processing instruction ends.
    //            CallBackEndTag(arg);
                tagState = &html_tag_class::endPI;
                //CallBackEndScript(arg);
                return tag;
            default:
                return tag;
            }
    else
        switch(kar)
            {
            case '>':                       // SGML (HTML) processing instruction ends here
                tagState = def;
                CallBackEndScript(arg);
                return endoftag;
            default:
                return tag;
            }
    }

estate html_tag_class::endPI(wint_t kar)
    {
    switch(kar)
        {
        case '>':                               // <? ...... ?>  (XML Processing Instruction)
            tagState = def;
            CallBackEndScript(arg);
            return endoftag;
        case '?':                               // <? ...... ??
            return tag;
        default:
            tagState = &html_tag_class::PI; // <? ...... ? ...
            return tag;
        }
    }

estate html_tag_class::DOCTYPE1(wint_t kar) // <!D
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'O':
            tagState = &html_tag_class::DOCTYPE2;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE2(wint_t kar) // <!DO
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'C':
            tagState = &html_tag_class::DOCTYPE3;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE3(wint_t kar) // <!DOC
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'T':
            tagState = &html_tag_class::DOCTYPE4;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE4(wint_t kar) // <!DOCT
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'Y':
            tagState = &html_tag_class::DOCTYPE5;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE5(wint_t kar) // <!DOCTY
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'P':
            tagState = &html_tag_class::DOCTYPE6;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE6(wint_t kar) // <!DOCTYP
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'E':
            tagState = &html_tag_class::DOCTYPE7;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE7(wint_t kar) // <!DOCTYPE
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndDOCTYPE(arg);
            return endoftag;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            CallBackStartDOCTYPE(arg);
            tagState = &html_tag_class::DOCTYPE8;
            return tag;
        default:
            tagState = &html_tag_class::unknownmarkup;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE8(wint_t kar) // <!DOCTYPE S
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndDOCTYPE(arg);
            return endoftag;
        case '[':
            tagState = &html_tag_class::DOCTYPE9;
            return tag;
        default:
            tagState = &html_tag_class::DOCTYPE8;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE9(wint_t kar)  // <!DOCTYPE S [
    {
    switch(kar)
        {
        case ']':
            tagState = &html_tag_class::DOCTYPE10;
            return tag;
        default:
            tagState = &html_tag_class::DOCTYPE9;
            return tag;
        }
    }

estate html_tag_class::DOCTYPE10(wint_t kar)  // <!DOCTYPE S [ ]
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackEndDOCTYPE(arg);
            return endoftag;
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            tagState = &html_tag_class::DOCTYPE10;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::CDATA1(wint_t kar) // <![
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'C':
            tagState = &html_tag_class::CDATA2;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::CDATA2(wint_t kar) // <![C
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
            //CallBackNoMoreAttributes(arg);
        case '>':
            tagState = def;
            return endoftag;
        case 'D':
            tagState = &html_tag_class::CDATA3;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::CDATA3(wint_t kar) // <![CD
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'A':
            tagState = &html_tag_class::CDATA4;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::CDATA4(wint_t kar) // <![CDA
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'T':
            tagState = &html_tag_class::CDATA5;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::CDATA5(wint_t kar) // <![CDAT
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case 'A':
            tagState = &html_tag_class::CDATA6;
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::CDATA6(wint_t kar) // <![CDATA
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            return endoftag;
        case '[':
            tagState = &html_tag_class::CDATA7;
            CallBackStartCDATA(arg);
            return tag;
        default:
            tagState = &html_tag_class::markup;
            return tag;
        }
    }

estate html_tag_class::CDATA7(wint_t kar) // <![CDATA[
    {
    switch(kar)
        {
        case ']':
            tagState = &html_tag_class::CDATA8;
            return tag;
        default:
            return tag;
        }
    }

estate html_tag_class::CDATA8(wint_t kar) // <![CDATA[ ]
    {
    switch(kar)
        {
        case ']':
            tagState = &html_tag_class::CDATA9;
            return tag;
        default:
            tagState = &html_tag_class::CDATA7;
            return tag;
        }
    }

estate html_tag_class::CDATA9(wint_t kar) // <![CDATA[ ]]
    {
    switch(kar)
        {
        case '>':  // <![CDATA[ ]]>
            tagState = def;
            CallBackEndCDATA(arg);
            return endoftag;
        default:
            tagState = &html_tag_class::CDATA7;
            return tag;
        }
    }


estate html_tag_class::h1(wint_t kar) // <!-
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackStartMarkUp(arg);
            return notag;
        case '>':
            tagState = def;
            return notag;
        case '-':
            tagState = &html_tag_class::h2;
            CallBackStartComment(arg);
            return tag;
        default:
            tagState = &html_tag_class::unknownmarkup;
            return tag;
        }
    }

estate html_tag_class::h2(wint_t kar) // <!--
    {
    switch(kar)
        {
/*      case '>':
            tagState = def;
            return endoftag;*/
        case '-':
            tagState = &html_tag_class::h3;
            return tag;
        default:
//          tagState = &html_tag_class::comment;
            return tag;
        }
    }













estate html_tag_class::h3(wint_t kar) // <!--  -
    {
    switch(kar)
        {
/*      case '>':
            tagState = def;
            return notag;*/
        case '-': // <!-- --
            tagState = &html_tag_class::markup;
            CallBackEndComment(arg);
            return tag;
        default:
            tagState = &html_tag_class::h2;
            return tag;
        }
    }

















estate html_tag_class::endtag(wint_t kar)
    {
    switch(kar)
        {
        case '<':
            tagState = &html_tag_class::lt;
            CallBackNoMoreAttributes(arg);
            CallBackStartMarkUp(arg);
            return endoftag_startoftag;
        case '>':
            tagState = def;
            CallBackNoMoreAttributes(arg);
            return endoftag;
        case 0xA0:
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return tag;
        default:
            if(('A' <= kar && kar <= 'Z') || ('a' <= kar && kar <= 'z') || (kar & 0x80))
                {
                tagState = &html_tag_class::elementonly;
                CallBackStartElementName(arg);
                return tag;
                }
            else
                {
                tagState = def;
                return notag;
                }
        }
    }
