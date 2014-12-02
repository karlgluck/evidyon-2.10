//------------------------------------------------------------------------------------------------
// File:  guitextboxelement.cpp
//
// Desc:  
//
// Copyright (c) 2007-2008 Unseen Studios.   All rights reserved.
//------------------------------------------------------------------------------------------------
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guilayer.h"
#include "guifont.h"
#include "guitextelement.h"
#include "guitextlabelelement.h"
#include "guitextboxelement.h"


//------------------------------------------------------------------------------------------------
// Name: getNextTextSegment
// Desc: Returns the text for this text box
//------------------------------------------------------------------------------------------------
bool GUITextBoxElement::getNextTextSegment(TextSegmentInfo* nextSegmentInfo)
{/*
  // Build a list of line number to line text pairs
  typedef std::list<std::pair<int,std::string>> LineList;
  LineList lines;
  {
  size_t offset = 0;
  int line = 0;
  while(offset < myText.npos) {
    const char* endline = "\r\n";
    size_t location = myText.find_first_of(endline, offset);
    lines.push_back(std::pair<int,std::string>(line, myText.substr(offset, location - 1)));
    offset = myText.find_first_not_of(endline, location);
    line += offset - location;
  }
  }

  // Wrap each element to the right-left boundary using a binary search for
  // the rightmost word.
  int wrap_width = mySourceCanvas->getArea()->calculateSize().width;
  for (LineList::iterator i = lines.begin(); i != lines.end(); ++i) {
    std::string& lineText = i->second;
    if (
  }*/
  return false;

}

