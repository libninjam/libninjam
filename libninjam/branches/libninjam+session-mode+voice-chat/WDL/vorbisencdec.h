/*
    WDL - vorbisencdec.h
    Copyright (C) 2005 Cockos Incorporated

    WDL is dual-licensed. You may modify and/or distribute WDL under either of 
    the following  licenses:
    
      This software is provided 'as-is', without any express or implied
      warranty.  In no event will the authors be held liable for any damages
      arising from the use of this software.

      Permission is granted to anyone to use this software for any purpose,
      including commercial applications, and to alter it and redistribute it
      freely, subject to the following restrictions:

      1. The origin of this software must not be misrepresented; you must not
         claim that you wrote the original software. If you use this software
         in a product, an acknowledgment in the product documentation would be
         appreciated but is not required.
      2. Altered source versions must be plainly marked as such, and must not be
         misrepresented as being the original software.
      3. This notice may not be removed or altered from any source distribution.
      

    or:

      WDL is free software; you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation; either version 2 of the License, or
      (at your option) any later version.

      WDL is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with WDL; if not, write to the Free Software
      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*

  This file provides simple interfaces for encoding and decoding of OGG Vorbis data.
  It is a wrapper around what their SDKs expose, which is not too easy to use.

  This stuff is pretty limited and simple, but works, usually.
 
*/

#ifndef _VORBISENCDEC_H_
#define _VORBISENCDEC_H_

#include "vorbis/vorbisenc.h"
#include "vorbis/codec.h"
#include "queue.h"

class VorbisDecoder
{
  public:
    VorbisDecoder()
    {
      m_samples_used=0;
    	packets=0;
	    memset(&oy,0,sizeof(oy));
	    memset(&os,0,sizeof(os));
	    memset(&og,0,sizeof(og));
	    memset(&op,0,sizeof(op));
	    memset(&vi,0,sizeof(vi));
	    memset(&vc,0,sizeof(vc));
	    memset(&vd,0,sizeof(vd));
	    memset(&vb,0,sizeof(vb));


      ogg_sync_init(&oy); /* Now we can read pages */
      m_err=0;
    }
    ~VorbisDecoder()
    {
      ogg_stream_clear(&os);
      vorbis_block_clear(&vb);
      vorbis_dsp_clear(&vd);
	    vorbis_comment_clear(&vc);
      vorbis_info_clear(&vi);

  	  ogg_sync_clear(&oy);
    }

    int GetSampleRate() { return vi.rate; }
    int GetNumChannels() { return vi.channels?vi.channels:1; }

    WDL_HeapBuf m_samples; // we let the size get as big as it needs to, so we don't worry about tons of mallocs/etc
    int m_samples_used;

    void *DecodeGetSrcBuffer(int srclen)
    {
		  return ogg_sync_buffer(&oy,srclen);
    }

    void DecodeWrote(int srclen)
    {
      ogg_sync_wrote(&oy,srclen);
  
		  while(ogg_sync_pageout(&oy,&og)>0)
		  {
			  int serial=ogg_page_serialno(&og);
			  if (!packets) ogg_stream_init(&os,serial);
			  else if (serial!=os.serialno)
			  {
				  vorbis_block_clear(&vb);
				  vorbis_dsp_clear(&vd);
				  vorbis_comment_clear(&vc);
				  vorbis_info_clear(&vi);

				  ogg_stream_clear(&os);
				  ogg_stream_init(&os,serial);
				  packets=0;
			  }
			  if (!packets)
			  {
				  vorbis_info_init(&vi);
				  vorbis_comment_init(&vc);
			  }
			  ogg_stream_pagein(&os,&og);
			  while(ogg_stream_packetout(&os,&op)>0)
			  {
				  if (packets<3)
				  {
					  if(vorbis_synthesis_headerin(&vi,&vc,&op)<0) return;
				  }
				  else
				  {
					  float ** pcm;
					  int samples;
					  if(vorbis_synthesis(&vb,&op)==0) vorbis_synthesis_blockin(&vd,&vb);
					  while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0)
					  {
						  int n,c;

              int newsize=(m_samples_used+(samples+4096)*vi.channels)*sizeof(float);

              if (m_samples.GetSize() < newsize) m_samples.Resize(newsize+32768);

              float *bufmem = (float *)m_samples.Get();

						  for(n=0;n<samples;n++)
						  {
							  for(c=0;c<vi.channels;c++)
							  {
								  bufmem[m_samples_used++]=pcm[c][n];
							  }							
						  }
						  vorbis_synthesis_read(&vd,samples);
					  }
				  }
				  packets++;
				  if (packets==3)
				  {
					  vorbis_synthesis_init(&vd,&vi);
					  vorbis_block_init(&vd,&vb);
				  }
			  }
		  }
    }

    void Reset()
    {
      m_samples_used=0;

			vorbis_block_clear(&vb);
			vorbis_dsp_clear(&vd);
			vorbis_comment_clear(&vc);
			vorbis_info_clear(&vi);

			ogg_stream_clear(&os);
			packets=0;
    }

  private:


    int m_err;
    int packets;

    ogg_sync_state   oy; /* sync and verify incoming physical bitstream */
    ogg_stream_state os; /* take physical pages, weld into a logical
			    stream of packets */
    ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
    ogg_packet       op; /* one raw packet of data for decode */
  
    vorbis_info      vi; /* struct that stores all the static vorbis bitstream
			    settings */
    vorbis_comment   vc; /* struct that stores all the bitstream user comments */
    vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
    vorbis_block     vb; /* local working space for packet->PCM decode */

};


class VorbisEncoder
{
public:
  VorbisEncoder(int srate, int nch, int bitrate, int serno)
  {
    m_ds=0;

    memset(&vi,0,sizeof(vi));
    memset(&vc,0,sizeof(vc));
    memset(&vd,0,sizeof(vd));
    memset(&vb,0,sizeof(vb));

    m_nch=nch;
    vorbis_info_init(&vi);

    float qv=0.0;
    if (nch == 2) bitrate=  (bitrate*5)/8;
    // at least for mono 44khz
    //-0.1 = ~40kbps
    //0.0 == ~64kbps
    //0.1 == 75
    //0.3 == 95
    //0.5 == 110
    //0.75== 140
    //1.0 == 240
    if (bitrate <= 32)
    {
      m_ds=1;
      bitrate*=2;
    }
   
    if (bitrate < 40) qv=-0.1f;
    else if (bitrate < 64) qv=-0.10f + (bitrate-40)*(0.10f/24.0f);
    else if (bitrate < 75) qv=(bitrate-64)*(0.1f/9.0f);
    else if (bitrate < 95) qv=0.1f+(bitrate-75)*(0.2f/20.0f);
    else if (bitrate < 110) qv=0.3f+(bitrate-95)*(0.2f/15.0f);
    else if (bitrate < 140) qv=0.5f+(bitrate-110)*(0.25f/30.0f);
    else qv=0.75f+(bitrate-140)*(0.25f/100.0f);

    if (qv<-0.10f)qv=-0.10f;
    if (qv>1.0f)qv=1.0f;

    m_err=vorbis_encode_init_vbr(&vi,nch,srate>>m_ds,qv);

    vorbis_comment_init(&vc);
    vorbis_analysis_init(&vd,&vi);
    vorbis_block_init(&vd,&vb);
    ogg_stream_init(&os,m_ser=serno);

    if (m_err) return;


    reinit(1);
  }

  void reinit(int bla=0)
  {
    if (!bla)
    {
      ogg_stream_clear(&os);
      vorbis_block_clear(&vb);
      vorbis_dsp_clear(&vd);

      vorbis_analysis_init(&vd,&vi);
      vorbis_block_init(&vd,&vb);
      ogg_stream_init(&os,m_ser++); //++?
 
      outqueue.Advance(outqueue.Available());
      outqueue.Compact();
    }


    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;
    vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
    ogg_stream_packetin(&os,&header); /* automatically placed in its own page */
    ogg_stream_packetin(&os,&header_comm);
    ogg_stream_packetin(&os,&header_code);

	  for (;;)
    {
      ogg_page og;
		  int result=ogg_stream_flush(&os,&og);
		  if(result==0)break;
      outqueue.Add(og.header,og.header_len);
		  outqueue.Add(og.body,og.body_len);
	  }
  }

  void Encode(float *in, int inlen, int advance=1, int spacing=1) // length in sample (PAIRS)
  {
    if (m_err) return;

    if (inlen == 0)
    {
      // disable this for now, it fucks us sometimes
      // maybe we should throw some silence in instead?
        vorbis_analysis_wrote(&vd,0);
    }
    else
    {
      inlen >>= m_ds;
      float **buffer=vorbis_analysis_buffer(&vd,inlen);
      int i,i2=0;
      for (i = 0; i < inlen; i ++)
      {
        buffer[0][i]=in[i2];
        if (m_nch==2) buffer[1][i]=in[i2+spacing];
        i2+=advance<<m_ds;
      }
      vorbis_analysis_wrote(&vd,i);
    }

    int eos=0;
    while(vorbis_analysis_blockout(&vd,&vb)==1)
    {
      vorbis_analysis(&vb,NULL);
      vorbis_bitrate_addblock(&vb);
      ogg_packet       op;

      while(vorbis_bitrate_flushpacket(&vd,&op))
      {
	
      	ogg_stream_packetin(&os,&op);

	      while (!eos)
        {
          ogg_page og;
		      int result=ogg_stream_flush(&os,&og);
		      if(result==0)break;
          outqueue.Add(og.header,og.header_len);
		      outqueue.Add(og.body,og.body_len);
          if(ogg_page_eos(&og)) eos=1;
	      }
      }
    }
  }

  int isError() { return m_err; }


  ~VorbisEncoder()
  {
    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    if (!m_err) vorbis_info_clear(&vi);
  }

  WDL_Queue outqueue;

private:
  int m_err,m_nch;

  ogg_stream_state os;
  vorbis_info      vi;
  vorbis_comment   vc;
  vorbis_dsp_state vd;
  vorbis_block     vb;
  int m_ser;
  int m_ds;

};


#endif//_VORBISENCDEC_H_
