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
// Copyright (c) 1995-2009 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: FileSnapDialog.cxx,v 1.28 2009-01-16 16:38:06 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include "bspf.hxx"

#include "BrowserDialog.hxx"
#include "EditTextWidget.hxx"
#include "FSNode.hxx"
#include "LauncherDialog.hxx"
#include "Settings.hxx"

#include "FileSnapDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FileSnapDialog::FileSnapDialog(
      OSystem* osystem, DialogContainer* parent,
      const GUI::Font& font, GuiObject* boss)
  : Dialog(osystem, parent, 0, 0, 0, 0),
    CommandSender(boss),
    myBrowser(NULL),
    myIsGlobal(boss != 0)
{
  const int lineHeight   = font.getLineHeight(),
            fontWidth    = font.getMaxCharWidth(),
            buttonWidth  = font.getStringWidth("Properties file:") + 20,
            buttonHeight = font.getLineHeight() + 4;
  const int vBorder = 8;
  int xpos, ypos;
  WidgetArray wid;
  ButtonWidget* b;

  // Set real dimensions
  _w = 52 * fontWidth + 8;
  _h = 10 * (lineHeight + 4) + 10;

  xpos = vBorder;  ypos = vBorder;

  // ROM path
  ButtonWidget* romButton = 
    new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                     "Rom path:", kChooseRomDirCmd);
  wid.push_back(romButton);
  xpos += buttonWidth + 10;
  myRomPath = new EditTextWidget(this, font, xpos, ypos + 2,
                                 _w - xpos - 10, lineHeight, "");
  wid.push_back(myRomPath);

  // State directory
  xpos = vBorder;  ypos += romButton->getHeight() + 3;
  b = new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                       "State path:", kChooseStateDirCmd);
  wid.push_back(b);
  xpos += buttonWidth + 10;
  myStatePath = new EditTextWidget(this, font, xpos, ypos + 2,
                                   _w - xpos - 10, lineHeight, "");
  wid.push_back(myStatePath);

  // Cheat file
  xpos = vBorder;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                       "Cheat file:", kChooseCheatFileCmd);
  wid.push_back(b);
  xpos += buttonWidth + 10;
  myCheatFile = new EditTextWidget(this, font, xpos, ypos + 2,
                                   _w - xpos - 10, lineHeight, "");
  wid.push_back(myCheatFile);

  // Palette file
  xpos = vBorder;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                       "Palette file:", kChoosePaletteFileCmd);
  wid.push_back(b);
  xpos += buttonWidth + 10;
  myPaletteFile = new EditTextWidget(this, font, xpos, ypos + 2,
                                     _w - xpos - 10, lineHeight, "");
  wid.push_back(myPaletteFile);

  // Properties file
  xpos = vBorder;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                       "Properties file:", kChoosePropsFileCmd);
  wid.push_back(b);
  xpos += buttonWidth + 10;
  myPropsFile = new EditTextWidget(this, font, xpos, ypos + 2,
                                   _w - xpos - 10, lineHeight, "");
  wid.push_back(myPropsFile);

  // Snapshot path
  xpos = vBorder;  ypos += b->getHeight() + 3;
  b = new ButtonWidget(this, font, xpos, ypos, buttonWidth, buttonHeight,
                       "Snapshot path:", kChooseSnapDirCmd);
  wid.push_back(b);
  xpos += buttonWidth + 10;
  mySnapPath = new EditTextWidget(this, font, xpos, ypos + 2,
                                  _w - xpos - 10, lineHeight, "");
  wid.push_back(mySnapPath);

  // Snapshot single or multiple saves
  xpos = 30;  ypos += b->getHeight() + 5;
  mySnapSingle = new CheckboxWidget(this, font, xpos, ypos,
                                    "Multiple snapshots");
  wid.push_back(mySnapSingle);

  // Snapshot in 1x mode (ignore scaling)
  xpos += mySnapSingle->getWidth() + 20;
  mySnap1x = new CheckboxWidget(this, font, xpos, ypos,
                                "Snapshot in 1x mode");
  wid.push_back(mySnap1x);

  // Add Defaults, OK and Cancel buttons
  b = new ButtonWidget(this, font, 10, _h - buttonHeight - 10,
                       font.getStringWidth("Defaults") + 20, buttonHeight,
                       "Defaults", kDefaultsCmd);
  wid.push_back(b);
  addOKCancelBGroup(wid, font);

  addToFocusList(wid);

  // All ROM settings are disabled while in game mode
  if(!myIsGlobal)
  {
    romButton->clearFlags(WIDGET_ENABLED);
    myRomPath->setEditable(false);
  }

  // Create file browser dialog
  myBrowser = new BrowserDialog(this, font);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FileSnapDialog::~FileSnapDialog()
{
  delete myBrowser;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FileSnapDialog::loadConfig()
{
  const Settings& settings = instance().settings();
  myRomPath->setEditString(settings.getString("romdir"));
  myStatePath->setEditString(settings.getString("statedir"));
  myCheatFile->setEditString(settings.getString("cheatfile"));
  myPaletteFile->setEditString(settings.getString("palettefile"));
  myPropsFile->setEditString(settings.getString("propsfile"));
  mySnapPath->setEditString(settings.getString("ssdir"));
  mySnapSingle->setState(!settings.getBool("sssingle"));
  mySnap1x->setState(settings.getBool("ss1x"));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FileSnapDialog::saveConfig()
{
  instance().settings().setString("romdir", myRomPath->getEditString());
  instance().settings().setString("statedir", myStatePath->getEditString());
  instance().settings().setString("cheatfile", myCheatFile->getEditString());
  instance().settings().setString("palettefile", myPaletteFile->getEditString());
  instance().settings().setString("propsfile", myPropsFile->getEditString());
  instance().settings().setString("ssdir", mySnapPath->getEditString());
  instance().settings().setBool("sssingle", !mySnapSingle->getState());
  instance().settings().setBool("ss1x", mySnap1x->getState());

  // Flush changes to disk and inform the OSystem
  instance().settings().saveConfig();
  instance().setConfigPaths();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FileSnapDialog::setDefaults()
{
  const string& basedir = instance().baseDir();
  const string& romdir = "roms";
  const string& statedir = basedir + BSPF_PATH_SEPARATOR + "state";
  const string& cheatfile = basedir + BSPF_PATH_SEPARATOR + "stella.cht";
  const string& palettefile = basedir + BSPF_PATH_SEPARATOR + "stella.pal";
  const string& propsfile = basedir + BSPF_PATH_SEPARATOR + "stella.pro";
  const string& ssdir = basedir + BSPF_PATH_SEPARATOR + "snapshots";

  myRomPath->setEditString(romdir);
  myStatePath->setEditString(statedir);
  myCheatFile->setEditString(cheatfile);
  myPaletteFile->setEditString(palettefile);
  myPropsFile->setEditString(propsfile);

  mySnapPath->setEditString(ssdir);
  mySnapSingle->setState(true);
  mySnap1x->setState(false);
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
        sendCommand(kRomDirChosenCmd, 0, 0);  // Let the boss know romdir has changed
      break;

    case kDefaultsCmd:
      setDefaults();
      break;

    case kChooseRomDirCmd:
      myBrowser->show("Select ROM directory:", myRomPath->getEditString(),
                      FilesystemNode::kListDirectoriesOnly, kRomDirChosenCmd);
      break;

    case kChooseStateDirCmd:
      myBrowser->show("Select state directory:", myStatePath->getEditString(),
                      FilesystemNode::kListDirectoriesOnly, kStateDirChosenCmd);
      break;

    case kChooseCheatFileCmd:
      myBrowser->show("Select cheat file:", myCheatFile->getEditString(),
                      FilesystemNode::kListAll, kCheatFileChosenCmd);
      break;

    case kChoosePaletteFileCmd:
      myBrowser->show("Select palette file:", myPaletteFile->getEditString(),
                      FilesystemNode::kListAll, kPaletteFileChosenCmd);
      break;

    case kChoosePropsFileCmd:
      myBrowser->show("Select properties file:", myPropsFile->getEditString(),
                      FilesystemNode::kListAll, kPropsFileChosenCmd);
      break;

    case kChooseSnapDirCmd:
      myBrowser->show("Select snapshot directory:", mySnapPath->getEditString(),
                      FilesystemNode::kListDirectoriesOnly, kSnapDirChosenCmd);
      break;

    case kRomDirChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myRomPath->setEditString(dir.getPath());
      break;
    }

    case kStateDirChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myStatePath->setEditString(dir.getPath());
      break;
    }

    case kCheatFileChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myCheatFile->setEditString(dir.getPath());
      break;
    }

    case kPaletteFileChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myPaletteFile->setEditString(dir.getPath());
      break;
    }

    case kPropsFileChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      myPropsFile->setEditString(dir.getPath());
      break;
    }

    case kSnapDirChosenCmd:
    {
      FilesystemNode dir(myBrowser->getResult());
      mySnapPath->setEditString(dir.getPath());
      break;
    }

    case kReloadRomDirCmd:
      sendCommand(kReloadRomDirCmd, 0, 0);	 
      break;

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
      break;
  }
}
