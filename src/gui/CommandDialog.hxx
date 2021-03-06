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
// Copyright (c) 1995-2015 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id$
//============================================================================

#ifndef COMMAND_DIALOG_HXX
#define COMMAND_DIALOG_HXX

class Properties;
class CommandSender;
class DialogContainer;
class OSystem;

#include "Dialog.hxx"

class CommandDialog : public Dialog
{
  public:
    CommandDialog(OSystem& osystem, DialogContainer& parent);
    virtual ~CommandDialog();

  protected:
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;

    enum {
      kSelectCmd     = 'Csel',
      kResetCmd      = 'Cres',
      kColorCmd      = 'Ccol',
      kBWCmd         = 'Cbwt',
      kLeftDiffACmd  = 'Clda',
      kLeftDiffBCmd  = 'Cldb',
      kRightDiffACmd = 'Crda',
      kRightDiffBCmd = 'Crdb',
      kSaveStateCmd  = 'Csst',
      kStateSlotCmd  = 'Ccst',
      kLoadStateCmd  = 'Clst',
      kSnapshotCmd   = 'Csnp',
      kFormatCmd     = 'Cfmt',
      kPaletteCmd    = 'Cpal',
      kReloadRomCmd  = 'Crom',
      kExitCmd       = 'Clex'
    };

    enum {
      kNumRows = 4,
      kNumCols = 4
    };

  private:
    // Following constructors and assignment operators not supported
    CommandDialog() = delete;
    CommandDialog(const CommandDialog&) = delete;
    CommandDialog(CommandDialog&&) = delete;
    CommandDialog& operator=(const CommandDialog&) = delete;
    CommandDialog& operator=(CommandDialog&&) = delete;
};

#endif
