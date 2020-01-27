/*
gl_decals.c - decal paste and rendering
Copyright (C) 2010 Uncle Mike
Copyright (C) 2012 Crow_bar

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#include <pspgu.h>


extern "C"
{
#include "../quakedef.h"
}

#define DECAL_DISTANCE		4	// too big values produce more clipped polygons
#define MAX_DECALCLIPVERT		32	// produced vertexes of fragmented decal
#define DECAL_CACHEENTRY		256	// MUST BE POWER OF 2 or code below needs to change!
#define DECAL_TRANSPARENT_THRESHOLD	230	// transparent decals draw with GL_MODULATE

// empirically determined constants for minimizing overalpping decals
#define MAX_OVERLAP_DECALS		6
#define DECAL_OVERLAP_DIST		8

// clip edges
#define LEFT_EDGE			0
#define RIGHT_EDGE			1
#define TOP_EDGE			2
#define BOTTOM_EDGE			3

// This structure contains the information used to create new decals
typedef struct
{
	vec3_t		m_Position;	// world coordinates of the decal center
	vec3_t		m_SAxis;		// the s axis for the decal in world coordinates
	model_t		*m_pModel;	// the model the decal is going to be applied in
	int		m_iTexture;	// The decal material
	int		m_Size;		// Size of the decal (in world coords)
	int		m_Flags;
	int		m_Entity;		// Entity the decal is applied to.
	float		m_scale;
	int		m_decalWidth;
	int		m_decalHeight;
	vec3_t		m_Basis[3];
} decalinfo_t;

typedef struct
{
	char name[64];
	int  texindex;
}decalreg_t;
static decalreg_t	gDecalReg[MAX_RENDER_DECALS];

static glvert_t	g_DecalClipVerts[MAX_DECALCLIPVERT];
static glvert_t	g_DecalClipVerts2[MAX_DECALCLIPVERT];

static decal_t	gDecalPool[MAX_RENDER_DECALS];
static int	gDecalCount;

void R_ClearDecals( void )
{
	int i;
	memset( gDecalPool, 0, sizeof( gDecalPool ));

	for(i = 0; i < MAX_RENDER_DECALS; i++)
	{
		strcpy(gDecalReg[i].name, "");
		if(gDecalReg[i].texindex > 0)
		   GL_UnloadTexture(gDecalReg[i].texindex);
		gDecalReg[i].texindex = -1;
	}

	gDecalCount = 0;
}

// unlink pdecal from any surface it's attached to
static void R_DecalUnlink( decal_t *pdecal )
{
	decal_t	*tmp;

	if( pdecal->psurface )
	{
		if( pdecal->psurface->pdecals == pdecal )
		{
			pdecal->psurface->pdecals = pdecal->pnext;
		}
		else 
		{
			tmp = pdecal->psurface->pdecals;
			if( !tmp ) Host_Error( "D_DecalUnlink: bad decal list\n" );

			while( tmp->pnext ) 
			{
				if( tmp->pnext == pdecal ) 
				{
					tmp->pnext = pdecal->pnext;
					break;
				}
				tmp = tmp->pnext;
			}
		}
	}

	pdecal->psurface = NULL;
}


// Just reuse next decal in list
// A decal that spans multiple surfaces will use multiple decal_t pool entries,
// as each surface needs it's own.
static decal_t *R_DecalAlloc( decal_t *pdecal )
{
	int	limit = MAX_RENDER_DECALS;

	if(r_decals.value < limit)
	   limit = r_decals.value;

	if(!limit)
	   return NULL;

	if( !pdecal ) 
	{
		int	count = 0;

		// check for the odd possiblity of infinte loop
		do 
		{
			gDecalCount++;
			if( gDecalCount >= limit )
				gDecalCount = 0;
			pdecal = gDecalPool + gDecalCount; // reuse next decal
			count++;
		} while(( pdecal->flags & FDECAL_PERMANENT ) && count < limit );
	}
	
	// If decal is already linked to a surface, unlink it.
	R_DecalUnlink( pdecal );

	return pdecal;	
}

//-----------------------------------------------------------------------------
// find decal image and grab size from it
//-----------------------------------------------------------------------------
static void R_GetDecalDimensions( int texture, int *width, int *height )
{
	if( width ) *width = 1;	// to avoid divide by zero
	if( height ) *height = 1;
	GL_GetTexfSize( width, height, texture );
}

//-----------------------------------------------------------------------------
// compute the decal basis based on surface normal, and preferred saxis
//-----------------------------------------------------------------------------
void R_DecalComputeBasis( msurface_t *surf, vec3_t pSAxis, vec3_t textureSpaceBasis[3] )
{
	vec3_t	surfaceNormal;

	// setup normal
	if( surf->flags & SURF_PLANEBACK )
		VectorNegate( surf->plane->normal, surfaceNormal );
	else
	    VectorCopy( surf->plane->normal, surfaceNormal );

	if( pSAxis )
	{
		// T = S cross N
		CrossProduct( pSAxis, textureSpaceBasis[2], textureSpaceBasis[1] );

		// Name sure they aren't parallel or antiparallel
		// In that case, fall back to the normal algorithm.
		if( DotProduct( textureSpaceBasis[1], textureSpaceBasis[1] ) > 1e-6 )
		{
			// S = N cross T
			CrossProduct( textureSpaceBasis[2], textureSpaceBasis[1], textureSpaceBasis[0] );
#if 0
			VectorNormalizeFast( textureSpaceBasis[0] );
			VectorNormalizeFast( textureSpaceBasis[1] );
#else
			VectorNormalize(textureSpaceBasis[0]);
	        VectorNormalize(textureSpaceBasis[1]);
#endif
			return;
		}
		// Fall through to the standard algorithm for parallel or antiparallel
	}

	// original Half-Life algorithm: get textureBasis from linked surface
	VectorCopy( surf->texinfo->vecs[0], textureSpaceBasis[0] );
	VectorCopy( surf->texinfo->vecs[1], textureSpaceBasis[1] );
	VectorCopy( surfaceNormal, textureSpaceBasis[2] );
#if 0
	VectorNormalizeFast( textureSpaceBasis[0] );
	VectorNormalizeFast( textureSpaceBasis[1] );
#else
	VectorNormalize(textureSpaceBasis[0]);
	VectorNormalize(textureSpaceBasis[1]);
#endif
}

void R_SetupDecalTextureSpaceBasis( decal_t *pDecal, msurface_t *surf, int texture, vec3_t textureSpaceBasis[3], float decalWorldScale[2] )
{
	float	*sAxis = NULL;
	int	width, height;

	if( pDecal->flags & FDECAL_USESAXIS )
		sAxis = pDecal->saxis;

	// Compute the non-scaled decal basis
	R_DecalComputeBasis( surf, sAxis, textureSpaceBasis );
	R_GetDecalDimensions( texture, &width, &height );

	// world width of decal = ptexture->width / pDecal->scale
	// world height of decal = ptexture->height / pDecal->scale
	// scale is inverse, scales world space to decal u/v space [0,1]
	// OPTIMIZE: Get rid of these divides
	decalWorldScale[0] = (float)pDecal->scale / width;
	decalWorldScale[1] = (float)pDecal->scale / height;
	
	VectorScale( textureSpaceBasis[0], decalWorldScale[0], textureSpaceBasis[0] );
	VectorScale( textureSpaceBasis[1], decalWorldScale[1], textureSpaceBasis[1] );
}

// Build the initial list of vertices from the surface verts into the global array, 'verts'.
void R_SetupDecalVertsForMSurface( decal_t *pDecal, msurface_t *surf,	vec3_t textureSpaceBasis[3], glvert_t *verts )
{
	int	i;
	glpoly_t *mpoly;

	mpoly = surf->polys;
	for( i = 0; i < mpoly->numverts; i++)
	{
        verts[i].xyz[0] = mpoly->verts[i].xyz[0];
        verts[i].xyz[1] = mpoly->verts[i].xyz[1];
        verts[i].xyz[2] = mpoly->verts[i].xyz[2];
		verts[i].st[0] = DotProduct( verts[i].xyz, textureSpaceBasis[0] ) - pDecal->dx + 0.5f;
		verts[i].st[1] = DotProduct( verts[i].xyz, textureSpaceBasis[1] ) - pDecal->dy + 0.5f;
	}

}

// Figure out where the decal maps onto the surface.
void R_SetupDecalClip( decal_t *pDecal, msurface_t *surf, int texture, vec3_t textureSpaceBasis[3], float decalWorldScale[2] )
{
	R_SetupDecalTextureSpaceBasis( pDecal, surf, texture, textureSpaceBasis, decalWorldScale );

	// Generate texture coordinates for each vertex in decal s,t space
	// probably should pre-generate this, store it and use it for decal-decal collisions
	// as in R_DecalsIntersect()
	pDecal->dx = DotProduct( pDecal->position, textureSpaceBasis[0] );
	pDecal->dy = DotProduct( pDecal->position, textureSpaceBasis[1] );
}

// Quick and dirty sutherland Hodgman clipper
// Clip polygon to decal in texture space
// JAY: This code is lame, change it later.  It does way too much work per frame
// It can be made to recursively call the clipping code and only copy the vertex list once
int R_ClipInside( glvert_t *vert, int edge )
{
	switch( edge )
	{
	case LEFT_EDGE:
		if( vert->st[0] > 0.0f )
			return 1;
		return 0;
	case RIGHT_EDGE:
		if( vert->st[0] < 1.0f )
			return 1;
		return 0;
	case TOP_EDGE:
		if( vert->st[1] > 0.0f )
			return 1;
		return 0;
	case BOTTOM_EDGE:
		if( vert->st[1] < 1.0f )
			return 1;
		return 0;
	}
	return 0;
}


void R_ClipIntersect( glvert_t *one, glvert_t *two, glvert_t *out, int edge )
{
	float	t;
	
	// t is the parameter of the line between one and two clipped to the edge
	// or the fraction of the clipped point between one & two
	// xyz[0], xyz[1], xyz[2] is X, Y, Z
	// st[0] is u
	// st[1] is v

	if( edge < TOP_EDGE )
	{
		if( edge == LEFT_EDGE )
		{
			// left
			t = ((one->st[0] - 0.0f) / (one->st[0] - two->st[0]));
			out->st[0] = 0.0f;
		}
		else
		{	
			// right
			t = ((one->st[0] - 1.0f) / (one->st[0] - two->st[0]));
			out->st[0] = 1.0f;
		}

		out->st[1] = one->st[1] + (two->st[1] - one->st[1]) * t;
	}
	else
	{
		if( edge == TOP_EDGE )
		{
			// top
			t = ((one->st[1] - 0.0f)  / (one->st[1] - two->st[1]));
			out->st[1] = 0.0f;
		}
		else
		{	
			// bottom
			t = ((one->st[1] - 1.0f) / (one->st[1] - two->st[1]));
			out->st[1] = 1.0f;
		}

		out->st[0] = one->st[0] + (two->st[0] - one->st[0]) * t;
	}

	VectorLerp( one->xyz, t, two->xyz, out->xyz );
}

static int SHClip( glvert_t *vert, int vertCount, glvert_t *out, int edge )
{
	int	j, outCount;
	glvert_t *s, *p;

	outCount = 0;

	s = &vert[(vertCount - 1)];

	for( j = 0; j < vertCount; j++ )
	{
		p = &vert[j];

		if( R_ClipInside( p, edge ))
		{
			if( R_ClipInside( s, edge ))
			{
				// Add a vertex and advance out to next vertex
				memcpy( out, p, sizeof(*out) );
				outCount++;
				out++;
			}
			else
			{
				R_ClipIntersect( s, p, out, edge );
				outCount++;
				out++;

				memcpy( out, p, sizeof(*out) );
				outCount++;
				out++;
			}
		}
		else
		{
			if( R_ClipInside( s, edge ))
			{
				R_ClipIntersect( p, s, out, edge );
				outCount++;
				out++;
			}
		}
		s = p;
	}

	return outCount;
}

glvert_t *R_DoDecalSHClip( glvert_t *pInVerts, decal_t *pDecal, int nStartVerts, int *pVertCount )
{
	int	outCount;
    glvert_t *pOutVerts = g_DecalClipVerts;

	// clip the polygon to the decal texture space
	outCount = SHClip( pInVerts,              nStartVerts, &g_DecalClipVerts2[0], LEFT_EDGE );
	outCount = SHClip( &g_DecalClipVerts2[0], outCount,    &g_DecalClipVerts[0],  RIGHT_EDGE );
	outCount = SHClip( &g_DecalClipVerts[0],  outCount,    &g_DecalClipVerts2[0], TOP_EDGE );
	outCount = SHClip( &g_DecalClipVerts2[0], outCount,    pOutVerts,             BOTTOM_EDGE );

	if( outCount ) 
	{
		if( pDecal->flags & FDECAL_CLIPTEST )
		{
			pDecal->flags &= ~FDECAL_CLIPTEST;	// we're doing the test
			
			// If there are exactly 4 verts and they are all 0,1 tex coords, then we've got an unclipped decal
			// A more precise test would be to calculate the texture area and make sure it's one, but this
			// should work as well.
			if( outCount == 4 )
			{
                int j;
				int clipped = 0;

				glvert_t *v = pOutVerts;
				for ( j = 0; j < outCount && !clipped; j++, v++ )
				{
					float s, t;
					s = v->st[0];
					t = v->st[1];

					if ( (s != 0.0 && s != 1.0) || (t != 0.0 && t != 1.0) )
						clipped = 1;
				}

				// We didn't need to clip this decal, it's a quad covering
				// the full texture space, optimize subsequent frames.
				if( !clipped ) pDecal->flags |= FDECAL_NOCLIP;
			}
		}
	}
	
	*pVertCount = outCount;

	return pOutVerts;
}

//-----------------------------------------------------------------------------
// Generate clipped vertex list for decal pdecal projected onto polygon psurf
//-----------------------------------------------------------------------------
glvert_t *R_DecalVertsClip( decal_t *pDecal, msurface_t *surf, int texture, int *pVertCount )
{
	float	decalWorldScale[2];
	vec3_t	textureSpaceBasis[3]; 

	// figure out where the decal maps onto the surface.
	R_SetupDecalClip( pDecal, surf, texture, textureSpaceBasis, decalWorldScale );

	// build the initial list of vertices from the surface verts.
	R_SetupDecalVertsForMSurface( pDecal, surf, textureSpaceBasis, g_DecalClipVerts );

	return R_DoDecalSHClip( g_DecalClipVerts, pDecal, surf->polys->numverts, pVertCount );
}

// Generate lighting coordinates at each vertex for decal vertices v[] on surface psurf
static void R_DecalVertsLight( glvert_t *v, msurface_t *surf, int vertCount )
{
#if 0
	float		s, t;
	mtexinfo_t	*tex;
	int		j;

	tex = surf->texinfo;

	for( j = 0; j < vertCount; j++)
	{
		// lightmap texture coordinates
		s = DotProduct( v[j].xyz, tex->vecs[0] ) + tex->vecs[0][3] - surf->texturemins[0];
		s += surf->light_s * 16;
		s += 8;
		s /= 128 * 16;

		t = DotProduct( v[j].xyz, tex->vecs[1] ) + tex->vecs[1][3] - surf->texturemins[1];
		t += surf->light_t * 16;
		t += 8;
		t /= 128 * 16;
		
		v[j+vertCount].st[0] = s;
		v[j+vertCount].st[1] = t;
	}
#endif
}

static glvert_t *R_DecalVertsNoclip( decal_t *pdecal, msurface_t *surf, int texture )
{
	glvert_t *vlist;
	int	outCount;

	// use the old code for now, and just cache them
	vlist = R_DecalVertsClip( pdecal, surf, texture, &outCount );

	R_DecalVertsLight( vlist, surf, 4 );

	return vlist;
}

//-----------------------------------------------------------------------------
// Purpose: Check for intersecting decals on this surface
// Input  : *psurf - 
//			*pcount - 
//			x - 
//			y - 
// Output : static decal_t
//-----------------------------------------------------------------------------
// UNDONE: This probably doesn't work quite right any more
// we should base overlap on the new decal basis matrix
// decal basis is constant per plane, perhaps we should store it (unscaled) in the shared plane struct
// BRJ: Note, decal basis is not constant when decals need to specify an s direction
// but that certainly isn't the majority case
static decal_t *R_DecalIntersect( decalinfo_t *decalinfo, msurface_t *surf, int *pcount )
{
	int		texture;
	decal_t		*plast, *pDecal;
	vec3_t		decalExtents[2];
	float		lastArea = 2;
	int		mapSize[2];

	plast = NULL;
	*pcount = 0;

	// (Same as R_SetupDecalClip).
	texture = decalinfo->m_iTexture;
	
	// precalculate the extents of decalinfo's decal in world space.
	R_GetDecalDimensions( texture, &mapSize[0], &mapSize[1] );
	VectorScale( decalinfo->m_Basis[0], ((mapSize[0] / decalinfo->m_scale) * 0.5f), decalExtents[0] );
	VectorScale( decalinfo->m_Basis[1], ((mapSize[1] / decalinfo->m_scale) * 0.5f), decalExtents[1] );

	pDecal = surf->pdecals;

	while( pDecal ) 
	{
		texture = pDecal->texture;

		// Don't steal bigger decals and replace them with smaller decals
		// Don't steal permanent decals
		if(!( pDecal->flags & FDECAL_PERMANENT ))
		{
			vec3_t	testBasis[3];
			vec3_t	testPosition[2];
			float	testWorldScale[2];
			vec2_t	vDecalMin, vDecalMax;
			vec2_t	vUnionMin, vUnionMax;

			R_SetupDecalTextureSpaceBasis( pDecal, surf, texture, testBasis, testWorldScale );

			VectorSubtract( decalinfo->m_Position, decalExtents[0], testPosition[0] );
			VectorSubtract( decalinfo->m_Position, decalExtents[1], testPosition[1] );

			// Here, we project the min and max extents of the decal that got passed in into
			// this decal's (pDecal's) [0,0,1,1] clip space, just like we would if we were
			// clipping a triangle into pDecal's clip space.
			Vector2Set( vDecalMin,
				DotProduct( testPosition[0], testBasis[0] ) - pDecal->dx + 0.5f,
				DotProduct( testPosition[1], testBasis[1] ) - pDecal->dy + 0.5f );

			VectorAdd( decalinfo->m_Position, decalExtents[0], testPosition[0] );
			VectorAdd( decalinfo->m_Position, decalExtents[1], testPosition[1] );

			Vector2Set( vDecalMax,
				DotProduct( testPosition[0], testBasis[0] ) - pDecal->dx + 0.5f,
				DotProduct( testPosition[1], testBasis[1] ) - pDecal->dy + 0.5f );

			// Now figure out the part of the projection that intersects pDecal's
			// clip box [0,0,1,1].
			Vector2Set( vUnionMin, fmax( vDecalMin[0], 0 ), fmax( vDecalMin[1], 0 ));
			Vector2Set( vUnionMax, fmin( vDecalMax[0], 1 ), fmin( vDecalMax[1], 1 ));

			if( vUnionMin[0] < 1 && vUnionMin[1] < 1 && vUnionMax[0] > 0 && vUnionMax[1] > 0 )
			{
				// Figure out how much of this intersects the (0,0) - (1,1) bbox.			
				float	flArea = (vUnionMax[0] - vUnionMin[1]) * (vUnionMax[1] - vUnionMin[1]);

				if( flArea > 0.6f )
				{
					*pcount += 1;

					if( !plast || flArea <= lastArea ) 
					{
						plast = pDecal;
						lastArea =  flArea;
					}
				}
			}
		}
		pDecal = pDecal->pnext;
	}
	return plast;
}

// Add the decal to the surface's list of decals.
static void R_AddDecalToSurface( decal_t *pdecal, msurface_t *surf, decalinfo_t *decalinfo )
{
	decal_t	*pold;

	pdecal->pnext = NULL;
	pold = surf->pdecals;

	if( pold ) 
	{
		while( pold->pnext ) 
			pold = pold->pnext;
		pold->pnext = pdecal;
	}
	else
	{
		surf->pdecals = pdecal;
	}

	// tag surface
	pdecal->psurface = surf;

	// at this point decal are linked with surface
	// and will be culled, drawing and sorting
	// together with surface
}

static void R_DecalCreate( decalinfo_t *decalinfo, msurface_t *surf, float x, float y )
{
	decal_t	*pdecal, *pold;
	int	count, vertCount;

	if( !surf )
	{
		Sys_Error("psurface NULL in R_DecalCreate!\n" );
		return;
	}
	
	pold = R_DecalIntersect( decalinfo, surf, &count );
	if( count < MAX_OVERLAP_DECALS ) pold = NULL;

	pdecal = R_DecalAlloc( pold );
	if( !pdecal ) return; // r_decals == 0 ???

	pdecal->flags = decalinfo->m_Flags;

	VectorCopy( decalinfo->m_Position, pdecal->position );

	if( pdecal->flags & FDECAL_USESAXIS )
		VectorCopy( decalinfo->m_SAxis, pdecal->saxis );

	pdecal->dx = x;
	pdecal->dy = y;
	pdecal->texture = decalinfo->m_iTexture;

	// set scaling
	pdecal->scale = decalinfo->m_scale;
	pdecal->entityIndex = decalinfo->m_Entity;

	// check to see if the decal actually intersects the surface
	// if not, then remove the decal
	R_DecalVertsClip( pdecal, surf, decalinfo->m_iTexture, &vertCount );
	
	if( !vertCount )
	{
		R_DecalUnlink( pdecal );
		return;
	}

	// add to the surface's list
	R_AddDecalToSurface( pdecal, surf, decalinfo );
}

void R_DecalSurface( msurface_t *surf, decalinfo_t *decalinfo )
{
	// get the texture associated with this surface
	mtexinfo_t	*tex = surf->texinfo;
	vec4_t		textureU, textureV;
	float		*sAxis = NULL;
	float		s, t, w, h;

	Vector4Copy( tex->vecs[0], textureU );
	Vector4Copy( tex->vecs[1], textureV );

	// project decal center into the texture space of the surface
	s = DotProduct( decalinfo->m_Position, textureU ) + textureU[3] - surf->texturemins[0];
	t = DotProduct( decalinfo->m_Position, textureV ) + textureV[3] - surf->texturemins[1];

	// Determine the decal basis (measured in world space)
	// Note that the decal basis vectors 0 and 1 will always lie in the same
	// plane as the texture space basis vectorstextureVecsTexelsPerWorldUnits.

	if( decalinfo->m_Flags & FDECAL_USESAXIS )
		sAxis = decalinfo->m_SAxis;

	R_DecalComputeBasis( surf, sAxis, decalinfo->m_Basis );

	// Compute an effective width and height (axis aligned) in the parent texture space
	// How does this work? decalBasis[0] represents the u-direction (width)
	// of the decal measured in world space, decalBasis[1] represents the 
	// v-direction (height) measured in world space.
	// textureVecsTexelsPerWorldUnits[0] represents the u direction of 
	// the surface's texture space measured in world space (with the appropriate
	// scale factor folded in), and textureVecsTexelsPerWorldUnits[1]
	// represents the texture space v direction. We want to find the dimensions (w,h)
	// of a square measured in texture space, axis aligned to that coordinate system.
	// All we need to do is to find the components of the decal edge vectors
	// (decalWidth * decalBasis[0], decalHeight * decalBasis[1])
	// in texture coordinates:

	w = fabs( decalinfo->m_decalWidth  * DotProduct( textureU, decalinfo->m_Basis[0] )) +
	    fabs( decalinfo->m_decalHeight * DotProduct( textureU, decalinfo->m_Basis[1] ));
	
	h = fabs( decalinfo->m_decalWidth  * DotProduct( textureV, decalinfo->m_Basis[0] )) +
	    fabs( decalinfo->m_decalHeight * DotProduct( textureV, decalinfo->m_Basis[1] ));

	// move s,t to upper left corner
	s -= ( w * 0.5f );
	t -= ( h * 0.5f );

	// Is this rect within the surface? -- tex width & height are unsigned
	if( s <= -w || t <= -h || s > (surf->extents[0] + w) || t > (surf->extents[1] + h))
	{
		return; // nope
	}

	// stamp it
	R_DecalCreate( decalinfo, surf, s, t );
}

//-----------------------------------------------------------------------------
// iterate over all surfaces on a node, looking for surfaces to decal
//-----------------------------------------------------------------------------
static void R_DecalNodeSurfaces( model_t *model, mnode_t *node, decalinfo_t *decalinfo )
{
	// iterate over all surfaces in the node
	msurface_t	*surf;
	int		i;

	surf = model->surfaces + node->firstsurface;

	for( i = 0; i < node->numsurfaces; i++, surf++ ) 
	{
		// never apply decals on the water or sky surfaces
		if( surf->flags & (SURF_DRAWTURB|SURF_DRAWSKY|SURF_CONVEYOR))
			continue;

		R_DecalSurface( surf, decalinfo );
	}
}

//-----------------------------------------------------------------------------
// Recursive routine to find surface to apply a decal to.  World coordinates of 
// the decal are passed in r_recalpos like the rest of the engine.  This should 
// be called through R_DecalShoot()
//-----------------------------------------------------------------------------
static void R_DecalNode( model_t *model, mnode_t *node, decalinfo_t *decalinfo )
{
	mplane_t	*splitplane;
	float	dist;
	
	if( node->contents < 0 )
	{
		// hit a leaf
		return;
	}

	splitplane = node->plane;
	dist = DotProduct( decalinfo->m_Position, splitplane->normal ) - splitplane->dist;

	// This is arbitrarily set to 10 right now. In an ideal world we'd have the 
	// exact surface but we don't so, this tells me which planes are "sort of 
	// close" to the gunshot -- the gunshot is actually 4 units in front of the 
	// wall (see dlls\weapons.cpp). We also need to check to see if the decal 
	// actually intersects the texture space of the surface, as this method tags
	// parallel surfaces in the same node always.
	// JAY: This still tags faces that aren't correct at edges because we don't 
	// have a surface normal
	if( dist > decalinfo->m_Size )
	{
		R_DecalNode( model, node->children[0], decalinfo );
	}
	else if( dist < -decalinfo->m_Size )
	{
		R_DecalNode( model, node->children[1], decalinfo );
	}
	else 
	{
		if( dist < DECAL_DISTANCE && dist > -DECAL_DISTANCE )
			R_DecalNodeSurfaces( model, node, decalinfo );

		R_DecalNode( model, node->children[0], decalinfo );
		R_DecalNode( model, node->children[1], decalinfo );
	}
}

// Shoots a decal onto the surface of the BSP.  position is the center of the decal in world coords
void R_DecalShoot( int textureIndex, int entityIndex, int modelIndex, vec3_t pos, int flags, vec3_t saxis )
{
	decalinfo_t	decalInfo;
	hull_t		*hull;

	entity_t	*ent = NULL;

	model_t		*model = NULL;
	int		width, height;

	if( textureIndex <= 0 || textureIndex >= 1024 )
	{
		Sys_Error("Decal has invalid texture!\n");
		return;
	}

	if( entityIndex > 0 )
	{
		ent = CL_EntityNum(entityIndex);
		
		if( modelIndex > 0 )
		    model = cl.model_precache[modelIndex];

		else if( ent != NULL )
		    model = cl.model_precache[ent->baseline.modelindex];
		else
		    return;
	}
	else if( modelIndex > 0 )
		model = cl.model_precache[modelIndex];
	else
	    model = cl.worldmodel;

	if( !model )
	    return;

	if( model->type != mod_brush )
	{
		return;
	}

	decalInfo.m_pModel = model;
	hull = &model->hulls[0];	// always use #0 hull
	
	if( ent )
	{
		vec3_t	pos_l;
		// transform decal position in local bmodel space
		if(ent->angles[0] != 0 || ent->angles[1] != 0 || ent->angles[2] != 0)
		{

			matrix4x4	matrix;
			Matrix4x4_CreateFromEntity( matrix, ent->angles, ent->origin, 1.0f );
			Matrix4x4_VectorITransform( matrix, pos, pos_l );
		}
		else
		{
			VectorSubtract( pos, ent->origin, pos_l );
		}
		VectorCopy( pos_l, decalInfo.m_Position );
	}
	else
	{
		// pass position in global
		VectorCopy( pos, decalInfo.m_Position );
	}

	// deal with the s axis if one was passed in
	if( saxis )
	{
		flags |= FDECAL_USESAXIS;
		VectorCopy( saxis, decalInfo.m_SAxis );
	}

	// more state used by R_DecalNode()
	decalInfo.m_iTexture = textureIndex;

	// don't optimize custom decals
	if(!( flags & FDECAL_CUSTOM ))
		flags |= FDECAL_CLIPTEST;
	
	decalInfo.m_Flags = flags;
	decalInfo.m_Entity = entityIndex;

	R_GetDecalDimensions( textureIndex, &width, &height );
	decalInfo.m_Size = width >> 1;
	if(( height >> 1 ) > decalInfo.m_Size )
		decalInfo.m_Size = height >> 1;

	decalInfo.m_scale = 1.0f;

	// compute the decal dimensions in world space
	decalInfo.m_decalWidth = width / decalInfo.m_scale;
	decalInfo.m_decalHeight = height / decalInfo.m_scale;

	R_DecalNode( model, &model->nodes[hull->firstclipnode], &decalInfo );
}

// Build the vertex list for a decal on a surface and clip it to the surface.
// This is a template so it can work on world surfaces and dynamic displacement 
// triangles the same way.
glvert_t *R_DecalSetupVerts( decal_t *pDecal, msurface_t *surf, int texture, int *outCount )
{
	glvert_t *v;

	if(pDecal->flags & FDECAL_NOCLIP)
	{
		v = R_DecalVertsNoclip( pDecal, surf, texture );
		*outCount = 4;
	}
	else
	{
		v = R_DecalVertsClip( pDecal, surf, texture, outCount );
		if( outCount )
		    R_DecalVertsLight( v, surf, *outCount );
	}

	return v;
}

#include "clipping.hpp"
using namespace quake;

void DrawSingleDecal( decal_t *pDecal, msurface_t *fa )
{
    glvert_t    *v;
	int		i, numVerts;

	v = R_DecalSetupVerts( pDecal, fa, pDecal->texture, &numVerts );

	if( !numVerts )
	     return;

	GL_Bind( pDecal->texture );
#if 1
	glvert_t* out = static_cast<glvert_t*>(sceGuGetMemory(sizeof(glvert_t) * numVerts));
	for( i = 0; i < numVerts; i++)
	{
		out[i].st[0]  = v[i].st[0];
		out[i].st[1]  = v[i].st[1];
		out[i].xyz[0] = v[i].xyz[0];
		out[i].xyz[1] = v[i].xyz[1];
		out[i].xyz[2] = v[i].xyz[2];
	}
	sceGuDrawArray(GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_VERTEX_32BITF, numVerts, 0, out);
#else
	const int		unclipped_vertex_count	= numVerts;
	glvert_t* const	unclipped_vertices		= static_cast<glvert_t*>(sceGuGetMemory(sizeof(glvert_t) * unclipped_vertex_count));
	for( i = 0; i < unclipped_vertex_count; i++)
	{
		unclipped_vertices[i].st[0]  = v[i].st[0];
		unclipped_vertices[i].st[1]  = v[i].st[1];
		unclipped_vertices[i].xyz[0] = v[i].xyz[0];
		unclipped_vertices[i].xyz[1] = v[i].xyz[1];
		unclipped_vertices[i].xyz[2] = v[i].xyz[2];
	}
	// Does this poly need clipped?
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
#endif
}

void DrawSurfaceDecals( msurface_t *fa )
{
	decal_t		*p;
	entity_t	*e;

	if( !fa->pdecals ) return;

	e = currententity;

	if( !ISADDITIVE(e) && !ISGLOW(e) && !ISTEXTURE(e) )
	{
		sceGuEnable( GU_BLEND );
	}
	
	sceGuDepthMask( GU_TRUE );

    sceGuDepthOffset(-256);
    sceGuBlendFunc(GU_ADD, GU_ONE_MINUS_SRC_ALPHA, GU_SRC_COLOR, 0, 0);

	for( p = fa->pdecals; p; p = p->pnext )
		DrawSingleDecal( p, fa );

    sceGuDepthOffset(0);

	if(!ISADDITIVE(e) && !ISGLOW(e) && !ISTEXTURE(e))
	{
        sceGuDepthMask( GU_FALSE );
		sceGuDisable( GU_BLEND );
	}

    if(ISADDITIVE(e))
    {
		float deg = e->renderamt / 255.0f;
		float alpha1 = deg;
	    float alpha2 = 1 - deg;

		if(deg <= 0.7)
           sceGuDepthMask(GU_TRUE);

		sceGuEnable (GU_BLEND);
		sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
        sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX,
		GU_COLOR(alpha1,alpha1,alpha1,alpha1),
		GU_COLOR(alpha2,alpha2,alpha2,alpha2));
	}
	else
	{
	   sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    }
}

static int R_DecalGetbyName(char *name)
{
	int i;
	for(i = 0; i < MAX_RENDER_DECALS; i++)
	{
		if(!strcasecmp(gDecalReg[i].name, name))
        {
		   return gDecalReg[i].texindex;
		}
	}
    //Con_Printf("Decal: %s not registred\n", name);
	return 0;
}

int R_DecalRegIndex(char *name)
{
	int tindex, i;
	tindex = R_DecalGetbyName(name);
	if(tindex)
       return tindex;
	
	tindex = loadtextureimage(va("decals/%s",name),  0, 0, qfalse, GU_LINEAR);
	if(!tindex) //find in decals.wad
	{
	   WAD3_LoadTextureWadFile("decals.wad");
	   tindex = WAD3_LoadTextureName(name);
       if(!tindex)
       {
           Con_Printf("Decal: %s not found\n", name);
           return 0;
	   }
	}
	for(i = 0; i < MAX_RENDER_DECALS; i++)
	{
		if(!strcasecmp(gDecalReg[i].name, ""))
		{
           strcpy(gDecalReg[i].name, name);
	       gDecalReg[i].texindex = tindex;
	       Con_Printf("Registred decal: %s\n",name);
	       return gDecalReg[i].texindex;
        }
	}
	return 0;
}
/*
=============================================================

  DECALS SERIALIZATION

=============================================================
*/
static qboolean R_DecalUnProject( decal_t *pdecal, decallist_t *entry )
{
/*
	entity_t	*ent;
*/
	if( !pdecal || !( pdecal->psurface ))
		return qfalse;
/*
	ent = CL_GetEntityByIndex( pdecal->entityIndex );

	if( ent && !VectorIsNull( ent->angles ))
	{
		// transform decal position back into world space
		matrix4x4	decalMatrix, entityMatrix, world;

		Matrix4x4_CreateFromEntity( decalMatrix, vec3_origin, pdecal->position, 1.0f );
#if 1
		// NOTE: we rotate the decal into identity pos of bmodel because svc_bspdecal
		// from signon come early then first valid delta-frame, so all entities have
		// identity origin and identity angles at this moment. If this releationship will be changed
		// (e.g. signon will be sended after first valid delta-frame) please set #if 1 to 0. 
		Matrix4x4_CreateFromEntity( entityMatrix, vec3_origin, vec3_origin, 1.0f );
#else
		Matrix4x4_CreateFromEntity( entityMatrix, ent->angles, ent->origin, 1.0f );
#endif
		Matrix4x4_Concat( world, entityMatrix, decalMatrix );
		Matrix4x4_OriginFromMatrix( world, entry->position );
	}
	else
	{
*/
		// give untransformed pos (world or static brushes)
		VectorCopy( pdecal->position, entry->position );
/*
	}
*/
	entry->entityIndex = pdecal->entityIndex;

	// Grab surface plane equation
	if( pdecal->psurface->flags & SURF_PLANEBACK )
		VectorNegate( pdecal->psurface->plane->normal, entry->impactPlaneNormal );
	else VectorCopy( pdecal->psurface->plane->normal, entry->impactPlaneNormal );

	return qtrue;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pList - 
//			count - 
// Output : static int
//-----------------------------------------------------------------------------
static int DecalListAdd( decallist_t *pList, int count )
{
	vec3_t		tmp;
	decallist_t	*pdecal;
	int		i;

	pdecal = pList + count;

	for( i = 0; i < count; i++ )
	{
		if( !Q_strcmp( pdecal->name, pList[i].name ) &&  pdecal->entityIndex == pList[i].entityIndex )
		{
			VectorSubtract( pdecal->position, pList[i].position, tmp );	// Merge

			if( VectorLength( tmp ) < 2 ) // UNDONE: Tune this '2' constant
				return count;
		}
	}

	// this is a new decal
	return count + 1;
}

static int DecalDepthCompare( const void *a, const void *b )
{
	const decallist_t	*elem1, *elem2;

	elem1 = (const decallist_t *)a;
	elem2 = (const decallist_t *)b;

	if( elem1->depth > elem2->depth )
		return 1;
	if( elem1->depth < elem2->depth )
		return -1;

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Called by CSaveRestore::SaveClientState
// Input  : *pList - 
// Output : int
//-----------------------------------------------------------------------------
int R_CreateDecalList( decallist_t *pList, qboolean changelevel )
{
	int	total = 0;
	int	i, depth;

	if( cl.worldmodel )
	{
		for( i = 0; i < MAX_RENDER_DECALS; i++ )
		{
			decal_t	*decal = &gDecalPool[i];
			decal_t	*pdecals;
			
			// decal is in use and is not a custom decal
			if( decal->psurface == NULL || ( decal->flags & FDECAL_DONTSAVE ))	
				 continue;

			// compute depth
			depth = 0;
			pdecals = decal->psurface->pdecals;

			while( pdecals && pdecals != decal )
			{
				depth++;
				pdecals = pdecals->pnext;
			}

			pList[total].depth = depth;
			pList[total].flags = decal->flags;
			
			R_DecalUnProject( decal, &pList[total] );

			//Q_strncpy( pList[total].name, R_GetTexture( decal->texture )->name, sizeof( pList[total].name ));

			// check to see if the decal should be added
			total = DecalListAdd( pList, total );
		}
	}

	// sort the decals lowest depth first, so they can be re-applied in order
	qsort( pList, total, sizeof( decallist_t ), DecalDepthCompare );

	return total;
}

/*
===============
R_DecalRemoveAll

remove all decals with specified texture
===============
*/
void R_DecalRemoveAll( int textureIndex )
{
	decal_t	*pdecal;
	int	i;

	if( textureIndex < 0 || textureIndex >= 1024 )
	{
		Sys_Error("Decal has invalid texture!\n" );
		return;
	}

	for( i = 0; i < gDecalCount; i++ )
	{
		pdecal = &gDecalPool[i];

		if( !textureIndex || pdecal->texture == textureIndex )
			R_DecalUnlink( pdecal );
	}
}
