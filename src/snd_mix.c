/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// snd_mix.c -- portable code to mix sounds for snd_dma.c

#include "quakedef.h"

#ifdef WIN32
#include "winquake.h"
#else
#define DWORD	unsigned long
#endif

#define	PAINTBUFFER_SIZE	512
portable_samplepair_t paintbuffer[PAINTBUFFER_SIZE+1];
//portable_samplepair_t roombuffer[PAINTBUFFER_SIZE+1];

int		snd_scaletable[32][256];
int 	*snd_p, snd_linear_count, snd_vol;
short	*snd_out;

void Snd_WriteLinearBlastStereo16 (void)
{
	int		i;
	int		val;

	for (i=0 ; i<snd_linear_count ; i+=2)
	{
		val = (snd_p[i]*snd_vol)>>8;
		if (val > 0x7fff)
			snd_out[i] = 0x7fff;
		else if (val < (short)0x8000)
			snd_out[i] = (short)0x8000;
		else
			snd_out[i] = val;

		val = (snd_p[i+1]*snd_vol)>>8;
		if (val > 0x7fff)
			snd_out[i+1] = 0x7fff;
		else if (val < (short)0x8000)
			snd_out[i+1] = (short)0x8000;
		else
			snd_out[i+1] = val;
	}
}

void S_TransferStereo16 (int endtime)
{
	int		lpos;
	int		lpaintedtime;
	DWORD	*pbuf;

	snd_vol = volume.value*256;

	snd_p = (int *) paintbuffer;
	lpaintedtime = paintedtime;

	pbuf = (DWORD *)shm->buffer;
	while (lpaintedtime < endtime)
	{
	// handle recirculating buffer issues
		lpos = lpaintedtime & ((shm->samples>>1)-1);

		snd_out = (short *) pbuf + (lpos<<1);

		snd_linear_count = (shm->samples>>1) - lpos;
		if (lpaintedtime + snd_linear_count > endtime)
			snd_linear_count = endtime - lpaintedtime;

		snd_linear_count <<= 1;

	// write a linear blast of samples
		Snd_WriteLinearBlastStereo16 ();

		snd_p += snd_linear_count;
		lpaintedtime += (snd_linear_count>>1);
	}
}
#if 0
void S_TransferPaintBuffer(int endtime)
{
	int 	out_idx;
	int 	count;
	int 	out_mask;
	int 	*p;
	int 	step;
	int		val;
	int		snd_vol;
	DWORD	*pbuf;

	if (shm->samplebits == 16 && shm->channels == 2)
	{
		S_TransferStereo16 (endtime);
		return;
	}
	
	p = (int *) paintbuffer;
	count = (endtime - paintedtime) * shm->channels;
	out_mask = shm->samples - 1; 
	out_idx = paintedtime * shm->channels & out_mask;
	step = 3 - shm->channels;
	snd_vol = volume.value*256;

	pbuf = (DWORD *)shm->buffer;
	if (shm->samplebits == 16)
	{
		short *out = (short *) pbuf;
		while (count--)
		{
			val = (*p * snd_vol) >> 8;
			p+= step;
			val = CLIP(val);
			
			out[out_idx] = val;
			out_idx = (out_idx + 1) & out_mask;
		}
	}
	else if (shm->samplebits == 8)
	{
		unsigned char *out = (unsigned char *) pbuf;
		while (count--)
		{
			val = (*p * snd_vol) >> 8;
			p+= step;
            val = CLIP(val);

			out[out_idx] = (val>>8) + 128;
			out_idx = (out_idx + 1) & out_mask;
		}
	}
}
#else
/*
===================
S_TransferPaintBuffer

===================
*/
void S_TransferPaintBuffer( int endtime )
{
	int	lpos, lpaintedtime, snd_vol;
	int	*snd_p, snd_linear_count;
	int	i, val, sampleMask;
	short	*snd_out;
	DWORD	*pbuf;

	pbuf = (DWORD *)shm->buffer;
	snd_p = (int *)paintbuffer;
	lpaintedtime = paintedtime;
	sampleMask = ((shm->samples >> 1) - 1);

	snd_vol = volume.value * 256;

	while( lpaintedtime < endtime )
	{
		// handle recirculating buffer issues
		lpos = lpaintedtime & sampleMask;

		snd_out = (short *)pbuf + (lpos << 1);

		snd_linear_count = (shm->samples>>1) - lpos;
		if( lpaintedtime + snd_linear_count > endtime )
			snd_linear_count = endtime - lpaintedtime;

		snd_linear_count <<= 1;

		// write a linear blast of samples
		for( i = 0; i < snd_linear_count; i += 2 )
		{
			val = (snd_p[i+0] * snd_vol) >> 8;

			if( val > 0x7fff ) snd_out[i+0] = 0x7fff;
			else if( val < (short)0x8000 )
				snd_out[i+0] = (short)0x8000;
			else snd_out[i+0] = val;

			val = (snd_p[i+1] * snd_vol) >> 8;
			if( val > 0x7fff ) snd_out[i+1] = 0x7fff;
			else if( val < (short)0x8000 )
				snd_out[i+1] = (short)0x8000;
			else snd_out[i+1] = val;
		}

		snd_p += snd_linear_count;
		lpaintedtime += (snd_linear_count >> 1);
	}
}
#endif

/*
===============================================================================

CHANNEL MIXING

===============================================================================
*/

void SND_PaintChannelFrom8 (channel_t *ch, sfxcache_t *sc, int endtime, portable_samplepair_t *buf);
void SND_PaintChannelFrom16 (channel_t *ch, sfxcache_t *sc, int endtime, portable_samplepair_t *buf);

void MIX_MixPaintbuffers( portable_samplepair_t *buf1, portable_samplepair_t *buf2, portable_samplepair_t *buf3, int count, float fgain )
{
	int i, gain;
    gain = 256 * fgain;

	// destination 2ch:
	// pb1 2ch + pb2 2ch		-> pb3 2ch
	// pb1 2ch + pb2 (4ch->2ch)		-> pb3 2ch
	// pb1 (4ch->2ch) + pb2 (4ch->2ch)	-> pb3 2ch

	// mix front channels
	for( i = 0; i < count; i++ )
	{
		buf3[i].left = buf1[i].left;
		buf3[i].right = buf1[i].right;
		buf3[i].left += (buf2[i].left * gain) >> 8;
		buf3[i].right += (buf2[i].right * gain) >> 8;
	}
}

void MIX_CompressPaintbuffer( portable_samplepair_t *buf, int count )
{
	int i;
	for( i = 0; i < count; i++, buf++ )
	{
		buf->left = CLIP( buf->left );
		buf->right = CLIP( buf->right );
	}
}

void MIX_MixChannelsToPaintbuffer( int endtime, portable_samplepair_t *buf )
{
	channel_t *ch;
	sfxcache_t	*sc;
	int		i, ltime, count;
	
	// mix each channel into paintbuffer
	ch = channels;
	for (i=0; i<total_channels ; i++, ch++)
	{
		if (!ch->sfx)
			continue;

		if (!ch->leftvol && !ch->rightvol)
			continue;

		sc = S_LoadSound (ch->sfx);
		if (!sc)
			continue;

		ltime = paintedtime;
		while (ltime < endtime)
		{	// paint up to end

			if (ch->end < endtime)
				count = ( ch->end - ltime );
			else
				count = ( endtime - ltime );

			if (count > 0)
			{
				if (sc->width == 1)
					SND_PaintChannelFrom8(ch, sc, count, buf);
				else
					SND_PaintChannelFrom16(ch, sc, count, buf);

				ltime += count;
			}

		// if at end of loop, restart
			if (ltime >= ch->end)
			{
				if (sc->loopstart >= 0)
				{
					ch->pos = sc->loopstart;
					ch->end = ltime + sc->length - ch->pos;
				}
				else
				{	// channel just stopped
					ch->sfx = NULL;
					break;
				}
			}
		}
	}
}

void S_PaintChannels(int endtime)
{
    int	end, count;
#ifdef DSP2
	float	dsp_room_gain;

    CheckNewDspPresets();

	// get dsp preset gain values, update gain crossfaders,
	// used when mixing dsp processed buffers into paintbuffer
	dsp_room_gain = DSP_GetGain( idsp_room );
#endif

	while (paintedtime < endtime)
	{
	    // if paintbuffer is smaller than DMA buffer
		end = endtime;
		if (endtime - paintedtime > PAINTBUFFER_SIZE)
			end = paintedtime + PAINTBUFFER_SIZE;

		count = end - paintedtime;

	    // clear the paint buffer
		Q_memset(paintbuffer, 0, count * sizeof(portable_samplepair_t));

	    // clear the room buffer
		//Q_memset(roombuffer, 0, count * sizeof(portable_samplepair_t));

	    MIX_MixChannelsToPaintbuffer( end, paintbuffer );

#ifdef DSP2
		// process all sounds with DSP
		DSP_Process( idsp_room, paintbuffer, count );
#else
        SX_RoomFX(end, 0, 1);
#endif
		MIX_CompressPaintbuffer( paintbuffer, count );

	    // transfer out according to DMA format
		S_TransferPaintBuffer(end);
		paintedtime = end;
	}
}

void SND_InitScaletable (void)
{
	int		i, j;

	for (i=0 ; i<32 ; i++)
		for (j=0 ; j<256 ; j++)
			snd_scaletable[i][j] = ((j < 128) ? j : j - 0xff) * i * 8; //FIXED See: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=26719

}


void SND_PaintChannelFrom8 (channel_t *ch, sfxcache_t *sc, int count, portable_samplepair_t *buf)
{
	int 	data;
	int		*lscale, *rscale;
	unsigned char *sfx;
	int		i;

	if (ch->leftvol > 255)
		ch->leftvol = 255;
	if (ch->rightvol > 255)
		ch->rightvol = 255;
		
	lscale = snd_scaletable[ch->leftvol >> 3];
	rscale = snd_scaletable[ch->rightvol >> 3];
	sfx = (unsigned char *) ((signed char *) sc->data + ch->pos);

	for (i=0 ; i<count ; i++)
	{
		data = sfx[i];
		buf[i].left += lscale[data];
		buf[i].right += rscale[data];
	}
	
	ch->pos += count;
}

void SND_PaintChannelFrom16 (channel_t *ch, sfxcache_t *sc, int count, portable_samplepair_t *buf)
{
	int data;
	int left, right;
	int leftvol, rightvol;
	signed short *sfx;
	int	i;

	leftvol = ch->leftvol;
	rightvol = ch->rightvol;
	sfx = (signed short *)sc->data + ch->pos;

	for (i=0 ; i<count ; i++)
	{
		data = sfx[i];
		left = (data * leftvol) >> 8;
		right = (data * rightvol) >> 8;
		buf[i].left += left;
		buf[i].right += right;
	}

	ch->pos += count;
}


