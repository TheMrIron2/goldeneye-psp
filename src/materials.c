/*

Crow_bar. 2012

Material list loader
*/

#include "quakedef.h"
#include <pspiofilemgr.h>
#include <ctype.h>  // isspace

#define MAT_TEXMAX          128
#define MAT_TEXNAMEMAX		13

static char material_type[MAT_TEXMAX];
static char material_name[MAT_TEXMAX][MAT_TEXNAMEMAX];
static int  material_num;
static int  material_init;

void MaterialsInit(char *name)
{
	SceUID fd;
	int i, j;
	char buffer[512];
	byte   *data;
	SceOff pos, flen;
	int fpos;

	if(material_init)
	   return;

	Q_memset(&(material_name[0][0]), 0, MAT_TEXMAX * MAT_TEXNAMEMAX);
	Q_memset(material_type, 0, MAT_TEXMAX);

	material_num = 0;
	Q_memset(buffer, 0, 512);

	//open file
	fd = sceIoOpen(name, PSP_O_RDONLY, 0777);
	if(fd < 0)
	{
	   //open error
	   material_init = 0;
	}

	//length
    pos = sceIoLseek(fd, 0, SEEK_CUR);
    flen = sceIoLseek(fd, 0, SEEK_END);
    sceIoLseek(fd, pos, SEEK_SET);

	//get memory
	data = (byte*)malloc(flen);

	//read
	sceIoRead(fd, data, flen);

	fpos=0;
	while ((COM_MemFgets( data, flen, &fpos, buffer, 511 ) != NULL) && (material_num < MAT_TEXMAX))
	{
  		// skip whitespace
		i = 0;
		while(buffer[i] && isspace(buffer[i]))
			i++;

		if (!buffer[i])
			continue;

		// skip comment lines
		if (buffer[i] == '/' || !isalpha(buffer[i]))
			continue;

		// get texture type
		material_type[material_num] = toupper(buffer[i++]);

		// skip whitespace
		while(buffer[i] && isspace(buffer[i]))
			i++;

		if (!buffer[i])
			continue;

		// get sentence name
		j = i;
		while (buffer[j] && !isspace(buffer[j]))
			j++;

		if (!buffer[j])
			continue;

		// null-terminate name and save in sentences array
		j = fmin (j, (MAT_TEXNAMEMAX - 1) + i);
		buffer[j] = 0;

		strcpy(&(material_name[material_num++][0]), &(buffer[i]));
	}

	free(data);
	sceIoClose(fd);

	material_init = 1;
}

void MaterialsShutdown(void)
{
	memset(&(material_name[0][0]), 0, MAT_TEXMAX * MAT_TEXNAMEMAX);
	memset(material_type, 0, MAT_TEXMAX);
	material_num = 0;
	material_init = 0;
}

char MaterialsCheck(char *name)
{
	int i;

	if(!material_init)
	   return (char)NULL;

	for( i = 0 ; i < material_num; i++ )
	{
		if(!Q_strcasecmp(material_name[i],name))
		   return material_type[i];
	}
	return (char)NULL;
}


