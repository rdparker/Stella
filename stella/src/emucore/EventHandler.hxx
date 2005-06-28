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
// $Id: EventHandler.hxx,v 1.41 2005-06-28 03:34:41 urchlay Exp $
//============================================================================

#ifndef EVENTHANDLER_HXX
#define EVENTHANDLER_HXX

#include <SDL.h>

#include "bspf.hxx"
#include "Event.hxx"

class Console;
class OSystem;

enum MouseButton {
  EVENT_LBUTTONDOWN,
  EVENT_LBUTTONUP,
  EVENT_RBUTTONDOWN,
  EVENT_RBUTTONUP,
  EVENT_WHEELDOWN,
  EVENT_WHEELUP
};

// Structure used for action menu items
struct ActionList {
  Event::Type event;
  string action;
  string key;
};

// Joystick related items
enum {
  kNumJoysticks  = 8,
  kNumJoyButtons = 24,
  kJAxisUp       = kNumJoyButtons - 4,  // Upper 4 buttons are actually
  kJAxisDown     = kNumJoyButtons - 3,  // directions
  kJAxisLeft     = kNumJoyButtons - 2,
  kJAxisRight    = kNumJoyButtons - 1
};

enum JoyType { JT_NONE, JT_REGULAR, JT_STELLADAPTOR_1, JT_STELLADAPTOR_2 };

struct Stella_Joystick {
  SDL_Joystick* stick;
  JoyType       type;
};


/**
  This class takes care of event remapping and dispatching for the
  Stella core, as well as keeping track of the current 'mode'.

  The frontend will send translated events here, and the handler will
  check to see what the current 'mode' is.

  If in emulation mode, events received from the frontend are remapped and
  sent to the emulation core.  If in menu mode, the events are sent
  unchanged to the menu class, where (among other things) changing key
  mapping can take place.

  @author  Stephen Anthony
  @version $Id: EventHandler.hxx,v 1.41 2005-06-28 03:34:41 urchlay Exp $
*/
class EventHandler
{
  public:
    /**
      Create a new event handler object
    */
    EventHandler(OSystem* osystem);
 
    /**
      Destructor
    */
    virtual ~EventHandler();

    // Enumeration representing the different states of operation
    enum State { S_NONE, S_EMULATE, S_LAUNCHER, S_MENU, S_DEBUGGER };

    /**
      Returns the event object associated with this handler class.

      @return The event object
    */
    Event* event();

    /**
      Set up any joysticks on the system.  This must be called *after* the
      framebuffer has been created, since SDL requires the video to be
      intialized before joysticks can be probed.
    */
    void setupJoysticks();

    /**
      Collects and dispatches any pending events.  This method should be
      called regularly (at X times per second, where X is the game framerate).

      @param time  The current time in milliseconds.
    */
    void poll(uInt32 time);

    /**
      Bind a key to an event/action

      @param event  The event we are remapping
      @param key    The key to bind to this event
    */
    void addKeyMapping(Event::Type event, uInt16 key);

    /**
      Bind a joystick button/direction to an event/action

      @param event  The event we are remapping
      @param stick  The joystick number and button
      @param code     to bind to this event
    */
    void addJoyMapping(Event::Type event, uInt8 stick, uInt32 code);

    /**
      Erase the specified mapping

      @event  The event for which we erase all mappings
    */
    void eraseMapping(Event::Type event);

    /**
      Resets the event mappings to default values
    */
    void setDefaultMapping();

    /**
      Returns the current state of the EventHandler

      @return The State type
    */
    inline State state() { return myState; }

    /**
      Resets the state machine of the EventHandler to the defaults

      @param state  The current state to set
    */
    void reset(State state);

    /**
      This method indicates whether a pause event has been received.
    */
    inline bool doPause() { return myPauseFlag; }

    /**
      This method indicates whether a quit event has been received.
    */
    inline bool doQuit() { return myQuitFlag; }

    /**
      This method indicates that the system should terminate.
    */
    inline void quit() { handleEvent(Event::Quit, 1); }

    /**
      Save state to explicit state number (debugger uses this)
    */
	 void saveState(int state);

    /**
      Load state from explicit state number (debugger uses this)
    */
	 void loadState(int state);

    /**
      Sets the mouse to act as paddle 'num'

      @param num          The paddle which the mouse should emulate
      @param showmessage  Print a message to the framebuffer
    */
    void setPaddleMode(uInt32 num, bool showmessage = false);

    inline bool kbdAlt(int mod)
    {
  #ifndef MAC_OSX
      return (mod & KMOD_ALT);
  #else
      return ((mod & KMOD_META) && (mod & KMOD_SHIFT));
  #endif
    }

    inline bool kbdControl(int mod)
    {
  #ifndef MAC_OSX
      return (mod & KMOD_CTRL) > 0;
  #else
      return ((mod & KMOD_META) && !(mod & KMOD_SHIFT));
  #endif
    }

    inline bool kbdShift(int mod)
    {
      return (mod & KMOD_SHIFT);
    }

    void enterMenuMode();
    void leaveMenuMode();
    void enterDebugMode();
    void leaveDebugMode();

    // Holds static strings for the remap menu
    static ActionList ourActionList[61];

    // Lookup table for paddle resistance events
    static const Event::Type Paddle_Resistance[4];

    // Lookup table for paddle button events
    static const Event::Type Paddle_Button[4];

    // Static lookup tables for Stelladaptor axis support
    static const Event::Type SA_Axis[2][2][3];
    static const Event::Type SA_DrivingValue[2];

  private:
    /**
      Send an event directly to the event handler.
      These events cannot be remapped.

      @param type  The event
      @param value The value for the event
    */
    void handleEvent(Event::Type type, Int32 value);

    /**
      Send a keyboard event to the handler.

      @param key   keysym
      @param mod   modifiers
      @param state state of key
    */
    void handleKeyEvent(int unicode, SDLKey key, SDLMod mod, uInt8 state);

    /**
      Send a mouse motion event to the handler.

      @param event The mouse motion event generated by SDL
    */
    void handleMouseMotionEvent(SDL_Event& event);

    /**
      Send a mouse button event to the handler.

      @param event The mouse button event generated by SDL
    */
    void handleMouseButtonEvent(SDL_Event& event, uInt8 state);

    /**
      Send a joystick event to the handler (directions are encoded as buttons)

      @param stick  SDL joystick
      @param code   Event code
      @param state  state of code (pressed/released)
    */
    void handleJoyEvent(uInt8 stick, uInt32 code, uInt8 state);

    /**
      The following methods take care of assigning action mappings.
    */
    void setActionMappings();
    void setSDLMappings();
    void setKeymap();
    void setJoymap();
    void setDefaultKeymap();
    void setDefaultJoymap();
    void saveKeyMapping();
    void saveJoyMapping();

    bool isValidList(string list, uInt32 length);

    void saveState();
    void changeState();
    void loadState();
    void takeSnapshot();

  private:
    // Global OSystem object
    OSystem* myOSystem;

    // Array of key events, indexed by SDLKey
    Event::Type myKeyTable[SDLK_LAST];

    // Array of joystick events
    Event::Type myJoyTable[kNumJoysticks * kNumJoyButtons];

    // Array of messages for each Event
    string ourMessageTable[Event::LastType];

    // Array of strings which correspond to the given SDL key
    string ourSDLMapping[SDLK_LAST];

    // Array of joysticks available to Stella
    Stella_Joystick ourJoysticks[kNumJoysticks];

    // Indicates the current state of the system (ie, which mode is current)
    State myState;

    // Global Event object
    Event* myEvent;

    // Indicates the current state to use for state loading/saving
    uInt32 myLSState;

    // Indicates the current pause status
    bool myPauseFlag;

    // Indicates whether to quit the emulator
    bool myQuitFlag;

    // Indicates whether the mouse cursor is grabbed
    bool myGrabMouseFlag;

    // Indicates whether to use launcher mode when exiting a game
    bool myUseLauncherFlag;

    // Indicates which paddle the mouse currently emulates
    Int8 myPaddleMode;

    // The current keymap in string form
    string myKeymapString;

    // The current joymap in string form
    string myJoymapString;
};

#endif
