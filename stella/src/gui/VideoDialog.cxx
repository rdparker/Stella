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
// $Id: VideoDialog.cxx,v 1.5 2005-03-27 03:07:34 stephena Exp $
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

enum {
  kVideoRowHeight = 12,
  kVideoWidth     = 200,
  kVideoHeight    = 100
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VideoDialog::VideoDialog(OSystem* osystem, uInt16 x, uInt16 y, uInt16 w, uInt16 h)
    : Dialog(osystem, x, y, w, h)
{
  int yoff = 10,
      xoff = 2,
      woff = _w - 130,
      labelWidth = 65;

  // Video driver (query OSystem for what's supported)
  myDriverPopup = new PopUpWidget(this, xoff, yoff, woff, kLineHeight,
                                  "(*)Driver: ", labelWidth);
//  myDriverPopup->appendEntry("First one", 1);
//  myDriverPopup->appendEntry("Another one", 2);
  yoff += kVideoRowHeight + 4;

// FIXME - get list of video drivers from OSystem
//  const Common::LanguageDescription *l = Common::g_languages;
//  for (; l->code; ++l) {
//    _langPopUp->appendEntry(l->description, l->id);
//  }

  // Video renderer
  myRendererPopup = new PopUpWidget(this, xoff, yoff, woff, kLineHeight,
                                    "(*)Renderer: ", labelWidth, kRendererChanged);
  myRendererPopup->appendEntry("Software", 1);
  myRendererPopup->appendEntry("OpenGL", 2);
  yoff += kVideoRowHeight + 4;

  // Video filter
  myFilterPopup = new PopUpWidget(this, xoff, yoff, woff, kLineHeight,
                                  "GL Filter: ", labelWidth);
  myFilterPopup->appendEntry("Linear", 1);
  myFilterPopup->appendEntry("Nearest", 2);
  yoff += kVideoRowHeight + 4;

  // Aspect ratio
  myAspectRatioSlider = new SliderWidget(this, xoff, yoff, woff - 14, kLineHeight,
                                         "(*)GL Aspect: ", labelWidth, kAspectRatioChanged);
  myAspectRatioSlider->setMinValue(1); myAspectRatioSlider->setMaxValue(100);
  myAspectRatioLabel = new StaticTextWidget(this, xoff + woff - 11, yoff, 24, kLineHeight,
                       "", kTextAlignLeft);
  myAspectRatioLabel->setFlags(WIDGET_CLEARBG);
  yoff += kVideoRowHeight + 4;

  // Palette
  myPalettePopup = new PopUpWidget(this, xoff, yoff, woff, kLineHeight, "Palette: ", labelWidth);
  myPalettePopup->appendEntry("Standard", 1);
  myPalettePopup->appendEntry("Original", 2);
  myPalettePopup->appendEntry("Z26", 3);
  yoff += kVideoRowHeight + 4;

  // Move over to the next column
  yoff = 10;
  xoff = xoff + 115;

  // Framerate
  myFrameRateSlider = new SliderWidget(this, xoff, yoff, woff - 14, kLineHeight,
                                       "Framerate: ", labelWidth, kFrameRateChanged);
  myFrameRateSlider->setMinValue(1); myFrameRateSlider->setMaxValue(300);
  myFrameRateLabel = new StaticTextWidget(this, xoff + woff - 11, yoff, 20, kLineHeight,
                                          "", kTextAlignLeft);
  myFrameRateLabel->setFlags(WIDGET_CLEARBG);
  yoff += kVideoRowHeight + 4;

  // Zoom level
  myZoomSlider = new SliderWidget(this, xoff, yoff, woff - 14, kLineHeight,
                                  "Zoom: ", labelWidth, kZoomChanged);
  myZoomSlider->setMinValue(0); myZoomSlider->setMaxValue(50);
  myZoomLabel = new StaticTextWidget(this, xoff + woff - 11, yoff, 20, kLineHeight,
                                     "", kTextAlignLeft);
  myZoomLabel->setFlags(WIDGET_CLEARBG);
  yoff += kVideoRowHeight + 10;

  myFullscreenCheckbox = new CheckboxWidget(this, xoff + 25, yoff, woff - 14, kLineHeight,
                                            "Fullscreen mode");
  yoff += kVideoRowHeight + 4;

  myUseDeskResCheckbox = new CheckboxWidget(this, xoff + 25, yoff, woff - 14, kLineHeight,
                                            "Desktop Res in FS");
  yoff += kVideoRowHeight + 20;

  // Add a short message about options that need a restart
  new StaticTextWidget(this, xoff-40, yoff, 170, kLineHeight,
                       "* Note that these options take effect", kTextAlignLeft);
  yoff += kVideoRowHeight;
  new StaticTextWidget(this, xoff-40, yoff, 170, kLineHeight,
                       "the next time you restart Stella.", kTextAlignLeft);

  // Add Defaults, OK and Cancel buttons
  addButton( 10, _h - 24, "Defaults", kDefaultsCmd, 0);
#ifndef MAC_OSX
  addButton(_w - 2 * (kButtonWidth + 7), _h - 24, "OK", kOKCmd, 0);
  addButton(_w - (kButtonWidth + 10), _h - 24, "Cancel", kCloseCmd, 0);
#else
  addButton(_w - 2 * (kButtonWidth + 7), _h - 24, "Cancel", kCloseCmd, 0);
  addButton(_w - (kButtonWidth + 10), _h - 24, "OK", kOKCmd, 0);
#endif

  setDefaults();
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
  uInt32 i;
  float f;

  // Driver setting
  myDriverPopup->setSelectedTag(0); // FIXME

  // Renderer setting
  s = instance()->settings().getString("video");
  if(s == "soft")
    myRendererPopup->setSelectedTag(1);
  else if(s == "gl")
    myRendererPopup->setSelectedTag(2);

  // Filter setting
  s = instance()->settings().getString("gl_filter");
  if(s == "linear")
    myFilterPopup->setSelectedTag(1);
  else if(s == "nearest")
    myFilterPopup->setSelectedTag(2);

  // Aspect ratio - another huge hack
  s = instance()->settings().getString("gl_aspect");
  f = instance()->settings().getFloat("gl_aspect");
  if(f == -1.0)
  {
    f = 1.1;
    s = "1.1";
  }
  else if(f < 1.1)
  {
    f = 1.1;
    s = "1.1";
  }
  else if(f > 2.0)
  {
    f = 2.0;
    s = "2.0";
  }
  i = (uInt32)((f * 10) - 10) * 10;
  myAspectRatioSlider->setValue(i);
  myAspectRatioLabel->setLabel(s);

  // Palette
  s = instance()->settings().getString("palette");
  if(s == "standard")
    myPalettePopup->setSelectedTag(1);
  else if(s == "original")
    myPalettePopup->setSelectedTag(2);
  else if(s == "z26")
    myPalettePopup->setSelectedTag(3);

  // Framerate
  myFrameRateSlider->setValue(instance()->settings().getInt("framerate"));
  myFrameRateLabel->setLabel(instance()->settings().getString("framerate"));

  // Zoom
  i = (instance()->settings().getInt("zoom") - 1) * 10;
  myZoomSlider->setValue(i);
  myZoomLabel->setLabel(instance()->settings().getString("zoom"));

  // Fullscreen
  b = instance()->settings().getBool("fullscreen");
  myFullscreenCheckbox->setState(b);

  // Use desktop resolution in fullscreen mode
  b = instance()->settings().getBool("gl_fsmax");
  myUseDeskResCheckbox->setState(b);

  // Make sure that mutually-exclusive items are not enabled at the same time
  i = myRendererPopup->getSelectedTag() - 1;
  handleRendererChange(i);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::saveConfig()
{
  string s;
  uInt32 i;
  bool b;

  // Driver setting
  s = myDriverPopup->getSelectedString();
  instance()->settings().setString("video_driver", s);

  // Renderer setting
  i = myRendererPopup->getSelectedTag();
  if(i == 1)
    instance()->settings().setString("video", "soft");
  else if(i == 2)
    instance()->settings().setString("video", "gl");

  // Filter setting
  i = myFilterPopup->getSelectedTag();
  if(i == 1)
    instance()->settings().setString("gl_filter", "linear");
  else if(i == 2)
    instance()->settings().setString("gl_filter", "nearest");
// FIXME - immediately change the filtering

  // Aspect ratio
  s = myAspectRatioLabel->getLabel();
  instance()->settings().setString("gl_aspect", s);

  // Palette
  i = myPalettePopup->getSelectedTag();
  if(i == 1)
    instance()->settings().setString("palette", "standard");
  else if(i == 2)
    instance()->settings().setString("palette", "original");
  else if(i == 3)
    instance()->settings().setString("palette", "z26");
  s = myPalettePopup->getSelectedString();
  instance()->settings().setString("palette", s); // FIXME - make this more efficient
  instance()->console().togglePalette(s);

  // Framerate
  i = myFrameRateSlider->getValue();
  instance()->settings().setInt("framerate", i);
// FIXME - immediately change the framerate

  // Zoom
  i = (myZoomSlider->getValue() / 10) + 1;
  instance()->settings().setInt("zoom", i);
  instance()->frameBuffer().resize(0, i);

  // Fullscreen (the toggleFullscreen function takes care of updating settings)
  b = myFullscreenCheckbox->getState();
  instance()->frameBuffer().toggleFullscreen(true, b);

  // Use desktop resolution in fullscreen mode
  b = myUseDeskResCheckbox->getState();
  instance()->settings().setBool("gl_fsmax", b);
// FIXME - immediately toggle gl_fsmax
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::setDefaults()
{
  // FIXME - for now, this option isn't available
  myDriverPopup->setSelectedTag(0);
  myDriverPopup->setEnabled(false);

  myRendererPopup->setSelectedTag(1);
  myFilterPopup->setSelectedTag(1);
  myPalettePopup->setSelectedTag(1);
  myFrameRateSlider->setValue(60);
  myFrameRateLabel->setLabel("60");

  // For some unknown reason (ie, a bug), slider widgets can only
  // take certain ranges of numbers.  So we have to fudge things ...
  myZoomSlider->setValue(10);
  myZoomLabel->setLabel("2");
  myAspectRatioSlider->setValue(100);
  myAspectRatioLabel->setLabel("2.0");

  myFullscreenCheckbox->setState(false);
  myUseDeskResCheckbox->setState(true);

  // Make sure that mutually-exclusive items are not enabled at the same time
  handleRendererChange(0);  // 0 indicates software mode

  instance()->frameBuffer().refresh();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::handleRendererChange(uInt32 item)
{
  // When we're in software mode, certain OpenGL-related options are disabled
  bool active = item == 0 ? false : true;

  myFilterPopup->setEnabled(active);
  myAspectRatioSlider->setEnabled(active);
  myAspectRatioLabel->setEnabled(active);
  myUseDeskResCheckbox->setEnabled(active);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void VideoDialog::handleCommand(CommandSender* sender, uInt32 cmd, uInt32 data)
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

    case kAspectRatioChanged:
    {
      // This is terribly dirty, but what can we do?
      float ratio = (((myAspectRatioSlider->getValue() + 9) / 10) / 10.0) + 1.0;
      ostringstream r;
      if(ratio == 2.0)
        r << ratio << ".0";
      else
        r << ratio;
      myAspectRatioLabel->setLabel(r.str());
      break;
    }

    case kFrameRateChanged:
      myFrameRateLabel->setValue(myFrameRateSlider->getValue());
      break;

    case kZoomChanged:
      myZoomLabel->setValue((myZoomSlider->getValue() + 10) / 10);
      break;

    default:
      Dialog::handleCommand(sender, cmd, data);
      break;
  }
}
