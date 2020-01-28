//=======================================================================
//			Copyright XashXT Group 2007 ©
//			r_studio.c - render studio models
//=======================================================================

//extern "C"
//{
#include "../quakedef.h"
//}

#include <pspgu.h>
#include <pspgum.h>

/*
=============================================================

  STUDIO MODELS

=============================================================
*/

#define STUDIO_API_VERSION		0.1

matrix3x4 m_pbonestransform [ MAXSTUDIOBONES ];
matrix3x4 m_plighttransform [ MAXSTUDIOBONES ];
matrix3x4 m_protationmatrix;
vec3_t m_plightcolor;	//ambient light color
vec3_t m_plightvec;		//ambleint light vector
vec3_t m_pshadevector;	//shadow vector

//lighting stuff
vec3_t *m_pxformverts;
vec3_t *m_pvlightvalues;
vec3_t m_blightvec [ MAXSTUDIOBONES ];
vec3_t g_xformverts[ MAXSTUDIOVERTS ];
vec3_t g_lightvalues[MAXSTUDIOVERTS];

//chrome stuff
int g_chrome[MAXSTUDIOVERTS][2];	// texture coords for surface normals
int g_chromeage[MAXSTUDIOBONES];	// last time chrome vectors were updated
vec3_t g_chromeup[MAXSTUDIOBONES];	// chrome vector "up" in bone reference frames
vec3_t g_chromeright[MAXSTUDIOBONES];	// chrome vector "right" in bone reference frames

int m_fDoInterp;			
int m_pStudioModelCount;
int m_PassNum;
model_t *m_pRenderModel;
entity_t *m_pCurrentEntity;
mstudiomodel_t *m_pSubModel;
studiohdr_t *m_pStudioHeader;
studiohdr_t *m_pTextureHeader;
mstudiobodyparts_t *m_pBodyPart;

int m_nCachedBones; // Number of bones in cache
char m_nCachedBoneNames[ MAXSTUDIOBONES ][ 32 ];
matrix3x4 m_rgCachedBonesTransform [ MAXSTUDIOBONES ];
matrix3x4 m_rgCachedLightTransform [ MAXSTUDIOBONES ];

/*
====================
R_StudioInit

====================
*/
void R_StudioInit( void )
{
	m_pBodyPart = NULL;
	m_pRenderModel  = NULL;
	m_pStudioHeader = NULL;
          m_pCurrentEntity = NULL;
}

void R_StudioShutdown( void )
{
}

/*
====================
Studio model loader
====================

image_t *R_StudioLoadTexture( model_t *mod, mstudiotexture_t *ptexture, byte *pin )
{
	image_t	*image;
	rgbdata_t	r_skin;

          r_skin.width = ptexture->width;
          r_skin.height = ptexture->height;
	r_skin.flags = (ptexture->flags & STUDIO_NF_TRANSPARENT) ? IMAGE_HAS_ALPHA : 0;
	r_skin.type = PF_INDEXED_24;
	r_skin.numMips = 1;
	r_skin.palette = pin + ptexture->width * ptexture->height + ptexture->index;
	r_skin.buffer = pin + ptexture->index; // texdata
	r_skin.size = ptexture->width * ptexture->height * 3; // for bounds cheking
			
	//load studio texture and bind it
	image = R_LoadImage(ptexture->name, &r_skin, it_skin );
	if(!image) 
	{
		Msg("Warning: %s has null texture %s\n", mod->name, ptexture->name );
		image = r_notexture;
	}
	ptexture->index = image->texnum[0];

	return image;
}
*/

studiohdr_t *R_StudioLoadHeader( model_t *mod, unsigned *buffer )
{
	int		i;
	byte		*pin;
	studiohdr_t	*phdr;
	mstudiotexture_t	*ptexture;
	
	pin = (byte *)buffer;
	phdr = (studiohdr_t *)pin;

	if (phdr->version != STUDIO_VERSION)
	{
		Host_Error("%s has wrong version number (%i should be %i)\n", phdr->name, phdr->version, STUDIO_VERSION);
		return NULL;
	}	

	ptexture = (mstudiotexture_t *)(pin + phdr->textureindex);
/*
	if (phdr->textureindex > 0 && phdr->numtextures <= MAXSTUDIOSKINS)
	{
		mod->numtexinfo = phdr->numtextures; //save info
		for (i = 0; i < phdr->numtextures; i++)
			mod->skins[i] = R_StudioLoadTexture( mod, &ptexture[i], pin );
	}
*/
	return (studiohdr_t *)buffer;
}

void R_StudioLoadModel (model_t *mod, void *buffer)
{
	studiohdr_t	*phdr = R_StudioLoadHeader( mod, buffer );
          studiohdr_t	*thdr;
	
	if(!phdr) return; //there were problems
	mod->phdr = (studiohdr_t *)Hunk_AllocName(LittleLong(phdr->length), "");
          memcpy(mod->phdr, buffer, LittleLong(phdr->length));
	
	if (phdr->numtextures == 0)
	{
		//buffer = FS_LoadFile ( R_ExtName( mod ), NULL );//use buffer again
		if(buffer)
		   thdr = R_StudioLoadHeader( mod, buffer );
		else
		   Con_Printf ("Warning: textures for %s not found!\n", mod->name );
	          

		if(!thdr) return; //there were problems
		mod->thdr = (studiohdr_t *)Hunk_AllocName(LittleLong(thdr->length), "");
          	memcpy(mod->thdr, buffer, LittleLong(thdr->length));
	}
          else mod->thdr = mod->phdr; //just make link

	R_StudioExtractBbox( phdr, 0, mod->mins, mod->maxs );
	//mod->registration_sequence = registration_sequence;
}

/*
====================
MISC STUDIO UTILS
====================
*/

//extract texture filename from modelname
char *R_ExtName( model_t *mod )
{
	static char texname[MAX_QPATH];
	strcpy( texname, mod->name );
	strcpy( &texname[strlen(texname) - 4], "T.mdl" );
	return texname;
}

// extract bbox from animation
int R_ExtractBbox( int sequence, float *mins, float *maxs )
{
	return R_StudioExtractBbox( m_pStudioHeader, sequence, mins, maxs );
}

int R_StudioExtractBbox( studiohdr_t *phdr, int sequence, float *mins, float *maxs )
{
	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)phdr + phdr->seqindex);

	if(sequence == -1) return 0;
	
	VectorCopy( pseqdesc[ sequence ].bbmin, mins );
	VectorCopy( pseqdesc[ sequence ].bbmax, maxs );

	return 1;
}


void SetBodygroup( int iGroup, int iValue )
{
	int iCurrent;
	mstudiobodyparts_t *m_pBodyPart;

	if (iGroup > m_pStudioHeader->numbodyparts)
		return;

	m_pBodyPart = (mstudiobodyparts_t *)((byte *)m_pStudioHeader + m_pStudioHeader->bodypartindex) + iGroup;

	if (iValue >= m_pBodyPart->nummodels)
		return;

	iCurrent = (m_pCurrentEntity->body / m_pBodyPart->base) % m_pBodyPart->nummodels;
	m_pCurrentEntity->body = (m_pCurrentEntity->body - (iCurrent * m_pBodyPart->base) + (iValue * m_pBodyPart->base));
}

int R_StudioGetBodygroup( int iGroup )
{
	mstudiobodyparts_t *m_pBodyPart;
	
	if (iGroup > m_pStudioHeader->numbodyparts)
		return 0;

	m_pBodyPart = (mstudiobodyparts_t *)((byte *)m_pStudioHeader + m_pStudioHeader->bodypartindex) + iGroup;

	if (m_pBodyPart->nummodels <= 1)
		return 0;

	return (m_pCurrentEntity->body / m_pBodyPart->base) % m_pBodyPart->nummodels;
}


/*
====================
StudioCalcBoneAdj

====================
*/
void R_StudioCalcBoneAdj( float dadt, float *adj, const byte *pcontroller1, const byte *pcontroller2, byte mouthopen )
{
	int i, j;
	float value;
	mstudiobonecontroller_t *pbonecontroller;
	
	pbonecontroller = (mstudiobonecontroller_t *)((byte *)m_pStudioHeader + m_pStudioHeader->bonecontrollerindex);

	for (j = 0; j < m_pStudioHeader->numbonecontrollers; j++)
	{
		i = pbonecontroller[j].index;

		if( i == 4) //mouth hardcoded at controller 4
		{
			value = mouthopen / 64.0;
			if (value > 1.0) value = 1.0;				
			value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			// Msg("%d %f\n", mouthopen, value );
		}
		else if (i <= MAXSTUDIOCONTROLLERS )
		{
			// check for 360% wrapping
			if (pbonecontroller[j].type & STUDIO_RLOOP)
			{
				if (abs(pcontroller1[i] - pcontroller2[i]) > 128)
				{
					int a, b;
					a = (pcontroller1[j] + 128) % 256;
					b = (pcontroller2[j] + 128) % 256;
					value = ((a * dadt) + (b * (1 - dadt)) - 128) * (360.0/256.0) + pbonecontroller[j].start;
				}
				else 
				{
					value = ((pcontroller1[i] * dadt + (pcontroller2[i]) * (1.0 - dadt))) * (360.0/256.0) + pbonecontroller[j].start;
				}
			}
			else 
			{
				value = (pcontroller1[i] * dadt + pcontroller2[i] * (1.0 - dadt)) / 255.0;
				if (value < 0) value = 0;
				if (value > 1.0) value = 1.0;
				value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
			// Con_Printf("%d %d %f : %f\n", m_pCurrentEntity->curstate.controller[j], m_pCurrentEntity->latched.prevcontroller[j], value, dadt );
		}

		switch(pbonecontroller[j].type & STUDIO_TYPES)
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			adj[j] = value * (M_PI / 180.0);
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			adj[j] = value;
			break;
		}
	}
}

/*
====================
StudioCalcBoneQuaterion

====================
*/
void R_StudioCalcBoneQuaterion( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q )
{
	int	j, k;
	vec4_t	q1, q2;
	vec3_t	angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j+3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j+3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j+3]);
			k = frame;
			
			// debug
			if (panimvalue->num.total < panimvalue->num.valid) k = 0;
			
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// Bah, missing blend!
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k+1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k+2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid+2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if (panimvalue->num.total > k + 1)
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
			angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
		}

		if (pbone->bonecontroller[j+3] != -1)
		{
			angle1[j] += adj[pbone->bonecontroller[j+3]];
			angle2[j] += adj[pbone->bonecontroller[j+3]];
		}
	}

	if (!VectorCompare( angle1, angle2 ))
	{
		AngleQuaternion( angle1, q1 );
		AngleQuaternion( angle2, q2 );
		QuaternionSlerp( q1, q2, s, q );
	}
	else
	{
		AngleQuaternion( angle1, q );
	}
}

/*
====================
StudioCalcBonePosition

====================
*/
void R_StudioCalcBonePosition( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *pos )
{
	int j, k;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j]; // default;
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);
			
			//if (j == 0) Con_Printf("%d  %d:%d  %f\n", frame, panimvalue->num.valid, panimvalue->num.total, s );
			k = frame;

			// debug
			if (panimvalue->num.total < panimvalue->num.valid) k = 0;
			// find span of values that includes the frame we want
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
  				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// if we're inside the span
			if (panimvalue->num.valid > k)
			{
				// and there's more data in the span
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k+1].value * (1.0 - s) + s * panimvalue[k+2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k+1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if (panimvalue->num.total <= k + 1)
				{
					pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0 - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if ( pbone->bonecontroller[j] != -1 && adj )
		{
			pos[j] += adj[pbone->bonecontroller[j]];
		}
	}
}

/*
====================
StudioSlerpBones

====================
*/
void R_StudioSlerpBones( vec4_t q1[], float pos1[][3], vec4_t q2[], float pos2[][3], float s )
{
	int	i;
	vec4_t	q3;
	float	s1;

	if (s < 0) s = 0;
	else if (s > 1.0) s = 1.0;

	s1 = 1.0 - s;

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		QuaternionSlerp( q1[i], q2[i], s, q3 );
		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];
		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}

/*
====================
StudioGetAnim

====================
*/
mstudioanim_t *R_StudioGetAnim( model_t *m_pSubModel, mstudioseqdesc_t *pseqdesc )
{
	mstudioseqgroup_t	*pseqgroup;
	cache_user_t *paSequences;
          byte *buf;
          int filesize;
	
	//WARNING! Not tested!!!
	
	pseqgroup = (mstudioseqgroup_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqgroupindex) + pseqdesc->seqgroup;

	if (pseqdesc->seqgroup == 0)
	{
		return (mstudioanim_t *)((byte *)m_pStudioHeader + pseqgroup->data + pseqdesc->animindex);
	}

	paSequences = (cache_user_t *)m_pSubModel->submodels;

	if (paSequences == NULL)
	{
		Con_Printf("loading %s\n", pseqgroup->name );
		//buf = FS_LoadFile (pseqgroup->name, &filesize);
		if (!buf)
		{
			Sys_Error("R_StudioGetAnim: %s not found", pseqgroup->name);
			memset (pseqgroup->name, 0, sizeof(pseqgroup->name));
			return NULL;
		}
        if(IDSEQGRPHEADER == LittleLong(*(unsigned *)buf))  //it's sequence group
        {
			byte		*pin = (byte *)buf;
			mstudioseqgroup_t	*pseqhdr = (mstudioseqgroup_t *)pin;
			
			paSequences = (cache_user_t *)Hunk_AllocName(LittleLong(filesize),"");
          	//m_pSubModel->submodels = (mmodel_t *)paSequences;
          		
          	memcpy((struct cache_user_s *)&paSequences[pseqdesc->seqgroup], buf, LittleLong(filesize));
		}		
	}
	return (mstudioanim_t *)((byte *)paSequences[pseqdesc->seqgroup].data + pseqdesc->animindex);
}

/*
====================
StudioSetUpTransform

====================
*/
void R_StudioSetUpTransform ( void )
{
	int	i, j;
	vec3_t	angles, modelpos;

	VectorCopy( m_pCurrentEntity->origin, modelpos );

	// TODO: should really be stored with the entity instead of being reconstructed
	// TODO: should use a look-up table
	// TODO: could cache lazily, stored in the entity

	angles[PITCH] = m_pCurrentEntity->angles[PITCH];
	angles[YAW] = m_pCurrentEntity->angles[YAW];
	angles[ROLL] = m_pCurrentEntity->angles[ROLL];

	if (m_pCurrentEntity->movetype == MOVETYPE_STEP)
	{
		float		f = 0;
		float		d;
                    mstudioseqdesc_t	*pseqdesc;//acess to studio flags
                    
		// don't do it if the goalstarttime hasn't updated in a while.
                    pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->sequence;
		
		// NOTE:  Because we need to interpolate multiplayer characters, the interpolation time limit
		//  was increased to 1.0 s., which is 2x the max lag we are accounting for.
		if ( ( r_newrefdef.time < m_pCurrentEntity->animtime + 1.0f ) && ( m_pCurrentEntity->animtime != m_pCurrentEntity->prev.animtime ) )
		{
			f = (r_newrefdef.time - m_pCurrentEntity->animtime) / (m_pCurrentEntity->animtime - m_pCurrentEntity->prev.animtime);
			//Msg("%4.2f %.2f %.2f\n", f, m_pCurrentEntity->animtime, r_newrefdef.time);
		}

		if (m_fDoInterp)
		{
			// ugly hack to interpolate angle, position. current is reached 0.1 seconds after being set
			f = f - 1.0;
		}
		else
		{
			f = 0;
		}

		if (pseqdesc->motiontype & STUDIO_LX )//enable interpolation only for walk\run
			for (i = 0; i < 3; i++) modelpos[i] += (m_pCurrentEntity->origin[i] - m_pCurrentEntity->prev.origin[i]) * f;

		for (i = 0; i < 3; i++)
		{
			float ang1, ang2;

			ang1 = m_pCurrentEntity->angles[i];
			ang2 = m_pCurrentEntity->prev.angles[i];

			d = ang1 - ang2;
			if (d > 180) d -= 360;
			else if (d < -180) d += 360;
			angles[i] += d * f;
		}
		//Msg("%.3f \n", f );
	}
	else if ( m_pCurrentEntity->movetype != MOVETYPE_NONE ) 
	{
		VectorCopy( m_pCurrentEntity->angles, angles );
	}

	//Msg("%.0f %0.f %0.f\n", modelpos[0], modelpos[1], modelpos[2] );
	//Msg("%.0f %0.f %0.f\n", angles[0], angles[1], angles[2] );
	AngleMatrix (angles, m_protationmatrix);

	m_protationmatrix[0][3] = modelpos[0];
	m_protationmatrix[1][3] = modelpos[1];
	m_protationmatrix[2][3] = modelpos[2];

	if (m_pCurrentEntity->scale != 0)
	{
		for (i = 0; i < 3; i++) 
			for (j = 0; j < 3; j++)
				m_protationmatrix[i][j] *= m_pCurrentEntity->scale;
	}
}


/*
====================
StudioEstimateInterpolant

====================
*/
float R_StudioEstimateInterpolant( void )
{
	float dadt = 1.0;

	if ( m_fDoInterp && ( m_pCurrentEntity->animtime >= m_pCurrentEntity->prev.animtime + 0.01 ) )
	{
		dadt = (r_newrefdef.time - m_pCurrentEntity->animtime) / 0.1;
		if (dadt > 2.0) dadt = 2.0;
	}
	return dadt;
}


/*
====================
StudioCalcRotations

====================
*/
void R_StudioCalcRotations ( float pos[][3], vec4_t *q, mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f )
{
	int		i;
	int		frame;
	mstudiobone_t	*pbone;

	float	s;
	float	adj[MAXSTUDIOCONTROLLERS];
	float	dadt;

	if (f > pseqdesc->numframes - 1) f = 0;	// bah, fix this bug with changing sequences too fast
	else if ( f < -0.01 ) f = -0.01;
	// BUG ( somewhere else ) but this code should validate this data.
	// This could cause a crash if the frame # is negative, so we'll go ahead
	//  and clamp it here

	frame = (int)f;

	// Msg("%d %.4f %.4f %.4f %.4f %d\n", m_pCurrentEntity->curstate.sequence, m_clTime, m_pCurrentEntity->animtime, m_pCurrentEntity->frame, f, frame );
	// Msg( "%f %f %f\n", m_pCurrentEntity->angles[ROLL], m_pCurrentEntity->angles[PITCH], m_pCurrentEntity->angles[YAW] );
	// Msg("frame %d %d\n", frame1, frame2 );

	dadt = R_StudioEstimateInterpolant();
	s = (f - frame);

	// add in programtic controllers
	pbone = (mstudiobone_t *)((byte *)m_pStudioHeader + m_pStudioHeader->boneindex);

	R_StudioCalcBoneAdj( dadt, adj, m_pCurrentEntity->controller, m_pCurrentEntity->prev.controller, m_pCurrentEntity->mouth );

	for (i = 0; i < m_pStudioHeader->numbones; i++, pbone++, panim++) 
	{
		R_StudioCalcBoneQuaterion( frame, s, pbone, panim, adj, q[i] );
		R_StudioCalcBonePosition( frame, s, pbone, panim, adj, pos[i] );
		// if (0 && i == 0) Msg("%d %d %d %d\n", m_pCurrentEntity->sequence, frame, j, k );
	}

	if (pseqdesc->motiontype & STUDIO_X) pos[pseqdesc->motionbone][0] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Y) pos[pseqdesc->motionbone][1] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Z) pos[pseqdesc->motionbone][2] = 0.0;

	s = 0 * ((1.0 - (f - (int)(f))) / (pseqdesc->numframes)) * m_pCurrentEntity->framerate;

	if (pseqdesc->motiontype & STUDIO_LX) pos[pseqdesc->motionbone][0] += s * pseqdesc->linearmovement[0];
	if (pseqdesc->motiontype & STUDIO_LY) pos[pseqdesc->motionbone][1] += s * pseqdesc->linearmovement[1];
	if (pseqdesc->motiontype & STUDIO_LZ) pos[pseqdesc->motionbone][2] += s * pseqdesc->linearmovement[2];
}

/*
====================
Studio_FxTransform

====================
*/
void R_StudioFxTransform( entity_t *ent, matrix3x4 transform )
{
	//TODO: add here some effects :)
}

/*
====================
StudioEstimateFrame

====================
*/
float R_StudioEstimateFrame( mstudioseqdesc_t *pseqdesc )
{
	double dfdt, f;
	
	if ( m_fDoInterp )
	{
		if ( r_newrefdef.time < m_pCurrentEntity->animtime ) dfdt = 0;
		else dfdt = (r_newrefdef.time - m_pCurrentEntity->animtime) * m_pCurrentEntity->framerate * pseqdesc->fps;
	}
	else dfdt = 0;

	if (pseqdesc->numframes <= 1) f = 0;
	else f = (m_pCurrentEntity->frame * (pseqdesc->numframes - 1)) / 256.0;
 
	f += dfdt;

	if (pseqdesc->flags & STUDIO_LOOPING) 
	{
		if (pseqdesc->numframes > 1) f -= (int)(f / (pseqdesc->numframes - 1)) *  (pseqdesc->numframes - 1);
		if (f < 0) f += (pseqdesc->numframes - 1);
	}
	else 
	{
		if (f >= pseqdesc->numframes - 1.001) f = pseqdesc->numframes - 1.001;
		if (f < 0.0)  f = 0.0;
	}

	return f;
}

/*
====================
StudioSetupBones

====================
*/
void R_StudioSetupBones ( void )
{
	int		i;
	double		f;

	mstudiobone_t	*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t	*panim;

	static float	pos[MAXSTUDIOBONES][3];
	static vec4_t	q[MAXSTUDIOBONES];
	matrix3x4		bonematrix;

	static float	pos2[MAXSTUDIOBONES][3];
	static vec4_t	q2[MAXSTUDIOBONES];
	static float	pos3[MAXSTUDIOBONES][3];
	static vec4_t	q3[MAXSTUDIOBONES];
	static float	pos4[MAXSTUDIOBONES][3];
	static vec4_t	q4[MAXSTUDIOBONES];

	if (m_pCurrentEntity->sequence >=  m_pStudioHeader->numseq) m_pCurrentEntity->sequence = 0;
	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->sequence;

	f = R_StudioEstimateFrame( pseqdesc );

	if (m_pCurrentEntity->prev.frame > f)
	{
		//Msg("%f %f\n", m_pCurrentEntity->prev.frame, f );
	}

	panim = R_StudioGetAnim( m_pRenderModel, pseqdesc );
	R_StudioCalcRotations( pos, q, pseqdesc, panim, f );

	if (pseqdesc->numblends > 1)
	{
		float	s;
		float	dadt;

		panim += m_pStudioHeader->numbones;
		R_StudioCalcRotations( pos2, q2, pseqdesc, panim, f );

		dadt = R_StudioEstimateInterpolant();
		s = (m_pCurrentEntity->blending[0] * dadt + m_pCurrentEntity->prev.blending[0] * (1.0 - dadt)) / 255.0;

		R_StudioSlerpBones( q, pos, q2, pos2, s );

		if (pseqdesc->numblends == 4)
		{
			panim += m_pStudioHeader->numbones;
			R_StudioCalcRotations( pos3, q3, pseqdesc, panim, f );

			panim += m_pStudioHeader->numbones;
			R_StudioCalcRotations( pos4, q4, pseqdesc, panim, f );

			s = (m_pCurrentEntity->blending[0] * dadt + m_pCurrentEntity->prev.blending[0] * (1.0 - dadt)) / 255.0;
			R_StudioSlerpBones( q3, pos3, q4, pos4, s );

			s = (m_pCurrentEntity->blending[1] * dadt + m_pCurrentEntity->prev.blending[1] * (1.0 - dadt)) / 255.0;
			R_StudioSlerpBones( q, pos, q3, pos3, s );
		}
	}
	
	if (m_fDoInterp && m_pCurrentEntity->prev.sequencetime && ( m_pCurrentEntity->prev.sequencetime + 0.2 > r_newrefdef.time) && ( m_pCurrentEntity->prev.sequence < m_pStudioHeader->numseq ))
	{
		// blend from last sequence
		static float  pos1b[MAXSTUDIOBONES][3];
		static vec4_t q1b[MAXSTUDIOBONES];
		float s;

		pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->prev.sequence;
		panim = R_StudioGetAnim( m_pRenderModel, pseqdesc );
		// clip prevframe
		R_StudioCalcRotations( pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->prev.frame );

		if (pseqdesc->numblends > 1)
		{
			panim += m_pStudioHeader->numbones;
			R_StudioCalcRotations( pos2, q2, pseqdesc, panim, m_pCurrentEntity->prev.frame );

			s = (m_pCurrentEntity->prev.seqblending[0]) / 255.0;
			R_StudioSlerpBones( q1b, pos1b, q2, pos2, s );

			if (pseqdesc->numblends == 4)
			{
				panim += m_pStudioHeader->numbones;
				R_StudioCalcRotations( pos3, q3, pseqdesc, panim, m_pCurrentEntity->prev.frame );

				panim += m_pStudioHeader->numbones;
				R_StudioCalcRotations( pos4, q4, pseqdesc, panim, m_pCurrentEntity->prev.frame );

				s = (m_pCurrentEntity->prev.seqblending[0]) / 255.0;
				R_StudioSlerpBones( q3, pos3, q4, pos4, s );

				s = (m_pCurrentEntity->prev.seqblending[1]) / 255.0;
				R_StudioSlerpBones( q1b, pos1b, q3, pos3, s );
			}
		}

		s = 1.0 - (r_newrefdef.time - m_pCurrentEntity->prev.sequencetime) / 0.2;
		R_StudioSlerpBones( q, pos, q1b, pos1b, s );
	}
	else
	{
		//Msg("prevframe = %4.2f\n", f);
		m_pCurrentEntity->prev.frame = f;
	}

	pbones = (mstudiobone_t *)((byte *)m_pStudioHeader + m_pStudioHeader->boneindex);

	for (i = 0; i < m_pStudioHeader->numbones; i++) 
	{
		QuaternionMatrix( q[i], bonematrix );

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1) 
		{
			R_ConcatTransforms (m_protationmatrix, bonematrix, m_pbonestransform[i]);

			// MatrixCopy should be faster...
			//ConcatTransforms (m_protationmatrix, bonematrix, m_plighttransform[i]);
			MatrixCopy( m_pbonestransform[i], m_plighttransform[i] );
			// Apply client-side effects to the transformation matrix
			R_StudioFxTransform( m_pCurrentEntity, m_pbonestransform[i] );
		} 
		else 
		{
			R_ConcatTransforms (m_pbonestransform[pbones[i].parent], bonematrix, m_pbonestransform[i]);
			R_ConcatTransforms (m_plighttransform[pbones[i].parent], bonematrix, m_plighttransform[i]);
		}
	}
}


/*
====================
StudioSaveBones

====================
*/
void R_StudioSaveBones( void )
{
	int i;
	mstudiobone_t *pbones = (mstudiobone_t *)((byte *)m_pStudioHeader + m_pStudioHeader->boneindex);
	m_nCachedBones = m_pStudioHeader->numbones;

	for (i = 0; i < m_pStudioHeader->numbones; i++) 
	{
		strcpy( m_nCachedBoneNames[i], pbones[i].name );
		MatrixCopy( m_pbonestransform[i], m_rgCachedBonesTransform[i] );
		MatrixCopy( m_plighttransform[i], m_rgCachedLightTransform[i] );
	}
}

void R_StudioRestoreBones( void )
{
}

/*
====================
StudioMergeBones

====================
*/
void R_StudioMergeBones ( model_t *m_pSubModel )
{
	int	i, j;
	double	f;
	int	do_hunt = true;

	mstudiobone_t	*pbones;
	mstudioseqdesc_t	*pseqdesc;
	mstudioanim_t	*panim;
	matrix3x4		bonematrix;

	static vec4_t	q[MAXSTUDIOBONES];
	static float	pos[MAXSTUDIOBONES][3];

	if (m_pCurrentEntity->sequence >=  m_pStudioHeader->numseq) m_pCurrentEntity->sequence = 0;
	pseqdesc = (mstudioseqdesc_t *)((byte *)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->sequence;

	f = R_StudioEstimateFrame( pseqdesc );

	//if (m_pCurrentEntity->prev.frame > f) Msg("%f %f\n", m_pCurrentEntity->prev.frame, f );

	panim = R_StudioGetAnim( m_pSubModel, pseqdesc );
	R_StudioCalcRotations( pos, q, pseqdesc, panim, f );
	pbones = (mstudiobone_t *)((byte *)m_pStudioHeader + m_pStudioHeader->boneindex);

	for (i = 0; i < m_pStudioHeader->numbones; i++) 
	{
		for (j = 0; j < m_nCachedBones; j++)
		{
			if (stricmp(pbones[i].name, m_nCachedBoneNames[j]) == 0)
			{
				MatrixCopy( m_rgCachedBonesTransform[j], m_pbonestransform[i] );
				MatrixCopy( m_rgCachedLightTransform[j], m_plighttransform[i] );
				break;
			}
		}
		if (j >= m_nCachedBones)
		{
			QuaternionMatrix( q[i], bonematrix );

			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];

			if (pbones[i].parent == -1) 
			{
				R_ConcatTransforms (m_protationmatrix, bonematrix, m_pbonestransform[i]);
				MatrixCopy( m_pbonestransform[i], m_plighttransform[i] );
				// Apply client-side effects to the transformation matrix
				R_StudioFxTransform( m_pCurrentEntity, m_pbonestransform[i] );
			} 
			else 
			{
				R_ConcatTransforms (m_pbonestransform[pbones[i].parent], bonematrix, m_pbonestransform[i]);
				R_ConcatTransforms (m_plighttransform[pbones[i].parent], bonematrix, m_plighttransform[i]);
			}
		}
	}
}


/*
====================
StudioCalcAttachments

====================
*/
void R_StudioCalcAttachments( void )
{
	int i;
	mstudioattachment_t *pattachment;

	if ( m_pStudioHeader->numattachments > MAXSTUDIOATTACHMENTS )
	{
		Msg("Warning: Too many attachments on %s\n", m_pCurrentEntity->model->name );
		m_pStudioHeader->numattachments = MAXSTUDIOATTACHMENTS;//reduce it
	}

	// calculate attachment points
	pattachment = (mstudioattachment_t *)((byte *)m_pStudioHeader + m_pStudioHeader->attachmentindex);
	for (i = 0; i < m_pStudioHeader->numattachments; i++)
	{
		VectorTransform( pattachment[i].org, m_plighttransform[pattachment[i].bone],  m_pCurrentEntity->attachment[i] );
	}
          
	// copy attachments into global entity array
	memcpy( m_pCurrentEntity->attachment, m_pCurrentEntity->attachment, sizeof( vec3_t ) * MAXSTUDIOATTACHMENTS );
}

static bool R_StudioComputeBBox( vec3_t *bbox )
{
	vec3_t vectors[3];
	entity_t *e = m_pCurrentEntity;
	vec3_t mins, maxs, tmp, angles;
	int i, seq = m_pCurrentEntity->sequence;

	if(!R_ExtractBbox( seq, mins, maxs ))
		return false;

	//compute a full bounding box
	for ( i = 0; i < 8; i++ )
	{
		if ( i & 1 ) tmp[0] = mins[0];
		else tmp[0] = maxs[0];
		if ( i & 2 ) tmp[1] = mins[1];
		else tmp[1] = maxs[1];
		if ( i & 4 ) tmp[2] = mins[2];
		else tmp[2] = maxs[2];
		VectorCopy( tmp, bbox[i] );
	}

	//rotate the bounding box
	VectorCopy( e->angles, angles );
	angles[PITCH] = -angles[PITCH];
	AngleVectors( angles, vectors[0], vectors[1], vectors[2] );

	for ( i = 0; i < 8; i++ )
	{
		VectorCopy( bbox[i], tmp );
		bbox[i][0] = DotProduct( vectors[0], tmp );
		bbox[i][1] = -DotProduct( vectors[1], tmp );
		bbox[i][2] = DotProduct( vectors[2], tmp );
		VectorAdd( e->origin, bbox[i], bbox[i] );
	}
	return true;
}

static bool R_StudioCheckBBox( void )
{
	int i, j;
	vec3_t bbox[8];

	int aggregatemask = ~0;

	if(m_pCurrentEntity->flags & RF_WEAPONMODEL)
		return true;          

	if(!R_StudioComputeBBox( bbox ))
          	return false;
	
	for ( i = 0; i < 8; i++ )
	{
		int mask = 0;
		for ( j = 0; j < 4; j++ )
		{
			float dp = DotProduct( frustum[j].normal, bbox[i] );
			if ( ( dp - frustum[j].dist ) < 0 ) mask |= ( 1 << j );
		}
		aggregatemask &= mask;
	}
          
	if ( aggregatemask )
		return false;
	return true;
}

/*
====================
StudioSetupModel

====================
*/
void R_StudioSetupModel ( int bodypart )
{
	int index;

	if (bodypart > m_pStudioHeader->numbodyparts) bodypart = 0;
	m_pBodyPart = (mstudiobodyparts_t *)((byte *)m_pStudioHeader + m_pStudioHeader->bodypartindex) + bodypart;

	index = m_pCurrentEntity->body / m_pBodyPart->base;
	index = index % m_pBodyPart->nummodels;

	m_pSubModel = (mstudiomodel_t *)((byte *)m_pStudioHeader + m_pBodyPart->modelindex) + index;
}

void R_StudioSetupLighting( void )
{
	int i;
          mstudiobone_t *pbone;
	
	//get light from floor
	m_plightvec[0] = 0.0f;
	m_plightvec[1] = 0.0f;
	m_plightvec[2] = -1.0f;

	if(currententity->flags & RF_FULLBRIGHT)
	{
		for (i = 0; i < 3; i++)
			m_plightcolor[i] = 1.0f;
	}
	else
	{
		R_LightPoint (m_pCurrentEntity->origin, m_plightcolor );
	
		if ( m_pCurrentEntity->flags & RF_WEAPONMODEL )
			r_lightlevel->value = bound(0, VectorLength(m_plightcolor) * 75.0f, 255); 

	}

	// TODO: only do it for bones that actually have textures
	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		pbone = (mstudiobone_t *)((byte *)m_pStudioHeader + m_pStudioHeader->boneindex + i);
		//if(pbone->flags & STUDIO_HAS_CHROME)
		{
			VectorIRotate( m_plightvec, m_pbonestransform[i], m_blightvec[i] );
		}
	}
}

void R_StudioLighting (float *lv, int bone, int flags, vec3_t normal)
{
	float lightcos;

	float shadelight = 192.0f;
	float illum = 32.0f;//RF_MINLIGHT & RF_FULLBRIGHT

	if (flags & STUDIO_NF_FLATSHADE)
	{
		illum += shadelight * 0.8f;
	}
          else
          {
		lightcos = DotProduct (normal, m_blightvec[bone]);// -1 colinear, 1 opposite
		if (lightcos > 1.0) lightcos = 1;

		illum += shadelight;
		lightcos = (lightcos + 0.5f) / 1.5f;// do modified hemispherical lighting
		if (lightcos > 0.0) illum -= shadelight * lightcos; 
	}
	illum = bound( 0, illum, 255);

	*lv = illum / 255.0; // Light from 0 to 1.0
}

void R_StudioSetupChrome(int *pchrome, int bone, vec3_t normal)
{
	float n;

	if (g_chromeage[bone] != m_pStudioModelCount)
	{
		// calculate vectors from the viewer to the bone. This roughly adjusts for position
		vec3_t	chromeupvec;	// g_chrome t vector in world reference frame
		vec3_t	chromerightvec;	// g_chrome s vector in world reference frame
		vec3_t	tmp;		// vector pointing at bone in world reference frame

		VectorScale( m_pCurrentEntity->origin, -1, tmp );
		tmp[0] += m_pbonestransform[bone][0][3];
		tmp[1] += m_pbonestransform[bone][1][3];
		tmp[2] += m_pbonestransform[bone][2][3];

		VectorNormalize( tmp );
		CrossProduct( tmp, vright, chromeupvec );
		VectorNormalize( chromeupvec );
		CrossProduct( tmp, chromeupvec, chromerightvec );
		VectorNormalize( chromerightvec );

		VectorIRotate( chromeupvec, m_pbonestransform[bone], g_chromeup[bone] );
		VectorIRotate( chromerightvec, m_pbonestransform[bone], g_chromeright[bone] );
		g_chromeage[bone] = m_pStudioModelCount;
	}

	// calc s coord
	n = DotProduct( normal, g_chromeright[bone] );
	pchrome[0] = (n + 1.0) * 32;// FIXME: make this a float

	// calc t coord
	n = DotProduct( normal, g_chromeup[bone] );
	pchrome[1] = (n + 1.0) * 32;// FIXME: make this a float
}

bool R_AcceptStudioPass( int flags, int pass )
{
	if(pass == RENDERPASS_SOLID)
	{
		if(!flags) return true;			// draw all
		if(flags & STUDIO_NF_CHROME) return true;	// chrome drawing once
		if(flags & STUDIO_NF_ADDITIVE) return false;	// draw it at second pass
		if(flags & STUDIO_NF_TRANSPARENT) return true;	// must be draw first always
	}	
	if(pass == RENDERPASS_ALPHA)
	{
		//pass for blended ents
		if(m_pCurrentEntity->flags & RF_TRANSLUCENT) 	return true;
		if(!flags) return false;			// skip all
		if(flags & STUDIO_NF_TRANSPARENT) return false;	// must be draw first always
		if(flags & STUDIO_NF_ADDITIVE) return true;	// draw it at second pass
		if(flags & STUDIO_NF_CHROME) return false;	// no need draw it again
	}
	return true;
}

void R_StudioDrawMeshes ( mstudiotexture_t * ptexture, short *pskinref, int pass )
{
	int i, j;
	float *av, *lv;
	float lv_tmp;
	vec3_t fbright = {0.95f, 0.95f, 0.95f};
	vec3_t irgoggles = {0.95f, 0.0f, 0.0f};//predefined lightcolor
	int flags;

	mstudiomesh_t *pmesh = (mstudiomesh_t *)((byte *)m_pStudioHeader + m_pSubModel->meshindex);
	byte *pnormbone = ((byte *)m_pStudioHeader + m_pSubModel->norminfoindex);
	vec3_t *pstudionorms = (vec3_t *)((byte *)m_pStudioHeader + m_pSubModel->normindex);
	
	lv = (float *)m_pvlightvalues;
	for (j = 0; j < m_pSubModel->nummesh; j++) 
	{
		flags = ptexture[pskinref[pmesh[j].skinref]].flags;
		if(!R_AcceptStudioPass(flags, pass )) continue;
		
		for (i = 0; i < pmesh[j].numnorms; i++, lv += 3, pstudionorms++, pnormbone++)
		{
			R_StudioLighting (&lv_tmp, *pnormbone, flags, (float *)pstudionorms);
                             
			// FIXME: move this check out of the inner loop
			if (flags & STUDIO_NF_CHROME)
				R_StudioSetupChrome( g_chrome[(float (*)[3])lv - m_pvlightvalues], *pnormbone, (float *)pstudionorms );
			VectorScale(m_plightcolor, lv_tmp, lv );
		}
	}

	for (j = 0; j < m_pSubModel->nummesh; j++) 
	{
		float	s, t;
		short	*ptricmds;

		pmesh = (mstudiomesh_t *)((byte *)m_pStudioHeader + m_pSubModel->meshindex) + j;
		ptricmds = (short *)((byte *)m_pStudioHeader + pmesh->triindex);

		flags = ptexture[pskinref[pmesh->skinref]].flags;

		if(!R_AcceptStudioPass(flags, pass )) 
			continue;

		s = 1.0/(float)ptexture[pskinref[pmesh->skinref]].width;
		t = 1.0/(float)ptexture[pskinref[pmesh->skinref]].height;
/*
		GL_Bind( ptexture[pskinref[pmesh->skinref]].index );

		qglShadeModel (GL_SMOOTH);

		GL_TexEnv( GL_MODULATE );

		while (i = *(ptricmds++))
		{
            int prim;
			if (i < 0)
			{
				prim = GU_TRIANGLE_FAN;
				i = -i;
			}
			else
			{
				prim = GU_TRIANGLE_STRIP;
			}

			// Allocate the vertices.
			struct vertex
			{
				float u, v;
				unsigned int color;
				float x, y, z;
			};

			for(; i > 0; i--, ptricmds += 4)
			{
				if (flags & STUDIO_NF_CHROME)
				{
					qglTexCoord2f(g_chrome[ptricmds[1]][0]*s, g_chrome[ptricmds[1]][1]*t);
                }
				else
				{
				    qglTexCoord2f(ptricmds[2]*s, ptricmds[3]*t);
                }
				lv = m_pvlightvalues[ptricmds[1]];
                                        
                if ( m_pCurrentEntity->flags & RF_FULLBRIGHT )
					lv = &fbright[0];

				if ( m_pCurrentEntity->flags & RF_MINLIGHT )//used for viewmodel only
					VectorBound(0.1f, lv, 1.0f );

				if ( r_newrefdef.rdflags & RDF_IRGOGGLES && m_pCurrentEntity->flags & RF_IR_VISIBLE)
					lv = &irgoggles[0];

				if (flags & STUDIO_NF_ADDITIVE)//additive is self-lighting texture
					qglColor4f( 1.0f, 1.0f, 1.0f, 0.8f );
				else if(m_pCurrentEntity->flags & RF_TRANSLUCENT)
					qglColor4f( 1.0f, 1.0f, 1.0f, m_pCurrentEntity->alpha );
				else qglColor4f( lv[0], lv[1], lv[2], 1.0f );//get light from floor

				av = m_pxformverts[ptricmds[0]];

				qglVertex3f(av[0], av[1], av[2]);
			}
			qglEnd();
		}
*/
	}
}

void R_StudioDrawPoints ( void )
{
	int		i;
	int		m_skinnum = m_pCurrentEntity->skin;
	byte		*pvertbone;
	byte		*pnormbone;
	vec3_t		*pstudioverts;
	vec3_t		*pstudionorms;
	mstudiotexture_t	*ptexture;
	short		*pskinref;
	    
	pvertbone = ((byte *)m_pStudioHeader + m_pSubModel->vertinfoindex);
	pnormbone = ((byte *)m_pStudioHeader + m_pSubModel->norminfoindex);
	ptexture = (mstudiotexture_t *)((byte *)m_pTextureHeader + m_pTextureHeader->textureindex);

	pstudioverts = (vec3_t *)((byte *)m_pStudioHeader + m_pSubModel->vertindex);
	pstudionorms = (vec3_t *)((byte *)m_pStudioHeader + m_pSubModel->normindex);

	pskinref = (short *)((byte *)m_pTextureHeader + m_pTextureHeader->skinindex);
	if (m_skinnum != 0 && m_skinnum < m_pTextureHeader->numskinfamilies)
		pskinref += (m_skinnum * m_pTextureHeader->numskinref);

	for (i = 0; i < m_pSubModel->numverts; i++)
	{
		VectorTransform (pstudioverts[i], m_pbonestransform[pvertbone[i]], m_pxformverts[i]);
	}
/*
	if (currententity->flags & RF_DEPTHHACK) // hack the depth range to prevent view model from poking into walls
		qglDepthRange (gldepthmin, gldepthmin + 0.3 * (gldepthmax-gldepthmin));

	if (( m_pCurrentEntity->flags & RF_WEAPONMODEL ) && ( r_lefthand->value == 1.0F ))
	{
		qglMatrixMode( GL_PROJECTION );
		qglPushMatrix();
		qglLoadIdentity();
		qglScalef( -1, 1, 1 );
	    	qglPerspective( r_newrefdef.fov_y, ( float ) r_newrefdef.width / r_newrefdef.height,  4,  4096);
		qglMatrixMode( GL_MODELVIEW );
		qglCullFace( GL_BACK );
	}          
	if(m_PassNum == RENDERPASS_SOLID && !(m_pCurrentEntity->flags & RF_TRANSLUCENT)) //setup for solid format
	{
		qglEnable (GL_ALPHA_TEST);
		qglBlendFunc(GL_ZERO, GL_ZERO);
                    R_StudioDrawMeshes( ptexture, pskinref, m_PassNum );
		qglColor4f( 1, 1, 1, 1 ); //reset color
		qglDisable(GL_ALPHA_TEST);
	}
	if(m_PassNum == RENDERPASS_ALPHA) //setup for alpha format
	{
		qglEnable(GL_BLEND);
		qglBlendFunc(GL_SRC_ALPHA, GL_ONE);
		R_StudioDrawMeshes( ptexture, pskinref, m_PassNum );
		qglColor4f( 1, 1, 1, 1 ); //reset color
		qglDisable(GL_BLEND);
	}	
	if (( m_pCurrentEntity->flags & RF_WEAPONMODEL ) && ( r_lefthand->value == 1.0F ))
	{
		qglMatrixMode( GL_PROJECTION );
		qglPopMatrix();
		qglMatrixMode( GL_MODELVIEW );
		qglCullFace( GL_FRONT );
	}
*/
}

void R_StudioDrawBones ( void )
{
#if 0
	int i;
	mstudiobone_t *pbones = (mstudiobone_t *) ((byte *)m_pStudioHeader + m_pStudioHeader->boneindex);
	qglDisable (GL_TEXTURE_2D);
	qglDisable (GL_DEPTH_TEST);

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		if (pbones[i].parent >= 0)
		{
			qglPointSize (3.0f);
			qglColor3f (1, 0.7f, 0);
			qglBegin (GL_LINES);
			qglVertex3f (m_pbonestransform[pbones[i].parent][0][3], m_pbonestransform[pbones[i].parent][1][3], m_pbonestransform[pbones[i].parent][2][3]);
			qglVertex3f (m_pbonestransform[i][0][3], m_pbonestransform[i][1][3], m_pbonestransform[i][2][3]);
			qglEnd ();

			qglColor3f (0, 0, 0.8f);
			qglBegin (GL_POINTS);
			if (pbones[pbones[i].parent].parent != -1)
				qglVertex3f (m_pbonestransform[pbones[i].parent][0][3], m_pbonestransform[pbones[i].parent][1][3], m_pbonestransform[pbones[i].parent][2][3]);
			qglVertex3f (m_pbonestransform[i][0][3], m_pbonestransform[i][1][3], m_pbonestransform[i][2][3]);
			qglEnd ();
		}
		else
		{
			// draw parent bone node
			qglPointSize (5.0f);
			qglColor3f (0.8f, 0, 0);
			qglBegin (GL_POINTS);
			qglVertex3f (m_pbonestransform[i][0][3], m_pbonestransform[i][1][3], m_pbonestransform[i][2][3]);
			qglEnd ();
		}
	}

	qglPointSize (1.0f);
	qglEnable (GL_DEPTH_TEST);
	qglEnable (GL_TEXTURE_2D);
#endif
}

void R_StudioDrawHitboxes ( void )
{
#if 0
	int i, j;

	qglDisable (GL_TEXTURE_2D);
	qglDisable (GL_CULL_FACE);

	if (m_pCurrentEntity->alpha < 1.0f ) qglDisable (GL_DEPTH_TEST);
	else qglEnable (GL_DEPTH_TEST);

	qglColor4f (1, 0, 0, 0.5f);

	qglPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	qglEnable (GL_BLEND);
	qglBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (i = 0; i < m_pStudioHeader->numhitboxes; i++)
	{
		mstudiobbox_t *pbboxes = (mstudiobbox_t *) ((byte *) m_pStudioHeader + m_pStudioHeader->hitboxindex);
		vec3_t v[8], v2[8], bbmin, bbmax;

		VectorCopy (pbboxes[i].bbmin, bbmin);
		VectorCopy (pbboxes[i].bbmax, bbmax);

		v[0][0] = bbmin[0];
		v[0][1] = bbmax[1];
		v[0][2] = bbmin[2];

		v[1][0] = bbmin[0];
		v[1][1] = bbmin[1];
		v[1][2] = bbmin[2];

		v[2][0] = bbmax[0];
		v[2][1] = bbmax[1];
		v[2][2] = bbmin[2];

		v[3][0] = bbmax[0];
		v[3][1] = bbmin[1];
		v[3][2] = bbmin[2];

		v[4][0] = bbmax[0];
		v[4][1] = bbmax[1];
		v[4][2] = bbmax[2];

		v[5][0] = bbmax[0];
		v[5][1] = bbmin[1];
		v[5][2] = bbmax[2];

		v[6][0] = bbmin[0];
		v[6][1] = bbmax[1];
		v[6][2] = bbmax[2];

		v[7][0] = bbmin[0];
		v[7][1] = bbmin[1];
		v[7][2] = bbmax[2];

		VectorTransform (v[0], m_pbonestransform[pbboxes[i].bone], v2[0]);
		VectorTransform (v[1], m_pbonestransform[pbboxes[i].bone], v2[1]);
		VectorTransform (v[2], m_pbonestransform[pbboxes[i].bone], v2[2]);
		VectorTransform (v[3], m_pbonestransform[pbboxes[i].bone], v2[3]);
		VectorTransform (v[4], m_pbonestransform[pbboxes[i].bone], v2[4]);
		VectorTransform (v[5], m_pbonestransform[pbboxes[i].bone], v2[5]);
		VectorTransform (v[6], m_pbonestransform[pbboxes[i].bone], v2[6]);
		VectorTransform (v[7], m_pbonestransform[pbboxes[i].bone], v2[7]);

		qglBegin (GL_QUAD_STRIP);
		for (j = 0; j < 10; j++) qglVertex3fv (v2[j & 7]);
		qglEnd ();
	
		qglBegin  (GL_QUAD_STRIP);
		qglVertex3fv (v2[6]);
		qglVertex3fv (v2[0]);
		qglVertex3fv (v2[4]);
		qglVertex3fv (v2[2]);
		qglEnd ();

		qglBegin  (GL_QUAD_STRIP);
		qglVertex3fv (v2[1]);
		qglVertex3fv (v2[7]);
		qglVertex3fv (v2[3]);
		qglVertex3fv (v2[5]);
		qglEnd ();			
	}

	qglPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_CULL_FACE);
	qglEnable (GL_DEPTH_TEST);
#endif
}

void R_StudioDrawAttachments( void )
{
#if 0
	int i;
	
	qglDisable (GL_TEXTURE_2D);
	qglDisable (GL_CULL_FACE);
	qglDisable (GL_DEPTH_TEST);
	
	for (i = 0; i < m_pStudioHeader->numattachments; i++)
	{
		mstudioattachment_t *pattachments = (mstudioattachment_t *) ((byte *)m_pStudioHeader + m_pStudioHeader->attachmentindex);
		vec3_t v[4];
		
		VectorTransform (pattachments[i].org, m_pbonestransform[pattachments[i].bone], v[0]);
		VectorTransform (pattachments[i].vectors[0], m_pbonestransform[pattachments[i].bone], v[1]);
		VectorTransform (pattachments[i].vectors[1], m_pbonestransform[pattachments[i].bone], v[2]);
		VectorTransform (pattachments[i].vectors[2], m_pbonestransform[pattachments[i].bone], v[3]);
		
		qglBegin (GL_LINES);
		qglColor3f (1, 0, 0);
		qglVertex3fv (v[0]);
		qglColor3f (1, 1, 1);
		qglVertex3fv (v[1]);
		qglColor3f (1, 0, 0);
		qglVertex3fv (v[0]);
		qglColor3f (1, 1, 1);
		qglVertex3fv (v[2]);
		qglColor3f (1, 0, 0);
		qglVertex3fv (v[0]);
		qglColor3f (1, 1, 1);
		qglVertex3fv (v[3]);
		qglEnd ();

		qglPointSize (5.0f);
		qglColor3f (0, 1, 0);
		qglBegin (GL_POINTS);
		qglVertex3fv (v[0]);
		qglEnd ();
		qglPointSize (1.0f);
	}
	qglEnable(GL_TEXTURE_2D);
	qglEnable(GL_CULL_FACE);
	qglEnable (GL_DEPTH_TEST);
#endif
}

void R_StudioDrawHulls ( void )
{
#if 0
	int i;
	vec3_t		bbox[8];

	if(m_pCurrentEntity->flags & RF_WEAPONMODEL) return;
	if(!R_StudioComputeBBox( bbox )) return;

	qglDisable( GL_CULL_FACE );
	qglPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	qglDisable( GL_TEXTURE_2D );
	qglBegin( GL_TRIANGLE_STRIP );
	for ( i = 0; i < 8; i++ )
	{
		qglVertex3fv( bbox[i] );
	}
	qglEnd();
	qglEnable( GL_TEXTURE_2D );
	qglPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	qglEnable( GL_CULL_FACE );
#endif
}

extern	vec3_t lightspot;

void R_StudioDrawShadow ( void )
{
	float		an = m_pCurrentEntity->angles[1] / 180 * M_PI;
	
	m_pshadevector[0] = cos(-an);
	m_pshadevector[1] = sin(-an);
	m_pshadevector[2] = 1;
	VectorNormalize (m_pshadevector);
}

/*
====================
StudioRenderModel

====================
*/
void R_StudioRenderModel( void )
{
	int i;
	
	for (i = 0; i < m_pStudioHeader->numbodyparts ; i++)
	{
		R_StudioSetupModel( i );
		R_StudioDrawPoints();
	}

	switch((int)r_drawentities->value)
	{
	case 2: R_StudioDrawBones(); break;
	case 3: R_StudioDrawHitboxes(); break;
	case 4: R_StudioDrawAttachments(); break;
	case 5: R_StudioDrawHulls(); break;
	}
}


/*
====================
StudioDrawModel

====================
*/
void R_DrawStudioModel( int passnum )
{
	m_pCurrentEntity = currententity;
	m_pRenderModel = m_pCurrentEntity->model;
	m_pStudioHeader = m_pRenderModel->phdr;
          m_pTextureHeader = m_pRenderModel->thdr;
	m_PassNum = passnum;

	R_StudioSetUpTransform();

	// see if the bounding box lets us trivially reject, also sets
	if (!R_StudioCheckBBox()) return;
		
	m_pStudioModelCount++; // render data cache cookie
	m_pxformverts = &g_xformverts[0];
	m_pvlightvalues = &g_lightvalues[0];
		
	//nothing to draw
	if (m_pStudioHeader->numbodyparts == 0) return;
	if ( m_pCurrentEntity->flags & RF_WEAPONMODEL && r_lefthand->value == 2) return;

	if (m_pCurrentEntity->movetype == MOVETYPE_FOLLOW) 
		R_StudioMergeBones( m_pRenderModel );
	else R_StudioSetupBones();

	R_StudioSetupLighting( );
	R_StudioSaveBones( );

	//if (flags & STUDIO_EVENTS) R_StudioCalcAttachments();

	R_StudioRenderModel();

	if (m_pCurrentEntity->weaponmodel)
	{
		entity_t saveent = *m_pCurrentEntity;
		model_t *pweaponmodel = m_pCurrentEntity->weaponmodel;
		
		m_pStudioHeader = pweaponmodel->phdr;
		R_StudioMergeBones( pweaponmodel);

		R_StudioRenderModel();
		R_StudioCalcAttachments();
		*m_pCurrentEntity = saveent;
	}
#if 0
	GL_TexEnv( GL_REPLACE );
	qglShadeModel (GL_FLAT);
	if (m_pCurrentEntity->flags & RF_DEPTHHACK) 
		qglDepthRange (gldepthmin, gldepthmax);

	if(r_minimap->value > 1) 
	{
		if(numRadarEnts >= MAX_RADAR_ENTS) return;
		if(currententity->flags & RF_VIEWERMODEL) return;
		if(currententity->flags & RF_WEAPONMODEL) return;

		if( currententity->flags & RF_GLOW)
		{ 
			RadarEnts[numRadarEnts].color[0]= 0.0;
			RadarEnts[numRadarEnts].color[1]= 1.0;
			RadarEnts[numRadarEnts].color[2]= 0.0;
			RadarEnts[numRadarEnts].color[3]= 0.5;
	 	} 
		/*else if( currententity->flags & RF2_MONSTER)
		{ 
			Vector4Set(RadarEnts[numRadarEnts].color, 1.0f, 0.0f, 2.0f, 1.0f ); 
		}*/
		else
		{
			Vector4Set(RadarEnts[numRadarEnts].color, 0.0f, 1.0f, 1.0f, 0.5f ); 
		}
		VectorCopy(currententity->origin, RadarEnts[numRadarEnts].org);
		VectorCopy(currententity->angles, RadarEnts[numRadarEnts].ang);
		numRadarEnts++;
	}

	if (gl_shadows->value && !(m_pCurrentEntity->flags & (RF_TRANSLUCENT | RF_WEAPONMODEL)))
	{
		qglPushMatrix();
		R_RotateForEntity(m_pCurrentEntity);
		qglDisable (GL_TEXTURE_2D);
		qglEnable (GL_BLEND);
		qglColor4f (0.0f, 0.0f, 0.0f, 0.5f );
		R_StudioDrawShadow();
		qglEnable (GL_TEXTURE_2D);
		qglDisable (GL_BLEND);
		qglPopMatrix ();
	}
	qglColor4f(1.0f, 1.0f, 1.0f, 1.0f );
#endif
}
