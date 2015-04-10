/*
    NINJAM - audiostream.h
    Copyright (C) 2005 Cockos Incorporated

    NINJAM is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    NINJAM is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NINJAM; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*

  This header is used by NINJAM clients to define an abstract audio streamer interface,
  as well as declare functions for creating instances of these audio streamers.

  On the legacy Win32 client, these functions are primarily called from audioconfig.cpp,
  and on the legacy Cocoa client the function is called from Controller.mm.
  Clients targeting other platforms would call these directly.

  The basic structure is:
    The client runs, creates an audioStreamer (below), giving it a SPLPROC,
    which is a client owned function that then in turn calls NJClient::AudioProc.

  But this is just the interface declaration etc.

*/

#ifndef _AUDIOSTREAM_H_
#define _AUDIOSTREAM_H_


/*\ the following switches remove support for specific audio APIs
|*|     #define NO_SUPPORT_ASIO
|*|     #define NO_SUPPORT_KS
|*|     #define NO_SUPPORT_DS
|*|     #define NO_SUPPORT_WAVE
|*|     #define NO_SUPPORT_JACK
|*|     #define NO_SUPPORT_ALSA
\*/


#include "constants.h"

#ifdef _WIN32
#  ifndef NO_SUPPORT_DS
#    include <dsound.h>
#    include <string>
#    include <vector>
struct DsDevice
{
  GUID guid              ;
  std::string deviceName ;
  std::string driverName ;
} ;
#  endif // NO_SUPPORT_DS
#endif // _WIN32


class   NJClient ;
typedef void (*SPLPROC)(float** input_buffer  , int n_input_channels  ,
                        float** output_buffer , int n_output_channels ,
                        int     n_samples     , int sample_rate       ) ;


class audioStreamer
{
public:

  /* audioStreamer public class constants */

  enum WinApi { WIN_AUDIO_ASIO , WIN_AUDIO_KS   ,
                WIN_AUDIO_DS   , WIN_AUDIO_WAVE } ;
  enum MacApi { MAC_AUDIO_CA } ;
  enum NixApi { NIX_AUDIO_JACK , NIX_AUDIO_ALSA } ;

  static const int   DEFAULT_N_INPUTS    = 2 ;
  static const int   DEFAULT_N_OUTPUTS   = 2 ;
  static const int   DEFAULT_SAMPLE_RATE = 44100 ;
  static const int   DEFAULT_BIT_DEPTH   = 16 ;
#ifdef _WIN32
#  ifndef NO_SUPPORT_ASIO
  static const int   DEFAULT_ASIO_DRIVER_N  = 0 ;
  static const int   DEFAULT_ASIO_INPUTB_N  = 0 ;
  static const int   DEFAULT_ASIO_INPUTE_N  = 1 ;
  static const int   DEFAULT_ASIO_OUTPUTB_N = 0 ;
  static const int   DEFAULT_ASIO_OUTPUTE_N = 1 ;
  static const bool  DEFAULT_ASIO_CONTROL   = FALSE ;
#  endif // NO_SUPPORT_ASIO
#  ifndef NO_SUPPORT_KS
  static const int   DEFAULT_KS_N_BLOCKS   = 8 ;
  static const int   DEFAULT_KS_BLOCK_SIZE = 512 ;
#  endif // NO_SUPPORT_KS
#  ifndef NO_SUPPORT_DS
  static const char* DEFAULT_DS_DEVICE_NAME ;
  static const int   DEFAULT_DS_N_BLOCKS   = 16 ;
  static const int   DEFAULT_DS_BLOCK_SIZE = 1024 ;
#  endif // NO_SUPPORT_DS
#  ifndef NO_SUPPORT_WAVE
  static const int   DEFAULT_WAVE_INPUT_N    = -1 ;
  static const int   DEFAULT_WAVE_OUTPUT_N   = -1 ;
  static const int   DEFAULT_WAVE_N_BLOCKS   = 8 ;
  static const int   DEFAULT_WAVE_BLOCK_SIZE = 4096 ;
#  endif // NO_SUPPORT_WAVE
#else // _WIN32
#  ifdef _MAC
  static const char* DEFAULT_CA_DEVICES ;
/* TODO: implement this
  static const char* DEFAULT_CA_DEVICES = "" ;
*/
#  else // _MAC
#    ifndef NO_SUPPORT_JACK
  static const char* DEFAULT_JACK_NAME ;
  static const char* DEFAULT_ALSA_INPUT ;
  static const char* DEFAULT_ALSA_OUTPUT ;
/* TODO: implement this
  static const char* DEFAULT_JACK_NAME      = "NINJAM" ;
  static const char* DEFAULT_ALSA_INPUT     = "hw:0,0" ;
  static const char* DEFAULT_ALSA_OUTPUT    = "hw:0,0" ;
*/
#    endif // NO_SUPPORT_JACK
#    ifndef NO_SUPPORT_ALSA
  static const int   DEFAULT_ALSA_N_BLOCKS   = 16 ;
  static const int   DEFAULT_ALSA_BLOCK_SIZE = 1024 ;
#    endif // NO_SUPPORT_ALSA
#  endif // _MAC
#endif // _WIN32


  /* audioStreamer public constructors */

#ifdef _WIN32
#  ifndef NO_SUPPORT_ASIO
  static audioStreamer* NewASIO(SPLPROC on_samples_proc                              ,
                                int     asio_driver_n       = DEFAULT_ASIO_DRIVER_N  ,
                                int     input_channel_b_n   = DEFAULT_ASIO_INPUTB_N  ,
                                int     input_channel_e_n   = DEFAULT_ASIO_INPUTE_N  ,
                                int     output_channel_b_n  = DEFAULT_ASIO_OUTPUTB_N ,
                                int     output_channel_e_n  = DEFAULT_ASIO_OUTPUTE_N ,
                                bool    should_show_asio_cp = DEFAULT_ASIO_CONTROL   ) ;
#  endif // NO_SUPPORT_ASIO
#  ifndef NO_SUPPORT_KS
  static audioStreamer* NewKS(  SPLPROC on_samples_proc) ;
  static audioStreamer* NewKS(  SPLPROC on_samples_proc                   ,
                                int     sample_rate     , int  bit_depth  ,
                                int*    n_buffers       , int* buffer_size) ;
#  endif // NO_SUPPORT_KS
#  ifndef NO_SUPPORT_DS
  static audioStreamer* NewDS(  SPLPROC on_samples_proc                           ,
                                GUID    input_device_guid  = GUID_NULL            ,
                                GUID    output_device_guid = GUID_NULL            ,
                                int     sample_rate        = DEFAULT_SAMPLE_RATE  ,
                                int     bit_depth          = DEFAULT_BIT_DEPTH    ,
                                int     n_buffers          = DEFAULT_DS_N_BLOCKS  ,
                                int     buffer_size        = DEFAULT_DS_BLOCK_SIZE) ;
#  endif // NO_SUPPORT_DS
#  ifndef NO_SUPPORT_WAVE
  static audioStreamer* NewWAVE(SPLPROC on_samples_proc                          ,
                                int     input_device_n  = DEFAULT_WAVE_INPUT_N   ,
                                int     output_device_n = DEFAULT_WAVE_OUTPUT_N  ,
                                int     sample_rate     = DEFAULT_SAMPLE_RATE    ,
                                int     bit_depth       = DEFAULT_BIT_DEPTH      ,
                                int     n_buffers       = DEFAULT_WAVE_N_BLOCKS  ,
                                int     buffer_size     = DEFAULT_WAVE_BLOCK_SIZE) ;
#  endif // NO_SUPPORT_WAVE
#else // _WIN32
#  ifdef _MAC
// TODO: implement this
  static audioStreamer* NewCA(  SPLPROC on_samples_proc                      ,
                                char**  audio_devices = DEFAULT_MAC_DEVICES  ,
                                int     n_channels    = DEFAULT_N_INPUTS     ,
                                int     sample_rate   = DEFAULT_SAMPLE_RATE  ,
                                int     bit_depth     = DEFAULT_BIT_DEPTH    ) ;
#  else // _MAC
#    ifndef NO_SUPPORT_JACK
// TODO: implement this
  static audioStreamer* NewJACK(SPLPROC     on_samples_proc                        ,
                                NJClient*   a_NJClient                             ,
                                const char* jack_client_name  = DEFAULT_JACK_NAME  ,
                                int         n_input_channels  = DEFAULT_N_INPUTS   ,
                                int         n_output_channels = DEFAULT_N_OUTPUTS  ) ;
#    endif // NO_SUPPORT_JACK
#    ifndef NO_SUPPORT_ALSA
// TODO: implement this
  static audioStreamer* NewALSA(SPLPROC     on_samples_proc                        ,
                                const char* input_device  = DEFAULT_ALSA_INPUT     ,
                                const char* output_device = DEFAULT_ALSA_OUTPUT    ,
                                int         n_channels    = DEFAULT_N_INPUTS       ,
                                int         sample_rate   = DEFAULT_SAMPLE_RATE    ,
                                int         bit_depth     = DEFAULT_BIT_DEPTH      ,
                                int         n_buffers     = DEFAULT_ALSA_N_BLOCKS  ,
                                int         buffer_size   = DEFAULT_ALSA_BLOCK_SIZE) ;
#    endif // NO_SUPPORT_ALSA
#  endif // _MAC
#endif // _WIN32
  virtual ~audioStreamer() { }


  /* audioStreamer public class methods */

#ifdef _WIN32
#  ifndef NO_SUPPORT_ASIO
  static bool        IsNjasiodrvAvailable() ;
  static std::string GetASIODriverName() ;
#  endif // NO_SUPPORT_ASIO
#  ifndef NO_SUPPORT_DS
  static std::vector<std::string> GetDsNames() ;
  static std::string              GetDsNamesCSV(   std::string separator) ;
  static std::vector<GUID>        GetDsGuids() ;
  static std::string              GetDsGuidsCSV(   std::string separator) ;
  static void                     GetDsGuidByName( std::string device_name , LPGUID guid) ;
  static int                      GetDsGuidIdx(    LPGUID guid) ;
  static std::string              DsGuidToString(  LPGUID guid) ;
  static GUID                     DsGuidFromString(std::string guid_string) ;
  static BOOL                     DsEnumDo(        LPDSENUMCALLBACK ds_enum_cb ,
                                                   LPVOID           user32     ) ;
#  endif // NO_SUPPORT_DS
#endif // _WIN32


private:

  /* audioStreamer private class constants */

#ifdef _WIN32
#  ifndef NO_SUPPORT_ASIO
  static const std::string ASIO_DLL_FILENAME ;
  static const LPCSTR      ASIO_CONSTRUCTOR_NAME ;
  static const LPCTSTR     ASIO_ARGS_FMT ;
#  endif // NO_SUPPORT_ASIO
#endif // _WIN32


  /* audioStreamer private class variables */

#ifdef _WIN32
#  ifndef NO_SUPPORT_ASIO
  static HINSTANCE   NjasiodrvDll ;
  static std::string ASIODriverName ;
#  endif // NO_SUPPORT_ASIO
#  ifndef NO_SUPPORT_DS
  static std::vector<DsDevice> DsDevices ;
#  endif // NO_SUPPORT_DS
#endif // _WIN32


  /* audioStreamer private class methods */

#ifdef _WIN32
#  ifndef NO_SUPPORT_ASIO
  static audioStreamer* (*CreateASIO)(char** cli_args , SPLPROC on_samples_proc) ;
  static bool           LoadNjasiodrvDll() ;
#  endif // NO_SUPPORT_ASIO
#  ifndef NO_SUPPORT_DS
  static void          LoadDsDevices() ;
  static BOOL CALLBACK StoreDsDevice(LPGUID  lpGUID      , LPCTSTR lpszDesc ,
                                     LPCTSTR lpszDrvName , LPVOID  unused   ) ;
#  endif // NO_SUPPORT_DS
#endif // _WIN32


public:

  /* audioStreamer public interface stubs */

  virtual const char* GetChannelName(int idx)       = 0 ;
  virtual const char* GetInputChannelName(int idx)  { return GetChannelName(idx) ; }
  virtual const char* GetOutputChannelName(int idx) { return GetChannelName(idx) ; }
  virtual       bool  addInputChannel()             { return false ; }
  virtual       bool  addOutputChannel()            { return false ; }


  /* audioStreamer public instance methods */

  int  getNInputChannels()  { return m_innch ;  }
  int  getNOutputChannels() { return m_outnch ; }
  int  getSampleRate()      { return m_srate ;  }
  int  getBitDepth()        { return m_bps ;    }


protected:

  /* audioStreamer private instance variables */

  int m_innch ;
  int m_outnch ;
  int m_srate ;
  int m_bps ;


  /* audioStreamer private constructors */

  audioStreamer()
  {
    m_innch  = DEFAULT_N_INPUTS ;
    m_outnch = DEFAULT_N_OUTPUTS ;
    m_srate  = DEFAULT_SAMPLE_RATE ;
    m_bps    = DEFAULT_BIT_DEPTH ;
  }
} ;


#ifndef _WIN32
#  ifndef _MAC
// legacy v0.06 (gNinjam)
audioStreamer* create_audioStreamer_JACK(const char* jack_client_name  ,
                                         int         n_input_channels  ,
                                         int         n_output_channels ,
                                         SPLPROC     on_samples_proc   ,
                                         NJClient*   a_NJClient        ) ;
audioStreamer* create_audioStreamer_ALSA(char* cli_args , SPLPROC on_samples_proc) ;
#  endif // _MAC
#endif // _WIN32

#endif // _AUDIOSTREAM_H_
