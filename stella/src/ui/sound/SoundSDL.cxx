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
// Copyright (c) 1995-2002 by Bradford W. Mott
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: SoundSDL.cxx,v 1.3 2003-11-06 22:22:33 stephena Exp $
//============================================================================

#include <SDL.h>

#include "SoundSDL.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::SoundSDL(bool activate)
    : myCurrentVolume(SDL_MIX_MAXVOLUME),
      myFragmentSize(1024),
      myIsInitializedFlag(false),
      myIsMuted(false),
      mySampleRate(31400),
      mySampleQueue(mySampleRate),
      myPauseStatus(false)
{
  if(activate)
  {
    if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
    {
      cerr << "WARNING: Couldn't initialize SDL audio system! " << endl;
      cerr << "         " << SDL_GetError() << endl;
      myIsInitializedFlag = false;
      mySampleRate = 0;
      return;
    }

    SDL_AudioSpec desired;
    desired.freq = mySampleRate;
    desired.format = AUDIO_U8;
    desired.channels = 1;
    desired.samples = myFragmentSize;
    desired.callback = callback;
    desired.userdata = (void*)this;

    if(SDL_OpenAudio(&desired, &myHardwareSpec) < 0)
    {
      cerr << "WARNING: Couldn't open SDL audio system! " << endl;
      cerr << "         " << SDL_GetError() << endl;
      myIsInitializedFlag = false;
      mySampleRate = 0;
      return;
    }

    // Make sure the sample buffer isn't to big (if it is the sound code
    // will not work so we'll need to disable the audio support)
    if(((float)myHardwareSpec.size / (float)myHardwareSpec.freq) >= 0.25)
    {
      cerr << "WARNING: Audio device doesn't support real time audio! Make ";
      cerr << "sure a sound" << endl;
      cerr << "         server isn't running.  Audio is disabled..." << endl;

      SDL_CloseAudio();

      myIsInitializedFlag = false;
      mySampleRate = 0;
      return;
    }

    myIsInitializedFlag = true;
    myIsMuted = false;
    mySampleRate = myHardwareSpec.freq;
    myFragmentSize = myHardwareSpec.samples;

//    cerr << "Freq: " << (int)mySampleRate << endl;
//    cerr << "Format: " << (int)myHardwareSpec.format << endl;
//    cerr << "Channels: " << (int)myHardwareSpec.channels << endl;
//    cerr << "Silence: " << (int)myHardwareSpec.silence << endl;
//    cerr << "Samples: " << (int)myHardwareSpec.samples << endl;
//    cerr << "Size: " << (int)myHardwareSpec.size << endl;

    SDL_PauseAudio(0);
  }
  else
  {
    myIsInitializedFlag = false;
    myIsMuted = true;
    mySampleRate = 0;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::~SoundSDL()
{
  closeDevice();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 SoundSDL::getSampleRate() const
{
  return myIsInitializedFlag ? mySampleRate : 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundSDL::isSuccessfullyInitialized() const
{
  return myIsInitializedFlag;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::mute(bool state)
{
  if(!myIsInitializedFlag)
  {
    return;
  }

  // Ignore multiple calls to do the same thing
  if(myIsMuted == state)
  {
    return;
  }

  myIsMuted = state;

  SDL_PauseAudio(myIsMuted ? 1 : 0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::closeDevice()
{
  if(myIsInitializedFlag)
  {
    SDL_CloseAudio();
  }

  myIsInitializedFlag = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::setSoundVolume(Int32 percent)
{
  if(myIsInitializedFlag)
  {
    if((percent >= 0) && (percent <= 100))
    {
      SDL_LockAudio();
      myCurrentVolume = (uInt32)(((float)percent / 100.0) * SDL_MIX_MAXVOLUME);
      SDL_UnlockAudio();
    }
    else if(percent == -1)   // If -1 has been specified, play sound at default volume
    {
      myCurrentVolume = SDL_MIX_MAXVOLUME;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::updateSound(MediaSource& mediaSource)
{
  if(myIsInitializedFlag)
  {
    if(myPauseStatus)
      return;

    // Make sure we have exclusive access to the sample queue
    SDL_LockAudio();

    // Move all of the generated samples into the our private sample queue
    uInt8 buffer[4096];
    while(mediaSource.numberOfAudioSamples() > 0)
    {
      uInt32 size = mediaSource.dequeueAudioSamples(buffer, 4096);
      mySampleQueue.enqueue(buffer, size);
    }

    // Release lock on the sample queue
    SDL_UnlockAudio();

    // Block until the sound thread has consumed all but 142 milliseconds
    // of the available audio samples
    uInt32 leave = mySampleRate / 7;
    for(;;)
    {
      uInt32 size = 0;
  
      SDL_LockAudio();
      size = mySampleQueue.size();
      SDL_UnlockAudio();

      if(size <= leave)
      {
        break;
      }
 
      SDL_Delay(5);
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::callback(void* udata, uInt8* stream, int len)
{
  SoundSDL* sound = (SoundSDL*)udata;

  if(!sound->isSuccessfullyInitialized())
  {
    return;
  }

  // Don't use samples unless there's at least 76 milliseconds worth of data
  if(sound->mySampleQueue.size() < (sound->mySampleRate / 13))
  {
    return;
  }

//cerr << "len: " << len << " Q.size: " << sound->mySampleQueue.size() << endl;

  if(sound->mySampleQueue.size() > 0)
  {
    Int32 offset;
    uInt8 buffer[4096];
    for(offset = 0; (offset < len) && (sound->mySampleQueue.size() > 0); )
    {
      uInt32 s = sound->mySampleQueue.dequeue(buffer, 
          (4096 > (len - offset) ? (len - offset) : 4096));
      SDL_MixAudio(stream + offset, buffer, s, sound->myCurrentVolume);
      offset += s;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::SampleQueue::SampleQueue(uInt32 capacity)
    : myCapacity(capacity),
      myBuffer(0),
      mySize(0),
      myHead(0),
      myTail(0)
{
  myBuffer = new uInt8[myCapacity];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundSDL::SampleQueue::~SampleQueue()
{
  delete[] myBuffer;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::SampleQueue::clear()
{
  myHead = myTail = mySize = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 SoundSDL::SampleQueue::dequeue(uInt8* buffer, uInt32 size)
{
  // We can only dequeue up to the number of items in the queue
  if(size > mySize)
  {
    size = mySize;
  }

  if((myHead + size) < myCapacity)
  {
    memcpy((void*)buffer, (const void*)(myBuffer + myHead), size);
    myHead += size;
  }
  else
  {
    uInt32 s1 = myCapacity - myHead;
    uInt32 s2 = size - s1;
    memcpy((void*)buffer, (const void*)(myBuffer + myHead), s1);
    memcpy((void*)(buffer + s1), (const void*)myBuffer, s2);
    myHead = (myHead + size) % myCapacity;
  }

  mySize -= size;

  return size;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundSDL::SampleQueue::enqueue(uInt8* buffer, uInt32 size)
{
  // If an attempt is made to enqueue more than the queue can hold then
  // we'll only enqueue the last myCapacity elements.
  if(size > myCapacity)
  {
    buffer += (size - myCapacity);
    size = myCapacity;
  }

  if((myTail + size) < myCapacity)
  {
    memcpy((void*)(myBuffer + myTail), (const void*)buffer, size);
    myTail += size;
  }
  else
  {
    uInt32 s1 = myCapacity - myTail;
    uInt32 s2 = size - s1;
    memcpy((void*)(myBuffer + myTail), (const void*)buffer, s1);
    memcpy((void*)myBuffer, (const void*)(buffer + s1), s2);
    myTail = (myTail + size) % myCapacity;
  }

  if((mySize + size) > myCapacity)
  {
    myHead = (myHead + ((mySize + size) - myCapacity)) % myCapacity;
    mySize = myCapacity;
  }
  else
  {
    mySize += size;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 SoundSDL::SampleQueue::size() const
{
  return mySize;
}

