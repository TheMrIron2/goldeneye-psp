/*
Copyright (C) 1996-1997 Id Software, Inc.
Copyright (C) 2007 Peter Mackay and Chris Swindle.

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
// gl_warp.c -- sky and water polygons

#include <pspgu.h>

extern "C"
{
#include "../quakedef.h"
}

#include "clipping.hpp"

using namespace quake;

extern	model_t	*loadmodel;

/*int		skytexturenum;*/

int		solidskytexture	= -1;
int		alphaskytexture	= -1;
float	speedscale;		// for top sky and bottom sky

int	    skytexorder[6] = {0,2,1,3,4,5};
int	    skyimage[6]; // Where sky images are stored
char	skybox_name[32] = ""; //name of current skybox, or "" if no skybox
char	*suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};

msurface_t	*warpface;

extern cvar_t gl_subdivide_size;

static void BoundPoly (int numverts, float *verts, vec3_t mins, vec3_t maxs)
{
	int		i, j;
	float	*v;

	mins[0] = mins[1] = mins[2] = 9999;
	maxs[0] = maxs[1] = maxs[2] = -9999;
	v = verts;
	for (i=0 ; i<numverts ; i++)
		for (j=0 ; j<3 ; j++, v++)
		{
			if (*v < mins[j])
				mins[j] = *v;
			if (*v > maxs[j])
				maxs[j] = *v;
		}
}

static void SubdividePolygon (int numverts, float *verts)
{
	int		i, j, k;
	vec3_t	mins, maxs;
	float	m;
	float	*v;
	vec3_t	front[64], back[64];
	int		f, b;
	float	dist[64];
	float	frac;
	glpoly_t	*poly;
	float	s, t;

	if (numverts > 60)
		Sys_Error ("numverts = %i", numverts);

	BoundPoly (numverts, verts, mins, maxs);

	for (i=0 ; i<3 ; i++)
	{
		m = (mins[i] + maxs[i]) * 0.5;
		m = gl_subdivide_size.value * floorf (m/gl_subdivide_size.value + 0.5);
		if (maxs[i] - m < 8)
			continue;
		if (m - mins[i] < 8)
			continue;

		// cut it
		v = verts + i;
		for (j=0 ; j<numverts ; j++, v+= 3)
			dist[j] = *v - m;

		// wrap cases
		dist[j] = dist[0];
		v-=i;
		VectorCopy (verts, v);

		f = b = 0;
		v = verts;
		for (j=0 ; j<numverts ; j++, v+= 3)
		{
			if (dist[j] >= 0)
			{
				VectorCopy (v, front[f]);
				f++;
			}
			if (dist[j] <= 0)
			{
				VectorCopy (v, back[b]);
				b++;
			}
			if (dist[j] == 0 || dist[j+1] == 0)
				continue;
			if ( (dist[j] > 0) != (dist[j+1] > 0) )
			{
				// clip point
				frac = dist[j] / (dist[j] - dist[j+1]);
				for (k=0 ; k<3 ; k++)
					front[f][k] = back[b][k] = v[k] + frac*(v[3+k] - v[k]);
				f++;
				b++;
			}
		}

		SubdividePolygon (f, front[0]);
		SubdividePolygon (b, back[0]);
		return;
	}

	poly = static_cast<glpoly_t*>(Hunk_Alloc (sizeof(glpoly_t) + (numverts - 1) * sizeof(glvert_t)));
	poly->next = warpface->polys;
	warpface->polys = poly;
	poly->numverts = numverts;
	for (i=0 ; i<numverts ; i++, verts+= 3)
	{
		VectorCopy (verts, poly->verts[i].xyz);
		s = DotProduct (verts, warpface->texinfo->vecs[0]);
		t = DotProduct (verts, warpface->texinfo->vecs[1]);
		poly->verts[i].st[0] = s;
		poly->verts[i].st[1] = t;
	}
}

/*
================
GL_SubdivideSurface

Breaks a polygon up along axial 64 unit
boundaries so that turbulent and sky warps
can be done reasonably.
================
*/
void GL_SubdivideSurface (msurface_t *fa)
{
	vec3_t		verts[64];
	int			numverts;
	int			i;
	int			lindex;
	float		*vec;

	warpface = fa;

	//
	// convert edges back to a normal polygon
	//
	numverts = 0;
	for (i=0 ; i<fa->numedges ; i++)
	{
		lindex = loadmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
			vec = loadmodel->vertexes[loadmodel->edges[lindex].v[0]].position;
		else
			vec = loadmodel->vertexes[loadmodel->edges[-lindex].v[1]].position;
		VectorCopy (vec, verts[numverts]);
		numverts++;
	}

	SubdividePolygon (numverts, verts[0]);
}

/*
================
GL_Surface
================
*/
void GL_Surface (msurface_t *fa)
{
	vec3_t		verts[64];
	int			numverts;
	int			i;
	int			lindex;
	float		*vec;
	glpoly_t	*poly;
//	float		texscale;
	float	s, t;

//	texscale = (1.0/32.0);

	//
	// convert edges back to a normal polygon
	//
	numverts = 0;
	for (i=0 ; i<fa->numedges ; i++)
	{
		lindex = loadmodel->surfedges[fa->firstedge + i];

		if (lindex > 0)
			vec = loadmodel->vertexes[loadmodel->edges[lindex].v[0]].position;
		else
			vec = loadmodel->vertexes[loadmodel->edges[-lindex].v[1]].position;
		VectorCopy (vec, verts[numverts]);
		numverts++;
	}

	//create the poly
	poly = static_cast<glpoly_t*>(Hunk_Alloc (sizeof(glpoly_t) + (numverts - 1) * sizeof(glvert_t)));
	poly->next = NULL;
	fa->polys = poly;
	poly->numverts = numverts;
	for (i=0, vec=(float *)verts; i<numverts; i++, vec+= 3)
	{
		VectorCopy (vec, poly->verts[i].xyz);
		s = DotProduct(vec, fa->texinfo->vecs[0]);// * texscale;
		t = DotProduct(vec, fa->texinfo->vecs[1]);// * texscale;
		poly->verts[i].st[0] = s;
		poly->verts[i].st[1] = t;
	}
}

//=========================================================



// speed up sin calculations - Ed
float	turbsin[] =
{
	#include "../gl_warp_sin.h"
};

/*
=============
EmitWaterPolys

Does a water warp on the pre-fragmented glpoly_t chain
=============
*/
void EmitWaterPolys (msurface_t *fa)
{
	const float real_time	= static_cast<float>(realtime);
	const float scale		= (1.0f / 64);
	const float turbscale	= (256.0f / (2.0f * static_cast<float>(M_PI)));

	// For each polygon...
	for (const glpoly_t* p = fa->polys; p; p = p->next)
	{
		// Allocate memory for this polygon.
		const int		unclipped_vertex_count	= p->numverts;
		glvert_t* const	unclipped_vertices		=
			static_cast<glvert_t*>(sceGuGetMemory(sizeof(glvert_t) * unclipped_vertex_count));

		// Generate each vertex.
		const glvert_t*	src			= p->verts;
		const glvert_t*	last_vertex = src + unclipped_vertex_count;
		glvert_t*		dst			= unclipped_vertices;
		while (src != last_vertex)
		{
			// Get the input UVs.
			const float	os = src->st[0];
			const float	ot = src->st[1];

			// Fill in the vertex data.
			dst->st[0] = (os + turbsin[(int) ((ot * 0.125f + real_time) * turbscale) & 255]) * scale;
			dst->st[1] = (ot + turbsin[(int) ((os * 0.125f + real_time) * turbscale) & 255]) * scale;
			dst->xyz[0] = src->xyz[0];
			dst->xyz[1] = src->xyz[1];
			dst->xyz[2] = src->xyz[2];

			// Next vertex.
			++src;
			++dst;
		}

		// Do these vertices need clipped?
		if (clipping::is_clipping_required(unclipped_vertices, unclipped_vertex_count))
		{
			// Clip the polygon.
			const glvert_t*	clipped_vertices;
			std::size_t		clipped_vertex_count;
			clipping::clip(
				unclipped_vertices,
				unclipped_vertex_count,
				&clipped_vertices,
				&clipped_vertex_count);

			// Any vertices left?
			if (clipped_vertex_count)
			{
				// Copy the vertices to the display list.
				const std::size_t buffer_size = clipped_vertex_count * sizeof(glvert_t);
				glvert_t* const display_list_vertices = static_cast<glvert_t*>(sceGuGetMemory(buffer_size));
				memcpy(display_list_vertices, clipped_vertices, buffer_size);

				// Draw the clipped vertices.
				sceGuDrawArray(
					GU_TRIANGLE_FAN,
					GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
					clipped_vertex_count, 0, display_list_vertices);
			}
		}
		else
		{
			// Draw the vertices.
			sceGuDrawArray(
				GU_TRIANGLE_FAN,
				GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
				unclipped_vertex_count, 0, unclipped_vertices);
		}
	}
}




/*
=============
EmitSkyPolys
=============
*/
void EmitSkyPolys (msurface_t *fa)
{
	for (const glpoly_t* p = fa->polys; p; p = p->next)
	{
		glvert_t* const vertices = static_cast<glvert_t*>(sceGuGetMemory(sizeof(glvert_t) * p->numverts));

		const glvert_t*	src			= p->verts;
		const glvert_t*	last_vertex = src + p->numverts;
		glvert_t*		dst			= vertices;
		while (src != last_vertex)
		{
			vec3_t	dir;
			VectorSubtract(src->xyz, r_origin, dir);
			dir[2] *= 3;	// flatten the sphere

			const float length = 6 * 63 / sqrtf(DotProduct(dir, dir));

			dir[0] *= length;
			dir[1] *= length;

			dst->st[0] = (speedscale + dir[0]) * (1.0f / 128.0f);
			dst->st[1] = (speedscale + dir[1]) * (1.0f / 128.0f);
			dst->xyz[0] = src->xyz[0];
			dst->xyz[1] = src->xyz[1];
			dst->xyz[2] = src->xyz[2];

			// Next vertex.
			++src;
			++dst;
		}
		
		sceGuDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF, p->numverts, 0, vertices);
	}
}

/*
=============
EmitScrollPolys
=============
*/
void EmitScrollPolys (msurface_t *fa)
{
    const float real_time	= static_cast<float>(realtime);
	entity_t	*e = currententity;
	float		flConveyorSpeed;
	float		flRate, flAngle;
	float		sOffset, sy;
	float		tOffset, cy;

	flConveyorSpeed = ((int(e->rendercolor[1])<<8)|(int(e->rendercolor[2]))) / 16.0f;
	if( e->rendercolor[0] ) flConveyorSpeed = -flConveyorSpeed;

	flRate = fabs( flConveyorSpeed ) / fa->texinfo->texture->width;
	flAngle = ( flConveyorSpeed >= 0 ) ? 180 : 0;

	SinCos( flAngle * ( M_PI / 180.0f ), &sy, &cy );
	sOffset = real_time * cy * flRate;
	tOffset = real_time * sy * flRate;

	// make sure that we are positive
	if( sOffset < 0.0f ) sOffset += 1.0f + -(int)sOffset;
	if( tOffset < 0.0f ) tOffset += 1.0f + -(int)tOffset;

	// make sure that we are in a [0,1] range
	sOffset = sOffset - (int)sOffset;
	tOffset = tOffset - (int)tOffset;

	// For each polygon...
	for (const glpoly_t* p = fa->polys; p; p = p->next)
	{
		// Allocate memory for this polygon.
		const int		unclipped_vertex_count	= p->numverts;
		glvert_t* const	unclipped_vertices		=
			static_cast<glvert_t*>(sceGuGetMemory(sizeof(glvert_t) * unclipped_vertex_count));

		// Generate each vertex.
		const glvert_t*	src			= p->verts;
		const glvert_t*	last_vertex = src + unclipped_vertex_count;
		glvert_t*		dst			= unclipped_vertices;
		while (src != last_vertex)
		{
			// Get the input UVs.
			const float	os = src->st[0];
			const float	ot = src->st[1];

			// Fill in the vertex data.
			dst->st[0] = (os + sOffset);
			dst->st[1] = (ot + tOffset);
			dst->xyz[0] = src->xyz[0];
			dst->xyz[1] = src->xyz[1];
			dst->xyz[2] = src->xyz[2];

			// Next vertex.
			++src;
			++dst;
		}

		// Do these vertices need clipped?
		if (clipping::is_clipping_required(unclipped_vertices, unclipped_vertex_count))
		{
			// Clip the polygon.
			const glvert_t*	clipped_vertices;
			std::size_t		clipped_vertex_count;
			clipping::clip(
				unclipped_vertices,
				unclipped_vertex_count,
				&clipped_vertices,
				&clipped_vertex_count);

			// Any vertices left?
			if (clipped_vertex_count)
			{
				// Copy the vertices to the display list.
				const std::size_t buffer_size = clipped_vertex_count * sizeof(glvert_t);
				glvert_t* const display_list_vertices = static_cast<glvert_t*>(sceGuGetMemory(buffer_size));
				memcpy(display_list_vertices, clipped_vertices, buffer_size);

				// Draw the clipped vertices.
				sceGuDrawArray(
					GU_TRIANGLE_FAN,
					GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
					clipped_vertex_count, 0, display_list_vertices);
			}
		}
		else
		{
			// Draw the vertices.
			sceGuDrawArray(
				GU_TRIANGLE_FAN,
				GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
				unclipped_vertex_count, 0, unclipped_vertices);
		}
	}
}

/*
===============
EmitBothSkyLayers

Does a sky warp on the pre-fragmented glpoly_t chain
This will be called for brushmodels, the world
will have them chained together.
===============
*/
void EmitBothSkyLayers (msurface_t *fa)
{
	GL_Bind (solidskytexture);
	speedscale = realtime*8;
	speedscale -= (int)speedscale & ~127 ;

	EmitSkyPolys (fa);

	sceGuEnable(GU_BLEND);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);

	GL_Bind (alphaskytexture);
	speedscale = realtime*16;
	speedscale -= (int)speedscale & ~127 ;

	EmitSkyPolys (fa);

	sceGuDisable(GU_BLEND);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
}


/*
===============
R_DrawScroll_SkyChain
===============
*/
void R_DrawScroll_SkyChain (msurface_t *s)
{
	msurface_t	*fa;

	// used when gl_texsort is on
	GL_Bind(solidskytexture);
	speedscale = realtime*8;
	speedscale -= (int)speedscale & ~127 ;

	for (fa=s ; fa ; fa=fa->texturechain)
		EmitSkyPolys (fa);

	sceGuEnable(GU_BLEND);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);

	GL_Bind (alphaskytexture);
	speedscale = realtime*16;
	speedscale -= (int)speedscale & ~127 ;

	for (fa=s ; fa ; fa=fa->texturechain)
		EmitSkyPolys (fa);

	sceGuDisable(GU_BLEND);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
}


//===============================================================

void UnloadSkyTexture (void)
{
	for (int i = 0; i < 6; i++)
	{
	    if (skyimage[i])
            GL_UnloadTexture(skyimage[i]);
		skyimage[i] = 0;
	}
}

/*
==================
R_LoadSkys
==================
*/
void Sky_LoadSkyBox (char *name)
{
	if (strcmp(skybox_name, name) == 0)
		return; //no change

    //purge old sky textures
    UnloadSkyTexture ();

	//turn off skybox if sky is set to ""
	if (name[0] == '0')
	{
		skybox_name[0] = 0;
		return;
	}

    for (int i = 0; i < 6; i++)
    {
        int mark = Hunk_LowMark ();
		if(!(skyimage[i] = loadtextureimage (va("gfx/env/%s%s", name, suf[i]), 0, 0, qfalse, GU_LINEAR)) &&
           !(skyimage[i] = loadtextureimage (va("gfx/env/%s_%s", name, suf[i]), 0, 0, qfalse, GU_LINEAR)))
		{
			Con_Printf("Sky: %s[%s] not found, used std\n", name, suf[i]);
		    if(!(skyimage[i] = loadtextureimage (va("gfx/env/skybox%s", suf[i]), 0, 0, qfalse, GU_LINEAR)))
		    {
			    Sys_Error("Sky: std not found!");
			}

		}
        Hunk_FreeToLowMark (mark);
    }
    strcpy(skybox_name, name);
}

/*
=================
Sky_NewMap
=================
*/
void Sky_NewMap (void)
{
	char	key[128], value[4096];
	char	*data;

    //purge old sky textures
    UnloadSkyTexture ();

	//
	// initially no sky
	//
	Sky_LoadSkyBox (""); //not used

	//
	// read worldspawn (this is so ugly, and shouldn't it be done on the server?)
	//
	data = cl.worldmodel->entities;
	if (!data)
		return; //FIXME: how could this possibly ever happen? -- if there's no
	// worldspawn then the sever wouldn't send the loadmap message to the client

	data = COM_Parse(data);

	if (!data) //should never happen
		return; // error

	if (com_token[0] != '{') //should never happen
		return; // error

	while (1)
	{
		data = COM_Parse(data);

		if (!data)
			return; // error

		if (com_token[0] == '}')
			break; // end of worldspawn

		if (com_token[0] == '_')
			strcpy(key, com_token + 1);
		else
			strcpy(key, com_token);
		while (key[strlen(key)-1] == ' ') // remove trailing spaces
			key[strlen(key)-1] = 0;

		data = COM_Parse(data);
		if (!data)
			return; // error

		strcpy(value, com_token);

        if (!strcmp("sky", key))
            Sky_LoadSkyBox(value);
        else if (!strcmp("skyname", key)) //half-life
            Sky_LoadSkyBox(value);
	    else if (!strcmp("qlsky", key)) //quake lives
            Sky_LoadSkyBox(value);
	}
}

/*
=================
Sky_SkyCommand_f
=================
*/
void Sky_SkyCommand_f (void)
{
	switch (Cmd_Argc())
	{
	case 1:
		Con_Printf("\"sky\" is \"%s\"\n", skybox_name);
		break;
	case 2:
		Sky_LoadSkyBox(Cmd_Argv(1));
		break;
	default:
		Con_Printf("usage: sky <skyname>\n");
	}
}

/*
=============
Sky_Init
=============
*/
void Sky_Init (void)
{
	int		i;

	Cmd_AddCommand ("sky",Sky_SkyCommand_f);

	for (i=0; i<6; i++)
		skyimage[i] = 0;
}

vec3_t	skyclip[6] = {
	{1,1,0},
	{1,-1,0},
	{0,-1,1},
	{0,1,1},
	{1,0,1},
	{-1,0,1}
};
int	c_sky;

// 1 = s, 2 = t, 3 = 2048
int	st_to_vec[6][3] =
{
	{3,-1,2},
	{-3,1,2},

	{1,3,2},
	{-1,-3,2},

	{-2,-1,3},		// 0 degrees yaw, look straight up
	{2,-1,-3}		// look straight down

//	{-1,2,3},
//	{1,2,-3}
};

// s = [0]/[2], t = [1]/[2]
int	vec_to_st[6][3] =
{
	{-2,3,1},
	{2,3,-1},

	{1,3,2},
	{-1,3,-2},

	{-2,-1,3},
	{-2,1,-3}

//	{-1,2,3},
//	{1,2,-3}
};

float	skymins[2][6], skymaxs[2][6];

void DrawSkyPolygon (int nump, vec3_t vecs)
{
	int		i,j;
	vec3_t	v, av;
	float	s, t, dv;
	int		axis;
	float	*vp;

	c_sky++;

	// decide which face it maps to
	VectorCopy (vec3_origin, v);
	for (i=0, vp=vecs ; i<nump ; i++, vp+=3)
	{
		VectorAdd (vp, v, v);
	}
	av[0] = fabs(v[0]);
	av[1] = fabs(v[1]);
	av[2] = fabs(v[2]);
	if (av[0] > av[1] && av[0] > av[2])
	{
		if (v[0] < 0)
			axis = 1;
		else
			axis = 0;
	}
	else if (av[1] > av[2] && av[1] > av[0])
	{
		if (v[1] < 0)
			axis = 3;
		else
			axis = 2;
	}
	else
	{
		if (v[2] < 0)
			axis = 5;
		else
			axis = 4;
	}

	// project new texture coords
	for (i=0 ; i<nump ; i++, vecs+=3)
	{
		j = vec_to_st[axis][2];
		if (j > 0)
			dv = vecs[j - 1];
		else
			dv = -vecs[-j - 1];

		j = vec_to_st[axis][0];
		if (j < 0)
			s = -vecs[-j -1] / dv;
		else
			s = vecs[j-1] / dv;
		j = vec_to_st[axis][1];
		if (j < 0)
			t = -vecs[-j -1] / dv;
		else
			t = vecs[j-1] / dv;

		if (s < skymins[0][axis])
			skymins[0][axis] = s;
		if (t < skymins[1][axis])
			skymins[1][axis] = t;
		if (s > skymaxs[0][axis])
			skymaxs[0][axis] = s;
		if (t > skymaxs[1][axis])
			skymaxs[1][axis] = t;
	}
}

#define	MAX_CLIP_VERTS	64
void ClipSkyPolygon (int nump, vec3_t vecs, int stage)
{
	float	*norm;
	float	*v;
	qboolean	front, back;
	float	d, e;
	float	dists[MAX_CLIP_VERTS];
	int		sides[MAX_CLIP_VERTS];
	vec3_t	newv[2][MAX_CLIP_VERTS];
	int		newc[2];
	int		i, j;

	if (nump > MAX_CLIP_VERTS-2)
		Sys_Error ("ClipSkyPolygon: MAX_CLIP_VERTS");

	if (stage == 6)
	{	// fully clipped, so draw it
		DrawSkyPolygon (nump, vecs);
		return;
	}

	front = back = qfalse;
	norm = skyclip[stage];
	for (i=0, v = vecs ; i<nump ; i++, v+=3)
	{
		d = DotProduct (v, norm);
		if (d > ON_EPSILON)
		{
			front = qtrue;
			sides[i] = SIDE_FRONT;
		}
		else if (d < ON_EPSILON)
		{
			back = qtrue;
			sides[i] = SIDE_BACK;
		}
		else
			sides[i] = SIDE_ON;
		dists[i] = d;
	}

	if (!front || !back)
	{	// not clipped
		ClipSkyPolygon (nump, vecs, stage+1);
		return;
	}

	// clip it
	sides[i] = sides[0];
	dists[i] = dists[0];
	VectorCopy (vecs, (vecs+(i*3)) );
	newc[0] = newc[1] = 0;

	for (i=0, v = vecs ; i<nump ; i++, v+=3)
	{
		switch (sides[i])
		{
		case SIDE_FRONT:
			VectorCopy (v, newv[0][newc[0]]);
			newc[0]++;
			break;
		case SIDE_BACK:
			VectorCopy (v, newv[1][newc[1]]);
			newc[1]++;
			break;
		case SIDE_ON:
			VectorCopy (v, newv[0][newc[0]]);
			newc[0]++;
			VectorCopy (v, newv[1][newc[1]]);
			newc[1]++;
			break;
		}

		if (sides[i] == SIDE_ON || sides[i+1] == SIDE_ON || sides[i+1] == sides[i])
			continue;

		d = dists[i] / (dists[i] - dists[i+1]);
		for (j=0 ; j<3 ; j++)
		{
			e = v[j] + d*(v[j+3] - v[j]);
			newv[0][newc[0]][j] = e;
			newv[1][newc[1]][j] = e;
		}
		newc[0]++;
		newc[1]++;
	}

	// continue
	ClipSkyPolygon (newc[0], newv[0][0], stage+1);
	ClipSkyPolygon (newc[1], newv[1][0], stage+1);
}
/*
==============
R_ClearSkyBox
==============
*/
void R_ClearSkyBox (void)
{
	int		i;

	for (i=0 ; i<6 ; i++)
	{
		skymins[0][i] = skymins[1][i] = 9999;
		skymaxs[0][i] = skymaxs[1][i] = -9999;
	}
}

static float s_axis;
static float t_axis;
static vec3_t v_axis;

void MakeSkyVec (float s, float t, int axis)
{
	vec3_t		b;
	int			j, k;

	b[0] = s*2560;
	b[1] = t*2560;
	b[2] = 2560;

	for (j=0 ; j<3 ; j++)
	{
		k = st_to_vec[axis][j];
		if (k < 0)
			v_axis[j] = -b[-k - 1];
		else
			v_axis[j] = b[k - 1];
		v_axis[j] += r_origin[j];
	}

	// avoid bilerp seam
	s = (s+1.0f)*0.5f;
	t = (t+1.0f)*0.5f;

	if (s < 1.0f/512.0f)
		s = 1.0f/512.0f;
	else if (s > 511.0f/512.0f)
		s = 511.0f/512.0f;

	if (t < 1.0f/512.0f)
		t = 1.0f/512.0f;
	else if (t > 511.0f/512.0f)
		t = 511.0f/512.0f;

	t = 1.0f - t;

	s_axis = s;
	t_axis = t;
}

/*
==============
R_DrawSkyBox
==============
*/
void R_DrawSkyBox (void)
{
    int		i;

	//sceGuDepthRange(32767, 65535); //not used

	for (i=0 ; i<6 ; i++)
	{
		// Allocate memory for this polygon.
		const int		unclipped_vertex_count	= 4;
		glvert_t* const	unclipped_vertices  =
			static_cast<glvert_t*>(sceGuGetMemory(sizeof(glvert_t) * unclipped_vertex_count));

		if (skymins[0][i] >= skymaxs[0][i]
		|| skymins[1][i] >= skymaxs[1][i])
			continue;

        GL_Bind (skyimage[skytexorder[i]]);

		MakeSkyVec (skymins[0][i], skymins[1][i], i);

        unclipped_vertices[0].st[0]	    = s_axis;
        unclipped_vertices[0].st[1]	    = t_axis;
        unclipped_vertices[0].xyz[0]	= v_axis[0];
        unclipped_vertices[0].xyz[1]	= v_axis[1];
        unclipped_vertices[0].xyz[2]	= v_axis[2];

	    MakeSkyVec (skymins[0][i], skymaxs[1][i], i);

        unclipped_vertices[1].st[0]	    = s_axis;
        unclipped_vertices[1].st[1]	    = t_axis;
        unclipped_vertices[1].xyz[0]	= v_axis[0];
        unclipped_vertices[1].xyz[1]	= v_axis[1];
        unclipped_vertices[1].xyz[2]	= v_axis[2];

        MakeSkyVec (skymaxs[0][i], skymaxs[1][i], i);

        unclipped_vertices[2].st[0]	    = s_axis;
        unclipped_vertices[2].st[1]	    = t_axis;
        unclipped_vertices[2].xyz[0]	= v_axis[0];
        unclipped_vertices[2].xyz[1]	= v_axis[1];
        unclipped_vertices[2].xyz[2]	= v_axis[2];

		MakeSkyVec (skymaxs[0][i], skymins[1][i], i);

        unclipped_vertices[3].st[0]	    = s_axis;
        unclipped_vertices[3].st[1]	    = t_axis;
        unclipped_vertices[3].xyz[0]	= v_axis[0];
        unclipped_vertices[3].xyz[1]	= v_axis[1];
        unclipped_vertices[3].xyz[2]	= v_axis[2];

        if (clipping::is_clipping_required(
            unclipped_vertices,
            unclipped_vertex_count))
        {
            // Clip the polygon.
            const glvert_t*	clipped_vertices;
            std::size_t		clipped_vertex_count;
            clipping::clip(
                unclipped_vertices,
                unclipped_vertex_count,
                &clipped_vertices,
                &clipped_vertex_count);

            // Did we have any vertices left?
            if (clipped_vertex_count)
            {
                // Copy the vertices to the display list.
                const std::size_t buffer_size = clipped_vertex_count * sizeof(glvert_t);
                glvert_t* const display_list_vertices = static_cast<glvert_t*>(sceGuGetMemory(buffer_size));
                memcpy(display_list_vertices, clipped_vertices, buffer_size);

                // Draw the clipped vertices.
                sceGuDrawArray(
                    GU_TRIANGLE_FAN,
                    GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
                    clipped_vertex_count, 0, display_list_vertices);
            }
        }
        else
        {
            // Draw the poly directly.
            sceGuDrawArray(
                GU_TRIANGLE_FAN,
                GU_TEXTURE_32BITF | GU_VERTEX_32BITF,
                unclipped_vertex_count, 0, unclipped_vertices);
        }
    }
    //sceGuDepthRange(0, 65535);
}

//===============================================================

/*
=================
R_DrawSkyChain
=================
*/
void R_DrawSkyChain (msurface_t *s)
{
	  msurface_t	*fa;
      int		    i;
      vec3_t	    verts[MAX_CLIP_VERTS];
      glpoly_t	    *p;


	  if (skybox_name[0]) // if the skybox has a name, draw the skybox
	  {
		  c_sky = 0;

		  // calculate vertex values for sky box
		  for (fa=s ; fa ; fa=fa->texturechain)
		  {
			  for (p=fa->polys ; p ; p=p->next)
			  {
				  for (i=0 ; i<p->numverts ; i++)
				  {
					  VectorSubtract (p->verts[i].xyz, r_origin, verts[i]);
				  }
				  ClipSkyPolygon (p->numverts, verts[0], 0);
			  }

		  }
	  }
	  else // otherwise, draw the normal quake sky
	  {
		  R_DrawScroll_SkyChain (s);
	  }
}

//===============================================================

/*
=============
R_InitSky

A sky texture is 256*128, with the right side being a masked overlay
==============
*/
void R_InitSky (byte *mt)
{
	byte	trans[128*128];

	const byte* const src = (byte *)mt; //  + mt->offsets[0];

	for (int i=0 ; i<128 ; i++)
	{
		for (int j=0 ; j<128 ; j++)
		{
			const byte p = src[i*256 + j + 128];
			trans[(i*128) + j] = p;
		}
	}

	if (solidskytexture == -1)
		solidskytexture = GL_LoadTexture("", 128, 128, trans, 1, qfalse, GU_LINEAR, 0);

	for (int i=0 ; i<128 ; i++)
	{
		for (int j=0 ; j<128 ; j++)
		{
			const byte p = src[i*256 + j];
			if (p == 0)
				trans[(i*128) + j] = 255;
			else
				trans[(i*128) + j] = p;
		}
	}

	if (alphaskytexture == -1)
		alphaskytexture = GL_LoadTexture("", 128, 128, trans, 1, qfalse, GU_LINEAR, 0);
}

