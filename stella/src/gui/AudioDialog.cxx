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
// Copyright (c) 1995-2008 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: AudioDialog.cxx,v 1.28 2008-06-13 13:14:51 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include <sstream>

#include "bspf.hxx"

#include "Console.hxx"
#include "Control.hxx"
#include "Dialog.hxx"
#include "Menu.hxx"
#include "OSystem.hxx"
#include "PopUpWidget.hxx"
#include "StringList.hxx"
#include "Settings.hxx"
#include "Sound.hxx"
#include "Widget.hxx"

#include "AudioDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AudioDialog::AudioDialog(OSystem* osystem, DialogContainer* parent,
                         const GUI::Font& font, int x, int y, int w, int h)
  : Dialog(osystem, parent, x, y, w, h)
{
  const int lineHeight   = font.getLineHeight(),
            fontWidth    = font.getMaxCharWidth(),
            fontHeight   = font.getFontHeight(),
            buttonWidth  = font.getStringWidth("Defaults") + 20,
            buttonHeight = font.getLineHeight() + 4;
  int xpos, ypos;
  int lwidth = font.getStringWidth("Fragment Size: "),
      pwidth = font.getStringWidth("4096");
  WidgetArray wid;
  StringList items;

  // Set real dimensions
//  _w = 35 * fontWidth + 10;
//  _h = 8 * (lineHeight + 4) + 10;

  // Volume
  xpos = 3 * fontWidth;  ypos = 10;

  myVolumeSlider = new SliderWidget(this, font, xpos, ypos, 6*fontWidth, lineHeight,
                                    "Volume: ", lwidth, kVolumeChanged);
  myVolumeSlider->setMinValue(1); myVolumeSlider->setMaxValue(100);
  wid.push_back(myVolumeSlider);
  myVolumeLabel = new StaticTextWidget(this, font,
                                       xpos + myVolumeSlider->getWidth() + 4,
                                       ypos + 1,
                                       3*fontWidth, fontHeight, "", kTextAlignLeft);

  myVolumeLabel->setFlags(WIDGET_CLEARBG);
  ypos += lineHeight + 4;

  // Fragment size
  items.clear();
  items.push_back("128");
  items.push_back("256");
  items.push_back("512");
  items.push_back("1024");
  items.push_back("2048");
  items.push_back("4096");
  myFragsizePopup = new PopUpWidget(this, font, xpos, ypos,
                                    pwidth + myVolumeLabel->getWidth() - 4, lineHeight,
                                    items, "Fragment size: ", lwidth);
  wid.push_back(myFragsizePopup);
  ypos += lineHeight + 4;

  // Output frequency
  items.clear();
  items.push_back("11025");
  items.push_back("22050");
  items.push_back("31400");
  items.push_back("44100");
  items.push_back("48000");
  myFreqPopup = new PopUpWidget(this, font, xpos, ypos,
                                pwidth + myVolumeLabel->getWidth() - 4, lineHeight,
                                items, "Output freq: ", lwidth);
  wid.push_back(myFreqPopup);
  ypos += lineHeight + 4;

  // TIA frequency
  // ... use same items as above
  myTiaFreqPopup = new PopUpWidget(this, font, xpos, ypos,
                                   pwidth + myVolumeLabel->getWidth() - 4, lineHeight,
                                   items, "TIA freq: ", lwidth);
  wid.push_back(myTiaFreqPopup);
  ypos += lineHeight + 4;

  // Clip volume
  myClipVolumeCheckbox = new CheckboxWidget(this, font, xpos+28, ypos,
                                            "Clip volume", 0);
  wid.push_back(myClipVolumeCheckbox);
  ypos += lineHeight + 4;

  // Enable sound
  mySoundEnableCheckbox = new CheckboxWidget(this, font, xpos+28, ypos,
                                             "Enable sound", kSoundEnableChanged);
  wid.push_back(mySoundEnableCheckbox);
  ypos += lineHeight + 12;

  // Add Defaults, OK and Cancel buttons
  ButtonWidget* b;
  b = new ButtonWidget(this, font, 10, _h - buttonHeight - 10,
                       buttonWidth, buttonHeight, "Defaults", kDefaultsCmd);
  wid.push_back(b);
  addOKCancelBGroup(wid, font);

  addToFocusList(wid);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AudioDialog::~AudioDialog()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void AudioDialog::loadConfig()
{
  bool b;
  int i;

  // Volume
  myVolumeSlider->setValue(instance().settings().getInt("volume"));
  myVolumeLabel->setLabel(instance().settings().getString("volume"));

  // Fragsize
  i = instance().settings().getInt("fragsize");
  if(i == 128)       i = 0;
  else if(i == 256)  i = 1;
  else if(i == 512)  i = 2;
  else if(i == 1024) i = 3;
  else if(i == 2048) i = 4;
  else if(i == 4096) i = 5;
  else               i = 2;  // default to '512'
  myFragsizePopup->setSelected(i);

  // Output frequency
  i = instance().settings().getInt("freq");
  if(i == 11025)      i = 0;
  else if(i == 22050) i = 1;
  else if(i == 31400) i = 2;
  else if(i == 44100) i = 3;
  else if(i == 48000) i = 4;
  else                i = 2;  // default to '31400'
  myFreqPopup->setSelected(i);

  // TIA frequency
  i = instance().settings().getInt("tiafreq");
  if(i == 11025)      i = 0;
  else if(i == 22050) i = 1;
  else if(i == 31400) i = 2;
  else if(i == 44100) i = 3;
  else if(i == 48000) i = 4;
  else                i = 2;  // default to '31400'
  myTiaFreqPopup->setSelected(i);

  // Clip volume
  b = instance().settings().getBool("clipvol");
  myClipVolumeCheckbox->setState(b);

  // Enable sound
  b = instance().settings().getBool("sound");
  mySoundEnableCheckbox->setState(b);

  // Make sure that mutually-exclusive items are not enabled at the same time
  handleSoundEnableChange(b);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void AudioDialog::saveConfig()
{
  Settings& settings = instance().settings();
  string s;
  bool b;
  int i;

  // Volume
  i = myVolumeSlider->getValue();
  instance().sound().setVolume(i);

  // Fragsize
  s = myFragsizePopup->getSelectedString();
  settings.setString("fragsize", s);

  // Output frequency
  s = myFreqPopup->getSelectedString();
  settings.setString("freq", s);

  // TIA frequency
  s = myTiaFreqPopup->getSelectedString();
  settings.setString("tiafreq", s);

  // Enable/disable volume clipping (requires a restart to take effect)
  b = myClipVolumeCheckbox->getState();
  settings.setBool("clipvol", b);

  // Enable/disable sound (requires a restart to take effect)
  b = mySoundEnableCheckbox->getState();
  instance().sound().setEnabled(b);

  // Only force a re-initialization when necessary, since it can
  // be a time-consuming operation
  if(&instance().console())
    instance().console().initializeAudio();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void AudioDialog::setDefaults()
{
  myVolumeSlider->setValue(100);
  myVolumeLabel->setLabel("100");

  myFragsizePopup->setSelected(2);  // 512 bytes
  myFreqPopup->setSelected(2);      // 31400 Hz
  myTiaFreqPopup->setSelected(2);   // 31400 Hz

  myClipVolumeCheckbox->setState(true);
  mySoundEnableCheckbox->setState(true);

  // Make sure that mutually-exclusive items are not enabled at the same time
  handleSoundEnableChange(true);

  _dirty = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void AudioDialog::handleSoundEnableChange(bool active)
{
  myVolumeSlider->setEnabled(active);
  myVolumeLabel->setEnabled(active);
  myFragsizePopup->setEnabled(active);
  myFreqPopup->setEnabled(active);
  myTiaFreqPopup->setEnabled(active);
  myClipVolumeCheckbox->setEnabled(active);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void AudioDialog::handleCommand(CommandSender* sender, int cmd,
                                int data, int id)
{
  switch(cmd)
  {
    case kOKCmd:
      saveConfig();
      close();
      break;

    case kDefaultsCmd:
      setDefaults();
      break;

    case kVolumeChanged:
      myVolumeLabel->setValue(myVolumeSlider->getValue());
      break;

    case kSoundEnableChanged:
      handleSoundEnableChange(data == 1);
      break;

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
      break;
  }
}
