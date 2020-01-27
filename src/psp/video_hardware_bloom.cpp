/*
Copyright (C) 1997-2001 Id Software, Inc.

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
// gl_bloom.c: 2D lighting post process effect
#include <valarray>
#include <vector>

extern"C"
{
#include "../quakedef.h"
}
#include <pspgu.h>
#include <pspgum.h>

/* 
============================================================================== 
 
                        LIGHT BLOOMS
 
============================================================================== 
*/ 

static float Diamond8x[8][8] = 
{ 
        0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.2f, 0.3f, 0.3f, 0.2f, 0.0f, 0.0f, 
        0.0f, 0.2f, 0.4f, 0.6f, 0.6f, 0.4f, 0.2f, 0.0f, 
        0.1f, 0.3f, 0.6f, 0.9f, 0.9f, 0.6f, 0.3f, 0.1f, 
        0.1f, 0.3f, 0.6f, 0.9f, 0.9f, 0.6f, 0.3f, 0.1f, 
        0.0f, 0.2f, 0.4f, 0.6f, 0.6f, 0.4f, 0.2f, 0.0f, 
        0.0f, 0.0f, 0.2f, 0.3f, 0.3f, 0.2f, 0.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f, 0.0f 
};

static float Diamond6x[6][6] = 
{ 
        0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f, 
        0.0f, 0.3f, 0.5f, 0.5f, 0.3f, 0.0f,  
        0.1f, 0.5f, 0.9f, 0.9f, 0.5f, 0.1f, 
        0.1f, 0.5f, 0.9f, 0.9f, 0.5f, 0.1f, 
        0.0f, 0.3f, 0.5f, 0.5f, 0.3f, 0.0f, 
        0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f 
};

static float Diamond4x[4][4] = 
{  
        0.3f, 0.4f, 0.4f, 0.3f,  
        0.4f, 0.9f, 0.9f, 0.4f, 
        0.4f, 0.9f, 0.9f, 0.4f, 
        0.3f, 0.4f, 0.4f, 0.3f 
};

static int      BLOOM_SIZE;

cvar_t	r_bloom					= {"r_bloom",              "0",   qtrue};
cvar_t	r_bloom_alpha			= {"r_bloom_alpha",        "0.3", qtrue};
cvar_t	r_bloom_diamond_size	= {"r_bloom_diamond_size", "8",   qtrue};
cvar_t	r_bloom_intensity		= {"r_bloom_intensity",    "1.3", qtrue};
cvar_t	r_bloom_darken			= {"r_bloom_darken",       "4",   qtrue};
cvar_t	r_bloom_sample_size		= {"r_bloom_sample_size",  "256", qtrue};
cvar_t	r_bloom_fast_sample		= {"r_bloom_fast_sample",  "0",   qtrue};

int r_bloomscreentexture;
int	r_bloomeffecttexture;
int r_bloombackuptexture;
int r_bloomdownsamplingtexture;

static int	r_screendownsamplingtexture_size;
static int  screen_texture_width, screen_texture_height;
//static int  r_screenbackuptexture_size;
static int  r_screenbackuptexture_width, r_screenbackuptexture_height; 

//current refdef size:
static int	curView_x;
static int  curView_y;
static int  curView_width;
static int  curView_height;

//texture coordinates of screen data inside screentexture
static float screenText_tcw;
static float screenText_tch;

static int  sample_width;
static int  sample_height;

//texture coordinates of adjusted textures
static float sampleText_tcw;
static float sampleText_tch;

//this macro is in sample size workspace coordinates
#define R_Bloom_SamplePass( xpos, ypos )              \
	struct vertex                                     \
	{                                                 \
		short			u, v;                         \
		short			x, y, z;                      \
	};                                                \
	vertex* const vertices = static_cast<vertex*>(sceGuGetMemory(sizeof(vertex) * 2)); \
	vertices[0].u		= 0;                          \
	vertices[0].v		= 0;                          \
	vertices[0].x		= xpos;                       \
	vertices[0].y		= ypos;                       \
	vertices[0].z		= 0;                          \
	vertices[1].u		= sample_width;               \
	vertices[1].v		= sample_height;              \
	vertices[1].x		= xpos+sample_width;          \
	vertices[1].y		= ypos+sample_height;         \
	vertices[1].z		= 0;                          \
	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);



    
#define R_Bloom_Quad( _x, _y, _width, _height, _textwidth, _textheight ) \
	struct vertex                                     \
	{                                                 \
		short			u, v;                         \
		short			x, y, z;                      \
	};                                                \
	vertex* const vertices = static_cast<vertex*>(sceGuGetMemory(sizeof(vertex) * 2)); \
	vertices[0].u		= 0;                          \
	vertices[0].v		= 0;                          \
	vertices[0].x		= x;                          \
	vertices[0].y		= y;                          \
	vertices[0].z		= 0;                          \
	vertices[1].u		= _textwidth;                 \
	vertices[1].v		= _textheight;                \
	vertices[1].x		= x + _width;                 \
	vertices[1].y		= y + _height;                \
	vertices[1].z		= 0;                          \
	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);
    
void GL_CAU(int index, const char *identifier, int x, int y, int width, int height)
{
	unsigned char   *data;

    data = (unsigned char*)malloc( width * height * 2 );
    memset( data, 0, width * height * 2 );

	sceGuCopyImage(GU_PSM_5650, x, y, width, height, 512,(void*)((u32)sceGeEdramGetAddr() + frameBuffer), 0, 0, 512, data);
	sceGuTexSync();
	index = GL_LoadTextureLM (identifier, width, height, data, 2, GU_LINEAR, qtrue);
	free(data);
}

/*
=================
R_Bloom_InitBackUpTexture
=================
*/
void R_Bloom_InitBackUpTexture( int width, int height )
{
	unsigned char   *data;
    
    data = (unsigned char*)malloc( width * height * 4 );
    memset( data, 0, width * height * 4 );
 
	r_screenbackuptexture_width  = width;
    r_screenbackuptexture_height = height;

  	r_bloombackuptexture = GL_LoadTextureLM ("bloomtex", width, height, data, 4, GU_LINEAR, qtrue);
    free ( data );
}

/*
=================
R_Bloom_InitEffectTexture
=================
*/
void R_Bloom_InitEffectTexture( void )
{
    unsigned char   *data;
    float   bloomsizecheck;
    
    if( r_bloom_sample_size.value < 32 )
        Cvar_SetValue ("r_bloom_sample_size", 32);

    //make sure bloom size is a power of 2
    BLOOM_SIZE = r_bloom_sample_size.value;
    bloomsizecheck = (float)BLOOM_SIZE;
    while(bloomsizecheck > 1.0f) bloomsizecheck /= 2.0f;
    if( bloomsizecheck != 1.0f )
    {
        BLOOM_SIZE = 32;
        while( BLOOM_SIZE < r_bloom_sample_size.value )
            BLOOM_SIZE *= 2;
    }

    //make sure bloom size doesn't have stupid values
    if( BLOOM_SIZE > screen_texture_width ||
        BLOOM_SIZE > screen_texture_height )
        BLOOM_SIZE = fmin( screen_texture_width, screen_texture_height );

    if( BLOOM_SIZE != r_bloom_sample_size.value )
        Cvar_SetValue ("r_bloom_sample_size", BLOOM_SIZE);

    data = (unsigned char*)malloc( BLOOM_SIZE * BLOOM_SIZE * 4 );
    memset( data, 0, BLOOM_SIZE * BLOOM_SIZE * 4 );

  	r_bloomeffecttexture = GL_LoadTextureLM ("***r_bloomeffecttexture***", BLOOM_SIZE, BLOOM_SIZE, data, 4, GU_LINEAR, qtrue);
    
    free ( data );
}

/*
=================
R_Bloom_InitTextures
=================
*/
void R_Bloom_InitTextures( void )
{
    unsigned char   *data;
    int     size;

    //find closer power of 2 to screen size 
    for (screen_texture_width = 1;screen_texture_width < vid.width;screen_texture_width *= 2);
	for (screen_texture_height = 1;screen_texture_height < vid.height;screen_texture_height *= 2);

    //disable blooms if we can't handle a texture of that size
    if( screen_texture_width > 2048 ||
        screen_texture_height > 2048 ) {
        screen_texture_width = screen_texture_height = 0;
        Cvar_SetValue ("r_bloom", 0);
        Con_Printf( "WARNING: 'R_InitBloomScreenTexture' too high resolution for Light Bloom. Effect disabled\n" );
        return;
    }

    //init the screen texture
    size = screen_texture_width * screen_texture_height * 4;
    data = (unsigned char*)malloc( size );
    memset( data, 255, size );
	r_bloomscreentexture = GL_LoadTextureLM ("***r_screenbackuptexture***", screen_texture_width, screen_texture_height, data, 4, GU_LINEAR, qtrue);
    free ( data );


    //validate bloom size and init the bloom effect texture
    R_Bloom_InitEffectTexture ();

    //if screensize is more than 2x the bloom effect texture, set up for stepped downsampling
    r_bloomdownsamplingtexture = 0;
    r_screendownsamplingtexture_size = 0;
    if( vid.width > (BLOOM_SIZE * 2) && !r_bloom_fast_sample.value )
    {
        r_screendownsamplingtexture_size = (int)(BLOOM_SIZE * 2);
        data = (unsigned char*)malloc( r_screendownsamplingtexture_size * r_screendownsamplingtexture_size * 4 );
        memset( data, 0, r_screendownsamplingtexture_size * r_screendownsamplingtexture_size * 4 );
		r_bloomdownsamplingtexture = GL_LoadTextureLM ("***r_bloomdownsamplingtexture***", r_screendownsamplingtexture_size, r_screendownsamplingtexture_size, data, 4, GU_LINEAR, qtrue);
		free ( data );
    }

    //Init the screen backup texture
    if( r_screendownsamplingtexture_size )
        R_Bloom_InitBackUpTexture( r_screendownsamplingtexture_size, r_screendownsamplingtexture_size );
    else
        R_Bloom_InitBackUpTexture( BLOOM_SIZE, BLOOM_SIZE );
}

/*
=================
R_InitBloomTextures
=================
*/
void R_InitBloomTextures( void )
{
    Cvar_RegisterVariable (&r_bloom);
	Cvar_RegisterVariable (&r_bloom_darken);
	Cvar_RegisterVariable (&r_bloom_alpha);
	Cvar_RegisterVariable (&r_bloom_diamond_size);
	Cvar_RegisterVariable (&r_bloom_intensity);
	Cvar_RegisterVariable (&r_bloom_sample_size);
	Cvar_RegisterVariable (&r_bloom_fast_sample);
	
    BLOOM_SIZE = 0;
    if( !r_bloom.value )
        return;

	if (!qmb_initialized)
		return;	

    R_Bloom_InitTextures ();
}


/*
=================
R_Bloom_DrawEffect
=================
*/
void R_Bloom_DrawEffect( void )
{
    GL_Bind(r_bloomeffecttexture);

    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_ONE_MINUS_DST_ALPHA, GU_ONE_MINUS_SRC_COLOR, 0, 0);

    sceGuColor(GU_COLOR(r_bloom_alpha.value, r_bloom_alpha.value, r_bloom_alpha.value, 0.8f));
	sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGB);

	struct vertex
	{
		short u, v;
		short x, y, z;
	};

	vertex* const vertices = static_cast<vertex*>(sceGuGetMemory(sizeof(vertex) * 2));
	vertices[0].u = 0;
	vertices[0].v = 0;
	vertices[0].x = curView_x;
	vertices[0].y = curView_y;
	vertices[0].z = 0;

	vertices[1].u = curView_width;
	vertices[1].v = curView_height;
	vertices[1].x = curView_x + curView_width;
	vertices[1].y = curView_y + curView_height;
	vertices[1].z = 0;

	sceGuDrawArray(GU_SPRITES, GU_TEXTURE_16BIT | GU_VERTEX_16BIT | GU_TRANSFORM_2D, 2, 0, vertices);

	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuDisable(GU_BLEND);
}

/*
=================
R_Bloom_GeneratexDiamonds
=================
*/
void R_Bloom_GeneratexDiamonds( void )
{

    int         i, j;
    static float intensity;

    //set up sample size workspace
    sceGuViewport( 0, 0, sample_width, sample_height );

    sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();

    sceGumOrtho(0, sample_width, sample_height, 0, -10, 100);

    sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity ();

    //copy small scene into r_bloomeffecttexture
	GL_CAU(r_bloomeffecttexture, "***r_bloomeffecttexture***", 0, 0, sample_width, sample_height);

    //start modifying the small scene corner
    sceGuColor(GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ));
    sceGuEnable(GU_BLEND);

    //darkening passes
    if( r_bloom_darken.value )
    {
        sceGuBlendFunc(GU_ADD, GU_DST_COLOR, GU_DST_ALPHA, 0, 0);
        sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGB);

	    for(i=0; i<r_bloom_darken.value ;i++) 
		{
            R_Bloom_SamplePass( 0, 0 );
        }
        GL_CAU(r_bloomeffecttexture, "***r_bloomeffecttexture***", 0, 0, sample_width, sample_height);
  }

    //bluring passes
	sceGuBlendFunc(GU_ADD, GU_ONE_MINUS_DST_ALPHA, GU_ONE_MINUS_SRC_COLOR, 0, 0);
    if( r_bloom_diamond_size.value > 7 || r_bloom_diamond_size.value <= 3)
    {
        if( r_bloom_diamond_size.value != 8 ) Cvar_SetValue( "r_bloom_diamond_size", 8 );

        for(i=0; i<r_bloom_diamond_size.value; i++) 
		{
            for(j=0; j<r_bloom_diamond_size.value; j++) 
			{
				if (cl.viewent.model->name == NULL)
					intensity = r_bloom_intensity.value * 0.1 * Diamond8x[i][j];
				else
					intensity = r_bloom_intensity.value * 0.3 * Diamond8x[i][j];
                if( intensity < 0.01f ) continue;
                sceGuColor(GU_COLOR( intensity, intensity, intensity, 1.0));
                R_Bloom_SamplePass( i-4, j-4 );
            }
        }
    } 
	else
	if( r_bloom_diamond_size.value > 5 )
	{
        
        if( r_bloom_diamond_size.value != 6 ) Cvar_SetValue( "r_bloom_diamond_size", 6 );

        for(i=0; i<r_bloom_diamond_size.value; i++) 
		{
            for(j=0; j<r_bloom_diamond_size.value; j++) 
			{
				if (cl.viewent.model->name == NULL)
					intensity = r_bloom_intensity.value * 0.1 * Diamond8x[i][j];
				else
					intensity = r_bloom_intensity.value * 0.5 * Diamond6x[i][j];
                if( intensity < 0.01f ) continue;
                sceGuColor(GU_COLOR( intensity, intensity, intensity, 1.0));
                R_Bloom_SamplePass( i-3, j-3 );
            }
        }
    } 
	else
	if( r_bloom_diamond_size.value > 3 )
	{
		if( r_bloom_diamond_size.value != 4 ) Cvar_SetValue( "r_bloom_diamond_size", 4 );

        for(i=0; i<r_bloom_diamond_size.value; i++) {
            for(j=0; j<r_bloom_diamond_size.value; j++) {
				
				if (cl.viewent.model->name == NULL)
					intensity = r_bloom_intensity.value * 0.1 * Diamond8x[i][j];
				else
					intensity = r_bloom_intensity.value * 0.8f * Diamond4x[i][j];
                if( intensity < 0.01f ) continue;
                sceGuColor(GU_COLOR( intensity, intensity, intensity, 1.0));
                R_Bloom_SamplePass( i-2, j-2 );
            }
        }
    }
    GL_CAU(r_bloomeffecttexture, "***r_bloomeffecttexture***", 0, 0, sample_width, sample_height);

    sceGuViewport( 0, 0, vid.width, vid.height );
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();

    sceGumOrtho(0, vid.width, vid.height, 0, -10, 100);

    sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity ();
}                                           

/*
=================
R_Bloom_DownsampleView
=================
*/
void R_Bloom_DownsampleView( void )
{
    sceGuDisable( GU_BLEND );
    sceGuColor(GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ));

    //stepped downsample
    if( r_screendownsamplingtexture_size )
    {
        int     midsample_width = r_screendownsamplingtexture_size * sampleText_tcw;
        int     midsample_height = r_screendownsamplingtexture_size * sampleText_tch;
        
        //copy the screen and draw resized
        GL_Bind(r_bloomscreentexture);
		GL_CAU(r_bloomscreentexture, "***r_bloomscreentexture***", curView_x, vid.height - (curView_y + curView_height), curView_width, curView_height);
        R_Bloom_Quad( 0,  vid.height-midsample_height, midsample_width, midsample_height, screenText_tcw, screenText_tch  );
        
        //now copy into Downsampling (mid-sized) texture
        GL_Bind(r_bloomdownsamplingtexture);
        GL_CAU(r_bloomdownsamplingtexture, "***r_bloomdownsamplingtexture***", 0, 0, midsample_width, midsample_height);
       
	    //now draw again in bloom size
        sceGuColor(GU_COLOR( 0.5f, 0.5f, 0.5f, 1.0f ));
        R_Bloom_Quad( 0,  vid.height-sample_height, sample_width, sample_height, sampleText_tcw, sampleText_tch );
        
        //now blend the big screen texture into the bloom generation space (hoping it adds some blur)
        sceGuEnable( GU_BLEND );
        sceGuBlendFunc(GU_ADD, GU_ONE_MINUS_DST_ALPHA, GU_ONE_MINUS_DST_ALPHA, 0, 0);
        sceGuColor(GU_COLOR( 0.5f, 0.5f, 0.5f, 1.0f ));
        GL_Bind(r_bloomscreentexture);
        R_Bloom_Quad( 0,  vid.height-sample_height, sample_width, sample_height, screenText_tcw, screenText_tch );
        sceGuColor4f(GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f ));
        sceGuDisable( GU_BLEND );

    }
	else
	{    //downsample simple

        GL_Bind(r_bloomscreentexture);
        GL_CAU(r_bloomscreentexture, "***r_bloomscreentexture***", curView_x, vid.height - (curView_y + curView_height), curView_width, curView_height);
        R_Bloom_Quad( 0, vid.height-sample_height, sample_width, sample_height, screenText_tcw, screenText_tch );
    }
}

/*
=================
R_BloomBlend
=================
*/
void R_BloomBlend (int bloom)
{
    if( !bloom || !r_bloom.value )
        return;

    if( !BLOOM_SIZE )
        R_Bloom_InitTextures();

    if( screen_texture_width < BLOOM_SIZE ||
        screen_texture_height < BLOOM_SIZE )
        return;

    //set up full screen workspace
    sceGuViewport( 0, 0, vid.width, vid.height );
    sceGuDisable( GU_DEPTH_TEST );
    sceGumMatrixMode( GU_PROJECTION );
    sceGumLoadIdentity ();
    sceGumOrtho(0, vid.width, vid.height, 0, -10, 100);
    sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
	sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity ();
    sceGuDisable(GU_CULL_FACE);

    sceGuDisable( GU_BLEND );
    sceGuEnable( GU_TEXTURE_2D );

    sceGuColor(GU_COLOR( 1, 1, 1, 1 ));

    //set up current sizes
	curView_x = 0;
	curView_y = 0;
    curView_width = vid.width;
    curView_height = vid.height;

    screenText_tcw = screen_texture_width;
    screenText_tch = screen_texture_height;

    if( vid.height > vid.width )
	{
        //sampleText_tcw = vid.height;
	    screenText_tcw = screen_texture_width;
		screenText_tch = screen_texture_height;
        sampleText_tch = 1.0f;
    } 
	else 
	{
        sampleText_tcw = 1.0f;
        sampleText_tch = vid.width;
    }
    sample_width = BLOOM_SIZE;
    sample_height = BLOOM_SIZE;
    //copy the screen space we'll use to work into the backup texture
	GL_Bind(r_bloombackuptexture);
    GL_CAU(r_bloombackuptexture, "***r_bloombackuptexture***", 0, 0, r_screenbackuptexture_width, r_screenbackuptexture_height);

     //create the bloom image
	R_Bloom_DownsampleView();
    R_Bloom_GeneratexDiamonds();

    //restore the screen-backup to the screen
    sceGuDisable(GU_BLEND);
	GL_Bind(r_bloombackuptexture);
    sceGuColor(GU_COLOR( 1, 1, 1, 1 ));

    R_Bloom_Quad( 0, vid.height - r_screenbackuptexture_height, r_screenbackuptexture_width, r_screenbackuptexture_height, 1.0, 1.0);
    R_Bloom_DrawEffect();
}
