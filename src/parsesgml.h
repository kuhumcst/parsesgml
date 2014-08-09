/*
CSTSGML - read SGML, HTML or XML

Copyright (C) 2012 Center for Sprogteknologi, University of Copenhagen

This file is part of CSTSGML.

CSTSGML is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

CSTSGML is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with CSTSGML; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SGMLTAG_H
#define SGMLTAG_H
#include <wchar.h>
class html_tag_class;

typedef enum {notag,tag,endoftag,endoftag_startoftag} estate;
extern estate (html_tag_class::*tagState)(wint_t kar);
extern estate (html_tag_class::*def)(wint_t kar);
extern void dummyCallBack(void *);

extern void parseAsXml();   // processor instruction ends with ?>
extern void parseAsHtml();  // script and style elements take CDATA

class html_tag_class
    {
    private:
#if defined NOSGMLCALLBACKS
        void setCallBackStartScript(void (*)(void *))          {}
        void setCallBackEndScript(void (*)(void *))            {}
        void setCallBackStartMarkUp(void (*)(void *))          {}
        void setCallBackEndMarkUp(void (*)(void *))            {}
        void setCallBackStartComment(void (*)(void *))         {}
        void setCallBackEndComment(void (*)(void *))           {}
        void setCallBackStartDOCTYPE(void (*)(void *))         {}
        void setCallBackEndDOCTYPE(void (*)(void *))           {}
        void setCallBackStartCDATA(void (*)(void *))           {}
        void setCallBackEndCDATA(void (*)(void *))             {}
        void setCallBackStartElementName(void (*)(void *))     {}
        void setCallBackEndElementName(void (*)(void *))       {}
        void setCallBackStartAttribute(void (*)(void *))       {}
        void setCallBackStartAttributeName(void (*)(void *))   {}
        void setCallBackEndAttributeName(void (*)(void *))     {}
        void setCallBackStartValue(void (*)(void *))           {}
        void setCallBackEndValue(void (*)(void *))             {}
        void setCallBackEmptyAttribute(void (*)(void *))       {}
        void setCallBackEndAttribute(void (*)(void *))         {}
        void setCallBackEndTag(void (*)(void *))               {}
        void setCallBackEmptyTag(void (*)(void *))             {}
        void setCallBackNoMoreAttributes(void (*)(void *))     {}
#else
        void * arg;
        void (*CallBackStartScript)(void *); // called when <? has been read
        void (*CallBackEndScript)(void *);  // called when <? > or <? ? or <? < has been read
        void (*CallBackStartMarkUp)(void *); // called when < has been read
        void (*CallBackEndMarkUp)(void *); // called when < > has been read and the markup is not script, comment CDATA or a tag.
        void (*CallBackStartComment)(void *); // called when <!-- has been read
        void (*CallBackEndComment)(void *); // called when <!-- -- has been read
        void (*CallBackStartDOCTYPE)(void *); // called when <!DOCTYPE [ has been read
        void (*CallBackEndDOCTYPE)(void *); // called when <!DOCTYPE [ ]> has been read
        void (*CallBackStartCDATA)(void *); // called when <![CDATA[ has been read
        void (*CallBackEndCDATA)(void *); // called when <![CDATA[ ]]> has been read
        void (*CallBackStartElementName)(void *);
        void (*CallBackEndElementName)(void *);
        void (*CallBackStartAttribute)(void *); // 20120223
        void (*CallBackStartAttributeName)(void *);
        void (*CallBackEndAttributeName)(void *);
        void (*CallBackStartValue)(void *);
        void (*CallBackEndValue)(void *);
        void (*CallBackEmptyAttribute)(void *); // 20120223
        void (*CallBackEndAttribute)(void *); // 20120223
        void (*CallBackEndTag)(void *);
        void (*CallBackEmptyTag)(void *);
        void (*CallBackNoMoreAttributes)(void *);
#endif
    public:
        estate def_pcdata(wint_t kar);
        estate def_cdata(wint_t kar);
        estate lt(wint_t kar);
        estate lt_cdata(wint_t kar);
        estate element(wint_t kar);
        estate elementonly(wint_t kar);
        estate gt(wint_t kar);
        estate emptytag(wint_t kar);
        estate atts(wint_t kar);
        estate name(wint_t kar);
        estate value(wint_t kar);
        estate atts_or_value(wint_t kar);
        estate invalue(wint_t kar);
        estate insinglequotedvalue(wint_t kar);
        estate indoublequotedvalue(wint_t kar);
        estate singlequotes(wint_t kar);
        estate doublequotes(wint_t kar);
        estate endvalue(wint_t kar);
        estate markup(wint_t kar);
        estate unknownmarkup(wint_t kar);
        estate scriptOrStyleElement(wint_t kar);
        estate perhapsScriptOrStyle(wint_t kar);
        estate PI(wint_t kar);
        estate endPI(wint_t kar);
        estate DOCTYPE1(wint_t kar);
        estate DOCTYPE2(wint_t kar);
        estate DOCTYPE3(wint_t kar);
        estate DOCTYPE4(wint_t kar);
        estate DOCTYPE5(wint_t kar);
        estate DOCTYPE6(wint_t kar);
        estate DOCTYPE7(wint_t kar);
        estate DOCTYPE8(wint_t kar);
        estate DOCTYPE9(wint_t kar);
        estate DOCTYPE10(wint_t kar);
        estate h1(wint_t kar);
        estate h2(wint_t kar);
        estate h3(wint_t kar);
        estate CDATA1(wint_t kar);
        estate CDATA2(wint_t kar);
        estate CDATA3(wint_t kar);
        estate CDATA4(wint_t kar);
        estate CDATA5(wint_t kar);
        estate CDATA6(wint_t kar);
        estate CDATA7(wint_t kar);
        estate CDATA8(wint_t kar);
        estate CDATA9(wint_t kar);
        estate endtag(wint_t kar);
#if defined NOSGMLCALLBACKS
        html_tag_class()
            {
            def = &html_tag_class::def_pcdata;
            tagState = def;
            }
#else
        html_tag_class(void * arg):arg(arg)
            {
            CallBackStartScript =
            CallBackEndScript =
            CallBackStartMarkUp =
            CallBackEndMarkUp =
            CallBackStartComment =
            CallBackEndComment =
            CallBackStartDOCTYPE =
            CallBackEndDOCTYPE =
            CallBackStartCDATA =
            CallBackEndCDATA =
            CallBackStartElementName = 
            CallBackEndElementName =
            CallBackStartAttribute = 
            CallBackStartAttributeName = 
            CallBackEndAttributeName =
            CallBackStartValue = 
            CallBackEndValue = 
            CallBackEmptyAttribute =
            CallBackEndAttribute =
                dummyCallBack;
            def = &html_tag_class::def_pcdata;
            tagState = def;
            }
        void setCallBackStartScript(void (*f)(void *))          {CallBackStartScript = f;}
        void setCallBackEndScript(void (*f)(void *))            {CallBackEndScript = f;}
        void setCallBackStartMarkUp(void (*f)(void *))          {CallBackStartMarkUp = f;}
        void setCallBackEndMarkUp(void (*f)(void *))            {CallBackEndMarkUp = f;}
        void setCallBackStartComment(void (*f)(void *))         {CallBackStartComment = f;}
        void setCallBackEndComment(void (*f)(void *))           {CallBackEndComment = f;}
        void setCallBackStartDOCTYPE(void (*f)(void *))         {CallBackStartDOCTYPE = f;}
        void setCallBackEndDOCTYPE(void (*f)(void *))           {CallBackEndDOCTYPE = f;}
        void setCallBackStartCDATA(void (*f)(void *))           {CallBackStartCDATA = f;}
        void setCallBackEndCDATA(void (*f)(void *))             {CallBackEndCDATA = f;}
        void setCallBackStartElementName(void (*f)(void *))     {CallBackStartElementName = f;}
        void setCallBackEndElementName(void (*f)(void *))       {CallBackEndElementName = f;}
        void setCallBackStartAttribute(void (*f)(void *))       {CallBackStartAttribute = f;}
        void setCallBackStartAttributeName(void (*f)(void *))   {CallBackStartAttributeName = f;}
        void setCallBackEndAttributeName(void (*f)(void *))     {CallBackEndAttributeName = f;}
        void setCallBackStartValue(void (*f)(void *))           {CallBackStartValue = f;}
        void setCallBackEndValue(void (*f)(void *))             {CallBackEndValue = f;}
        void setCallBackEmptyAttribute(void (*f)(void *))       {CallBackEmptyAttribute = f;}
        void setCallBackEndAttribute(void (*f)(void *))         {CallBackEndAttribute = f;}
        void setCallBackEndTag(void (*f)(void *))               {CallBackEndTag = f;}
        void setCallBackEmptyTag(void (*f)(void *))             {CallBackEmptyTag = f;}
        void setCallBackNoMoreAttributes(void (*f)(void *))     {CallBackNoMoreAttributes = f;}
#endif
    };

#endif
