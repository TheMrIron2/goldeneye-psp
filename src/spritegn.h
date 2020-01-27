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
//
// spritegn.h: header file for sprite generation program
//

// **********************************************************
// * This file must be identical in the spritegen directory *
// * and in the Quake directory, because it's used to       *
// * pass data from one to the other via .spr files.        *
// **********************************************************

//-------------------------------------------------------
// This program generates .spr sprite package files.
// The format of the files is as follows:
//
// dsprite_t file header structure
// <repeat dsprite_t.numframes times>
//   <if spritegroup, repeat dspritegroup_t.numframes times>
//     dspriteframe_t frame header structure
//     sprite bitmap
//   <else (single sprite frame)>
//     dspriteframe_t frame header structure
//     sprite bitmap
// <endrepeat>
//-------------------------------------------------------

#ifdef INCLUDELIBS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "cmdlib.h"
#include "scriplib.h"
#include "dictlib.h"
#include "trilib.h"
#include "lbmlib.h"
#include "mathlib.h"

#endif

#define SPRITE_VERSION	    1               // Quake     sprites
#define SPRITE32_VERSION    32

// must match definition in modelgen.h
#ifndef SYNCTYPE_T
#define SYNCTYPE_T

typedef enum
{
	ST_SYNC=0,
	ST_RAND

} synctype_t;
#endif

// TODO: shorten these?
typedef struct
{
	int			ident;
	int			version;
	int         type;
	float		boundingradius;
	int			width;
	int			height;
	int			numframes;
	float		beamlength;
	synctype_t	synctype;
} dsprite_t;

enum
{
	SPR_VP_PARALLEL_UPRIGHT = 0,
	SPR_FACING_UPRIGHT,
	SPR_VP_PARALLEL,
	SPR_ORIENTED,
	SPR_VP_PARALLEL_ORIENTED,
	SPR_LABEL,
	SPR_LABEL_SCALE,
};

typedef struct {
	int			origin[2];
	int			width;
	int			height;
} dspriteframe_t;


typedef struct {
	int			numframes;
} dspritegroup_t;

typedef struct {
	float	interval;
} dspriteinterval_t;

typedef enum
{
	SPR_SINGLE=0,
	SPR_GROUP
} spriteframetype_t;


typedef struct {
	spriteframetype_t	type;
} dspriteframetype_t;

//HL
#define HLSPRITE_VERSION	2				// Half-Life sprites

enum
{
	SPR_NORMAL = 0,
	SPR_ADDITIVE,
	SPR_INDEXALPHA,
	SPR_ALPHTEST,
};

enum
{
	SPR_CULL_FRONT = 0,			// oriented sprite will be draw with one face
	SPR_CULL_NONE,			// oriented sprite will be draw back face too
};

typedef struct
{
	int		        ident;		// LittleLong 'ISPR'
	int		        version;		// current version 2
	int	            type;		// camera align
	int	            texFormat;	// rendering mode
	int		        boundingradius;	// quick face culling
	int			    width;
	int			    height;
	int		        numframes;	// including groups
	int	            facetype;		// cullface (Xash3D ext)
	synctype_t	    synctype;		// animation synctype
} dspritehl_t;
//HL


#define IDSPRITEHEADER	(('P'<<24)+('S'<<16)+('D'<<8)+'I')
														// little-endian "IDSP"
#define IDSPRITE2HEADER	(('2'<<24)+('S'<<16)+('D'<<8)+'I')
		                                                // little-endian "IDS2"
#define SPRITE2_VERSION	2
#define MAX_SKINNAME 64

typedef struct
{
	int		width, height;
	int		origin_x, origin_y;		// raster coordinates inside pic
	char	name[MAX_SKINNAME];		// name of pcx file
} dmd2sprframe_t;

typedef struct {
	int			ident;
	int			version;
	int			numframes;
	dmd2sprframe_t	frames[1];			// variable sized
} dmd2sprite_t;

