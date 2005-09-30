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
// $Id: GameInfoDialog.cxx,v 1.14 2005-09-30 00:40:34 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include <cctype>
#include <algorithm>

#include "GuiUtils.hxx"
#include "OSystem.hxx"
#include "Props.hxx"
#include "Widget.hxx"
#include "Dialog.hxx"
#include "EditTextWidget.hxx"
#include "PopUpWidget.hxx"
#include "TabWidget.hxx"
#include "GameInfoDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GameInfoDialog::GameInfoDialog(
      OSystem* osystem, DialogContainer* parent, GuiObject* boss,
      int x, int y, int w, int h)
  : Dialog(osystem, parent, x, y, w, h),
    CommandSender(boss)
{
  const GUI::Font& font = instance()->font();
  const int fontHeight = font.getFontHeight(),
            lineHeight = font.getLineHeight();

  const int vBorder = 4;
  int xpos, ypos, lwidth, fwidth, tabID;
  unsigned int i;
  WidgetArray wid;

  // The tab widget
  xpos = 2; ypos = vBorder;
  myTab = new TabWidget(this, xpos, ypos, _w - 2*xpos, _h - 24 - 2*ypos);

  // 1) Cartridge properties
  wid.clear();
  tabID = myTab->addTab("Cartridge");

  xpos = 10;
  lwidth = font.getStringWidth("Manufacturer: ");
  fwidth = _w - xpos - lwidth - 10;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Name:", kTextAlignLeft);
  myName = new EditTextWidget(myTab, xpos+lwidth, ypos, fwidth, fontHeight, "");
  wid.push_back(myName);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "MD5:", kTextAlignLeft);
  myMD5 = new StaticTextWidget(myTab, xpos+lwidth, ypos,
                               fwidth, fontHeight,
                               "", kTextAlignLeft);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Manufacturer:", kTextAlignLeft);
  myManufacturer = new EditTextWidget(myTab, xpos+lwidth, ypos,
                                      100, fontHeight, "");
  wid.push_back(myManufacturer);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Model:", kTextAlignLeft);
  myModelNo = new EditTextWidget(myTab, xpos+lwidth, ypos,
                                 100, fontHeight, "");
  wid.push_back(myModelNo);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Rarity:", kTextAlignLeft);
  myRarity = new EditTextWidget(myTab, xpos+lwidth, ypos,
                                100, fontHeight, "");
  wid.push_back(myRarity);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Note:", kTextAlignLeft);
  myNote = new EditTextWidget(myTab, xpos+lwidth, ypos,
                              fwidth, fontHeight, "");
  wid.push_back(myNote);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Sound:", kTextAlignLeft);
  mySound = new PopUpWidget(myTab, xpos+lwidth, ypos,
                            font.getStringWidth("Stereo") + 15, lineHeight,
                            "", 0, 0);
  mySound->appendEntry("Mono", 1);
  mySound->appendEntry("Stereo", 2);
  wid.push_back(mySound);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Type:", kTextAlignLeft);
  myType = new PopUpWidget(myTab, xpos+lwidth, ypos,
                           font.getStringWidth("Auto-detect") + 15, lineHeight,
                           "", 0, 0);
  myType->appendEntry("Auto-detect", 1);
  myType->appendEntry("2K", 2);
  myType->appendEntry("3E", 3);
  wid.push_back(myType);  // FIXME - add rest of types

  // Add items for tab 0
  addToFocusList(wid, tabID);


  // 2) Console/Controller properties
  wid.clear();
  tabID = myTab->addTab("Console");

  xpos = 10; ypos = vBorder;
  lwidth = font.getStringWidth("Right Difficulty: ");
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Left Difficulty:", kTextAlignLeft);
  myLeftDiff = new PopUpWidget(myTab, xpos+lwidth, ypos,
                               font.getStringWidth("   ") + 15, lineHeight,
                               "", 0, 0);
  myLeftDiff->appendEntry("B", 1);
  myLeftDiff->appendEntry("A", 2);
  wid.push_back(myLeftDiff);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Right Difficulty:", kTextAlignLeft);
  myRightDiff = new PopUpWidget(myTab, xpos+lwidth, ypos,
                                font.getStringWidth("   ") + 15, lineHeight,
                                "", 0, 0);
  myRightDiff->appendEntry("B", 1);
  myRightDiff->appendEntry("A", 2);
  wid.push_back(myRightDiff);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "TV Type:", kTextAlignLeft);
  myTVType = new PopUpWidget(myTab, xpos+lwidth, ypos,
                             font.getStringWidth("B & W") + 15, lineHeight,
                             "", 0, 0);
  myTVType->appendEntry("Color", 1);
  myTVType->appendEntry("B & W", 2);
  wid.push_back(myTVType);

  // Add items for tab 1
  addToFocusList(wid, tabID);


  // 3) Controller properties
  wid.clear();
  tabID = myTab->addTab("Controller");

  xpos = 10; ypos = vBorder;
  lwidth = font.getStringWidth("Right Controller: ");
  fwidth = font.getStringWidth("Booster-Grip");
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Left Controller:", kTextAlignLeft);
  myLeftController = new PopUpWidget(myTab, xpos+lwidth, ypos,
                                     fwidth + 15, lineHeight,
                                     "", 0, 0);
  for(i = 0; i < 5; ++i)
    myLeftController->appendEntry(ourControllerList[i].name, i+1);
  wid.push_back(myLeftController);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Right Controller:", kTextAlignLeft);
  myRightController = new PopUpWidget(myTab, xpos+lwidth, ypos,
                                      fwidth + 15, lineHeight,
                                      "", 0, 0);
  for(i = 0; i < 5; ++i)
    myRightController->appendEntry(ourControllerList[i].name, i+1);
  wid.push_back(myRightController);

  // Add items for tab 2
  addToFocusList(wid, tabID);


  // 4) Display properties
  wid.clear();
  tabID = myTab->addTab("Display");

  xpos = 10; ypos = vBorder;
  lwidth = font.getStringWidth("Use HMBlanks: ");
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Format:", kTextAlignLeft);
  myFormat = new PopUpWidget(myTab, xpos+lwidth, ypos,
                             font.getStringWidth("NTSC") + 15, lineHeight,
                             "", 0, 0);
  myFormat->appendEntry("NTSC", 1);
  myFormat->appendEntry("PAL", 2);
  wid.push_back(myFormat);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "XStart:", kTextAlignLeft);
  myXStart = new EditTextWidget(myTab, xpos+lwidth, ypos,
                                25, fontHeight, "");
  wid.push_back(myXStart);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Width:", kTextAlignLeft);
  myWidth = new EditTextWidget(myTab, xpos+lwidth, ypos,
                               25, fontHeight, "");
  wid.push_back(myWidth);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "YStart:", kTextAlignLeft);
  myYStart = new EditTextWidget(myTab, xpos+lwidth, ypos,
                                25, fontHeight, "");
  wid.push_back(myYStart);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Height:", kTextAlignLeft);
  myHeight = new EditTextWidget(myTab, xpos+lwidth, ypos,
                                25, fontHeight, "");
  wid.push_back(myHeight);

  ypos += lineHeight + 3;
  new StaticTextWidget(myTab, xpos, ypos+1, lwidth, fontHeight,
                       "Use HMBlanks:", kTextAlignLeft);
  myHmoveBlanks = new PopUpWidget(myTab, xpos+lwidth, ypos,
                                  font.getStringWidth("Yes") + 15, lineHeight,
                                  "", 0, 0);
  myHmoveBlanks->appendEntry("Yes", 1);
  myHmoveBlanks->appendEntry("No", 2);
  wid.push_back(myHmoveBlanks);

  // Add items for tab 3
  addToFocusList(wid, tabID);


  // Activate the first tab
  myTab->setActiveTab(0);

  // Add message concerning usage
  new StaticTextWidget(this, 10, _h - 20, 120, fontHeight,
                       "(*) Requires a ROM reload", kTextAlignLeft);

  // Add Defaults, OK and Cancel buttons
#ifndef MAC_OSX
  addButton(_w - 2 * (kButtonWidth + 7), _h - 24, "OK", kOKCmd, 0);
  addButton(_w - (kButtonWidth + 10), _h - 24, "Cancel", kCloseCmd, 0);
#else
  addButton(_w - 2 * (kButtonWidth + 7), _h - 24, "Cancel", kCloseCmd, 0);
  addButton(_w - (kButtonWidth + 10), _h - 24, "OK", kOKCmd, 0);
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GameInfoDialog::~GameInfoDialog()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GameInfoDialog::loadConfig()
{
  string s;
  int i;

  // Cartridge properties
  s = myGameProperties->get("Cartridge.Name");
  myName->setEditString(s);

  s = myGameProperties->get("Cartridge.MD5");
  myMD5->setLabel(s);

  s = myGameProperties->get("Cartridge.Manufacturer");
  myManufacturer->setEditString(s);

  s = myGameProperties->get("Cartridge.ModelNo");
  myModelNo->setEditString(s);

  s = myGameProperties->get("Cartridge.Rarity");
  myRarity->setEditString(s);

  s = myGameProperties->get("Cartridge.Note");
  myNote->setEditString(s);

  s = myGameProperties->get("Cartridge.Sound", true);
  if(s == "MONO")
    mySound->setSelectedTag(1);
  else if(s == "STEREO")
    mySound->setSelectedTag(2);
  else
    mySound->setSelectedTag(0);

  // FIXME - add type properties

  // Console properties
  s = myGameProperties->get("Console.LeftDifficulty", true);
  if(s == "B")
    myLeftDiff->setSelectedTag(1);
  else if(s == "A")
    myLeftDiff->setSelectedTag(2);
  else
    myLeftDiff->setSelectedTag(0);

  s = myGameProperties->get("Console.RightDifficulty", true);
  if(s == "B")
    myRightDiff->setSelectedTag(1);
  else if(s == "A")
    myRightDiff->setSelectedTag(2);
  else
    myRightDiff->setSelectedTag(0);

  s = myGameProperties->get("Console.RightDifficulty", true);
  if(s == "B")
    myRightDiff->setSelectedTag(1);
  else if(s == "A")
    myRightDiff->setSelectedTag(2);
  else
    myRightDiff->setSelectedTag(0);

  s = myGameProperties->get("Console.TelevisionType", true);
  if(s == "COLOR")
    myTVType->setSelectedTag(1);
  else if(s == "BLACKANDWHITE")
    myTVType->setSelectedTag(2);
  else
    myTVType->setSelectedTag(0);

  // Controller properties
  s = myGameProperties->get("Controller.Left", true);
  for(i = 0; i < 5; ++i)
  {
    if(s == ourControllerList[i].comparitor)
      break;
  }
  i = (i == 5) ? 0: i + 1;
  myLeftController->setSelectedTag(i);

  s = myGameProperties->get("Controller.Right", true);
  for(i = 0; i < 5; ++i)
  {
    if(s == ourControllerList[i].comparitor)
      break;
  }
  i = (i == 5) ? 0: i + 1;
  myRightController->setSelectedTag(i);

  // Display properties
  s = myGameProperties->get("Display.Format", true);
  if(s == "NTSC")
    myFormat->setSelectedTag(1);
  else if(s == "PAL")
    myFormat->setSelectedTag(2);
  else
    myFormat->setSelectedTag(0);

  s = myGameProperties->get("Display.XStart");
  myXStart->setEditString(s);

  s = myGameProperties->get("Display.Width");
  myWidth->setEditString(s);

  s = myGameProperties->get("Display.YStart");
  myYStart->setEditString(s);

  s = myGameProperties->get("Display.Height");
  myHeight->setEditString(s);

  s = myGameProperties->get("Emulation.HmoveBlanks", true);
  if(s == "YES")
    myHmoveBlanks->setSelectedTag(1);
  else if(s == "NO")
    myHmoveBlanks->setSelectedTag(2);
  else
    myHmoveBlanks->setSelectedTag(0);

  myTab->loadConfig();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GameInfoDialog::saveConfig()
{
  string s;
  int i;

  // Cartridge properties
  s = myName->getEditString();
  myGameProperties->set("Cartridge.Name", s);

  s = myManufacturer->getEditString();
  myGameProperties->set("Cartridge.Manufacturer", s);

  s = myModelNo->getEditString();
  myGameProperties->set("Cartridge.ModelNo", s);

  s = myRarity->getEditString();
  myGameProperties->set("Cartridge.Rarity", s);

  s = myNote->getEditString();
  myGameProperties->set("Cartridge.Note", s);

  i = mySound->getSelectedTag();
  s = (i == 1) ? "Mono" : "Stereo";
  myGameProperties->set("Cartridge.Sound", s);

  // FIXME - type



}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GameInfoDialog::handleCommand(CommandSender* sender, int cmd,
                                          int data, int id)
{
  switch (cmd)
  {
    case kOKCmd:
      saveConfig();
      close();
      break;

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
      break;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const PropType GameInfoDialog::ourControllerList[5] = {
  { "Booster-Grip", "BOOSTER-GRIP" },
  { "Driving",      "DRIVING"      },
  { "Keyboard",     "KEYBOARD"     },
  { "Paddles",      "PADDLES"      },
  { "Joystick",     "JOYSTICK"     }
};
