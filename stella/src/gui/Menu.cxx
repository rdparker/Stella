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
// Copyright (c) 1995-2005 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: Menu.cxx,v 1.6 2005-05-03 19:11:27 stephena Exp $
//============================================================================

#include <SDL.h>

#include "OSystem.hxx"
#include "Dialog.hxx"
#include "OptionsDialog.hxx"
#include "Stack.hxx"
#include "bspf.hxx"
#include "Menu.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Menu::Menu(OSystem* osystem)
    : myOSystem(osystem),
      myOptionsDialog(NULL)
{
  myOSystem->attach(this);  

  myCurrentKeyDown.keycode = 0;
  myLastClick.x = myLastClick.y = 0;
  myLastClick.time = 0;
  myLastClick.count = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Menu::~Menu()
{
  if(myOptionsDialog)
    delete myOptionsDialog;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::initialize()
{
  if(myOptionsDialog)
  {
    delete myOptionsDialog;
    myOptionsDialog = NULL;
  }

  // Create the top-level menu
  myOptionsDialog = new OptionsDialog(myOSystem);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::draw()
{
  // Draw all the dialogs on the stack
  for(Int32 i = 0; i < myDialogStack.size(); i++)
  {
    myDialogStack[i]->open();
    myDialogStack[i]->drawDialog();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::addDialog(Dialog* d)
{
  myDialogStack.push(d);
  myOSystem->frameBuffer().refresh();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::removeDialog()
{
  if(!myDialogStack.empty())
  {
    myDialogStack.pop();
    myOSystem->frameBuffer().refresh();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::reStack()
{
  // Pop all items from the stack, and then add the base menu
  while(!myDialogStack.empty())
  {
    Dialog* d = myDialogStack.pop();
    d->close();
  }
  myDialogStack.push(myOptionsDialog);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::handleKeyEvent(SDLKey key, SDLMod mod, uInt8 state)
{
  if(myDialogStack.empty())
    return;

  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();

  // Convert SDL values to ascii so the ScummVM subsystem can work with it
  if(state == 1)
    activeDialog->handleKeyDown(key, key, mod);
  else
    activeDialog->handleKeyUp(key, key, mod);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::handleMouseMotionEvent(Int32 x, Int32 y, Int32 button)
{
  if(myDialogStack.empty())
    return;

  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();
  activeDialog->handleMouseMoved(x - activeDialog->_x,
                                 y - activeDialog->_y,
                                 button);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::handleMouseButtonEvent(MouseButton b, Int32 x, Int32 y, uInt8 state)
{
  if(myDialogStack.empty())
    return;

  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();

  // Get the current time for detecting double clicks
  uInt32 time = myOSystem->getTicks() / 1000;  // we only need millisecond precision

  switch(b)
  {
    case EVENT_LBUTTONDOWN:
    case EVENT_RBUTTONDOWN:
      // If more than two clicks have been recorded, we start over
      if(myLastClick.count == 2)
      {
        myLastClick.x = myLastClick.y = 0;
        myLastClick.time = 0;
        myLastClick.count = 0;
      }

      if(myLastClick.count && (time < myLastClick.time + 500)  // DoubleClickDelay
         && ABS(myLastClick.x - x) < 3
         && ABS(myLastClick.y - y) < 3)
      {
        myLastClick.count++;
      }
      else
      {
        myLastClick.x = x;
        myLastClick.y = y;
        myLastClick.count = 1;
      }
      myLastClick.time = time;
      activeDialog->handleMouseDown(x - activeDialog->_x, y - activeDialog->_y,
                                    1, myLastClick.count);
      break;

    case EVENT_LBUTTONUP:
    case EVENT_RBUTTONUP:
      activeDialog->handleMouseUp(x - activeDialog->_x, y - activeDialog->_y,
                                  1, myLastClick.count);
      break;

    case EVENT_WHEELUP:
      activeDialog->handleMouseWheel(x - activeDialog->_x, y - activeDialog->_y, -1);
      break;

    case EVENT_WHEELDOWN:
      activeDialog->handleMouseWheel(x - activeDialog->_x, y - activeDialog->_y, 1);
      break;
  }
}
