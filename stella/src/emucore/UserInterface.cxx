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
// Copyright (c) 1995-1998 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: UserInterface.cxx,v 1.3 2003-09-26 17:35:05 stephena Exp $
//============================================================================

#include "bspf.hxx"
#include "Console.hxx"
#include "Event.hxx"
#include "Settings.hxx"
#include "MediaSrc.hxx"
#include "UserInterface.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
UserInterface::UserInterface(Console* console, MediaSource* mediasrc)
    : myConsole(console),
      myMediaSource(mediasrc),
      myCurrentWidget(NONE),
      myRemapEventSelectedFlag(false),
      mySelectedEvent(Event::NoType),
      myMessageTime(0),
      myMessageText("")
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
UserInterface::~UserInterface(void)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UserInterface::showMainMenu(bool show)
{
  myCurrentWidget = show ? MAIN_MENU : NONE;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UserInterface::sendKeyEvent(StellaEvent::KeyCode key, Int32 state)
{
  if(myCurrentWidget == NONE || state != 1)
    return;
  else if(myCurrentWidget == MAIN_MENU)
  {
    if(key == StellaEvent::KCODE_RETURN)
      myCurrentWidget = currentSelectedWidget();
    else if(key == StellaEvent::KCODE_UP)
      moveCursorUp();
    else if(key == StellaEvent::KCODE_DOWN)
      moveCursorDown();
  }
  else if(myCurrentWidget == REMAP_MENU)
  {
    if(myRemapEventSelectedFlag)
    {
      if(key == StellaEvent::KCODE_ESCAPE)
        // associate nothing with the selected event
cerr << "delete binding for " << mySelectedEvent << endl;
      else
        // associate this stellaevent with the selected event
cerr << "add binding " << key << " for " << mySelectedEvent << endl;

      myRemapEventSelectedFlag = false;
    }
    else if(key == StellaEvent::KCODE_RETURN)
    {
cerr << "event selected for remapping\n";
      mySelectedEvent = currentSelectedEvent();
      myRemapEventSelectedFlag = true;
    }
    else if(key == StellaEvent::KCODE_UP)
      moveCursorUp();
    else if(key == StellaEvent::KCODE_DOWN)
      moveCursorDown();
    else if(key == StellaEvent::KCODE_ESCAPE)
      myCurrentWidget = MAIN_MENU;
  }
  else if(myCurrentWidget == INFO_MENU)
  {
cerr << "key received while in info menu\n";
    if(key == StellaEvent::KCODE_ESCAPE)
      myCurrentWidget = MAIN_MENU;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UserInterface::sendJoyEvent(StellaEvent::JoyStick stick,
     StellaEvent::JoyCode code, Int32 state)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UserInterface::update()
{
  uInt8* frontbuffer = myMediaSource->currentFrameBuffer();

  if(myCurrentWidget == NONE)
  {
    return;  // this shouldn't happen
  }
  else if(myCurrentWidget == MAIN_MENU)
  {
    ; // draw main menu
  }
  else if(myCurrentWidget == REMAP_MENU)
  {
    ; // draw remap menu
  }
  else if(myCurrentWidget == INFO_MENU)
  {

// FIXME - this will disappear soon ...
  // First, draw the surrounding box
  for(uInt32 x = 0; x < 100; ++x)
  {
    for(uInt32 y = 0; y < 100; ++y)
    {
      uInt32 position = ((20 + y) * myMediaSource->width()) + x + 20;

      if((x == 0) || (x == 200 - 1) || (y == 0) || (y == 200 - 1))
        frontbuffer[position] = 10;
      else
        frontbuffer[position] = 0;
    }
  }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
UserInterface::Widget UserInterface::currentSelectedWidget()
{
  return REMAP_MENU; // FIXME
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event::Type UserInterface::currentSelectedEvent()
{
  return Event::ConsoleSelect; // FIXME
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UserInterface::moveCursorUp()
{
cerr << "cursor up\n";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UserInterface::moveCursorDown()
{
cerr << "cursor down\n";
}


/*
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void UserInterface::drawMessageText(string& message)
{
  // First, get access to the framebuffer
  uInt8* buffer = myMediaSource->currentFrameBuffer();

  // Make message uppercase, since there are no lowercase fonts defined
  uInt8 length = message.length();
  for(uInt32 i = 0; i < length; ++i)
    message[i] = toupper(message[i]);

  // Set up the correct coordinates to draw the surrounding box
  uInt32 xBoxOffSet = 2 + myXStart;
  uInt32 yBoxOffSet = myHeight - 18;
  uInt32 boxToTextXOffSet = 2;
  uInt32 boxToTextYOffSet = 4;

  // Set up the correct coordinates to print the message
  uInt32 xTextOffSet = xBoxOffSet + boxToTextXOffSet;
  uInt32 yTextOffSet = yBoxOffSet + boxToTextYOffSet;

  // Used to indicate the current x/y position of a pixel
  uInt32 xPos, yPos;

  // The actual font data for a letter
  uInt32 data;

  // The index into the palette to color the current text and background
  uInt8 fontColor, backColor;

  // These will probably change in the future ...
  fontColor = 10;
  backColor = 0;

  // Clip the length if its wider than the screen
//  uInt8 length = message.length();
  if(((length * 5) + xTextOffSet) >= myWidth)
    length = (myWidth - xTextOffSet) / 5;

  // Reset the offsets to center the message
  uInt32 boxWidth  = (5 * length) + boxToTextXOffSet;
  uInt32 boxHeight = 8 + (2 * (yTextOffSet - yBoxOffSet));
  xBoxOffSet = (myWidth >> 1) - (boxWidth >> 1);
  xTextOffSet = xBoxOffSet + boxToTextXOffSet;

  // First, draw the surrounding box
  for(uInt32 x = 0; x < boxWidth; ++x)
  {
    for(uInt32 y = 0; y < boxHeight; ++y)
    {
      uInt32 position = ((yBoxOffSet + y) * myWidth) + x + xBoxOffSet;

      if((x == 0) || (x == boxWidth - 1) || (y == 0) || (y == boxHeight - 1))
        buffer[position] = fontColor;
      else
        buffer[position] = backColor;
    }
  }

  // Then, draw the text
  for(uInt8 x = 0; x < length; ++x)
  {
    char letter = message[x];

    if((letter >= 'A') && (letter <= 'Z'))
      data = ourFontData[(int)letter - 65];
    else if((letter >= '0') && (letter <= '9'))
      data = ourFontData[(int)letter - 48 + 26];
    else   // unknown character or space
    {
      xTextOffSet += 3;
      continue;
    }

    // start scanning the font data from the bottom up
    yPos = 7;

    for(uInt8 y = 0; y < 32; ++y)
    {
      // determine the correct scanline
      xPos = y % 4;
      if(xPos == 0)
        --yPos;

      if((data >> y) & 1)
      {
        uInt32 position = (yPos + yTextOffSet) * myWidth + (4 - xPos) + xTextOffSet;
        buffer[position] = fontColor;
      }
    }

    // move left to the next character
    xTextOffSet += 5;
  }
}
*/

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const uInt32 UserInterface::ourFontData[36] = { 
  0x699f999, // A
  0xe99e99e, // B
  0x6988896, // C
  0xe99999e, // D
  0xf88e88f, // E
  0xf88e888, // F
  0x698b996, // G
  0x999f999, // H
  0x7222227, // I
  0x72222a4, // J
  0x9accaa9, // K
  0x888888f, // L
  0x9ff9999, // M
  0x9ddbb99, // N
  0x6999996, // O
  0xe99e888, // P
  0x69999b7, // Q
  0xe99ea99, // R
  0x6986196, // S
  0x7222222, // T
  0x9999996, // U
  0x9999966, // V
  0x9999ff9, // W
  0x99fff99, // X
  0x9996244, // Y
  0xf12488f, // Z
  0x69bd996, // 0
  0x2622227, // 1
  0x691248f, // 2
  0x6916196, // 3
  0xaaaf222, // 4
  0xf88e11e, // 5
  0x698e996, // 6
  0xf112244, // 7
  0x6996996, // 8
  0x6997196  // 9
};

/*  uInt8* frontbuffer = myMediaSource->currentFrameBuffer();
  uInt8* backbuffer  = myMediaSource->previousFrameBuffer();

  // First, draw the surrounding box
  for(uInt32 x = 0; x < 100; ++x)
  {
    for(uInt32 y = 0; y < 100; ++y)
    {
      uInt32 position = ((20 + y) * myWidth) + x + 20;

      if((x == 0) || (x == 200 - 1) || (y == 0) || (y == 200 - 1))
        frontbuffer[position] = backbuffer[position] = 10;
      else
        frontbuffer[position] = backbuffer[position] = 0;
    }
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TIA::showMessage(string& message, Int32 duration)
{
  myMessageText = message;
  myMessageTime = duration;

  // Make message uppercase, since there are no lowercase fonts defined
  uInt32 length = myMessageText.length();
  for(uInt32 i = 0; i < length; ++i)
    myMessageText[i] = toupper(myMessageText[i]);
}
*/
