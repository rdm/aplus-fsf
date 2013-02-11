///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSGUI/MSFontObject.H>

MSFontObject::MSFontObject(const XFontStruct *pFontStruct_) : _fontStruct(pFontStruct_)
{}

MSFontObject::~MSFontObject(void)
{ _fontStruct=0; }








