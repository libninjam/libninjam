for simplicity the MSVC project expects to find
    all of the required libraries in this directory

  * libogg_static.lib        - from libogg
  * libogg_static_dbg.lib    - from libogg             (debug build)
  * libvorbis_static.lib     - from libvorbis
  * libvorbis_static_dbg.lib - from libvorbis          (debug build)
  * dsound.lib               - from the DirectX SDK

the libninjam artifacts also will be copied to this directory upon successful build

  * libninjam_static.lib     - this is our output file
  * libninjam_static_dbg.lib - this is our output file (debug build)

the DirectX SDK is not strictly necessary unless you wish to support the DirectSound API
    but ogg and vorbis are currently strict requisites
the *_dbg.lib files obviously may be ommitted unless you need a debug build

the files noted above can be found at their official websites:
  libogg      -> https://www.xiph.org/downloads/
  libvorbis   -> https://www.xiph.org/downloads/
  DirectX SDK -> http://www.microsoft.com/en-us/download/details.aspx?id=6812
