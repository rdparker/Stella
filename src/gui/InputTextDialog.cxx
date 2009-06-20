//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: InputTextDialog.cxx,v 1.12 2006-03-23 16:16:33 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "OSystem.hxx"
#include "Widget.hxx"
#include "EditTextWidget.hxx"
#include "Dialog.hxx"
#include "GuiObject.hxx"
#include "GuiUtils.hxx"
#include "InputTextDialog.hxx"

#include "bspf.hxx"

enum {
  kAcceptCmd = 'ACPT'
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputTextDialog::InputTextDialog(GuiObject* boss, const GUI::Font& font,
                                 const StringList& labels, int x, int y)
  : Dialog(boss->instance(), boss->parent(), x, y, 16, 16),
    CommandSender(boss),
    myErrorFlag(false)
{
  const int fontWidth  = font.getMaxCharWidth(),
            fontHeight = font.getFontHeight(),
            lineHeight = font.getLineHeight(),
            bwidth  = font.getStringWidth(" Cancel "),
            bheight = font.getLineHeight() + 2;
  unsigned int xpos, ypos, i, lwidth = 0, maxIdx = 0;

  // Calculate real dimensions
  _w = fontWidth * 25;
  _h = lineHeight * 4 + labels.size() * (lineHeight + 5);

  // Determine longest label
  for(i = 0; i < labels.size(); ++i)
  {
    if(labels[i].length() > lwidth)
    {
      lwidth = labels[i].length();
      maxIdx = i;
    }
  }
  lwidth = font.getStringWidth(labels[maxIdx]);

  // Create editboxes for all labels
  WidgetArray wid;
  ypos = lineHeight;
  for(i = 0; i < labels.size(); ++i)
  {
    xpos = 10;
    new StaticTextWidget(this, font, xpos, ypos,
                         lwidth, fontHeight,
                         labels[i], kTextAlignLeft);

    xpos += lwidth + fontWidth;
    EditTextWidget* w = new EditTextWidget(this, font, xpos, ypos,
                                           _w - xpos - 10, lineHeight, "");
    wid.push_back(w);

    myInput.push_back(w);
    ypos += lineHeight + 5;
  }
  addToFocusList(wid);

  xpos = 10;
  myTitle = new StaticTextWidget(this, font, xpos, ypos, _w - 2*xpos, fontHeight,
                                 "", kTextAlignCenter);
  myTitle->setColor(kTextColorEm);

#ifndef MAC_OSX
  new ButtonWidget(this, font, _w - 2 * (bwidth + 10), _h - bheight - 10,
                   bwidth, bheight, "OK", kAcceptCmd, 0);
  new ButtonWidget(this, font, _w - (bwidth + 10), _h - bheight - 10,
                   bwidth, bheight, "Cancel", kCloseCmd, 0);
#else
  new ButtonWidget(this, font, _w - 2 * (bwidth + 10), _h - bheight - 10,
                   bwidth, bheight, "Cancel", kCloseCmd, 0);
  new ButtonWidget(this, font, _w - (bwidth + 10), _h - bheight - 10,
                   bwidth, bheight, "OK", kAcceptCmd, 0);
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputTextDialog::~InputTextDialog()
{
  myInput.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::setTitle(const string& title)
{
  myTitle->setLabel(title);
  myErrorFlag = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string& InputTextDialog::getResult(int idx)
{
  if((unsigned int)idx < myInput.size())
    return myInput[idx]->getEditString();
  else
    return EmptyString;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::setEditString(const string& str, int idx)
{
  if((unsigned int)idx < myInput.size())
    myInput[idx]->setEditString(str);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::setFocus(int idx)
{
  if((unsigned int)idx < myInput.size())
    Dialog::setFocus(getFocusList()[idx]);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputTextDialog::handleCommand(CommandSender* sender, int cmd,
                                    int data, int id)
{
  switch (cmd)
  {
    case kAcceptCmd:
    case kEditAcceptCmd:
    {
      // Send a signal to the calling class that a selection has been made
      // Since we aren't derived from a widget, we don't have a 'data' or 'id'
      if(myCmd)
        sendCommand(myCmd, 0, 0);

      // We don't close, but leave the parent to do it
      // If the data isn't valid, the parent may wait until it is
      break;
    }

    case kEditChangedCmd:
      // Erase the invalid message once editing is restarted
      if(myErrorFlag)
      {
        myTitle->setLabel("");
        myErrorFlag = false;
      }
      break;

    case kEditCancelCmd:
      Dialog::handleCommand(sender, kCloseCmd, data, id);
      break;


    default:
      Dialog::handleCommand(sender, cmd, data, id);
      break;
  }
}
