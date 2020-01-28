/*
Crow_bar 2012(c).
Beams, rope, electric wire physics
*/

extern "C"
{
#include "../quakedef.h"
}

#include <pspgu.h>
#include <pspgum.h>


typedef struct
{
   int          type;
   int          seg;
   float        amp;

   unsigned int color;
   int          texindex;

   vec3_t       start;
   vec3_t       end;
} linep_t;

static linep_t	buf_linep[MAX_LINEP];
static int      count_linep;

void R_InitLinep(void)
{
     R_ClearLinep();
}

void R_ClearLinep(void)
{
    int i;
	for(i = 0; i < MAX_LINEP; i++)
	{
		buf_linep[i].type = 0;
		buf_linep[i].seg = 0;
		buf_linep[i].amp = 0;

		buf_linep[i].color = 0xFFFFFFFF;
		buf_linep[i].texindex = -1;

		buf_linep[i].start[0] = 0;
		buf_linep[i].start[1] = 0;
		buf_linep[i].start[2] = 0;

		buf_linep[i].end[0] = 0;
		buf_linep[i].end[1] = 0;
		buf_linep[i].end[2] = 0;
	}
	count_linep = 0;
}

void R_ShutdownLinep(void)
{
    R_ClearLinep();
}

void R_Draw_Linep(void)
{
	int   i, j;

	for(i = 0; i < MAX_LINEP; i++)
	{
	  	vec3_t dir, point;
		float length;
		float step;

		if(buf_linep[i].type != 0)
		   continue;

		VectorSubtract( buf_linep[i].end, buf_linep[i].start, dir );
		length = VectorLength( dir );
	        VectorNormalize( dir );
	        step = length / buf_linep[i].seg;

		// Allocate the vertices.
		struct vertex
		{
			float x, y, z;
		};
	        vertex* const vertices = static_cast<vertex*>(sceGuGetMemory(sizeof(vertex) * (buf_linep[i].seg+1)));
		vertices[0].x = buf_linep[i].start[0];
		vertices[0].y = buf_linep[i].start[1];
		vertices[0].z = buf_linep[i].start[2];

		VectorCopy(buf_linep[i].start, point);

		for(j = 1; j < (num_seg+1); j++)
		{
		    VectorMA (point, step, dir, point);

		    vertices[j].x = point[0];
		    vertices[j].y = point[1];
		    vertices[j].z = point[2];
		}

		sceGuDrawArray(GU_LINE_STRIP, GU_VERTEX_32BITF|GU_TRANSFORM_3D, (buf_linep[i].seg+1), 0, vertices);
	}
}

int R_AddNewLinep(int type, int seg, float amp, unsigned int color, int texindex, vec3_t start, vec3_t end)
{
	int i, freeindex;
	for(i = 0; i < MAX_LINEP; i++)
	{
		if(buf_linep[i].type != 0)
		   continue;
		buf_linep[i].type = type;
		buf_linep[i].seg = seg;
		buf_linep[i].amp = amp;
		buf_linep[i].color = color;
		buf_linep[i].texindex = texinde;
		buf_linep[i].start = start;
		buf_linep[i].end = end;
		return 1;
	}
	return 0;
}




