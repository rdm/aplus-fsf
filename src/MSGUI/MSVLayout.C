///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1997-2001 Morgan Stanley Dean Witter & Co. All rights reserved. 
// See .../src/LICENSE for terms of distribution
//
//
///////////////////////////////////////////////////////////////////////////////

#include <MSGUI/MSVLayout.H>

MSVLayout::MSVLayout(MSWidget *owner_,const char *title_) : MSLayout(owner_,title_) 
{ init(); }
MSVLayout::MSVLayout(MSWidget *owner_,const MSStringVector& title_) : MSLayout(owner_,title_) 
{ init(); }
MSVLayout::~MSVLayout(void) {}

void MSVLayout::init(void)
{ _orientation=MSLayoutManager::Vertical; }










