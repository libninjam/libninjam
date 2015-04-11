for simplicity the MSVC project expects to find
    all of the required library headers in this directory

  * ogg           - (directory)
    * ogg.h       - from libogg
    * os_types.h  - from libogg
  * vorbis        - (directory)
    * codec.h     - from libvorbis
    * vorbisenc.h - from libvorbis
  * dsound.h      - from the DirectX SDK

the DirectX SDK is not strictly necessary unless you wish to support the DirectSound API
    but ogg and vorbis are currently strict requisites

the files noted above can be found at their official websites:
  libogg      -> https://www.xiph.org/downloads/
  libvorbis   -> https://www.xiph.org/downloads/
  DirectX SDK -> http://www.microsoft.com/en-us/download/details.aspx?id=6812
