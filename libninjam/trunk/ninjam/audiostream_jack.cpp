/*
    NINJAM - audiostream_alsa.cpp
    Copyright (C) 2004-2005 Cockos Incorporated

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

  This file implements a audioStreamer that uses ALSA.
  It only exposes the following functions:

    audioStreamer *create_audioStreamer_ALSA(char *cfg, SPLPROC proc);
  
    cfg is a string that has a list of parameter/value pairs (space delimited) 
    for the config:
      in     - input device i.e. hw:0,0
      out    - output device i.e. hw:0,0
      srate  - sample rate i.e. 48000
      bps    - bits/sample i.e. 16
      nch    - channels i.e. 2
      bsize  - block size (bytes) i.e. 2048
      nblock - number of blocks i.e. 16


  (everything else in this file is used internally)

*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include <jack/jack.h>

#include <WDL/pcmfmtcvt.h>

#include <WDL/ptrlist.h>
#include "njclient.h"

#include "audiostream.h"

static void audiostream_onunder() { }
static void audiostream_onover() { }


const int c_ppqn = 192;


class audioStreamer_JACK : public audioStreamer
{
    public:
	audioStreamer_JACK( char *cfg, SPLPROC proc );
	~audioStreamer_JACK();

	int process( jack_nframes_t nframes );
	void timebase_cb(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos );


	const char *GetChannelName(int idx)
	{
	    if (idx == 0) return "Channel 0";
	    if (idx == 1) return "Channel 1";
	    if (idx == 2) return "Channel 2";
	    if (idx == 3) return "Channel 3";
	    return NULL;
	    return NULL;
	    return NULL;
	}
    private:
	jack_client_t *client;
	jack_port_t *in1, *in2, *in3, *in4;
	jack_port_t *out1, *out2, *out3, *out4;

	SPLPROC splproc;
	NJClient *njc;
    public:
	void set_njclient( NJClient *njclient ) { njc = njclient; }
};


int
process_cb( jack_nframes_t nframes, audioStreamer_JACK *as ) {
    return as->process( nframes );
}

void jack_timebase_cb(jack_transport_state_t state,
                                     jack_nframes_t nframes,
                                     jack_position_t *pos,
                                     int new_pos,
                                     audioStreamer_JACK *as)
{
    as->timebase_cb( state, nframes, pos, new_pos );
}



//////////////// JACK driver
audioStreamer_JACK::audioStreamer_JACK( char *cfg, SPLPROC proc) 
{ 

    njc = NULL;
    splproc = proc;

    if ((client = jack_client_new ("ninjam")) == 0) {
	fprintf (stderr, "jack server not running?\n");
	exit(20);
    }

    jack_set_process_callback (client, (JackProcessCallback) process_cb, this);

    jack_set_timebase_callback( client, 0, (JackTimebaseCallback) jack_timebase_cb, this );

    out1 = jack_port_register (client, "out1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    out2 = jack_port_register (client, "out2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    out3 = jack_port_register (client, "out3", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    out4 = jack_port_register (client, "out4", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    in1 = jack_port_register (client, "in1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    in2 = jack_port_register (client, "in2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    in3 = jack_port_register (client, "in3", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    in4 = jack_port_register (client, "in4", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    if (jack_activate (client)) {
	fprintf (stderr, "cannot activate client");
	exit(20);
    }

    m_innch = m_outnch = 4;
    m_srate = jack_get_sample_rate( client );
    m_bps = 32;
}

audioStreamer_JACK::~audioStreamer_JACK() 
{
    jack_deactivate( client );
    sleep(1);
}

int
audioStreamer_JACK::process( jack_nframes_t nframes ) {
    float *inports[4];
    float *outports[4];

    inports[0] = (float *) jack_port_get_buffer( in1, nframes );
    inports[1] = (float *) jack_port_get_buffer( in2, nframes );
    inports[2] = (float *) jack_port_get_buffer( in3, nframes );
    inports[3] = (float *) jack_port_get_buffer( in4, nframes );
    outports[0] = (float *) jack_port_get_buffer( out1, nframes );
    outports[1] = (float *) jack_port_get_buffer( out2, nframes );
    outports[2] = (float *) jack_port_get_buffer( out3, nframes );
    outports[3] = (float *) jack_port_get_buffer( out4, nframes );

    splproc( inports, 4, outports, 4, nframes, jack_get_sample_rate( client ) );
    return 0;
}


void
audioStreamer_JACK::timebase_cb(jack_transport_state_t state, jack_nframes_t nframes, jack_position_t *pos, int new_pos ) {

    static double jack_tick;
    static jack_nframes_t last_frame;
    static jack_nframes_t current_frame;
    static jack_transport_state_t state_current;
    static jack_transport_state_t state_last;

    if( state != JackTransportRolling )
    {
	jack_transport_start( client );
    }

    if( !njc ) return;

    float bpm = njc->GetActualBPM();
    int posi, len;
    posi = njc->GetSessionPosition() * m_srate / 1000;
    len = (int)(njc->GetBPI() * m_srate * 60 / bpm);

    // sync jack_transport_frame to njframe
    
    current_frame = jack_get_current_transport_frame( client );
    
    // FIXME: This will not work right, if there are slow-sync clients....

    int diff =abs(current_frame % len) - (posi %len);

    if( diff > nframes ) {
	jack_transport_locate( client, (current_frame / len) * len + (posi%len) + 2*nframes );
	
	//printf( "no:  current= %d diff = %d\n", (current_frame % len) -  (posi % len), diff );
    }

    // taken from seq24-0.7.0 perform.cpp

    state_current = state;


    //printf( "jack_timebase_callback() [%d] [%d] [%d]", state, new_pos, current_frame);
    
    pos->valid = JackPositionBBT;
    pos->beats_per_bar = 4;
    pos->beat_type = 4;
    pos->ticks_per_beat = c_ppqn * 10;    
    pos->beats_per_minute = bpm;
    
    
    /* Compute BBT info from frame number.  This is relatively
     * simple here, but would become complex if we supported tempo
     * or time signature changes at specific locations in the
     * transport timeline. */

    // if we are in a new position
    if (  state_last    ==  JackTransportStarting &&
          state_current ==  JackTransportRolling ){

        //printf ( "Starting [%d] [%d]\n", last_frame, current_frame );
        
        jack_tick = 0.0;
        last_frame = current_frame;
    }

    if ( current_frame > last_frame ){

        double jack_delta_tick =
            (current_frame - last_frame) *
            pos->ticks_per_beat *
            pos->beats_per_minute / (pos->frame_rate * 60.0);
        
        jack_tick += jack_delta_tick;

        last_frame = current_frame;
    }
    
    long ptick = 0, pbeat = 0, pbar = 0;
    
    pbar  = (long) ((long) jack_tick / (pos->ticks_per_beat *  pos->beats_per_bar ));
    
    pbeat = (long) ((long) jack_tick % (long) (pos->ticks_per_beat *  pos->beats_per_bar ));
    pbeat = pbeat / (long) pos->ticks_per_beat;
    
    ptick = (long) jack_tick % (long) pos->ticks_per_beat;
    
    pos->bar = pbar + 1;
    pos->beat = pbeat + 1;
    pos->tick = ptick;;
    pos->bar_start_tick = pos->bar * pos->beats_per_bar *
        pos->ticks_per_beat;

    //printf( " bbb [%2d:%2d:%4d]\n", pos->bar, pos->beat, pos->tick );

    state_last = state_current;

}

audioStreamer *create_audioStreamer_JACK(char *cfg, SPLPROC proc, NJClient *njclient)
{
  audioStreamer_JACK *au = new audioStreamer_JACK( cfg, proc);
  au->set_njclient( njclient );
  return au;
}
