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
// $Id: VideoDialog.cxx,v 1.43 2007-06-20 16:33:23 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2002-2004 The ScummVM project
//============================================================================

#include <sstream>

#include "OSystem.hxx"
#include "Settings.hxx"
#include "Menu.hxx"
#include "Control.hxx"
#include "Widget.hxx"
#include "PopUpWidget.hxx"
#include "Dialog.hxx"
#include "VideoDialog.hxx"
#include "GuiUtils.hxx"

#include "bspf.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VideoDialog::VideoDialog(OSystem* osystem, DialogContainer* parent,
                         const GUI::Font& font, int x, int y, int w, int h)
  : Dialog(osystem, parent, x, y, w, h)
{
  const int lineHeight = font.getLineHeight(),
            fontHeight = font.getFontHeight();
  int xpos, ypos;
  int lwidth = font.getStringWidth("Dirty Rects: "),
      pwidth = font.getStringWidth("Software");
  WidgetArray wid;

  xpos = 5;  ypos = 10;

  // Video renderer
  myRendererPopup = new PopUpWidget(this, font, xpos, ypos,
                                    pwidth, lineHeight, "Renderer: ", lwidth,
                                    kRendererChanged);
  myRendererPopup->appendEntry("Software", 1);
#ifdef DISPLAY_OPENGL
  myRendererPopup->appendEntry("OpenGL", 2);
#endif
  wid.push_back(myRendererPopup);
  ypos += lineHeight + 4;

  // Video filter
  myFilterPopup = new PopUpWidget(this, font, xpos, ypos,
                                  pwidth, lineHeight, "GL Filter: ", lwidth);
  myFilterPopup->appendEntry("Linear", 1);
  myFilterPopup->appendEntry("Nearest", 2);
  wid.push_back(myFilterPopup);
  ypos += lineHeight + 4;

  // GL FS stretch
  myFSStretchPopup = new PopUpWidget(this, font, xpos, ypos,
                                     pwidth, lineHeight, "GL Stretch: ", lwidth);
  myFSStretchPopup->appendEntry("Never", 1);
  myFSStretchPopup->appendEntry("UI mode", 2);
  myFSStretchPopup->appendEntry("TIA mode", 3);
  myFSStretchPopup->appendEntry("Always", 4);
  wid.push_back(myFSStretchPopup);
  ypos += lineHeight + 4;

  // Palette
  myPalettePopup = new PopUpWidget(this, font, xpos, ypos, pwidth,
                                   lineHeight, "Palette: ", lwidth);
  myPalettePopup->appendEntry("Standard", 1);
  myPalettePopup->appendEntry("Original", 2);
  myPalettePopup->appendEntry("Z26", 3);
  myPalettePopup->appendEntry("User", 4);
  wid.push_back(myPalettePopup);
  ypos += lineHeight + 4;

  // Available UI zoom levels
  myUIZoomSlider = new SliderWidget(this, font, xpos, ypos, pwidth, lineHeight,
                                    "UI Zoom: ", lwidth, kUIZoomChanged);
  myUIZoomSlider->setMinValue(1); myUIZoomSlider->setMaxValue(10);
  wid.push_back(myUIZoomSlider);
  myUIZoomLabel = new StaticTextWidget(this, font,
                                       xpos + myUIZoomSlider->getWidth() + 4,
                                       ypos + 1,
                                       15, fontHeight, "", kTextAlignLeft);
  myUIZoomLabel->setFlags(WIDGET_CLEARBG);
  ypos += lineHeight + 4;

  // Available TIA zoom levels
  myTIAZoomSlider = new SliderWidget(this, font, xpos, ypos, pwidth, lineHeight,
                                     "TIA Zoom: ", lwidth, kTIAZoomChanged);
  myTIAZoomSlider->setMinValue(1); myTIAZoomSlider->setMaxValue(10);
  wid.push_back(myTIAZoomSlider);
  myTIAZoomLabel = new StaticTextWidget(this, font,
                                        xpos + myTIAZoomSlider->getWidth() + 4,
                                        ypos + 1,
                                        15, fontHeight, "", kTextAlignLeft);
  myTIAZoomLabel->setFlags(WIDGET_CLEARBG);

  // Move over to the next column
  xpos += 115;  ypos = 10;

  // Framerate
  myFrameRateSlider = new SliderWidget(this, font, xpos, ypos, 30, lineHeight,
                                       "Framerate: ", lwidth, kFrameRateChanged);
  myFrameRateSlider->setMinValue(1); myFrameRateSlider->setMaxValue(300);
  wid.push_back(myFrameRateSlider);
  myFrameRateLabel = new StaticTextWidget(this, font,
                                          xpos + myFrameRateSlider->getWidth() + 4,
                                          ypos + 1,
                                          15, fontHeight, "", kTextAlignLeft);
  myFrameRateLabel->setFlags(WIDGET_CLEARBG);
  ypos += lineHeight + 4;

  // Fullscreen
  myFullscreenCheckbox = new CheckboxWidget(this, font, xpos + 5, ypos,
                                            "Fullscreen mode", kFullScrChanged);
  wid.push_back(myFullscreenCheckbox);
  ypos += lineHeight + 4;

  // PAL color-loss effect
  myColorLossCheckbox = new CheckboxWidget(this, font, xpos + 5, ypos,
                                           "PAL color-loss");
  wid.push_back(myColorLossCheckbox);
  ypos += lineHeight + 4;

  // Use sync to vblank in OpenGL
  myUseVSyncCheckbox = new CheckboxWidget(this, font, xpos + 5, ypos,
                                          "GL VSync");
  wid.push_back(myUseVSyncCheckbox);
  ypos += lineHeight + 4;

  // Center window (in windowed mode)
  myCenterCheckbox = new CheckboxWidget(this, font, xpos + 5, ypos,
                                        "Center window (*)");
  wid.push_back(myCenterCheckbox);
  ypos += lineHeight + 4;

  // Add message concerning usage
  lwidth = font.getStringWidth("(*) Requires application restart");
  new StaticTextWidget(this, font, 10, _h - 38, lwidth, fontHeight,
                       "(*) Requires application restart",
                       kTextAlignLeft);

  // Add Defaults, OK and Cancel buttons
  ButtonWidget* b;
  b = addButton(font, 10, _h - 24, "Defaults", kDefaultsCmd);
  wid.push_back(b);
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

  addToFocusList(wid);

  // Disable certain functions when we know they aren't present
#ifndef DISPLAY_GL
  myFilterPopup->clearFlags(WIDGET_ENABLED);
  myFSStretchPopup->clearFlags(WIDGET_ENABLED);
  myUseVSyncCheckbox->clearFlags(WIDGET_ENABLED);
#endif
#ifndef WINDOWED_SUPPORT
  myUIZoomSlider->clearFlags(WIDGET_ENABLED);
  myUIZoomLabel->clearFlags(WIDGET_ENABLED);
  myTIAZoomSlider->clearFlags(WIDGET_ENABLED);
  myTIAZoomLabel->clearFlags(WIDGET_ENABLED);
  myFullscreenCheckbox->clearFlags(WIDGET_ENABLED);
  myCenterCheckbox->clearFlags(WIDGET_ENABLED);
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VideoDialog::~VideoDialog()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::loadConfig()
{
  string s;
  bool b;
  int i;

  // Renderer setting
  s = instance()->settings().getString("video");
  if(s == "soft")    myRendererPopup->setSelectedTag(1);
  else if(s == "gl") myRendererPopup->setSelectedTag(2);

  // Filter setting
  s = instance()->settings().getString("gl_filter");
  if(s == "linear")       myFilterPopup->setSelectedTag(1);
  else if(s == "nearest") myFilterPopup->setSelectedTag(2);

  // GL stretch setting
  s = instance()->settings().getString("gl_fsmax");
  if(s == "never")        myFSStretchPopup->setSelectedTag(1);
  else if(s == "ui")      myFSStretchPopup->setSelectedTag(2);
  else if(s == "tia")     myFSStretchPopup->setSelectedTag(3);
  else if(s == "always")  myFSStretchPopup->setSelectedTag(4);
  else                    myFSStretchPopup->setSelectedTag(1);

  // Palette
  s = instance()->settings().getString("palette");
  if(s == "standard")      myPalettePopup->setSelectedTag(1);
  else if(s == "original") myPalettePopup->setSelectedTag(2);
  else if(s == "z26")      myPalettePopup->setSelectedTag(3);
  else if(s == "user")     myPalettePopup->setSelectedTag(4);

  // UI zoom level
  s = instance()->settings().getString("zoom_ui");
  i = instance()->settings().getInt("zoom_ui");
  myUIZoomSlider->setValue(i);
  myUIZoomLabel->setLabel(s);

  // TIA zoom level
  s = instance()->settings().getString("zoom_tia");
  i = instance()->settings().getInt("zoom_tia");
  myTIAZoomSlider->setValue(i);
  myTIAZoomLabel->setLabel(s);

  // FIXME - what to do with this??
  myFrameRateSlider->setEnabled(false);

  // Fullscreen
  b = instance()->settings().getBool("fullscreen");
  myFullscreenCheckbox->setState(b);
  handleFullscreenChange(b);

  // PAL color-loss effect
  b = instance()->settings().getBool("colorloss");
  myColorLossCheckbox->setState(b);

  // Use sync to vertical blank (GL mode only)
  b = instance()->settings().getBool("gl_vsync");
  myUseVSyncCheckbox->setState(b);

  // Center window
  b = instance()->settings().getBool("center");
  myCenterCheckbox->setState(b);

  // Make sure that mutually-exclusive items are not enabled at the same time
  i = myRendererPopup->getSelectedTag();
  handleRendererChange(i);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::saveConfig()
{
  string s;
  int i;
  bool b;

  // Renderer setting
  i = myRendererPopup->getSelectedTag();
  if(i == 1)       s = "soft";
  else if(i == 2)  s = "gl";
  instance()->settings().setString("video", s);

  // Filter setting
  i = myFilterPopup->getSelectedTag();
  if(i == 1)      s = "linear";
  else if(i == 2) s = "nearest";
  instance()->settings().setString("gl_filter", s);

  // GL stretch setting
  i = myFSStretchPopup->getSelectedTag();
  if(i == 1)       s = "never";
  else if(i == 2)  s = "ui";
  else if(i == 3)  s = "tia";
  else if(i == 4)  s = "always";
  instance()->settings().setString("gl_fsmax", s);

/*
  // Aspect ratio
  s = myAspectRatioLabel->getLabel();
  instance()->settings().setString("gl_aspect", s);
*/

  // Palette
  i = myPalettePopup->getSelectedTag();
  if(i == 1)       s = "standard";
  else if(i == 2)  s = "original";
  else if(i == 3)  s = "z26";
  else if(i == 4)  s = "user";
  instance()->settings().setString("palette", s);

  // UI Scaler
  s = myUIZoomLabel->getLabel();
  instance()->settings().setString("zoom_ui", s);

  // TIA Scaler
  s = myTIAZoomLabel->getLabel();
  instance()->settings().setString("zoom_tia", s);

  // Framerate   FIXME - I haven't figured out what to do with this yet
/*
  i = myFrameRateSlider->getValue();
  if(i > 0)
    instance()->setFramerate(i);
*/

  // Fullscreen
  b = myFullscreenCheckbox->getState();
  instance()->settings().setBool("fullscreen", b);

  // PAL color-loss effect
  b = myColorLossCheckbox->getState();
  instance()->settings().setBool("colorloss", b);

  // Use sync to vertical blank (GL mode only)
  b = myUseVSyncCheckbox->getState();
  instance()->settings().setBool("gl_vsync", b);

  // Center window
  b = myCenterCheckbox->getState();
  instance()->settings().setBool("center", b);

  // Finally, issue a complete framebuffer re-initialization
  instance()->createFrameBuffer(false);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::setDefaults()
{
  myRendererPopup->setSelectedTag(1);
  myFilterPopup->setSelectedTag(1);
  myFSStretchPopup->setSelectedTag(1);
  myPalettePopup->setSelectedTag(1);
  myUIZoomSlider->setValue(2);
  myUIZoomLabel->setLabel("2");
  myTIAZoomSlider->setValue(2);
  myTIAZoomLabel->setLabel("2");
//  myFrameRateSlider->setValue(0);
//  myFrameRateLabel->setLabel("0");

  myFullscreenCheckbox->setState(false);
  myColorLossCheckbox->setState(false);
  myUseVSyncCheckbox->setState(true);
  myCenterCheckbox->setState(true);

  // Make sure that mutually-exclusive items are not enabled at the same time
  handleRendererChange(1);  // 1 indicates software mode
  handleFullscreenChange(false);  // indicates fullscreen deactivated
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::handleRendererChange(int item)
{
#ifdef DISPLAY_OPENGL
  // When we're in software mode, certain OpenGL-related options are disabled
  bool gl = (item > 1) ? true : false;

  myFilterPopup->setEnabled(gl);
  myFSStretchPopup->setEnabled(gl);
  myFSStretchPopup->setEnabled(gl);
  myUseVSyncCheckbox->setEnabled(gl);

  _dirty = true;
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::handleFullscreenChange(bool enable)
{
#ifdef WINDOWED_SUPPORT
  myUIZoomSlider->setEnabled(!enable);
  myUIZoomLabel->setEnabled(!enable);
  myTIAZoomSlider->setEnabled(!enable);
  myTIAZoomLabel->setEnabled(!enable);

  _dirty = true;
#endif
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::handleCommand(CommandSender* sender, int cmd,
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

    case kRendererChanged:
      handleRendererChange(data);
      break;

    case kUIZoomChanged:
      myUIZoomLabel->setValue(myUIZoomSlider->getValue());
      break;

    case kTIAZoomChanged:
      myTIAZoomLabel->setValue(myTIAZoomSlider->getValue());
      break;

    case kFrameRateChanged:
      myFrameRateLabel->setValue(myFrameRateSlider->getValue());
      break;

    case kFullScrChanged:
      handleFullscreenChange(myFullscreenCheckbox->getState());
      break;

    default:
      Dialog::handleCommand(sender, cmd, data, 0);
      break;
  }
}
