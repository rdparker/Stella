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
// $Id: Menu.cxx,v 1.2 2005-03-10 22:59:40 stephena Exp $
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
#ifdef DEBUG
  cerr << "Menu::Menu()\n";
#endif

  myOSystem->attach(this);  

  // Create a stack to hold the various dialog boxes

  // Create the top-level menu
  myOptionsDialog = new OptionsDialog(myOSystem);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Menu::~Menu()
{
#ifdef DEBUG
  cerr << "Menu::~Menu()\n";
#endif

  delete myOptionsDialog;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::draw()
{
#ifdef DEBUG
  cerr << "Menu::draw()\n";
#endif

  // Draw all the dialogs on the stack
  for(Int32 i = 0; i < myDialogStack.size(); i++)
    myDialogStack[i]->drawDialog();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::addDialog(Dialog* d)
{
  myDialogStack.push(d);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::removeDialog()
{
  if(!myDialogStack.empty())
    myDialogStack.pop();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::reset()
{
  // Pop all items from the stack, and then add the base menu
  for(Int32 i = 0; i < myDialogStack.size(); i++)
    myDialogStack.pop();

  myDialogStack.push(myOptionsDialog);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Menu::handleKeyEvent(SDLKey key, SDLMod mod, uInt8 state)
{
  // Send the event to the dialog box on the top of the stack
  Dialog* activeDialog = myDialogStack.top();

  if(state == 1)
    activeDialog->handleKeyDown(key, mod);
  else
    activeDialog->handleKeyUp(key, mod);
}
