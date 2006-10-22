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
// $Id: hq2x.cxx,v 1.1 2006-10-22 18:58:45 stephena Exp $
//
//   Based on code from ScummVM - Scumm Interpreter
//   Copyright (C) 2001-2006 The ScummVM project
//============================================================================

#include "intern.hxx"

#ifdef USE_NASM
// Assembly version of HQ2x

extern "C" {

#if !defined(_WIN32) && !defined(MACOSX)
#define hq2x_16 _hq2x_16
#endif

void hq2x_16(const byte *, byte *, uint32, uint32, uint32, uint32);

}

void HQ2x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch, int width, int height) {
	hq2x_16(srcPtr, dstPtr, width, height, srcPitch, dstPitch);
}

#else

#define PIXEL00_0	*(q) = w5;
#define PIXEL00_10	*(q) = interpolate16_2<bitFormat,3,1>(w5, w1);
#define PIXEL00_11	*(q) = interpolate16_2<bitFormat,3,1>(w5, w4);
#define PIXEL00_12	*(q) = interpolate16_2<bitFormat,3,1>(w5, w2);
#define PIXEL00_20	*(q) = interpolate16_3<bitFormat,2,1,1>(w5, w4, w2);
#define PIXEL00_21	*(q) = interpolate16_3<bitFormat,2,1,1>(w5, w1, w2);
#define PIXEL00_22	*(q) = interpolate16_3<bitFormat,2,1,1>(w5, w1, w4);
#define PIXEL00_60	*(q) = interpolate16_3<bitFormat,5,2,1>(w5, w2, w4);
#define PIXEL00_61	*(q) = interpolate16_3<bitFormat,5,2,1>(w5, w4, w2);
#define PIXEL00_70	*(q) = interpolate16_3<bitFormat,6,1,1>(w5, w4, w2);
#define PIXEL00_90	*(q) = interpolate16_3<bitFormat,2,3,3>(w5, w4, w2);
#define PIXEL00_100	*(q) = interpolate16_3<bitFormat,14,1,1>(w5, w4, w2);

#define PIXEL01_0	*(q+1) = w5;
#define PIXEL01_10	*(q+1) = interpolate16_2<bitFormat,3,1>(w5, w3);
#define PIXEL01_11	*(q+1) = interpolate16_2<bitFormat,3,1>(w5, w2);
#define PIXEL01_12	*(q+1) = interpolate16_2<bitFormat,3,1>(w5, w6);
#define PIXEL01_20	*(q+1) = interpolate16_3<bitFormat,2,1,1>(w5, w2, w6);
#define PIXEL01_21	*(q+1) = interpolate16_3<bitFormat,2,1,1>(w5, w3, w6);
#define PIXEL01_22	*(q+1) = interpolate16_3<bitFormat,2,1,1>(w5, w3, w2);
#define PIXEL01_60	*(q+1) = interpolate16_3<bitFormat,5,2,1>(w5, w6, w2);
#define PIXEL01_61	*(q+1) = interpolate16_3<bitFormat,5,2,1>(w5, w2, w6);
#define PIXEL01_70	*(q+1) = interpolate16_3<bitFormat,6,1,1>(w5, w2, w6);
#define PIXEL01_90	*(q+1) = interpolate16_3<bitFormat,2,3,3>(w5, w2, w6);
#define PIXEL01_100	*(q+1) = interpolate16_3<bitFormat,14,1,1>(w5, w2, w6);

#define PIXEL10_0	*(q+nextlineDst) = w5;
#define PIXEL10_10	*(q+nextlineDst) = interpolate16_2<bitFormat,3,1>(w5, w7);
#define PIXEL10_11	*(q+nextlineDst) = interpolate16_2<bitFormat,3,1>(w5, w8);
#define PIXEL10_12	*(q+nextlineDst) = interpolate16_2<bitFormat,3,1>(w5, w4);
#define PIXEL10_20	*(q+nextlineDst) = interpolate16_3<bitFormat,2,1,1>(w5, w8, w4);
#define PIXEL10_21	*(q+nextlineDst) = interpolate16_3<bitFormat,2,1,1>(w5, w7, w4);
#define PIXEL10_22	*(q+nextlineDst) = interpolate16_3<bitFormat,2,1,1>(w5, w7, w8);
#define PIXEL10_60	*(q+nextlineDst) = interpolate16_3<bitFormat,5,2,1>(w5, w4, w8);
#define PIXEL10_61	*(q+nextlineDst) = interpolate16_3<bitFormat,5,2,1>(w5, w8, w4);
#define PIXEL10_70	*(q+nextlineDst) = interpolate16_3<bitFormat,6,1,1>(w5, w8, w4);
#define PIXEL10_90	*(q+nextlineDst) = interpolate16_3<bitFormat,2,3,3>(w5, w8, w4);
#define PIXEL10_100	*(q+nextlineDst) = interpolate16_3<bitFormat,14,1,1>(w5, w8, w4);

#define PIXEL11_0	*(q+1+nextlineDst) = w5;
#define PIXEL11_10	*(q+1+nextlineDst) = interpolate16_2<bitFormat,3,1>(w5, w9);
#define PIXEL11_11	*(q+1+nextlineDst) = interpolate16_2<bitFormat,3,1>(w5, w6);
#define PIXEL11_12	*(q+1+nextlineDst) = interpolate16_2<bitFormat,3,1>(w5, w8);
#define PIXEL11_20	*(q+1+nextlineDst) = interpolate16_3<bitFormat,2,1,1>(w5, w6, w8);
#define PIXEL11_21	*(q+1+nextlineDst) = interpolate16_3<bitFormat,2,1,1>(w5, w9, w8);
#define PIXEL11_22	*(q+1+nextlineDst) = interpolate16_3<bitFormat,2,1,1>(w5, w9, w6);
#define PIXEL11_60	*(q+1+nextlineDst) = interpolate16_3<bitFormat,5,2,1>(w5, w8, w6);
#define PIXEL11_61	*(q+1+nextlineDst) = interpolate16_3<bitFormat,5,2,1>(w5, w6, w8);
#define PIXEL11_70	*(q+1+nextlineDst) = interpolate16_3<bitFormat,6,1,1>(w5, w6, w8);
#define PIXEL11_90	*(q+1+nextlineDst) = interpolate16_3<bitFormat,2,3,3>(w5, w6, w8);
#define PIXEL11_100	*(q+1+nextlineDst) = interpolate16_3<bitFormat,14,1,1>(w5, w6, w8);

#define YUV(x)	RGBtoYUV[w ## x]


#define bitFormat 565
void HQ2x(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch, int width, int height) {
	#include "hq2x.hxx"
}
#undef bitFormat

#endif //Assembly version
