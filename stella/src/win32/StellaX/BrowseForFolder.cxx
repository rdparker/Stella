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
// Copyright (c) 1998 Scott D. Killen
// Copyright (c) 2004 by Stephen Anthony
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: BrowseForFolder.cxx,v 1.3 2004-07-06 22:51:58 stephena Exp $
//============================================================================ 

#include "pch.hxx"
#include "BrowseForFolder.hxx"


CBrowseForFolder::CBrowseForFolder(
    const HWND hParent, 
    const LPITEMIDLIST pidl, 
    LPCTSTR strTitle)
{
  myHwnd = NULL;

  myBrowseInfo.pidlRoot = pidl;
  myBrowseInfo.hwndOwner = NULL;
  myBrowseInfo.pszDisplayName = mySelected;
  myBrowseInfo.lpszTitle = strTitle;
  myBrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS;
  myBrowseInfo.lParam = reinterpret_cast<LPARAM>( this );
  myBrowseInfo.lpfn = BrowseCallbackProc;
}

CBrowseForFolder::~CBrowseForFolder()
{
}

LPCTSTR CBrowseForFolder::GetSelectedFolder() const
{
  return mySelected;
}

bool CBrowseForFolder::SelectFolder()
{
  bool bRet = false;

  LPITEMIDLIST pidl;
  if ((pidl = SHBrowseForFolder(&myBrowseInfo)) != NULL)
  {
    myPath.Set( _T("") );
    if (SUCCEEDED(SHGetPathFromIDList(pidl, mySelected)))
    {
      bRet = true;
      myPath.Set( mySelected );
    }

    LPMALLOC pMalloc;
    //Retrieve a pointer to the shell's IMalloc interface
    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
    {
      // free the PIDL that SHBrowseForFolder returned to us.
      pMalloc->Free(pidl);
      // release the shell's IMalloc interface
      (void)pMalloc->Release();
    }
  }
  myHwnd = NULL;

  return bRet;
}

void CBrowseForFolder::OnSelChanged(const LPITEMIDLIST pidl) const
{
  (void)pidl;
}

void CBrowseForFolder::EnableOK(const bool bEnable) const
{
  if (myHwnd == NULL)
    return;

  // (void)SendMessage(myHwnd, BFFM_ENABLEOK, static_cast(bEnable), NULL);
  (void)SendMessage( myHwnd, BFFM_ENABLEOK, NULL, static_cast<LPARAM>(bEnable) );
}

void CBrowseForFolder::SetSelection(const LPITEMIDLIST pidl) const
{
  if (myHwnd == NULL)
    return;

  (void)SendMessage(myHwnd, BFFM_SETSELECTION, FALSE, reinterpret_cast<LPARAM>(pidl));
}

void CBrowseForFolder::SetSelection(LPCTSTR strPath) const
{
  if (myHwnd == NULL)
    return;

  (void)SendMessage(myHwnd, BFFM_SETSELECTION, TRUE, reinterpret_cast<LPARAM>(strPath));
}

int __stdcall CBrowseForFolder::BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
  CBrowseForFolder* pbff = reinterpret_cast<CBrowseForFolder*>( lpData );
  pbff->myHwnd = hwnd;

  if (uMsg == BFFM_SELCHANGED)
    pbff->OnSelChanged( reinterpret_cast<LPITEMIDLIST>( lParam ));
	
  return 0;
}
