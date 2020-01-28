
#include "../quakedef.h"
#include <malloc.h>
#include <pspgu.h>

extern model_t	*loadmodel;
extern char	 *loadname[32];
/*
=================
Mod_LoadSpriteFrame
=================
*/
dspriteframetype_t* Mod_LoadHLSpriteFrame (void * pin, mspriteframe_t **ppframe, int framenum)
{
	dspriteframe_t		*pinframe;
	mspriteframe_t		*pspriteframe;
	int					width, height, size, origin[2];
	char				name[64];

 printf("<-line: %i\n",__LINE__);

	pinframe = (dspriteframe_t *)pin;

	width = LittleLong (pinframe->width);
	height = LittleLong (pinframe->height);
	size = width * height;
	printf("HL width %i height %i\n", width, height);

	pspriteframe = Hunk_AllocName (sizeof (mspriteframe_t),loadname);
	Q_memset (pspriteframe, 0, sizeof (mspriteframe_t));

	*ppframe = pspriteframe;

	pspriteframe->width = width;
	pspriteframe->height = height;
	origin[0] = LittleLong (pinframe->origin[0]);
	origin[1] = LittleLong (pinframe->origin[1]);

	pspriteframe->up = origin[1];
	pspriteframe->down = origin[1] - height;
	pspriteframe->left = origin[0];
	pspriteframe->right = width + origin[0];

	sprintf (name, "%s_%i", loadmodel->name, framenum);

	pspriteframe->gl_texturenum = GL_LoadPaletteTexture (name, width, height, (byte *)(pinframe + 1), (byte*)host_basepal, PAL_RGB, true, GU_LINEAR, 0);

	return (dspriteframetype_t*)((byte *)(pinframe + 1) + size);
}


/*
=================
Mod_LoadSpriteGroup
=================
*/
dspriteframetype_t* Mod_LoadHLSpriteGroup (void * pin, mspriteframe_t **ppframe, int framenum)
{
	dspritegroup_t		*pingroup;
	mspritegroup_t		*pspritegroup;
	int					i, numframes;
	dspriteinterval_t	*pin_intervals;
	float				*poutintervals;
	void		*ptemp;

    printf("<-line: %i\n",__LINE__);

	pingroup = (dspritegroup_t *)pin;


	numframes = LittleLong (pingroup->numframes);
	printf("numframes: %i\n",numframes);

	pspritegroup = Hunk_AllocName (sizeof (mspritegroup_t) +
				(numframes - 1) * sizeof (pspritegroup->frames[0]), loadname);


	pspritegroup->numframes = numframes;

   	*ppframe = (mspriteframe_t *)pspritegroup;

	pin_intervals = (dspriteinterval_t *)(pingroup + 1);

	poutintervals = Hunk_AllocName (numframes * sizeof (float), loadname);

	pspritegroup->intervals = poutintervals;

	for (i=0 ; i<numframes ; i++)
	{
		*poutintervals = LittleFloat (pin_intervals->interval);
/*
		if (*poutintervals <= 0.0)
		{
			Sys_Error ("Mod_LoadSpriteGroup: interval<=0");
		}
*/
		if( *poutintervals <= 0.0f )
			*poutintervals = 1.0f; // set error value

		poutintervals++;
		pin_intervals++;
	}

	ptemp = (void *)pin_intervals;

	for (i=0 ; i<numframes ; i++)
	{
		ptemp = Mod_LoadHLSpriteFrame (ptemp, &pspritegroup->frames[i], framenum * 100 + i);
	}

	return (dspriteframetype_t*)ptemp;
}

/*
=================
Mod_LoadSpriteModel
=================
*/
void Mod_LoadHLSpriteModel (model_t *mod, dspritehl_t   *pin)
{
	int					i;
	msprite_t		    *psprite;
	int					numframes;
	int					size;
	dspriteframetype_t	*pframetype;
	int                 sptype;
	short               *numi;
	unsigned char pal[256*4];

	printf("HL_MODEL\n");

    sptype = LittleLong (pin->type);
	numframes = LittleLong (pin->numframes);
	size = sizeof (msprite_t) +	(numframes - 1) * sizeof (psprite->frames) + 770;

	psprite = Hunk_AllocName (size, loadname);
    mod->cache.data = psprite;
	psprite->type = sptype;
	mod->synctype = LittleLong (pin->synctype);

	psprite->numframes = numframes;
    psprite->maxwidth = LittleLong (pin->width);


    psprite->maxheight = LittleLong (pin->height);
    psprite->beamlength = LittleFloat (pin->facetype);

	mod->mins[0] = mod->mins[1] = -psprite->maxwidth/2;
	mod->maxs[0] = mod->maxs[1] = psprite->maxwidth/2;
	mod->mins[2] = -psprite->maxheight/2;
	mod->maxs[2] = psprite->maxheight/2;

    numi = (short *)(pin+1);
    byte *src = (numi + 1);

	if (LittleShort(*numi) != 256)
	{
		Host_Error("%s has wrong number of palette indexes (we only support 256)\n", mod->name);
	}

	for (i = 0; i < 256; i++)
	{
		pal[i*4+0] = *src++;
		pal[i*4+1] = *src++;
		pal[i*4+2] = *src++;
		pal[i*4+3] = 255;
	}

	pframetype = (dspriteframetype_t *)(src);


//
// load the frames
//

	if (numframes < 1)
	{
		Con_Printf ("Mod_LoadSpriteModel: Invalid # of frames: %d\n", numframes);
	}

	mod->numframes = numframes;

	for (i=0 ; i<numframes ; i++)
	{
		spriteframetype_t	frametype;

		frametype = LittleFloat(pframetype->type);
		psprite->frames[i].type = frametype;

		if (frametype == SPR_SINGLE)
		{
			pframetype = Mod_LoadHLSpriteFrame (pframetype + 1, &psprite->frames[i].frameptr, i);
		}
		else
		{
			pframetype = Mod_LoadHLSpriteGroup (pframetype + 1, &psprite->frames[i].frameptr, i);
		}
		if(pframetype == NULL)
		   break;
	}
	mod->type = mod_sprite;
}
