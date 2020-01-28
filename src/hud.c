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
#include "quakedef.h"
#include <pspgu.h>
int      	hudpic;	
int			sb_updates;
cvar_t scope = {"scope", "0"};
cvar_t mp_startmoney = {"mp_startmoney", "800"};
cvar_t hud_alpha = {"hud_alpha", "255"};
//scope here
qpic_t      *scope_sniper;	

void Hud_LoadPics(void)
{
	hudpic = loadtextureimage_hud("textures/hud");
	scope_sniper = Draw_CachePic ("gfx/scope.lmp");
}
void Hud_Init (void)
{
	Hud_LoadPics(); //we are loading our precached pictures
	Cvar_RegisterVariable (&scope);
	Cvar_RegisterVariable (&mp_startmoney);
	Cvar_RegisterVariable (&hud_alpha);
}
/*
void Hud_DrawPicAlpha (int x, int y, qpic_t *pic, float alpha)
{
	Draw_AlphaPic (x + ((vid.width - 320)>>1), y + (vid.height - 24), pic, alpha);
}

void Hud_DrawPic (int x, int y, qpic_t *pic)
{
	Draw_AlphaPic (x + ((vid.width - 320)>>1), y + (vid.height - 24), pic,1.0f);
}
*/
void Hud_Changed (void)
{
	sb_updates = 0;	
}
void DrawMoney(int x, int y, int number)
{
	int a,b,c,d,e;
	int numpoints[] ={0,25,45,70,95,120,145,170,190,215,0}; 
	a = number / 10000;
	b = (number/1000)%10;
	c = (number/100)%10;
	d = (number/10)%10;
	e = number%10;
	if(number >= 10000)
		showimgpart (x, y, numpoints[a], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(number >= 1000)
		showimgpart (x+20, y, numpoints[b], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(number >= 100)
		showimgpart (x+40, y, numpoints[c], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41,(int)hud_alpha.value));
	if(number >= 10)
		showimgpart (x+60, y, numpoints[d], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	showimgpart (x+80, y, numpoints[e], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
}
void DrawNumber(int x, int y, int number)
{
	/*
	int a,b,c;
	int numpoints[] ={0,25,45,70,95,120,145,170,190,215,0}; 
	a = number / 100;
	b = number / 10;
	c = number % 10;
	if(number == 100)
		showimgpart (x, y, 25, 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, 255));
	if(number <= 100 && number >= 10)
		showimgpart (x+20, y, numpoints[b], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, 255));
	showimgpart (x+40, y, numpoints[c], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, 255));
	*/
	int a,b,c,d,e;
	int numpoints[] ={0,25,45,70,95,120,145,170,190,215,0}; 
	a = number / 10000;
	b = (number/1000)%10;
	c = (number/100)%10;
	d = (number/10)%10;
	e = number%10;
	if(number >= 100)
		showimgpart (x, y, numpoints[c], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(number >= 10)
		showimgpart (x+20, y, numpoints[d], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	showimgpart (x+40, y, numpoints[e], 0, 25, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
}
void Hud_Draw (void)
{
	if (scr_con_current == vid.height)
		return;		
    if (scr_viewsize.value == 130)
        return;
	int money;
	money = (int)mp_startmoney.value;
	sb_updates++;
	
	showimgpart (0, 240, 50, 25, 20, 22, hudpic, 1,GU_RGBA(180, 124, 41,(int)hud_alpha.value));
	if(cl.stats[STAT_HEALTH] <= 0)
		DrawNumber(30,240,0);
	else
		DrawNumber(30,240,cl.stats[STAT_HEALTH]);
	/*
	if(cl.stats[STAT_ACTIVEWEAPON] == IT_USP)
		showimgpart (450, 235, 96, 72, 24, 24, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(cl.stats[STAT_ACTIVEWEAPON] == IT_GLOCK)
		showimgpart (450, 235, 48, 72, 24, 24, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(cl.stats[STAT_ACTIVEWEAPON] == IT_DEAGLE)
		showimgpart (450, 235, 25, 72, 24, 24, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(cl.stats[STAT_ACTIVEWEAPON] == IT_M3)
		showimgpart (450, 235, 0, 72, 24, 24, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(cl.stats[STAT_ACTIVEWEAPON] == IT_AUG || cl.stats[STAT_ACTIVEWEAPON] == IT_GALIL || cl.stats[STAT_ACTIVEWEAPON] == IT_FAMAS|| cl.stats[STAT_ACTIVEWEAPON] == IT_M4A1)
		showimgpart (450, 235, 0, 96, 24, 24, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(cl.stats[STAT_ACTIVEWEAPON] == IT_AWP)
		showimgpart (450, 235, 25, 96, 24, 24, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	if(cl.stats[STAT_ACTIVEWEAPON] == IT_AK47)
		showimgpart (450, 235, 74, 72, 24, 24, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	*/
	DrawNumber(390,240,cl.stats[STAT_SHELLS]); //ammo
	DrawNumber(300,240,cl.stats[STAT_AMMO]); //clip
	//money $
	//showimgpart (340, 200, 195, 25, 15, 25, hudpic, 1,GU_RGBA(180, 124, 41, (int)hud_alpha.value));
	//DrawMoney(350,200,money); //money lel

	if (((int)scope.value == 1) &&  (cl.stats[STAT_ACTIVEWEAPON] == IT_AWP))
		Draw_Pic (0,0, scope_sniper);
	
}
