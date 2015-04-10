/*
  This file provides the basic code for loading a pre-compiled binary
      NINJAM<->ASIO interface 'NJASIODRV.DLL' if it is exists.
  The code for that interface is a separate project (in 'ninjam/njasiodrv')
      because it requires headers from the ASIO SDK which are not GPL'ed.
*/


#ifndef NO_SUPPORT_ASIO

#include "audiostream.h"

#include <strsafe.h>


/* audioStreamer private class constants */

const std::string audioStreamer::ASIO_DLL_FILENAME     = "njasiodrv.dll" ;
const LPCSTR      audioStreamer::ASIO_CONSTRUCTOR_NAME = "create_asio_streamer" ;
const LPCTSTR     audioStreamer::ASIO_ARGS_FMT         = "%d%c:%d,%d:%d,%d" ;


/* audioStreamer private class variables */

HINSTANCE   audioStreamer::NjasiodrvDll ;   // LoadNjasiodrvDll()
std::string audioStreamer::ASIODriverName ; // NewASIO()


/* audioStreamer public constructors */

audioStreamer* audioStreamer::NewASIO(SPLPROC on_samples_proc     , int asio_driver_n      ,
                                      int     input_channel_b_n   , int input_channel_e_n  ,
                                      int     output_channel_b_n  , int output_channel_e_n ,
                                      bool    should_show_asio_cp                          )
{
/* cli_args format - 'c' forces control panel open - the rest are ints like:
       [ driverN [ + 'c' ] [ + ':' + input_ch_begin_n  [ + ',' + input_ch_end_n  ] ]
                           [ + ':' + output_ch_begin_n [ + ',' + output_ch_end_n ] ] ]
   CreateASIO() modifies cli_args - on return cli_args will point to driver name */

  const int n_bytes    = NINJAM::MAX_DEVICE_NAME_LEN ;
  char  cli_args_buffer[n_bytes] ;
  char* cli_args       = cli_args_buffer ;
  char  should_show_cp = (should_show_asio_cp ) ? 'c' : ' ' ;

  if (!IsNjasiodrvAvailable()                                                        ||
      FAILED(StringCbPrintf(cli_args_buffer    , n_bytes            , ASIO_ARGS_FMT ,
                            asio_driver_n      , should_show_cp                     ,
                            input_channel_b_n  , input_channel_e_n                  ,
                            output_channel_b_n , output_channel_e_n                 )))
    return NULL ;

  audioStreamer* audio = CreateASIO(&cli_args , on_samples_proc) ;

  if (!!audio) ASIODriverName = cli_args ;

  return (!!audio) ? audio : NULL ;
}


/* audioStreamer public class methods */

bool audioStreamer::IsNjasiodrvAvailable() { return LoadNjasiodrvDll() ; }

std::string audioStreamer::GetASIODriverName() { return ASIODriverName ; }


/* audioStreamer private class methods */

audioStreamer* (*audioStreamer::CreateASIO)(char** cli_args , SPLPROC on_samples_proc) ;

bool audioStreamer::LoadNjasiodrvDll()
{
  if (!!NjasiodrvDll && !!CreateASIO) return true ;

  // get filepath of host binary - e.g. "c:\Program Files\aNinjam\aNinjam.exe"
  char  buffer[NINJAM::MAX_FILEPATH_LEN] ;
  GetModuleFileName(GetModuleHandle(NULL) , buffer , sizeof(buffer)) ;
  std::string filepath(buffer , 0 , NINJAM::MAX_FILEPATH_LEN) ;

  // replace binary filename with ASIO_DLL_FILENAME
  size_t filename_begin = filepath.find_last_of('\\') + 1 ;
  filepath.replace(filename_begin , std::string::npos , ASIO_DLL_FILENAME) ;

  // load NjasiodrvDll if present
  if (NjasiodrvDll = LoadLibrary(filepath.c_str()))
    *((void**)&CreateASIO) = (void*)GetProcAddress(NjasiodrvDll , ASIO_CONSTRUCTOR_NAME) ;

  return !!NjasiodrvDll && !!CreateASIO ;
}

#endif // NO_SUPPORT_ASIO
