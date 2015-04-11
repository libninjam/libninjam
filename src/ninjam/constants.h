
#include <string>


namespace NINJAM
{
  // NOTE: most of libninjam does not yet respect these constants explicitly
  // TODO: have yet to locate the code that handles host and username validation
  //           but experiments have shown usernames are trunctuated to 16 chars
  //           and valid username chars include [0-9] , [a-z] , '-' , '_'
  //           (njclient.h states that usernames are case insensitive)
  const  std::string HOST_CHARS           = "0123456789abcdefghijklmnopqrstuvwxyz-." ;
  const  std::string NICK_CHARS           = "0123456789abcdefghijklmnopqrstuvwxyz-_" ;
  // TODO: have yet to locate the code that handles channelname validation
  //           but experiments have shown channelnames may be absurdly long (> 2000 chars)
  static const int   MAX_USER_NAME_LEN    = 16 ;
  static const int   MAX_CHANNEL_NAME_LEN = 256 ;
  static const int   MAX_DEVICE_NAME_LEN  = 256 ;
  static const int   MAX_FILEPATH_LEN     = 1024 ;
}
