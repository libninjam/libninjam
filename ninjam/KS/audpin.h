//**@@@*@@@****************************************************
//
// Microsoft Windows
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//**@@@*@@@****************************************************

//
// FileName:    audpin.h
//
// Abstract:    
//      This is the header file for C++ classes that expose
//      functionality of KS pins that support PCM audio formats
//

#pragma once
#ifndef __KSAUDPIN_H
#define __KSAUDPIN_H

#include <ksmedia.h>

#ifndef _MSC_VER
typedef struct {
    KSDATAFORMAT    DataFormat;
    WAVEFORMATEX    WaveFormatEx;
} KSDATAFORMAT_WAVEFORMATEX, *PKSDATAFORMAT_WAVEFORMATEX;
#endif

#ifndef _WAVEFORMATEXTENSIBLE_
#define _WAVEFORMATEXTENSIBLE_
typedef struct {
    WAVEFORMATEX    Format;
    union {
        WORD wValidBitsPerSample;       /* bits of precision  */
        WORD wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
        WORD wReserved;                 /* If neither applies, set to zero. */
    } Samples;
    DWORD           dwChannelMask;      /* which channels are */
                                        /* present in stream  */
    GUID            SubFormat;
} WAVEFORMATEXTENSIBLE, *PWAVEFORMATEXTENSIBLE;
#endif // !_WAVEFORMATEXTENSIBLE_

#if !defined(WAVE_FORMAT_EXTENSIBLE)
#define  WAVE_FORMAT_EXTENSIBLE                 0xFFFE
#endif // !defined(WAVE_FORMAT_EXTENSIBLE)

// Forward Declarations
class CKsNode;
class CKsAudFilter;

////////////////////////////////////////////////////////////////////////////////
//
//  class CKsAudPin
//
//  Class Description:
//      This is the base class for an Audio pin.
//
//
//

class CKsAudPin : public CKsPin
{
public: 
    // Data Members
    TList<KSDATARANGE_AUDIO> m_listDataRange;

public:
    // constructor
    CKsAudPin(CKsAudFilter* pFilter, ULONG nId, HRESULT *phr);

    // destructor
    virtual ~CKsAudPin(void);

    // Various other methods
    HRESULT SetFormat(const WAVEFORMATEX* pwfx);
    const WAVEFORMATEX* GetFormat(void) {return m_pWaveFormatEx;}

    HRESULT GetPosition(KSAUDIO_POSITION* Pos);
    HRESULT SetPosition(KSAUDIO_POSITION* Pos);

    BOOL    IsFormatSupported(const WAVEFORMATEX* pwfx);

protected:
    CKsAudFilter*   m_pAudFilter;

private:
    HRESULT         Init(void);
    CKsNode*        GetNode(ULONG ulNodeID);

    TList<CKsNode>              m_listNodes;

    WAVEFORMATEX*               m_pWaveFormatEx;
    KSDATAFORMAT_WAVEFORMATEX*  m_pksDataFormatWfx; // Just a reference into m_pksPinCreate
};


////////////////////////////////////////////////////////////////////////////////
//
//  class CKsAudRenPin
//
//  Class Description:
//      This is the base class for a Audio Render pin.
//
//
//

class CKsAudRenPin : public CKsAudPin
{
public:
    // constructor
    CKsAudRenPin(CKsAudFilter* pFilter, ULONG nId, HRESULT *phr);

    // destructor
    virtual ~CKsAudRenPin(void);

};


////////////////////////////////////////////////////////////////////////////////
//
//  class CKsAudCapPin
//
//  Class Description:
//      This is the base class for an Audio Capture pin.
//
//
//

class CKsAudCapPin : public CKsAudPin
{
public:
    // constructor
    CKsAudCapPin(CKsAudFilter* pFilter, ULONG nId, HRESULT *phr);

    // destructor
    virtual ~CKsAudCapPin(void);
};

#endif //__KSAUDPIN_H
