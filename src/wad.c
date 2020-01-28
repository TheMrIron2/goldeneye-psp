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
// wad.c

#include "quakedef.h"
#include <pspgu.h>
int			wad_numlumps;
lumpinfo_t	*wad_lumps;
byte		*wad_base;

void SwapPic (qpic_t *pic);

/*
==================
W_CleanupName

Lowercases name and pads with spaces and a terminating 0 to the length of
lumpinfo_t->name.
Used so lumpname lookups can proceed rapidly by comparing 4 chars at a time
Space padding is so names can be printed nicely in tables.
Can safely be performed in place.
==================
*/
void W_CleanupName (char *in, char *out)
{
	int		i;
	int		c;
	
	for (i=0 ; i<16 ; i++ )
	{
		c = in[i];
		if (!c)
			break;
			
		if (c >= 'A' && c <= 'Z')
			c += ('a' - 'A');
		out[i] = c;
	}
	
	for ( ; i< 16 ; i++ )
		out[i] = 0;
}



/*
====================
W_LoadWadFile
====================
*/
void W_LoadWadFile (char *filename)
{
	lumpinfo_t		*lump_p;
	wadinfo_t		*header;
	unsigned		i;
	int				infotableofs;
	
	wad_base = COM_LoadHunkFile (filename);
	if (!wad_base)
		Sys_Error ("W_LoadWadFile: couldn't load %s", filename);

	header = (wadinfo_t *)wad_base;
	
	if (header->identification[0] != 'W'
	|| header->identification[1] != 'A'
	|| header->identification[2] != 'D'
	|| header->identification[3] != '2')
		Sys_Error ("Wad file %s doesn't have WAD2 id\n",filename);
		
	wad_numlumps = LittleLong(header->numlumps);
	infotableofs = LittleLong(header->infotableofs);
	wad_lumps = (lumpinfo_t *)(wad_base + infotableofs);
	
	for (i=0, lump_p = wad_lumps ; i<wad_numlumps ; i++,lump_p++)
	{
		lump_p->filepos = LittleLong(lump_p->filepos);
		lump_p->size = LittleLong(lump_p->size);
		W_CleanupName (lump_p->name, lump_p->name);
		if (lump_p->type == TYP_QPIC)
			SwapPic ( (qpic_t *)(wad_base + lump_p->filepos));
	}
}


/*
=============
W_GetLumpinfo
=============
*/
lumpinfo_t	*W_GetLumpinfo (char *name)
{
	int		i;
	lumpinfo_t	*lump_p;
	char	clean[16];
	
	W_CleanupName (name, clean);
	
	for (lump_p=wad_lumps, i=0 ; i<wad_numlumps ; i++,lump_p++)
	{
		if (!strcmp(clean, lump_p->name))
			return lump_p;
	}
	
	Sys_Error ("W_GetLumpinfo: %s not found", name);
	return NULL;
}

void *W_GetLumpName (char *name)
{
	lumpinfo_t	*lump;
	
	lump = W_GetLumpinfo (name);
	
	return (void *)(wad_base + lump->filepos);
}

void *W_GetLumpNum (int num)
{
	lumpinfo_t	*lump;
	
	if (num < 0 || num > wad_numlumps)
		Sys_Error ("W_GetLumpNum: bad number: %i", num);
		
	lump = wad_lumps + num;
	
	return (void *)(wad_base + lump->filepos);
}

/*
=============================================================================

automatic byte swapping

=============================================================================
*/

void SwapPic (qpic_t *pic)
{
	pic->width = LittleLong(pic->width);
	pic->height = LittleLong(pic->height);	
}

/*
=============================================================================

WAD3 Loading

Used code from "Fuhquake" modify by Crow_bar
=============================================================================
*/

#define TEXWAD_MAXIMAGES 16384
typedef struct
{
	char name[16];
	char filename[128]; //Crow_bar: Vip for PSP
	int position;
	int size;
} texwadlump_t;

texwadlump_t	texwadlump[TEXWAD_MAXIMAGES];
int numwadtextures;



void WAD3_LoadTextureWadFile (char *filename)
{
	lumpinfo_t *lumps, *lump_p;
	wadinfo_t header;
	int i, j, infotableofs, numlumps, lowmark;
	char name_file[256];
	FILE *file;
	
	if (FS_FOpenFile (va("textures/wad3/%s", filename), &file) != -1)
		strcpy(name_file, va("textures/wad3/%s", filename));
	else if (FS_FOpenFile (va("textures/halflife/%s", filename), &file) != -1)
        strcpy(name_file, va("textures/halflife/%s", filename));
	else if (FS_FOpenFile (va("textures/%s", filename), &file) != -1)
	    strcpy(name_file, va("textures/%s", filename));
	else if (FS_FOpenFile (filename, &file) != -1)
	    strcpy(name_file, filename);
	else
 	    Host_Error ("Couldn't load halflife wad \"%s\"\n", filename);

	if (fread(&header, 1, sizeof(wadinfo_t), file) != sizeof(wadinfo_t))
	{
		Con_Printf ("WAD3_LoadTextureWadFile: unable to read wad header");
        fclose(file);
		return;
	}

	if (memcmp(header.identification, "WAD3", 4))
	{
		Con_Printf ("WAD3_LoadTextureWadFile: Wad file %s doesn't have WAD3 id\n",filename);
        fclose(file);
		return;
	}

	numlumps = LittleLong(header.numlumps);

	if (numlumps < 1 || numlumps > TEXWAD_MAXIMAGES)
	{
		Con_Printf ("WAD3_LoadTextureWadFile: invalid number of lumps (%i)\n", numlumps);
        fclose(file);
		return;
	}

	infotableofs = LittleLong(header.infotableofs);

	if (fseek(file, infotableofs, SEEK_SET))
	{
		Con_Printf ("WAD3_LoadTextureWadFile: unable to seek to lump table");
        fclose(file);
		return;
	}

	lowmark = Hunk_LowMark();

	if (!(lumps = Hunk_Alloc(sizeof(lumpinfo_t) * numlumps)))
	{
		Con_Printf ("WAD3_LoadTextureWadFile: unable to allocate temporary memory for lump table");
        fclose(file);
		return;
	}

	if (fread(lumps, 1, sizeof(lumpinfo_t) * numlumps, file) != sizeof(lumpinfo_t) * numlumps)
	{
		Con_Printf ("WAD3_LoadTextureWadFile: unable to read lump table");
		Hunk_FreeToLowMark(lowmark);
		fclose(file);
		return;
	}

	for (i = 0, lump_p = lumps; i < numlumps; i++,lump_p++)
	{
        W_CleanupName (lump_p->name, lump_p->name);
		for (j = 0;j < numwadtextures;j++)
		{
			if (!strcmp(lump_p->name, texwadlump[j].name)) // name match, replace old one
				break;
		}
		if (j >= TEXWAD_MAXIMAGES)
			break; // abort loading
		if (j == numwadtextures)
		{
			W_CleanupName (lump_p->name, texwadlump[j].name);
			//texwadlump[j].file = file;
			strcpy(texwadlump[j].filename, name_file);
			texwadlump[j].position = LittleLong(lump_p->filepos);
			texwadlump[j].size = LittleLong(lump_p->disksize);
			numwadtextures++;
		}
	}

    //leaves the file open
	Hunk_FreeToLowMark(lowmark);
	fclose(file);
}

//converts paletted to rgba
int ConvertWad3ToRGBA(miptex_t *tex)
{
	byte *in, *data, *pal;
	int i, p, image_size;

	if (!tex->offsets[0])
		Sys_Error("ConvertWad3ToRGBA: tex->offsets[0] == 0");

	image_size = tex->width * tex->height;

	in = (byte *) ((byte *) tex + tex->offsets[0]);
    pal = in + ((image_size * 85) >> 6) + 2;


	data = (byte*)malloc(image_size);
    for (i = 0; i < image_size; i++)
	{
        p = *in++;
/*
		if (tex->name[0] == '{' && p == 255)
			data[i] = 0;
		else
*/
			data[i] = p;
	}

	int level = 0;
    if (r_mipmaps.value > 0)
		level = 3;
	
	int index = GL_LoadPaletteTexture (tex->name, tex->width, tex->height, (const byte*)data, (byte *)pal, PAL_RGB, true, GU_LINEAR, level);
	free(data);
	return index;

}

int WAD3_LoadTexture(miptex_t *mt)
{
	char texname[MAX_QPATH];
	int i, j, lowmark = 0;
	FILE *file;
	miptex_t *tex;
	int index;

	if (mt->offsets[0])
 	    return ConvertWad3ToRGBA(mt);

	texname[sizeof(texname) - 1] = 0;
	W_CleanupName (mt->name, texname);

	for (i = 0;i < numwadtextures;i++)
	{
		if (!texwadlump[i].name[0])
			break;
	    if (strcmp(texname, texwadlump[i].name))
			continue;
		FS_FOpenFile (texwadlump[i].filename, &file);
		if (fseek(file, texwadlump[i].position, SEEK_SET))
		{
			Con_Printf("WAD3_LoadTexture: corrupt WAD3 file");
			fclose(file);
            return 0;
		}
		lowmark = Hunk_LowMark();
		tex = Hunk_Alloc(texwadlump[i].size);
		if (fread(tex, 1, texwadlump[i].size, file) < texwadlump[i].size)
		{
            Con_Printf("WAD3_LoadTexture: corrupt WAD3 file");
			Hunk_FreeToLowMark(lowmark);
			fclose(file);
            return 0;
		}
		tex->width = LittleLong(tex->width);
		tex->height = LittleLong(tex->height);
		if (tex->width != mt->width || tex->height != mt->height)
		{
			Hunk_FreeToLowMark(lowmark);
			fclose(file);
			return 0;
		}
		for (j = 0;j < MIPLEVELS;j++)
			tex->offsets[j] = LittleLong(tex->offsets[j]);

		index = ConvertWad3ToRGBA(tex);

		Hunk_FreeToLowMark(lowmark);
		fclose(file);
	    return index;
	}
 	return  0;
}

int WAD3_LoadTextureName(char *name)
{
	char texname[MAX_QPATH];
	int i, j, lowmark = 0;
	FILE *file;
    miptex_t *tex;
	int index;

	texname[sizeof(texname) - 1] = 0;
    W_CleanupName (name, texname);

	for (i = 0;i < numwadtextures;i++)
	{
		if (!texwadlump[i].name[0])
			break;
	    if (strcmp(texname, texwadlump[i].name))
			continue;
		FS_FOpenFile (texwadlump[i].filename, &file);
		if (fseek(file, texwadlump[i].position, SEEK_SET))
		{
			Con_Printf("WAD3_LoadTexture: corrupt WAD3 file");
			fclose(file);
            return 0;
		}
		lowmark = Hunk_LowMark();
		tex = Hunk_Alloc(texwadlump[i].size);
		if (fread(tex, 1, texwadlump[i].size, file) < texwadlump[i].size)
		{
            Con_Printf("WAD3_LoadTexture: corrupt WAD3 file");
			Hunk_FreeToLowMark(lowmark);
			fclose(file);
            return 0;
		}
		tex->width = LittleLong(tex->width);
		tex->height = LittleLong(tex->height);
#if 0
		if (tex->width != mt->width || tex->height != mt->height)
		{
			Hunk_FreeToLowMark(lowmark);
			fclose(file);
			return 0;
		}
#endif
		for (j = 0;j < MIPLEVELS;j++)
			tex->offsets[j] = LittleLong(tex->offsets[j]);

	    index = ConvertWad3ToRGBA(tex);

		Hunk_FreeToLowMark(lowmark);
		fclose(file);
	    return index;
	}
 	return  0;
}


