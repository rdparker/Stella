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
// Copyright (c) 1995-2007 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: FileSnapDialog.cxx,v 1.8 2007-07-20 13:31:11 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "FSNode.hxx"
#include "DialogContainer.hxx"
#include "BrowserDialog.hxx"
#include "TabWidget.hxx"
#include "bspf.hxx"
#include "LauncherDialog.hxx"
#include "FileSnapDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FileSnapDialog::FileSnapDialog(
      OSystem* osystem, DialogContainer* parent,
      const GUI::Font& font, GuiObject* boss,
      int x, int y, int w, int h)
  : Dialog(osystem, parent, x, y, w, h),
    CommandSender(boss),
    myBrowser(NULL),
    myIsGlobal(boss != 0)
{
  const int vBorder = 4;
  int xpos, ypos, bwidth, bheight;
  WidgetArray wid;
  ButtonWidget* b;
  int tabID;

  bwidth  = font.getStringWidth("Cancel") + 20;
  bheight = font.getLineHeight() + 4;

  // The tab widget
  xpos = 2; ypos = vBorder;
  myTab = new TabWidget(this, font, xpos, ypos, _w - 2*xpos, _h - 2*bheight - ypos);
  addTabWidget(myTab);
  addFocusWidget(myTab);

  // 1) The browser settings tab
  wid.clear();
  tabID = myTab->addTab("Browser Settings");

  // ROM path
  xpos = 15;  ypos += 5;
  b = new ButtonWidget(myTab, font, xpos, ypos, bwidth, bheight, "Path",
                       kChooseRomDirCmd);
  wid.push_back(b);
  xpos += bwidth + 10;
  myRomPath = new StaticTextWidget(myTab, font, xpos, ypos + 3,
                                   _w - xpos - 10, font.getLineHeight(),
                                   "", kTextAlignLeft);

  // Use ROM browse mode
  xpos = 30;  ypos += myRomPath->getHeight() + 8;
  myBrowseCheckbox = new CheckboxWidget(myTab, font, xpos, ypos,
                                        "Browse folders", kBrowseDirCmd);
  wid.push_back(myBrowseCheckbox);

  // Reload current ROM listing
  xpos += myBrowseCheckbox->getWidth() + 20;
  myReloadButton = new ButtonWidget(myTab, font, xpos, ypos-2,
                                    font.getStringWidth("Reload ROM Listing") + 20,
                                    bheight,
                                    "Reload ROM Listing", kReloadRomDirCmd);
//  myReloadButton->setEditable(true);
  wid.push_back(myReloadButton); 

  // ROM settings are disabled while in game mode
  if(!myIsGlobal)
  {
    myTab->disableTab(0);
    // TODO - until I get the above method working, we also need to
    //        disable the specific widgets ourself
    myRomPath->clearFlags(WIDGET_ENABLED);
    for(unsigned int i = 0; i < wid.size(); ++i)
      wid[i]->clearFlags(WIDGET_ENABLED);
  }
  // Add focus widgets for ROM tab
  addToFocusList(wid, tabID);

  // 2) The configuration files tab
  wid.clear();
  tabID = myTab->addTab(" Config Files ");

  bwidth  = font.getStringWidth("Properties file:") + 20;

  // State directory
  xpos = 15;  ypos = vBorder + 5;
  b = new ButtonWidget(myTab, font, xpos, ypos, bwidth, bheight, "State path:",
                       kChooseStateDirCmd);
  wid.push_back(b);
  xpos += bwidth + 10;
  myStatePath = new StaticTextWidget(myTab, font, xpos, ypos + 3,
                                     _w - xpos - 10, font.getLineHeight(),
                                     "", kTextAlignLeft);

  // Cheat file
  xpos = 15;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(myTab, font, xpos, ypos, bwidth, bheight, "Cheat file:",
                       kChooseCheatFileCmd);
  wid.push_back(b);
  xpos += bwidth + 10;
  myCheatFile = new StaticTextWidget(myTab, font, xpos, ypos + 3,
                                     _w - xpos - 10, font.getLineHeight(),
                                     "", kTextAlignLeft);

  // Palette file
  xpos = 15;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(myTab, font, xpos, ypos, bwidth, bheight, "Palette file:",
                       kChoosePaletteFileCmd);
  wid.push_back(b);
  xpos += bwidth + 10;
  myPaletteFile = new StaticTextWidget(myTab, font, xpos, ypos + 3,
                                       _w - xpos - 10, font.getLineHeight(),
                                       "", kTextAlignLeft);

  // Properties file
  xpos = 15;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(myTab, font, xpos, ypos, bwidth, bheight, "Properties file:",
                       kChoosePropsFileCmd);
  wid.push_back(b);
  xpos += bwidth + 10;
  myPropsFile = new StaticTextWidget(myTab, font, xpos, ypos + 3,
                                     _w - xpos - 10, font.getLineHeight(),
                                     "", kTextAlignLeft);

  // Snapshot path
  xpos = 15;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(myTab, font, xpos, ypos, bwidth, bheight, "Snapshot path:",
                       kChooseSnapDirCmd);
  wid.push_back(b);
  xpos += bwidth + 10;
  mySnapPath = new StaticTextWidget(myTab, font, xpos, ypos + 3,
                                    _w - xpos - 10, font.getLineHeight(),
                                    "", kTextAlignLeft);

  // Snapshot single or multiple saves
  xpos = 30;  ypos += mySnapPath->getHeight() + 8;
  mySnapSingleCheckbox = new CheckboxWidget(myTab, font, xpos, ypos,
                                            "Multiple snapshots");
  wid.push_back(mySnapSingleCheckbox);

  // Add focus widgets for Snapshot tab
  addToFocusList(wid, tabID);

  // Activate the first tab
  myTab->setActiveTab(0);

  // Add OK & Cancel buttons
  wid.clear();
#ifndef MAC_OSX
  b = addButton(font, _w - 2 * (kButtonWidth + 7), _h - 24, "OK", kOKCmd);
  wid.push_back(b);
  addOKWidget(b);
  b = addButton(font, _w - (kButtonWidth + 10), _h - 24, "Cancel", kCloseCmd);
  wid.push_back(b);
  addCancelWidget(b);
#else
  b = addButton(font, _w - 2 * (kButtonWidth + 7), _h - 24, "Cancel", kCloseCmd);
  wid.push_back(b);
  addCancelWidget(b);
  b = addButton(font, _w - (kButtonWidth + 10), _h - 24, "OK", kOKCmd);
  wid.push_back(b);
  addOKWidget(b);
#endif
  addBGroupToFocusList(wid);

  // Create file browser dialog
  myBrowser = new BrowserDialog(this, font, 60, 20, 200, 200);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FileSnapDialog::~FileSnapDialog()
{
  delete myBrowser;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FileSnapDialog::loadConfig()
{
  myRomPath->setLabel(instance()->settings().getString("romdir"));
  bool b = instance()->settings().getBool("rombrowse");
  myBrowseCheckbox->setState(b);
  myReloadButton->setEnabled(myIsGlobal && !b);

  myStatePath->setLabel(instance()->stateDir());
  myCheatFile->setLabel(instance()->cheatFile());
  myPaletteFile->setLabel(instance()->paletteFile());
  myPropsFile->setLabel(instance()->propertiesFile());

  mySnapPath->setLabel(instance()->settings().getString("ssdir"));
  mySnapSingleCheckbox->setState(!instance()->settings().getBool("sssingle"));

  myTab->loadConfig();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FileSnapDialog::saveConfig()
{
  instance()->settings().setString("romdir", myRomPath->getLabel());
  instance()->settings().setBool("rombrowse", myBrowseCheckbox->getState());

  instance()->settings().setString("statedir", myStatePath->getLabel());
  instance()->settings().setString("cheatfile", myCheatFile->getLabel());
  instance()->settings().setString("palettefile", myPaletteFile->getLabel());
  instance()->settings().setString("propsfile", myPropsFile->getLabel());

  instance()->settings().setString("ssdir", mySnapPath->getLabel());
  instance()->settings().setBool("sssingle", !mySnapSingleCheckbox->getState());

  // Flush changes to disk and inform the OSystem
  instance()->settings().saveConfig();
  instance()->setConfigPaths();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FileSnapDialog::openBrowser(const string& title, const string& startpath,
                                 FilesystemNode::ListMode mode, int cmd)
{
  parent()->addDialog(myBrowser);

  myBrowser->setTitle(title);
  myBrowser->setEmitSignal(cmd);
  myBrowser->setStartPath(startpath, mode);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FileSnapDialog::handleCommand(CommandSender* sender, int cmd,
                                   int data, int id)
{
  switch (cmd)
  {
    case kOKCmd:
      saveConfig();
      close();
      if(myIsGlobal)
      {
        sendCommand(kBrowseChangedCmd, 0, 0); // Call this before refreshing ROMs
        sendCommand(kRomDirChosenCmd, 0, 0);  // Let the boss know romdir has changed
      }
      break;

    case kChooseRomDirCmd:
      openBrowser("Select ROM directory:", myRomPath->getLabel(),
                  FilesystemNode::kListDirectoriesOnly, kRomDirChosenCmd);
      break;

    case kChooseStateDirCmd:
      openBrowser("Select state directory:", myStatePath->getLabel(),
                  FilesystemNode::kListDirectoriesOnly, kStateDirChosenCmd);
      break;

    case kChooseCheatFileCmd:
      openBrowser("Select cheat file:", myCheatFile->getLabel(),
                  FilesystemNode::kListAll, kCheatFileChosenCmd);
      break;

    case kChoosePaletteFileCmd:
      openBrowser("Select palette file:", myPaletteFile->getLabel(),
                  FilesystemNode::kListAll, kPaletteFileChosenCmd);
      break;

    case kChoosePropsFileCmd:
      openBrowser("Select properties file:", myPropsFile->getLabel(),
                  FilesystemNode::kListAll, kPropsFileChosenCmd);
      break;

    case kChooseSnapDirCmd:
      openBrowser("Select snapshot directory:", mySnapPath->getLabel(),
                  FilesystemNode::kListDirectoriesOnly, kSnapDirChosenCmd);
      break;

    case kRomDirChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myRomPath->setLabel(dir.path());
      break;
    }

    case kStateDirChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myStatePath->setLabel(dir.path());
      break;
    }

    case kCheatFileChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myCheatFile->setLabel(dir.path());
      break;
    }

    case kPaletteFileChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myPaletteFile->setLabel(dir.path());
      break;
    }

    case kPropsFileChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myPropsFile->setLabel(dir.path());
      break;
    }

    case kSnapDirChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      mySnapPath->setLabel(dir.path());
      break;
    }

    case kReloadRomDirCmd:
    {
      sendCommand(kReloadRomDirCmd, 0, 0);
      break;
    }

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
      break;
  }
}
