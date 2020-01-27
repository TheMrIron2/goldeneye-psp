/*
Copyright (C) 2011-2012 Crow_bar.

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
// snd_dsp.c -- Digital Signal Processing algorithms for audio FX

#include "quakedef.h"

extern portable_samplepair_t paintbuffer[];

#define PBITS		12		// parameter bits
#define PMAX		((1 << PBITS)-1)	// parameter max size

#define SXDLY_MAX		0.400							// max delay in seconds
#define SXRVB_MAX		0.100							// max reverb reflection time
#define SXSTE_MAX		0.100							// max stereo delay line time

#define SOUNDCLIP(x)	((x) > (32767*256) ? (32767*256) : ((x) < (-32767*256) ? (-32767*256) : (x)))

#define DSP_CONSTANT_GAIN	128                         // gain

typedef int sample_t;									// delay lines must be 32 bit, now that we have 16 bit samples

#define CSXLPMAX		10								// lowpass filter memory
int rgsxlp[CSXLPMAX];

typedef struct dlyline_s
{
	int cdelaysamplesmax;								// size of delay line in samples
	int lp;												// lowpass flag 0 = off, 1 = on

	int idelayinput;									// i/o indices into circular delay line
	int idelayoutput;
	int idelayoutputxf;									// crossfade output pointer
	int xfade;											// crossfade value

	int delaysamples;									// current delay setting
	int delayfeed;										// current feedback setting

    int     lpf[6];                                     // lowpass filter buffer
    
	int mod;											// sample modulation count
	int modcur;

	sample_t *lpdelayline;								// buffer
} dlyline_t;

#define CSXDLYMAX		4

#define ISXMONODLY		0								// mono delay line
#define ISXRVB			1								// first of the reverb delay lines
#define CSXRVBMAX		2
#define ISXSTEREODLY	3								// 50ms left side delay

dlyline_t rgsxdly[CSXDLYMAX];							// array of delay lines

#define gdly0 (rgsxdly[ISXMONODLY])
#define gdly1 (rgsxdly[ISXRVB])
#define gdly2 (rgsxdly[ISXRVB + 1])
#define gdly3 (rgsxdly[ISXSTEREODLY])

int sxamodl, sxamodr;									// amplitude modulation values
int sxamodlt, sxamodrt;									// modulation targets
int sxmod1, sxmod2;
int sxmod1cur, sxmod2cur;

// Mono Delay parameters
cvar_t sxdly_delay      = { "room_delay",    "0"   };	   // current delay in seconds
cvar_t sxdly_feedback   = { "room_feedback", "0.2" };	   // cyles
cvar_t sxdly_lp			= { "room_dlylp",    "1"   };	   // lowpass filter

float sxdly_delayprev;									   // previous delay setting value

// Mono Reverb parameters
cvar_t sxrvb_size		= {  "room_size",    "0"   };		// room size 0 (off) 0.1 small - 0.35 huge
cvar_t sxrvb_feedback	= {  "room_refl",    "0.7" };		// reverb decay 0.1 short - 0.9 long
cvar_t sxrvb_lp			= {  "room_rvblp",   "1"   };		// lowpass filter

float sxrvb_sizeprev;

// Stereo delay (no feedback)
cvar_t sxste_delay		= {  "room_left",    "0"   };		// straight left delay
float sxste_delayprev;

// Underwater/special fx modulations
cvar_t sxmod_lowpass	= {  "room_lp",      "0"   };
cvar_t sxmod_mod		= {  "room_mod",     "0"   };

// Main interface
cvar_t sxroom_type		= {  "room_type",    "0"   };		// legacy support
cvar_t sxroomwater_type = {  "waterroom_type","15" };	    // legacy support
float sxroom_typeprev;

cvar_t sxroom_off		= {  "room_off",      "0"  };		// legacy support

qboolean SXDLY_Init(int idelay, float delay);
void SXDLY_Free(int idelay);
void SXDLY_DoDelay(int count);
void SXRVB_DoReverb(int count);
void SXDLY_DoStereoDelay(int count);
void SXRVB_DoAMod(int count);

//=====================================================================
// Init/release all structures for sound effects
//=====================================================================

void SX_Init(void)
{

	Q_memset(rgsxdly, 0, sizeof (dlyline_t) * CSXDLYMAX);
	Q_memset(rgsxlp, 0, sizeof(int) * CSXLPMAX);

	sxdly_delayprev = -1.0;
	sxrvb_sizeprev = -1.0;
	sxste_delayprev = -1.0;
	sxroom_typeprev = -1.0;

	// initialize SX cvars
	Cvar_RegisterVariable (&sxdly_delay);
	Cvar_RegisterVariable (&sxdly_feedback);
	Cvar_RegisterVariable (&sxdly_lp);
	Cvar_RegisterVariable (&sxrvb_size);
	Cvar_RegisterVariable (&sxrvb_feedback);
	Cvar_RegisterVariable (&sxrvb_lp);
	Cvar_RegisterVariable (&sxste_delay);
	Cvar_RegisterVariable (&sxmod_lowpass);
	Cvar_RegisterVariable (&sxmod_mod);
	Cvar_RegisterVariable (&sxroom_type);
	Cvar_RegisterVariable (&sxroomwater_type);
	Cvar_RegisterVariable (&sxroom_off);

	// init amplitude modulation params

	sxamodl = sxamodr = 255;
	sxamodlt = sxamodrt = 255;

	sxmod1 = 350 * (shm->speed / 11025);	// 11k was the original sample rate all dsp was tuned at
	sxmod2 = 450 * (shm->speed / 11025);
	sxmod1cur = sxmod1;
	sxmod2cur = sxmod2;

	Con_Printf("FX Processor Initialized\n" );
}

void SX_Free(void)
{
	int i;

	// release mono delay line

	SXDLY_Free(ISXMONODLY);

	// release reverb lines

	for (i = 0; i < CSXRVBMAX; i++)
		SXDLY_Free(i + ISXRVB);

	SXDLY_Free(ISXSTEREODLY);
}

// Set up a delay line buffer allowing a max delay of 'delay' seconds
// Frees current buffer if it already exists. idelay indicates which of
// the available delay lines to init.

qboolean SXDLY_Init(int idelay, float delay)
{
	int   cbsamples;
	byte	*lpData;
	dlyline_t *pdly;

	pdly = &(rgsxdly[idelay]);

	if (delay > SXDLY_MAX)
		delay = SXDLY_MAX;

	if (pdly->lpdelayline)
	{
		free(pdly->lpdelayline);
		pdly->lpdelayline = NULL;
	}

	if (delay == 0.0)
		return true;

	pdly->cdelaysamplesmax = shm->speed * delay;
	pdly->cdelaysamplesmax += 1;

	cbsamples = pdly->cdelaysamplesmax * sizeof (sample_t);
	lpData = malloc(cbsamples);
	if (!lpData)
	{
		Con_Printf("Sound FX: Out of memory.\n");
		return false;
	}
	Q_memset (lpData, 0, cbsamples);
	pdly->lpdelayline = (sample_t *)lpData;
	
	// init delay loop input and output counters.
	// NOTE: init of idelayoutput only valid if pdly->delaysamples is set
	// NOTE: before this call!
	pdly->idelayinput = 0;
	pdly->idelayoutput = pdly->cdelaysamplesmax - pdly->delaysamples;
	pdly->xfade = 0;
	pdly->lp = 1;
	pdly->mod = 0;
	pdly->modcur = 0;

	// init lowpass filter memory
    Q_memset(pdly->lpf, 0, sizeof(pdly->lpf));

	return true;
}

// release delay buffer and deactivate delay

void SXDLY_Free(int idelay)
{
	dlyline_t *pdly = &(rgsxdly[idelay]);
	if (pdly->lpdelayline)
	{
		free(pdly->lpdelayline);
		pdly->lpdelayline = NULL;				// this deactivates the delay
	}
}


// check for new stereo delay param

void SXDLY_CheckNewStereoDelayVal()
{
	dlyline_t *pdly = &(rgsxdly[ISXSTEREODLY]);
	int delaysamples;

	// set up stereo delay

	if (sxste_delay.value != sxste_delayprev)
	{
		if (sxste_delay.value == 0.0)
		{

			// deactivate delay line
			SXDLY_Free(ISXSTEREODLY);
			sxste_delayprev = 0.0;

		}
		else
		{

			delaysamples = fmin(sxste_delay.value, SXSTE_MAX) * shm->speed;

			// init delay line if not active
			if (pdly->lpdelayline == NULL)
			{
				pdly->delaysamples = delaysamples;
				SXDLY_Init(ISXSTEREODLY, SXSTE_MAX);
			}

			// do crossfade to new delay if delay has changed
			if (delaysamples != pdly->delaysamples)
			{
				// set up crossfade from old pdly->delaysamples to new delaysamples
				pdly->idelayoutputxf = pdly->idelayinput - delaysamples;
				if (pdly->idelayoutputxf < 0)
					pdly->idelayoutputxf += pdly->cdelaysamplesmax;

				pdly->xfade = 128;
			}

			sxste_delayprev = sxste_delay.value;

			// UNDONE: modulation disabled
			// FIXED: left chanel modulation
			pdly->mod = 500 * (shm->speed / 11025);		// change delay every n samples
			//pdly->mod = 0;
			pdly->modcur = pdly->mod;

			// deactivate line if rounded down to 0 delay
			if (pdly->delaysamples == 0)
				SXDLY_Free(ISXSTEREODLY);

		}
	}
}

// stereo delay, left channel only, no feedback

void SXDLY_DoStereoDelay(int count)
{
	int left;
	sample_t sampledly;
	sample_t samplexf;
	portable_samplepair_t *pbuf;
	int countr;

	// process delay line if active
	if (rgsxdly[ISXSTEREODLY].lpdelayline)
	{

		pbuf = paintbuffer;
		countr = count;

		// process each sample in the paintbuffer...
		while (countr--)
		{

			if (gdly3.mod && (--gdly3.modcur < 0))
				gdly3.modcur = gdly3.mod;

			// get delay line sample from left line
			sampledly = *(gdly3.lpdelayline  + gdly3.idelayoutput);
			
			left = pbuf->left;

			// only process if left value or delayline value are non-zero or xfading
			if (gdly3.xfade || sampledly || left)
			{
				// if we're not crossfading, and we're not modulating, but we'd like to be modulating,
				// then setup a new crossfade.
				if (!gdly3.xfade && !gdly3.modcur && gdly3.mod)
				{
					// set up crossfade to new delay value, if we're not already doing an xfade
					gdly3.idelayoutputxf = gdly3.idelayoutput +
							((RandomLong(0,0xFF) * gdly3.delaysamples) >> 9); // 100 = ~ 9ms

					if (gdly3.idelayoutputxf >= gdly3.cdelaysamplesmax)
						gdly3.idelayoutputxf -= gdly3.cdelaysamplesmax;

					gdly3.xfade = 128;
				}

				// modify sampledly if crossfading to new delay value

				if (gdly3.xfade)
				{
					samplexf = (*(gdly3.lpdelayline  + gdly3.idelayoutputxf) * (128 - gdly3.xfade)) >> 7;
					sampledly = ((sampledly * gdly3.xfade) >> 7) + samplexf;

					if (++gdly3.idelayoutputxf >= gdly3.cdelaysamplesmax)
						gdly3.idelayoutputxf = 0;

					if (--gdly3.xfade == 0)
					{
						gdly3.idelayoutput = gdly3.idelayoutputxf;
                    }
				}

	            // save output value into delay line
				// left = CLIP(left);
				*(gdly3.lpdelayline + gdly3.idelayinput) = SOUNDCLIP( left );

				// save delay line sample into output buffer
				pbuf->left = SOUNDCLIP( sampledly );

			}
			else
			{
				// keep clearing out delay line, even if no signal in or out
				*(gdly3.lpdelayline  + gdly3.idelayinput) = 0;
			}

			// update delay buffer pointers

			if (++gdly3.idelayinput >= gdly3.cdelaysamplesmax)
				gdly3.idelayinput = 0;

			if (++gdly3.idelayoutput >= gdly3.cdelaysamplesmax)
				gdly3.idelayoutput = 0;

			pbuf++;
		}

	}
}

// If sxdly_delay or sxdly_feedback have changed, update delaysamples
// and delayfeed values.  This applies only to delay 0, the main echo line.

void SXDLY_CheckNewDelayVal()
{
	dlyline_t *pdly = &(rgsxdly[ISXMONODLY]);

	if (sxdly_delay.value != sxdly_delayprev)
	{
		if (sxdly_delay.value == 0.0)
		{

			// deactivate delay line

			SXDLY_Free(ISXMONODLY);
			sxdly_delayprev = sxdly_delay.value;

		}
		else
		{
			// init delay line if not active

			pdly->delaysamples = fmin(sxdly_delay.value, SXDLY_MAX) * shm->speed;

			if (pdly->lpdelayline == NULL)
				SXDLY_Init(ISXMONODLY, SXDLY_MAX);

			// flush delay line and filters

			if (pdly->lpdelayline)
			{
				Q_memset(pdly->lpdelayline,  0, pdly->cdelaysamplesmax * sizeof(sample_t));
				Q_memset(pdly->lpf, 0, sizeof(pdly->lpf));
			}

			// init delay loop input and output counters
			pdly->idelayinput = 0;
			pdly->idelayoutput = pdly->cdelaysamplesmax - pdly->delaysamples;

			sxdly_delayprev = sxdly_delay.value;

			// deactivate line if rounded down to 0 delay

			if (pdly->delaysamples == 0)
				SXDLY_Free(ISXMONODLY);

		}
	}

	pdly->lp = (int)(sxdly_lp.value);
	pdly->delayfeed = sxdly_feedback.value * 255;
}


// This routine updates both left and right output with
// the mono delayed signal.  Delay is set through console vars room_delay
// and room_feedback.

void SXDLY_DoDelay(int count)
{
	int val;
	int val_sum;
	int valt[2];
	int left;
	int right;
	sample_t sampledly;
	portable_samplepair_t *pbuf;
	int countr;
#if 0
	float fgain;
#endif
	int gain;

	Q_memset(valt, 0, sizeof(valt));
	

	// process mono delay line if active
	if (rgsxdly[ISXMONODLY].lpdelayline)
	{

		// calculate gain of delay line with feedback, and use it to
		// reduce output.  ie: make delay line approx unity gain

		// for constant input x with feedback fb:

		// out = x + x*fb + x * fb^2 + x * fb^3...
		// gain = out/x
		// so gain = 1 + fb + fb^2 + fb^3...
		// which, by the miracle of geometric series, equates to 1/1-fb
		// thus, gain = 1/1-fb
#if 0
		fgain = (1.0 / ((255 - (float)gdly0.delayfeed) / (float)255.0 ));
		gain = (int)((1.0 / fgain) * 255);
		gain *= 4;
		gain = fmin( gain, 255 );
		Con_Printf("%s:fgain: %f gain: %i\n",__FUNCTION__, fgain, gain);
#else
        gain = DSP_CONSTANT_GAIN;
#endif
   		pbuf = paintbuffer;
		countr = count;

		// process each sample in the paintbuffer...

		while (countr--)
		{

			// get delay line sample

			sampledly = *(gdly0.lpdelayline  + gdly0.idelayoutput);

			left = pbuf->left;
			right = pbuf->right;

			// only process if delay line and paintbuffer samples are non zero

			if (sampledly || left || right)
			{
				// get current sample from delay buffer
				// calculate delayed value from avg of left and right channels
				val_sum = (left + right) >> 1;
				val  = (gdly0.delayfeed * sampledly) >> 8;

				// limit val to short
				val = SOUNDCLIP(val);
				

				// lowpass
				if (gdly0.lp)
				{
					valt[0] = (gdly0.lpf[0] + gdly0.lpf[1] + gdly0.lpf[2] + gdly0.lpf[3] + val + val_sum) / 5;
					valt[1] = (gdly0.lpf[0] + gdly0.lpf[1] + gdly0.lpf[2] + gdly0.lpf[3] + val) / 5;
					
					gdly0.lpf[0] = gdly0.lpf[1];
					gdly0.lpf[1] = gdly0.lpf[2];
					gdly0.lpf[2] = gdly0.lpf[3];
					gdly0.lpf[3] = val + val_sum;
				}
				else
				{
					valt[0] = val + val_sum;
					valt[1] = val;
				}

				// store delay output value into output buffer
				*(gdly0.lpdelayline  + gdly0.idelayinput) = SOUNDCLIP(valt[0]);

				// mono delay in left and right channels

				// decrease output value by max gain of delay with feedback
				// to provide for unity gain reverb
				// note: this gain varies with the feedback value.
				//left = ((left << 1) + (left << 3)) >> 6;
				//right = ((right << 1) + (right << 3)) >> 6;

		     	int temp[2];
				temp[0] = left  + valt[1];
			    temp[1] = right + valt[1];
			    temp[0] = SOUNDCLIP(temp[0]);
			    temp[1] = SOUNDCLIP(temp[1]);

				pbuf->left = temp[0];
				pbuf->right = temp[1];
			}
			else
			{
				// not playing samples, but must still flush lowpass buffer and delay line
				   gdly0.lpf[0] = gdly0.lpf[1] = gdly0.lpf[2] = gdly0.lpf[3] = 0;
                   valt[0] = valt[1] = 0;
		           *(gdly0.lpdelayline + gdly0.idelayinput) = 0;
			}

			// update delay buffer pointers

			if (++gdly0.idelayinput >= gdly0.cdelaysamplesmax)
				gdly0.idelayinput = 0;

			if (++gdly0.idelayoutput >= gdly0.cdelaysamplesmax)
				gdly0.idelayoutput = 0;

			pbuf++;
		}
	}
}

// Check for a parameter change on the reverb processor

#define RVB_XFADE    (32 *  (shm->speed / 11025))	// xfade time between new delays
#define RVB_MODRATE1 (500 * (shm->speed / 11025))	// how often, in samples, to change delay (1st rvb)
#define RVB_MODRATE2 (700 * (shm->speed / 11025))	// how often, in samples, to change delay (2nd rvb)

void SXRVB_CheckNewReverbVal()
{
	dlyline_t *pdly;
	int delaysamples;
	int i;
	int	mod;

	if (sxrvb_size.value != sxrvb_sizeprev)
	{
		sxrvb_sizeprev = sxrvb_size.value;
		if (sxrvb_size.value == 0.0)
		{
			// deactivate all delay lines
			SXDLY_Free(ISXRVB);
			SXDLY_Free(ISXRVB + 1);
		}
		else
		{
			for(i = ISXRVB; i < ISXRVB + CSXRVBMAX; i++)
			{
				// init delay line if not active

				pdly = &(rgsxdly[i]);

				switch (i)
				{
					case ISXRVB:
						delaysamples = fmin(sxrvb_size.value, SXRVB_MAX) * shm->speed;
						pdly->mod = RVB_MODRATE1;
						break;
					case ISXRVB+1:
						delaysamples = fmin(sxrvb_size.value * 0.71, SXRVB_MAX) * shm->speed;
						pdly->mod = RVB_MODRATE2;
						break;
					default:
						delaysamples = 0;
						break;
				}

				mod = pdly->mod;				// KB: bug, SXDLY_Init clears mod, modcur, xfade and lp - save mod before call

				if (pdly->lpdelayline == NULL)
				{
					pdly->delaysamples = delaysamples;
					SXDLY_Init(i, SXRVB_MAX);
				}

				pdly->modcur = pdly->mod = mod;	// KB: bug, SXDLY_Init clears mod, modcur, xfade and lp - restore mod after call

				// do crossfade to new delay if delay has changed
				if (delaysamples != pdly->delaysamples)
				{
					// set up crossfade from old pdly->delaysamples to new delaysamples
					pdly->idelayoutputxf = pdly->idelayinput - delaysamples;

					if (pdly->idelayoutputxf < 0)
						pdly->idelayoutputxf += pdly->cdelaysamplesmax;

					pdly->xfade = RVB_XFADE;
				}

				// deactivate line if rounded down to 0 delay
				if (pdly->delaysamples == 0)
					SXDLY_Free(i);
			}
		}
	}

	rgsxdly[ISXRVB].delayfeed = (sxrvb_feedback.value) * 255;
	rgsxdly[ISXRVB].lp = sxrvb_lp.value;

	rgsxdly[ISXRVB + 1].delayfeed = (sxrvb_feedback.value) * 255;
	rgsxdly[ISXRVB + 1].lp = sxrvb_lp.value;

}


// main routine for updating the paintbuffer with new reverb values.
// This routine updates both left and right lines with
// the mono reverb signal.  Delay is set through console vars room_reverb
// and room_feedback.  2 reverbs operating in parallel.

void SXRVB_DoReverb(int count)
{
	int val;
	int val_sum;
	int valt[2];
	int left;
	int right;
	sample_t sampledly;
	sample_t samplexf;
	portable_samplepair_t *pbuf;
	int countr;
	int voutm;
#if 0
	float fgain1;
	float fgain2;
#endif
	int gain;
	
	// process reverb lines if active
	if (rgsxdly[ISXRVB].lpdelayline)
	{

#if 0
        // calculate reverb gains
		fgain1 = (1.0 / ((255 - (float)gdly1.delayfeed) / (float)255.0 ));
		fgain2 = (1.0 / ((255 - (float)gdly2.delayfeed) / (float)255.0 )) + fgain1;
		
        // inverse gain of parallel reverbs
		gain = (int)((1.0 / fgain2) * 255);
		gain *= 4;
		gain = fmin( gain, 255);
		
		//Con_Printf("%s: fgain: %f gain: %i\n",__FUNCTION__, fgain2, gain);
#else
        gain = DSP_CONSTANT_GAIN;
#endif

		pbuf = paintbuffer;
		countr = count;

		// process each sample in the paintbuffer...

		while (countr--)
		{

			left = pbuf->left;
			right = pbuf->right;
			val_sum = (left + right) >> 1;
	        voutm = 0;

			// UNDONE: ignored
			if (--gdly1.modcur < 0)
				gdly1.modcur = gdly1.mod;

			// ========================== ISXRVB============================

			// get sample from delay line
			sampledly = *(gdly1.lpdelayline  + gdly1.idelayoutput);

			// only process if something is non-zero
			if (gdly1.xfade || sampledly || left || right)
			{
				// modulate delay rate
				// UNDONE: modulation disabled
				if (0 && !gdly1.xfade && !gdly1.modcur && gdly1.mod)
				{
					// set up crossfade to new delay value, if we're not already doing an xfade
					gdly1.idelayoutputxf = gdly1.idelayoutput +
							((RandomLong(0,0xFF) * gdly1.delaysamples) >> 9); // 100 = ~ 9ms

					if (gdly1.idelayoutputxf >= gdly1.cdelaysamplesmax)
						gdly1.idelayoutputxf -= gdly1.cdelaysamplesmax;

					gdly1.xfade = RVB_XFADE;
				}

				// modify sampledly if crossfading to new delay value

				if (gdly1.xfade)
				{
					samplexf  = (*(gdly1.lpdelayline  + gdly1.idelayoutputxf) * (RVB_XFADE - gdly1.xfade)) / RVB_XFADE;
					sampledly = ((sampledly * gdly1.xfade) / RVB_XFADE) + samplexf;


					if (++gdly1.idelayoutputxf >= gdly1.cdelaysamplesmax)
						gdly1.idelayoutputxf = 0;

					if (--gdly1.xfade == 0)
						gdly1.idelayoutput = gdly1.idelayoutputxf;
				}
				
				if (sampledly)
				{
					// get current sample from delay buffer
					// calculate delayed value from avg of left and right channels
					val  = ((gdly1.delayfeed * sampledly) >> 8);

				    // limit to short
					//val = CLIP(val);
				}
				else
				{
					val = 0;
				}

				// lowpass
				if (gdly1.lp)
				{
					valt[0] = (gdly1.lpf[0] + gdly1.lpf[1] + ((val + val_sum)<<1)) >> 2;
					valt[1] = (gdly1.lpf[0] + gdly1.lpf[1] + ((val)<<1)) >> 2;
					gdly1.lpf[1] = gdly1.lpf[0];
					gdly1.lpf[0] = val + val_sum;
				}
				else
				{
					valt[0] = val + val_sum;
					valt[1] = val;
				}

				// store delay output value into output buffer
			    *(gdly1.lpdelayline  + gdly1.idelayinput) = valt[0];
				voutm = valt[1];
			}
			else
			{
				// not playing samples, but still must flush lowpass buffer & delay line
				gdly1.lpf[0] = gdly1.lpf[1] = 0;
				*(gdly1.lpdelayline + gdly1.idelayinput) = 0;
				voutm = 0;
			}

			// update delay buffer pointers

			if (++gdly1.idelayinput >= gdly1.cdelaysamplesmax)
				gdly1.idelayinput = 0;

			if (++gdly1.idelayoutput >= gdly1.cdelaysamplesmax)
				gdly1.idelayoutput = 0;

			// ========================== ISXRVB + 1========================

			// UNDONE: ignored
			//if (--gdly2.modcur < 0)
			//	gdly2.modcur = gdly2.mod;

			if (gdly2.mod && (--gdly2.modcur < 0))
				gdly2.modcur = gdly2.mod;

			if (gdly2.lpdelayline)
			{
				// get sample from delay line
				sampledly = *(gdly2.lpdelayline + gdly2.idelayoutput);

				// only process if something is non-zero
				if (gdly2.xfade || sampledly || left || right)
				{
					// UNDONE: modulation disabled
					if (0 && !gdly2.xfade && gdly2.modcur && gdly2.mod)
					{
						// set up crossfade to new delay value, if we're not already doing an xfade
						gdly2.idelayoutputxf = gdly2.idelayoutput +
								((RandomLong(0,0xFF) * gdly2.delaysamples) >> 9); // 100 = ~ 9ms


						if (gdly2.idelayoutputxf >= gdly2.cdelaysamplesmax)
							gdly2.idelayoutputxf -= gdly2.cdelaysamplesmax;

						gdly2.xfade = RVB_XFADE;
					}

					// modify sampledly if crossfading to new delay value

					if (gdly2.xfade)
					{
						samplexf = (*(gdly2.lpdelayline  + gdly2.idelayoutputxf) * (RVB_XFADE - gdly2.xfade)) / RVB_XFADE;
						sampledly = ((sampledly * gdly2.xfade) / RVB_XFADE) + samplexf;

						if (++gdly2.idelayoutputxf >= gdly2.cdelaysamplesmax)
							gdly2.idelayoutputxf = 0;

						if (--gdly2.xfade == 0)
							gdly2.idelayoutput = gdly2.idelayoutputxf;
					}

					if (sampledly)
					{
						// get current sample from delay buffer
						// calculate delayed value from avg of left and right channels
						val = (gdly2.delayfeed * sampledly) >> 8;

					    // limit to short
						//val = CLIP(val);
					}
					else
					{
						val = 0;
					}

					// lowpass
					if (gdly2.lp)
					{
						valt[0] = (gdly2.lpf[0] + gdly2.lpf[1] + ((val + val_sum)<<1)) >> 2;
						valt[1] = (gdly2.lpf[0] + gdly2.lpf[1] + (val<<1)) >> 2;
						gdly2.lpf[1] = gdly2.lpf[0];
						gdly2.lpf[0] = val + val_sum;
					}
					else
					{
						valt[0] = val + val_sum;
					    valt[1] = val;
					}

					// store delay output value into output buffer
					*(gdly2.lpdelayline  + gdly2.idelayinput) = valt[0];
					voutm += valt[1];
				}
				else
				{
					// not playing samples, but still must flush lowpass buffer
					gdly2.lpf[0] = gdly2.lpf[1] = 0;
					*(gdly2.lpdelayline  + gdly2.idelayinput) = 0;
				}

				// update delay buffer pointers
				if (++gdly2.idelayinput >= gdly2.cdelaysamplesmax)
					gdly2.idelayinput = 0;

				if (++gdly2.idelayoutput >= gdly2.cdelaysamplesmax)
					gdly2.idelayoutput = 0;
			}


			// ============================ Mix================================
			// add mono delay to left and right channels

			// drop output by inverse of cascaded gain for both reverbs
			int temp[2];
			temp[0] = left  + voutm;
			temp[1] = right + voutm;

			temp[0] = SOUNDCLIP(temp[0]);
			temp[1] = SOUNDCLIP(temp[1]);

			pbuf->left = temp[0];
			pbuf->right = temp[1];

			pbuf++;
		}
	}
}

// amplitude modulator, low pass filter for underwater weirdness

void SXRVB_DoAMod(int count)
{

	int valtl, valtr;
	int left;
	int right;
	portable_samplepair_t *pbuf;
	int countr;
	int fLowpass;
	int fmod;

	// process reverb lines if active

	if (sxmod_lowpass.value != 0.0 || sxmod_mod.value != 0.0)
	{

		pbuf = paintbuffer;
		countr = count;

		fLowpass = (sxmod_lowpass.value != 0.0);
		fmod = (sxmod_mod.value != 0.0);

		// process each sample in the paintbuffer...

		while (countr--)
		{

	        left = pbuf->left;
			right = pbuf->right;

			// only process if non-zero
			if (fLowpass)
			{

				valtl = left;
				valtr = right;

				left = (rgsxlp[0] + rgsxlp[1] + rgsxlp[2] + rgsxlp[3] + rgsxlp[4] + left);
				left = ((left << 1) + (left << 3)) >> 6; 
				
				right = (rgsxlp[5] + rgsxlp[6] + rgsxlp[7]+ rgsxlp[8] + rgsxlp[9] + right);
				right = ((right << 1) + (right << 3)) >> 6;

				rgsxlp[0] = rgsxlp[1];
				rgsxlp[1] = rgsxlp[2];
				rgsxlp[2] = rgsxlp[3];
				rgsxlp[3] = rgsxlp[4];
				rgsxlp[4] = valtl;
				
				rgsxlp[5] = rgsxlp[6];
				rgsxlp[6] = rgsxlp[7];
				rgsxlp[7] = rgsxlp[8];
				rgsxlp[8] = rgsxlp[9];
				rgsxlp[9] = valtr;
			}


			if (fmod)
			{
				if (--sxmod1cur < 0)
					sxmod1cur = sxmod1;

				if (!sxmod1)
					sxamodlt = RandomLong(32,255);

				if (--sxmod2cur < 0)
					sxmod2cur = sxmod2;

				if (!sxmod2)
					sxamodlt = RandomLong(32,255);

				left = (left * sxamodl) >> 8;
				right = (right * sxamodr) >> 8;

				if (sxamodl < sxamodlt)
					sxamodl++;
				else if (sxamodl > sxamodlt)
					sxamodl--;

				if (sxamodr < sxamodrt)
					sxamodr++;
				else if (sxamodr > sxamodrt)
					sxamodr--;
			}

			left = SOUNDCLIP(left);
			right = SOUNDCLIP(right);

			pbuf->left = left;
			pbuf->right = right;

			pbuf++;
		}
	}
}


typedef struct
{
	float room_lp;					// for water fx, lowpass for entire room
	float room_mod;					// stereo amplitude modulation for room

	float room_size;				// reverb: initial reflection size
	float room_refl;				// reverb: decay time
	float room_rvblp;				// reverb: low pass filtering level

	float room_delay;				// mono delay: delay time
	float room_feedback;			// mono delay: decay time
	float room_dlylp;				// mono delay: low pass filtering level

	float room_left;				// left channel delay time
}sx_preset_t;


sx_preset_t rgsxpre[CSXROOM] =
{

// SXROOM_OFF					0

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.0},

// SXROOM_GENERIC				1		// general, low reflective, diffuse room

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.065,	0.1,	0.0,	0.01},

// SXROOM_METALIC_S				2		// highly reflective, parallel surfaces
// SXROOM_METALIC_M				3
// SXROOM_METALIC_L				4

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.02,	0.75,	0.0,	0.01}, // 0.001
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.03,	0.78,	0.0,	0.02}, // 0.002
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.77,	0.0,	0.03}, // 0.003


// SXROOM_TUNNEL_S				5		// resonant reflective, long surfaces
// SXROOM_TUNNEL_M				6
// SXROOM_TUNNEL_L				7

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.05,	0.85,	1.0,	0.018,	0.7,	2.0,	0.01}, // 0.01
	{0.0,	0.0,	0.05,	0.88,	1.0,	0.020,	0.7,	2.0,	0.02}, // 0.02
	{0.0,	0.0,	0.05,	0.92,	1.0,	0.025,	0.7,	2.0,	0.04}, // 0.04

// SXROOM_CHAMBER_S				8		// diffuse, moderately reflective surfaces
// SXROOM_CHAMBER_M				9
// SXROOM_CHAMBER_L				10

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.05,	0.84,	1.0,	0.0,	0.0,	2.0,	0.012}, // 0.003
	{0.0,	0.0,	0.05,	0.90,	1.0,	0.0,	0.0,	2.0,	0.008}, // 0.002
	{0.0,	0.0,	0.05,	0.95,	1.0,	0.0,	0.0,	2.0,	0.004}, // 0.001

// SXROOM_BRITE_S				11		// diffuse, highly reflective
// SXROOM_BRITE_M				12
// SXROOM_BRITE_L				13

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.05,	0.7,	0.0,	0.0,	0.0,	2.0,	0.012}, // 0.003
	{0.0,	0.0,	0.055,	0.78,	0.0,	0.0,	0.0,	2.0,	0.008}, // 0.002
	{0.0,	0.0,	0.05,	0.86,	0.0,	0.0,	0.0,	2.0,	0.002}, // 0.001

// SXROOM_WATER1				14		// underwater fx
// SXROOM_WATER2				15
// SXROOM_WATER3				16

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{1.0,	0.0,	0.0,	0.0,	1.0,	0.0,	0.0,	2.0,	0.01},
	{1.0,	0.0,	0.0,	0.0,	1.0,	0.06,	0.85,	2.0,	0.02},
	{1.0,	0.0,	0.0,	0.0,	1.0,	0.2,	0.6,	2.0,	0.05},

// SXROOM_CONCRETE_S			17		// bare, reflective, parallel surfaces
// SXROOM_CONCRETE_M			18
// SXROOM_CONCRETE_L			19

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.05,	0.8,	1.0,	0.0,	0.48,	2.0,	0.016}, // 0.15 delay, 0.008 left
	{0.0,	0.0,	0.06,	0.9,	1.0,	0.0,	0.52,	2.0,	0.01 }, // 0.22 delay, 0.005 left
	{0.0,	0.0,	0.07,	0.94,	1.0,	0.3,	0.6,	2.0,	0.008}, // 0.001

// SXROOM_OUTSIDE1				20		// echoing, moderately reflective
// SXROOM_OUTSIDE2				21		// echoing, dull
// SXROOM_OUTSIDE3				22		// echoing, very dull

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.3,	0.42,	2.0,	0.0},
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.35,	0.48,	2.0,	0.0},
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.38,	0.6,	2.0,	0.0},

// SXROOM_CAVERN_S				23		// large, echoing area
// SXROOM_CAVERN_M				24
// SXROOM_CAVERN_L				25

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	0.0,	0.05,	0.9,	1.0,	0.2,	0.28,	0.0,	0.0},
	{0.0,	0.0,	0.07,	0.9,	1.0,	0.3,	0.4,	0.0,	0.0},
	{0.0,	0.0,	0.09,	0.9,	1.0,	0.35,	0.5,	0.0,	0.0},

// SXROOM_WEIRDO1				26
// SXROOM_WEIRDO2				27
// SXROOM_WEIRDO3				28
// SXROOM_WEIRDO3				29

//	lp		mod		size	refl	rvblp	delay	feedbk	dlylp	left
	{0.0,	1.0,	0.01,	0.9,	0.0,	0.0,	0.0,	2.0,	0.05},
	{0.0,	0.0,	0.0,	0.0,	1.0,	0.009,	0.999,	2.0,	0.04},
	{0.0,	0.0,	0.001,	0.999,	0.0,	0.2,	0.8,	2.0,	0.05}

};



// force next call to sx_roomfx to reload all room parameters.
// used when switching to/from hires sound mode.

void SX_ReloadRoomFX()
{
	// reset all roomtype parms

	sxroom_typeprev = -1.0;

	sxdly_delayprev = -1.0;
	sxrvb_sizeprev = -1.0;
	sxste_delayprev = -1.0;
	sxroom_typeprev = -1.0;

	// UNDONE: handle sxmod and mod parms?
}


// main routine for processing room sound fx
// if fFilter is TRUE, then run in-line filter (for underwater fx)
// if fTimefx is TRUE, then run reverb and delay fx
// NOTE: only processes preset room_types from 0-29 (CSXROOM)

void SX_RoomFX(int endtime, int fFilter, int fTimefx)
{
	int fReset;
	int i;
	int sampleCount;
	float roomType;

	// return right away if fx processing is turned off

	if (sxroom_off.value)
		return;

	sampleCount = endtime - paintedtime;
	if (sampleCount < 0)
		return;

	fReset = 0;
	
	if (key_dest == key_menu)
		roomType = 0;
	else if (sv.active && sv_player->v.waterlevel > 2 )
		roomType = sxroomwater_type.value;
	else 
	    roomType = sxroom_type.value;

	// only process legacy roomtypes here
	if ( (int)roomType >= CSXROOM )
		return;

	if (roomType != sxroom_typeprev)
	{

		// Msg ("Room_type: %2.1f\n", roomType);

		sxroom_typeprev = roomType;

		i = (int)(roomType);

		if (i < CSXROOM && i >= 0)
		{
			Cvar_SetValue("room_lp", 	   rgsxpre[i].room_lp);
            Cvar_SetValue("room_mod",      rgsxpre[i].room_mod);
            Cvar_SetValue("room_size",     rgsxpre[i].room_size);
            Cvar_SetValue("room_refl",     rgsxpre[i].room_refl);
            Cvar_SetValue("room_rvblp",    rgsxpre[i].room_rvblp);
            Cvar_SetValue("room_delay",    rgsxpre[i].room_delay);
			Cvar_SetValue("room_feedback", rgsxpre[i].room_feedback);
            Cvar_SetValue("room_dlylp",    rgsxpre[i].room_dlylp);
            Cvar_SetValue("room_left",     rgsxpre[i].room_left);
		}

		SXRVB_CheckNewReverbVal();
		SXDLY_CheckNewDelayVal();
		SXDLY_CheckNewStereoDelayVal();

		fReset = 1;
	}

	if ( fReset || roomType != 0.0 )
	{
		// debug code
		SXRVB_CheckNewReverbVal();
		SXDLY_CheckNewDelayVal();
		SXDLY_CheckNewStereoDelayVal();
		// debug code

		if ( fFilter )
			SXRVB_DoAMod(sampleCount);

		if ( fTimefx )
		{
			SXRVB_DoReverb(sampleCount);
			SXDLY_DoDelay(sampleCount);
			SXDLY_DoStereoDelay(sampleCount);
		}
	}
}

