
/*
Copyright (C) 1996-1997 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later vvoersion.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "quakedef.h"
#include "net_dgrm.h"
#ifdef WIN32
#include "winquake.h"
#endif

#ifdef PSP
#include <pspkernel.h>
#include <psputility.h>
#include "net_dgrm.h"
#include <pspwlan.h>

extern cvar_t	accesspoint;
extern cvar_t	r_wateralpha;
extern cvar_t	r_vsync;
extern cvar_t	in_disable_analog;
extern cvar_t	in_analog_strafe;
extern cvar_t   in_analog_freelook;
extern cvar_t	in_x_axis_adjust;
extern cvar_t	in_y_axis_adjust;
extern cvar_t	crosshair;
extern cvar_t	crosshaircolor;
extern cvar_t	r_dithering;
extern cvar_t   r_shadows;
extern cvar_t	sp;
extern cvar_t	mp;
extern cvar_t	op;
extern cvar_t	cr;
extern cvar_t	ex;

extern cvar_t	jg;
extern cvar_t	cg;
extern cvar_t	set;
extern cvar_t	inf;
extern cvar_t	ah;

extern cvar_t	song;

extern cvar_t	crosshair;
extern cvar_t	accesspoint;
extern cvar_t	qmb_particles;

#endif

#ifdef PSP_MP3_HWDECODE
extern int changeMp3Volume;
#endif

extern qboolean bmg_type_changed;

cvar_t	scr_centermenu = {"scr_centermenu", "1"};

int	m_yofs = 0;

void (*vid_menudrawfn)(void);
void (*vid_menukeyfn)(int key);
void Draw_Fill (int x, int y, int w, int h, int c);
//void Draw_FadeScreenNew (int x, int y, int x1, int y1, int r, int g, int b, int alpha);

enum {m_none,m_maps, m_main, m_singleplayer,m_aim,m_server,m_stest,
m_taunts, m_buyt,m_buyct, m_ingame, m_chat, m_orders, 
m_pistolst,m_pistolsct, m_smgst, m_riflest,m_smgsct, 
m_riflesct, m_shotguns, m_machineguns, m_equipment ,m_multiplayer, 
m_setup, m_net, m_options, m_video, m_keys, m_help, m_quit, 
m_serialconfig, m_modemconfig, m_lanconfig, m_gameoptions,
m_search, m_slist, m_osk, m_team, m_counter, m_terror, m_bots, m_sedit,m_networks} m_state;

extern void SList_Shutdown (void);
extern void SList_Load (void);
void M_Menu_Maps_f (void);
void M_Menu_Main_f (void);
	void M_Menu_SinglePlayer_f (void);
     void M_Menu_Buyt_f (void);
     void M_Menu_Buyct_f (void);
     void M_Menu_Chat_f (void); 
     void M_Menu_Ingame_f (void);
     void M_Menu_Orders_f (void);
     void M_Menu_Pistolst_f (void);
     void M_Menu_Smgst_f (void);
     void M_Menu_Riflest_f (void);
     void M_Menu_Pistolsct_f (void);
     void M_Menu_Smgsct_f (void);
     void M_Menu_Riflesct_f (void);
     void M_Menu_Shotguns_f (void);
     void M_Menu_Machineguns_f (void);
     void M_Menu_Equipment_f (void);
     void M_Menu_Team_f (void);
     void M_Menu_Counter_f (void);
     void M_Menu_Taunts_f (void);
     void M_Menu_Server_f (void);
     void M_Menu_Terror_f (void);
void M_Menu_Networks_f (void);
	void M_Menu_MultiPlayer_f (void);
		void M_Menu_Setup_f (void);
		void M_Menu_Net_f (void);
	void M_Menu_Options_f (void);
		void M_Menu_Keys_f (void);
		void M_Menu_Video_f (void);
	void M_Menu_Help_f (void);
	void M_Menu_Quit_f (void);
void M_Menu_SerialConfig_f (void);
	void M_Menu_ModemConfig_f (void);
void M_Menu_LanConfig_f (void);
void M_Menu_GameOptions_f (void);
void M_Menu_Search_f (void);
void M_Menu_ServerList_f (void);

void M_Maps_Draw (void);
void M_Main_Draw (void);
	void M_SinglePlayer_Draw (void);
     void M_Buyt_Draw (void);
     void M_Buyct_Draw (void);
      void M_Chat_Draw (void);
      void M_Orders_Draw (void); 
     void M_Ingame_Draw (void);
     void M__Pistolst_Draw (void);
     void M__Pistolsct_Draw (void);
     void M__Taunts_Draw (void);
     void M_Smgst_Draw (void);
     void M_Smgsct_Draw (void);
     void M_Server_Draw (void);
     void M_Riflest_Draw (void);
     void M_Riflesct_Draw (void);
     void M_Shotguns_Draw (void);
     void M_Machineguns_Draw (void);
     void M_Equipment_Draw (void);
     void M_Team_Draw (void);
     void M_Counter_Draw (void);
     void M_Terror_Draw (void);
void M_Networks_Draw (void);
	void M_MultiPlayer_Draw (void);
		void M_Setup_Draw (void);
		void M_Net_Draw (void);
	void M_Options_Draw (void);
		void M_Keys_Draw (void);
		void M_Video_Draw (void);
	void M_Help_Draw (void);
	void M_Quit_Draw (void);
void M_SerialConfig_Draw (void);
	void M_ModemConfig_Draw (void);
void M_LanConfig_Draw (void);
void M_GameOptions_Draw (void);
void M_Search_Draw (void);
void M_Bots_Draw (void);
void M_Search_Draw (void);
void M_ServerList_Draw (void);


void M_Maps_Key (int key);
void M_Main_Key (int key);
	void M_SinglePlayer_Key (int key);
    void M_Buyt_Key (int key);
    void M_Buyct_Key (int key);
      void M_Chat_Key (int key);
      void M_Ingame_Key (int key);
      void M_Orders_Key (int key);
     void M_Pistolst_Key (int key);
     void M_Smgst_Key (int key);
     void M_Riflest_Key (int key);
     void M_Pistolsct_Key (int key);
     void M_Smgsct_Key (int key);
     void M_Riflesct_Key (int key);
     void M_Team_Key (int key);
     void M_Counter_Key (int key);
     void M_Taunts_Key (int key);
     void M_Terror_Key (int key);
     void M_Server_Key (int key);
     void M_Shotguns_Key (int key);
     void M_Machineguns_Key (int key);
     void M_Equipment_Key (int key);
void M_Networks_Key (int key);
	void M_MultiPlayer_Key (int key);
		void M_Setup_Key (int key);
		void M_Net_Key (int key);
	void M_Options_Key (int key);
		void M_Keys_Key (int key);
		void M_Video_Key (int key);
	void M_Help_Key (int key);
	void M_Quit_Key (int key);
void M_SerialConfig_Key (int key);
	void M_ModemConfig_Key (int key);
void M_LanConfig_Key (int key);
void M_GameOptions_Key (int key);
void M_Search_Key (int key);
void M_STest_Key (int key);
void M_Search_Key (int key);
void M_FoundServers_Key (int key);

void Con_SetOSKActive(qboolean active);
void M_Menu_OSK_f (char *input, char *output, int outlen);

qboolean	m_entersound;		// play after drawing a frame, so caching
								// won't disrupt the sound
qboolean	m_recursiveDraw;

int			m_return_state;
qboolean	m_return_onerror;
char		m_return_reason [32];

#define StartingGame	(m_multiplayer_cursor == 1)
#define JoiningGame		(m_multiplayer_cursor == 0)
#define SerialConfig	(m_net_cursor == 0)
#define DirectConfig	(m_net_cursor == 1)
#define	IPXConfig		(m_net_cursor == 2)
#define	TCPIPConfig		(m_net_cursor == 3)

int playing;
void M_ConfigureNetSubsystem(void);
void Draw_Menudot (int x,int y,int h,int w);

/*
================
M_DrawCharacter

Draws one solid graphics character
================
*/
void M_DrawCharacter (int cx, int line, int num)
{
	Draw_Character ( cx + ((vid.width - 320)>>1), line, num);
}

void M_Print (int cx, int cy, char *str)
{
	while (*str)
	{
		M_DrawCharacter (cx, cy, (*str)+128);
		str++;
		cx += 8;
	}
}

void M_PrintWhite (int cx, int cy, char *str)
{
	while (*str)
	{
		M_DrawCharacter (cx, cy, *str);
		str++;
		cx += 8;
	}
}

void M_DrawTransPic (int x, int y, qpic_t *pic)
{
	Draw_TransPic (x + ((vid.width - 320)>>1), y, pic);
}

void M_DrawPic (int x, int y, qpic_t *pic)
{
	Draw_Pic (x , y, pic);
}

byte identityTable[256];
byte translationTable[256];

void M_BuildTranslationTable(int top, int bottom)
{
	int		j;
	byte	*dest, *source;

	for (j = 0; j < 256; j++)
		identityTable[j] = j;
	dest = translationTable;
	source = identityTable;
	memcpy (dest, source, 256);

	if (top < 128)	// the artists made some backwards ranges.  sigh.
		memcpy (dest + TOP_RANGE, source + top, 16);
	else
		for (j=0 ; j<16 ; j++)
			dest[TOP_RANGE+j] = source[top+15-j];

	if (bottom < 128)
		memcpy (dest + BOTTOM_RANGE, source + bottom, 16);
	else
		for (j=0 ; j<16 ; j++)
			dest[BOTTOM_RANGE+j] = source[bottom+15-j];
}


void M_DrawTransPicTranslate (int x, int y, qpic_t *pic)
{
	Draw_TransPicTranslate (x + ((vid.width - 320)>>1), y, pic, translationTable);
}


void M_DrawTextBox (int x, int y, int width, int lines)
{
	qpic_t	*p;
	int		cx, cy;
	int		n;

	// draw left side
	cx = x;
	cy = y;
	p = Draw_CachePic ("gfx/box_tl.lmp");
	M_DrawTransPic (cx, cy, p);
	p = Draw_CachePic ("gfx/box_ml.lmp");
	for (n = 0; n < lines; n++)
	{
		cy += 8;
		M_DrawTransPic (cx, cy, p);
	}
	p = Draw_CachePic ("gfx/box_bl.lmp");
	M_DrawTransPic (cx, cy+8, p);

	// draw middle
	cx += 8;
	while (width > 0)
	{
		cy = y;
		p = Draw_CachePic ("gfx/box_tm.lmp");
		M_DrawTransPic (cx, cy, p);
		p = Draw_CachePic ("gfx/box_mm.lmp");
		for (n = 0; n < lines; n++)
		{
			cy += 8;
			if (n == 1)
				p = Draw_CachePic ("gfx/box_mm2.lmp");
			M_DrawTransPic (cx, cy, p);
		}
		p = Draw_CachePic ("gfx/box_bm.lmp");
		M_DrawTransPic (cx, cy+8, p);
		width -= 2;
		cx += 16;
	}

	// draw right side
	cy = y;
	p = Draw_CachePic ("gfx/box_tr.lmp");
	M_DrawTransPic (cx, cy, p);
	p = Draw_CachePic ("gfx/box_mr.lmp");
	for (n = 0; n < lines; n++)
	{
		cy += 8;
		M_DrawTransPic (cx, cy, p);
	}
	p = Draw_CachePic ("gfx/box_br.lmp");
	M_DrawTransPic (cx, cy+8, p);
}



void M_DrawCheckbox (int x, int y, int on)
{
#if 0
	if (on)
		M_DrawCharacter (x, y, 131);
	else
		M_DrawCharacter (x, y, 129);
#endif
	if (on)
		M_Print (x, y, "on");
	else
		M_Print (x, y, "off");
}

void M_DrawCheckboxNew (int x, int y, int on)
{
#if 0
	if (on)
		M_DrawCharacter (x, y, 131);
	else
		M_DrawCharacter (x, y, 129);
#endif
	if (on)
            M_DrawTransPic (x, y, Draw_CachePic ("gfx/on.lmp") );
	else
            M_DrawTransPic (x, y, Draw_CachePic ("gfx/off.lmp") );
}


//=============================================================================


int m_save_demonum;

/*
================
M_ToggleMenu_f
================
*/
void M_ToggleMenu_f (void)
{

	m_entersound = true;

	if (key_dest == key_menu)
	{
		if (m_state != m_main)
		{
			M_Menu_Main_f ();
			return;
		}
		key_dest = key_game;
		m_state = m_none;
		return;
	}
	if (key_dest == key_console)
	{
		Con_ToggleConsole_f ();
	}
	else
	{
		M_Menu_Main_f ();
	}
}


//=============================================================================
/* MAIN MENU */

int	m_main_cursor;
#define	MAIN_ITEMS	5


void M_Menu_Main_f (void)
{
	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_main;
	m_entersound = true;

}


void M_Main_Draw (void)
{
	int		f;
	qpic_t	*p, *background;
	background = Draw_CachePic ("gfx/conback.tga");
    M_DrawPic (0, 0, background);;
    M_PrintWhite(-50,200,"New Game");
	if(sv.active)
		M_PrintWhite(-50,210,"Bots");
	else
		M_PrintWhite(-50,210,"Multiplayer game");
	M_PrintWhite(-50,220,"Options");
	M_PrintWhite(-50,230,"Credits");
	M_PrintWhite(-50,240,"Exit");
	
	switch(m_main_cursor)
	{
		case 0:
		M_Print(-50,200,"New Game");
		break;
		case 1:
		if(sv.active)
			M_Print(-50,210,"Bots");
		else
			M_Print(-50,210,"Multiplayer game");
		break;
		case 2:
		M_Print(-50,220,"Options");
		break;
		case 3:
		M_Print(-50,230,"Credits");
		break;
		case 4:
		M_Print(-50,240,"Exit");
		break;
	}
	
}


void M_Main_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		M_ToggleMenu_f();
	break;
	//add select for changing team???
	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_main_cursor >= MAIN_ITEMS)
			m_main_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_main_cursor < 0)
			m_main_cursor = MAIN_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_main_cursor)
		{
		case 0:  
			M_Menu_Maps_f();
			break;
		case 1:
			if(sv.active)
				M_Menu_Bots_f();
			else
				M_Menu_Networks_f ();
			break;
		case 2:
			M_Menu_Options_f ();
			break;
		case 3:
			M_Menu_Help_f ();
			break;

		case 4:
			M_Menu_Quit_f ();
			break;

		}
	}
}

//=============================================================================
/* SINGLE PLAYER MENU */

int	m_singleplayer_cursor;
#define	SINGLEPLAYER_ITEMS	3


void M_Menu_SinglePlayer_f (void)
{
	key_dest = key_menu;
	m_state = m_singleplayer;
	m_entersound = true;
}


void M_SinglePlayer_Draw (void)
{
	int		f;
	qpic_t	*p;


    //M_DrawTransPic (60,  0, Draw_CachePic ("gfx/training.lmp") );  


  M_PrintWhite (76, 39, "Dust");
  M_PrintWhite (76, 59, "Dust2");
  M_PrintWhite (76, 79, "Multiplayer Menu");

	M_DrawTransPic (54, 32 + m_singleplayer_cursor * 20,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_SinglePlayer_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		M_Menu_Main_f ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_singleplayer_cursor >= SINGLEPLAYER_ITEMS)
			m_singleplayer_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_singleplayer_cursor < 0)
			m_singleplayer_cursor = SINGLEPLAYER_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_singleplayer_cursor)
		{
		case 0:				
                  Cbuf_AddText ("disconnect\n");
			Cbuf_AddText ("maxplayers 4\n");
			Cbuf_AddText ("map de_dust\n");

		case 1:
                  Cbuf_AddText ("disconnect\n");
			Cbuf_AddText ("maxplayers 4\n");
			Cbuf_AddText ("map de_dust2\n");
           case 2 : 
                   M_Menu_MultiPlayer_f (); 
                    break;


		}
	}
}



//=============================================================================
/* IN Game Menu */

int	m_ingame_cursor;
#define	INGAME_ITEMS	3


void M_Menu_Ingame_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_ingame;
	m_entersound = true;
}


void M_Ingame_Draw (void)
{
	int		f;
	qpic_t	*p;
      int team;


M_DrawTransPic (0, 60, Draw_CachePic ("gfx/menu/mainmenu.lmp") );

M_DrawTransPic (-40, 60 + m_ingame_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Ingame_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_ingame_cursor >= INGAME_ITEMS)
			m_ingame_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_ingame_cursor < 0)
			m_ingame_cursor = INGAME_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_ingame_cursor)
		{
		case 0:
			Cbuf_AddText ("impulse 105\n");
			break;

		case 1:
			M_Menu_Chat_f ();
			break;

		case 2:
			M_Menu_Team_f ();
			break;
		}
	}
}
//=============================================================================
/* Bots MENU */

int m_bots_cursor;
#define    BOTS_ITEMS 2

void M_Menu_Bots_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_bots;
	m_entersound = true;
}

void M_Bots_Draw (void)
{
	M_PrintWhite (75, 40, "Add T bot");
	M_PrintWhite (75, 50, "Add CT bot");
	switch(m_bots_cursor)
	{
		case 0:
		M_Print(75, 40, "Add T bot");
		break;
		
		case 1:
		M_Print(75, 50, "Add CT bot");
		break;
	}
}


void M_Bots_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_bots_cursor >= BOTS_ITEMS)
			m_bots_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_bots_cursor < 0)
			m_bots_cursor = BOTS_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_bots_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 101\n"); 
				break;
		case 1:
		Cbuf_AddText ("impulse 100\n"); 
				break;
		}
		key_dest = key_game;
	}
}





//=============================================================================
/* In Game Buy Menu Counter-Terrorist */

int	m_buyct_cursor;
#define	BUYCT_ITEMS	6



void M_Menu_Buyct_f (void)
{
	key_dest = key_menu;
	m_state = m_buyct;
	m_entersound = true;
}


void M_Buyct_Draw (void)
{
	int		f;
	qpic_t	*p;

M_DrawTransPic (0, 20, Draw_CachePic ("gfx/menu/buymenu.lmp") );

	M_DrawTransPic (-40, 20 + m_buyct_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );
}


void M_Buyct_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_buyct_cursor >= BUYCT_ITEMS)
			m_buyct_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_buyct_cursor < 0)
			m_buyct_cursor = BUYCT_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		switch (m_buyct_cursor)
		{
		case 0:
			M_Menu_Pistolsct_f ();
			break;

		case 1:
			M_Menu_Shotguns_f ();
			break;
		case 2:
			M_Menu_Smgsct_f ();
			break;

		case 3:
			M_Menu_Riflesct_f ();
			break;

		case 4:
			M_Menu_Machineguns_f ();
			break;

		case 5:
			M_Menu_Equipment_f ();
			break;

		}
	}
}

//=============================================================================
/* In Game Buy Menu Buy Terrorist */

int	m_buyt_cursor;
#define	BUYT_ITEMS	6



void M_Menu_Buyt_f (void)
{
	key_dest = key_menu;
	m_state = m_buyt;
	m_entersound = true;
}


void M_Buyt_Draw (void)
{
	int		f;
	qpic_t	*p;

M_DrawTransPic (0, 20, Draw_CachePic ("gfx/menu/buymenu.lmp") );
M_DrawTransPic (-40, 20 + m_buyt_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );
}


void M_Buyt_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_buyt_cursor >= BUYT_ITEMS)
			m_buyt_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_buyt_cursor < 0)
			m_buyt_cursor = BUYT_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_buyt_cursor)
		{
		case 0:
			M_Menu_Pistolst_f ();
			break;

		case 1:
			M_Menu_Shotguns_f ();
			break;
		case 2:
			M_Menu_Smgst_f ();
			break;

		case 3:
			M_Menu_Riflest_f ();
			break;

		case 4:
			M_Menu_Machineguns_f ();
			break;

		case 5:
			M_Menu_Equipment_f ();
			break;

		}
	}
}
//=============================================================================
/* Pistols MENU  Terrorist*/


int m_pistolst_cursor;
#define    PISTOLST_ITEMS 5

void M_Menu_Pistolst_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_pistolst;
	m_entersound = true;
}

void M_Pistolst_Draw (void)
{
	int		f;
	qpic_t	*p;

M_DrawTransPic (76, 0, Draw_CachePic ("gfx/pistols.lmp") );



    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_glock.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_usp.lmp") );
    M_DrawTransPic (76, 116, Draw_CachePic ("gfx/buy/w_deagle.lmp") );
    M_DrawTransPic (76, 154, Draw_CachePic ("gfx/buy/w_p228.lmp") );
    M_DrawTransPic (76, 192, Draw_CachePic ("gfx/buy/w_elite.lmp") );



M_DrawTransPic (54, 40 + m_pistolst_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Pistolst_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_pistolst_cursor >= PISTOLST_ITEMS)
			m_pistolst_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_pistolst_cursor < 0)
			m_pistolst_cursor = PISTOLST_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_pistolst_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 20\n"); //w_glock
				break;
		case 1:
		Cbuf_AddText ("impulse 21\n"); //w_usp
				break;
		case 2:
		Cbuf_AddText ("impulse 22\n"); //w_deagle
				break;
		case 3:
		Cbuf_AddText ("impulse 23\n"); //w_p228
				break;
		case 4:
		Cbuf_AddText ("impulse 25\n"); //w_elite
				break;

		}
	}
}

//=============================================================================
/* Pistols MENU  Counter_Terrorist*/

int fivesevenbought;

int m_pistolsct_cursor;
#define    PISTOLSCT_ITEMS 5

void M_Menu_Pistolsct_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_pistolsct;
	m_entersound = true;
}

void M_Pistolsct_Draw (void)
{
	int		f;
	qpic_t	*p;

M_DrawTransPic (76, 0, Draw_CachePic ("gfx/pistols.lmp") );


    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_glock.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_usp.lmp") );
    M_DrawTransPic (76, 116, Draw_CachePic ("gfx/buy/w_deagle.lmp") );
    M_DrawTransPic (76, 154, Draw_CachePic ("gfx/buy/w_p228.lmp") );
    M_DrawTransPic (76, 192, Draw_CachePic ("gfx/buy/w_fiveseven.lmp") );

		


M_DrawTransPic (54, 40 + m_pistolsct_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Pistolsct_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_pistolsct_cursor >= PISTOLSCT_ITEMS)
			m_pistolsct_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_pistolsct_cursor < 0)
			m_pistolsct_cursor = PISTOLSCT_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_pistolsct_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 20\n"); 
				break;
		case 1:
		Cbuf_AddText ("impulse 21\n"); 
				break;
		case 2:
		Cbuf_AddText ("impulse 22\n"); 
				break;
		case 3:
		Cbuf_AddText ("impulse 23\n"); 
				break;
		case 4:
		Cbuf_AddText ("impulse 24\n"); 
				break;
		}
	}
}


//=============================================================================
/* MachineGuns MENU */

int m249bought;

int m_machineguns_cursor;
#define    MACHINEGUNS_ITEMS 1

void M_Menu_Machineguns_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_machineguns;
	m_entersound = true;
}

void M_Machineguns_Draw (void)
{
	int		f;
	qpic_t	*p;



M_DrawTransPic (76, 0, Draw_CachePic ("gfx/mg.lmp") );


    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_m249.lmp") );


M_DrawTransPic (54, 40 + m_machineguns_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Machineguns_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_machineguns_cursor >= MACHINEGUNS_ITEMS)
			m_machineguns_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_machineguns_cursor < 0)
			m_machineguns_cursor = MACHINEGUNS_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_machineguns_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 64\n"); 
				break;
		}
	}
}

//=============================================================================
/* Equipment MENU */

int m_equipment_cursor;
#define    EQUIPMENT_ITEMS 3

void M_Menu_Equipment_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_equipment;
	m_entersound = true;
}

void M_Equipment_Draw (void)
{
	int		f;
	qpic_t	*p;



M_DrawTransPic (76, 0, Draw_CachePic ("gfx/eq.lmp") );

    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_kevlar.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_he.lmp") );
    M_DrawTransPic (76, 116, Draw_CachePic ("gfx/buy/w_ammo.lmp") );


M_DrawTransPic (54, 40 + m_equipment_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Equipment_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_equipment_cursor >= EQUIPMENT_ITEMS)
			m_equipment_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_equipment_cursor < 0)
			m_equipment_cursor = EQUIPMENT_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_equipment_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 44\n"); 
				break;

		case 1:
		Cbuf_AddText ("impulse 41\n"); 
				break;

		case 2:
		Cbuf_AddText ("impulse 81\n"); 
				break;

		}
	}
}

//=============================================================================
/* SubMachineGun MENU  Terrorist*/

int m_smgst_cursor;
#define    SMGST_ITEMS 3

void M_Menu_Smgst_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_smgst;
	m_entersound = true;
}

void M_Smgst_Draw (void)
{
	int		f;
	qpic_t	*p;



  M_DrawTransPic (76, 0, Draw_CachePic ("gfx/smg.lmp") );


    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_mp5.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_p90.lmp") );
    M_DrawTransPic (76, 116, Draw_CachePic ("gfx/buy/w_mac10.lmp") );


M_DrawTransPic (54, 40 + m_smgst_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Smgst_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_smgst_cursor >= SMGST_ITEMS)
			m_smgst_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_smgst_cursor < 0)
			m_smgst_cursor = SMGST_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_smgst_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 42\n"); 
				break;
		case 1:
		Cbuf_AddText ("impulse 65\n"); 
				break;
		case 2:
		Cbuf_AddText ("impulse 88\n"); 
				break;

		}
	}
}

//=============================================================================
/* SubMachineGun MENU Counter_terrorist */

int m_smgsct_cursor;
#define    SMGSCT_ITEMS 2

void M_Menu_Smgsct_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_smgsct;
	m_entersound = true;
}

void M_Smgsct_Draw (void)
{
	int		f;
	qpic_t	*p;



  M_DrawTransPic (76, 0, Draw_CachePic ("gfx/smg.lmp") );

//MP5 
//P90 

    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_mp5.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_p90.lmp") );


M_DrawTransPic (54, 40 + m_smgsct_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Smgsct_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_smgsct_cursor >= SMGSCT_ITEMS)
			m_smgsct_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_smgsct_cursor < 0)
			m_smgsct_cursor = SMGSCT_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_smgsct_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 42\n"); 
				break;
		case 1:
		Cbuf_AddText ("impulse 65\n"); 
				break;

		}
	}
}

//=============================================================================
/* Rifles MENU Terrorist */

int m_riflest_cursor;
#define    RIFLEST_ITEMS 4

void M_Menu_Riflest_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_riflest;
	m_entersound = true;
}

void M_Riflest_Draw (void)
{
	int		f;
	qpic_t	*p;



M_DrawTransPic (76, 0, Draw_CachePic ("gfx/rifle.lmp") );

//Imi Gallil 
//AK-47 
//SG-552 
//Scout 
//AWP 
//SG-1

    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_galil.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_ak47.lmp") );
    M_DrawTransPic (76, 116, Draw_CachePic ("gfx/buy/w_awp.lmp") );
    M_DrawTransPic (76, 154, Draw_CachePic ("gfx/buy/w_sg1.lmp") );

M_DrawTransPic (54, 40 + m_riflest_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );


}


void M_Riflest_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_riflest_cursor >= RIFLEST_ITEMS)
			m_riflest_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_riflest_cursor < 0)
			m_riflest_cursor = RIFLEST_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_riflest_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 89\n"); 
				break;

		case 1:
		Cbuf_AddText ("impulse 43\n"); 
				break;

		case 2:
		Cbuf_AddText ("impulse 46\n"); 
				break;
		case 3:
		Cbuf_AddText ("impulse 90\n"); 
				break;

		}
	}
}

//=============================================================================
/* Rifles MENU Counter_teorrist */

int m_riflesct_cursor;
#define    RIFLESCT_ITEMS 4

void M_Menu_Riflesct_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_riflesct;
	m_entersound = true;
}

void M_Riflesct_Draw (void)
{
	int		f;
	qpic_t	*p;



M_DrawTransPic (76, 0, Draw_CachePic ("gfx/rifle.lmp") );

//FAMAS 
//M4A1 
//Steyr AUG 
//AWP 


    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_famas.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_m4a1.lmp") );
    M_DrawTransPic (76, 116, Draw_CachePic ("gfx/buy/w_aug.lmp") );
    M_DrawTransPic (76, 154, Draw_CachePic ("gfx/buy/w_awp.lmp") );


M_DrawTransPic (54, 40 + m_riflesct_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );


}


void M_Riflesct_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_riflesct_cursor >= RIFLESCT_ITEMS)
			m_riflesct_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_riflesct_cursor < 0)
			m_riflesct_cursor = RIFLESCT_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_riflesct_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 84\n"); 
				break;

		case 1:
		Cbuf_AddText ("impulse 45\n"); 
				break;

		case 2:
		Cbuf_AddText ("impulse 82\n"); 
				break;
		case 3:
		Cbuf_AddText ("impulse 46\n"); 
				break;


		}
	}
}



//=============================================================================
/* Shotguns MENU */

int m_shotguns_cursor;
#define    SHOTGUNS_ITEMS 2

void M_Menu_Shotguns_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_shotguns;
	m_entersound = true;
}

void M_Shotguns_Draw (void)
{
	int		f;
	qpic_t	*p;



  M_DrawTransPic (76, 0, Draw_CachePic ("gfx/shotgun.lmp") );
  //M_PrintWhite (76, 39, "M3 Super 90");
  //M_PrintWhite (180, 39, "Price 30$");
  //M_PrintWhite (76, 59, "XM1014");
  //M_PrintWhite (180, 59, "Price 40$");

    M_DrawTransPic (76, 40, Draw_CachePic ("gfx/buy/w_m3.lmp") );
    M_DrawTransPic (76, 78, Draw_CachePic ("gfx/buy/w_xm1014.lmp") );


M_DrawTransPic (54, 40 + m_shotguns_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Shotguns_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_shotguns_cursor >= SHOTGUNS_ITEMS)
			m_shotguns_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_shotguns_cursor < 0)
			m_shotguns_cursor = SHOTGUNS_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_shotguns_cursor)
		{
		case 0:
		Cbuf_AddText ("impulse 62\n"); 
				break;
		case 1:
		Cbuf_AddText ("impulse 63\n"); 
				break;
		}
	}
}

//=============================================================================
/* Ingame Chat MENU */

int	m_chat_cursor;
#define	CHAT_ITEMS	2


void M_Menu_Chat_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_chat;
	m_entersound = true;
}


void M_Chat_Draw (void)
{
	int		f;
	qpic_t	*p;



M_DrawTransPic (76, 0, Draw_CachePic ("gfx/chat.lmp") );

  M_PrintWhite (76, 39, "Orders");
  M_PrintWhite (76, 59, "Quick Chat");

M_DrawTransPic (54, 32 + m_chat_cursor * 20,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Chat_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_chat_cursor >= CHAT_ITEMS)
			m_chat_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_chat_cursor < 0)
			m_chat_cursor = CHAT_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_chat_cursor)
		{
		case 0:
			M_Menu_Orders_f ();
			break;

		case 1:
			M_Menu_Taunts_f ();
			break;


		}
	}
}
//=============================================================================
/* Chat Menu Orders */

int	m_orders_cursor;
#define	ORDERS_ITEMS	10


void M_Menu_Orders_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_orders;
	m_entersound = true;
}


void M_Orders_Draw (void)
{
	int		f;
	qpic_t	*p;


  M_PrintWhite (76, 11, "Orders");
  M_PrintWhite (76, 39, "Go Go Go");
  M_PrintWhite (76, 59, "Need Backup");
  M_PrintWhite (76, 79, "Affirmativ");
  M_PrintWhite (76, 99, "Cover Me");
  M_PrintWhite (76, 119, "Roger That");
  M_PrintWhite (76, 139,"Negative");
  M_PrintWhite (76, 159,"Fire in the Hole");
  M_PrintWhite (76, 179, "Hold this Position");
  M_PrintWhite (76, 199,"Enemy Down");
  M_PrintWhite (76, 219,"Enemy Spotted");

M_DrawTransPic (54, 32 + m_orders_cursor * 20,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Orders_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_orders_cursor >= ORDERS_ITEMS)
			m_orders_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_orders_cursor < 0)
			m_orders_cursor = ORDERS_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_orders_cursor)
		{
		case 0:
			Cbuf_AddText ("say Follow me\n");
                  Cbuf_AddText ("impulse 70\n");	
		       break;
		case 1:
			Cbuf_AddText ("say I need Backup\n");
                  Cbuf_AddText ("impulse 71");
			break;

		case 2:
			Cbuf_AddText ("say Affirmativ\n");
                  Cbuf_AddText ("impulse 72");
          		break;

		case 3:
			Cbuf_AddText ("say Cover Me\n");
                  Cbuf_AddText ("impulse 73");
			break;
		case 4:
			Cbuf_AddText ("say Roger that\n");
                  Cbuf_AddText ("impulse 80");
			break;
		case 5:
			Cbuf_AddText ("say Negative\n");
                  Cbuf_AddText ("impulse 78");
			break;
		case 6:
			Cbuf_AddText ("say Fire in the Hole!!\n");
                  Cbuf_AddText ("impulse 75");
			break;
		case 7:
			Cbuf_AddText ("say Hold this Position\n");
                  Cbuf_AddText ("impulse 79");
			break;
		case 8:
			Cbuf_AddText ("say Enemy Down\n");
                  Cbuf_AddText ("impulse 76");
			break;
		case 9:
			Cbuf_AddText ("say Enemy Spotted\n");
                  Cbuf_AddText ("impulse 74");
			break;


		}
	}
}

//=============================================================================
/* Quick Chat Menu Orders */

int	m_taunts_cursor;
#define	TAUNTS_ITEMS	7


void M_Menu_Taunts_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_taunts;
	m_entersound = true;
}


void M_Taunts_Draw (void)
{
	int		f;
	qpic_t	*p;
int drawinput;


  M_PrintWhite (76, 11, "Chat");
  M_PrintWhite (76, 39, "Hello");
  M_PrintWhite (76, 59, "Bye");
  M_PrintWhite (76, 79, "******");
  M_PrintWhite (76, 99, "Follow Me");
  M_PrintWhite (76, 119, "NOOOOOB!!");


Con_DrawNotify ();

	if (drawinput)
		Con_DrawInput ();
		
	Con_DrawOSK();	


M_DrawTransPic (54, 32 + m_taunts_cursor * 20,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Taunts_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_taunts_cursor >= TAUNTS_ITEMS)
			m_taunts_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_taunts_cursor < 0)
			m_taunts_cursor = TAUNTS_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_taunts_cursor)
		{
		case 0:
			Cbuf_AddText ("say Hello\n");	
		       break;
		case 1:
			Cbuf_AddText ("say Bye\n");
			break;

		case 2:
			Cbuf_AddText ("say ********\n");
          		break;

		case 3:
			Cbuf_AddText ("say Follow Me\n");
			break;
		case 4:
			Cbuf_AddText ("say NOOOOB!!\n");
			break;



		}
	}
}



//=============================================================================
/* Team Menu */

int	m_team_cursor;
#define	TEAM_ITEMS	2


void M_Menu_Team_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_team;
	m_entersound = true;
}


void M_Team_Draw (void)
{
	int		f;
	qpic_t	*p;
	switch(m_team_cursor)
	{
		case 0:
		M_Print(75, 40, "Join Guards team");
		break;
		
		case 1:
		M_Print(75, 40, "Join Spy team");
		break;
	}
	M_Print(0, 60, "Guards");
	M_Print(0, 90, "The Spy");
//M_DrawTransPic (0, 60, Draw_CachePic ("gfx/ChangeTeam.lmp") );
//M_DrawTransPic (250, 200,Draw_CachePic("gfx/help1.lmp") );
M_DrawTransPic (-40, 50 + m_team_cursor * 38,Draw_CachePic( va("gfx/menudot.lmp") ) );

}


void M_Team_Key (int key)
{
	switch (key)
	{
		/*
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;
	*/
	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_team_cursor >= TEAM_ITEMS)
			m_team_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_team_cursor < 0)
			m_team_cursor = TEAM_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_team_cursor)
		{
		case 0:
			//M_Menu_Terror_f ();
			Cbuf_AddText ("impulse 54\n");
			break;
		case 1:
			//M_Menu_Counter_f ();
			Cbuf_AddText ("impulse 94\n");
			break;
		}
	}
}
//=============================================================================
/* Terrorist Menu */

int	m_terror_cursor;
#define	TERROR_ITEMS	4


void M_Menu_Terror_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_terror;
	m_entersound = true;
}


void M_Terror_Draw (void)
{
	int		f;
	qpic_t	*p;

if (m_terror_cursor ==0)
{
    Draw_TransPic(150, 60, Draw_CachePic ("gfx/team/terror.lmp"));
    M_PrintWhite (-70, 200, "Having estabilished a reputation for killing anyone");
    M_PrintWhite (-70, 210, "gets in their way, the Pheonix Faction is one of");
    M_PrintWhite (-70, 220, "the most feared terrorist groups in Eastern Europe.");
    M_PrintWhite (-70, 230, "Formed shortly after the breakup of the USSR.");

}
else if (m_terror_cursor ==1)
{
    Draw_TransPic(150, 60,Draw_CachePic("gfx/team/leet.lmp"));
    M_PrintWhite (-70, 200, "Middle Easter Fundamentalist group bent on");
    M_PrintWhite (-70, 210, "world domination and various other evil deeds");
}	
else if (m_terror_cursor ==2)
{
    Draw_TransPic (150,60,Draw_CachePic("gfx/team/arctic.lmp"));

    M_PrintWhite (-70, 200, "Swedish terrorist faction founded in 1977.");
   M_PrintWhite (-70, 210, "Famous for their bombing of the Canadian embassy in 1990.");

}
else if (m_terror_cursor ==3)
{
    Draw_TransPic (150,60,Draw_CachePic("gfx/team/guerilla.lmp"));

    M_PrintWhite (-70, 200, "A terrorist faction founded in the Middle East, this");
   M_PrintWhite (-70, 210, "group has a reputation for reputation ruthlessness. Their");
   M_PrintWhite (-70, 220, "disgust for teh American lifestyle was demonstrated in");
   M_PrintWhite (-70, 230, "their 1982 bombing of a school bis full of Rock and Roll");
   M_PrintWhite (-70, 240, "musicians.");
}

  M_PrintWhite (-30, 11, "CHOOSE A CLASS");

  M_PrintWhite (-30, 39, "PHOENIX CONNEXION");
  M_PrintWhite (-30, 59, "L337 CREW");
  M_PrintWhite (-30, 79, "ARTIC AVENGERS");
  M_PrintWhite (-30, 99, "GUERILLA WARFARE");

M_DrawTransPic (250, 200,Draw_CachePic("gfx/help1.lmp") );

M_DrawTransPic (-50, 32 + m_terror_cursor * 20,Draw_CachePic( va("gfx/menudot.lmp") ) );
}


void M_Terror_Key (int key)
{
	switch (key)
	{
	/*
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;
	*/
	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_terror_cursor >= TERROR_ITEMS)
			m_terror_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_terror_cursor < 0)
			m_terror_cursor = TERROR_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_terror_cursor)
		{
		case 0:
		    Cbuf_AddText ("impulse 54\n");
			break;

		case 1:
			Cbuf_AddText ("impulse 55\n");
			break;
		case 2:
			Cbuf_AddText ("impulse 56\n");
			break;

		case 3:
			Cbuf_AddText ("impulse 57\n");
			break;

		}
	}
}

//=============================================================================
/* Counter-Terrorist Menu */

int	m_counter_cursor;
#define	COUNTER_ITEMS	4


void M_Menu_Counter_f (void)
{

	if (key_dest != key_menu)
	{
		m_save_demonum = cls.demonum;
		cls.demonum = -1;
	}
	key_dest = key_menu;
	m_state = m_counter;
	m_entersound = true;
}


void M_Counter_Draw (void)
{
	int		f;
	qpic_t	*p;

if (m_counter_cursor ==0)
{
    Draw_TransPic(150,60, Draw_CachePic ("gfx/team/urban.lmp"));

    M_PrintWhite (-70, 200, "ST-6 (to be known later es DEVGRU was founded in 1980");
    M_PrintWhite (-70, 210, "under the command of Lieutnant-Commander Richard");
    M_PrintWhite (-70, 220, "Marcincko. ST-6 was placed on permanent alert to respond");
    M_PrintWhite (-70, 230, "to terrorist attacks against American targets worldwide.");

}
else if (m_counter_cursor ==1)
{
    Draw_TransPic (150,60,Draw_CachePic("gfx/team/gsg9.lmp"));

    M_PrintWhite (-70, 200, "GSG-9 was born out of the tragic events that led to the");
    M_PrintWhite (-70, 210, "death of several Israeli athletes during the");
    M_PrintWhite (-70, 220, "1972 Olympic games in Munich, Germany.");

}	
else if (m_counter_cursor ==2)
{
    Draw_TransPic (150,60,Draw_CachePic("gfx/team/sas.lmp"));
    M_PrintWhite (-70, 200, "The world-renowned British SAS was founded in the Second");
    M_PrintWhite (-70, 210, "World War by a man named David Stirling. Their");
    M_PrintWhite (-70, 220, "role during WW2 involved gathering intelligence behind");
    M_PrintWhite (-70, 230, "enemy lines an executive sabotage strikes and");
    M_PrintWhite (-70, 240, "assassinations against key targets.");

}
else if (m_counter_cursor ==3)
{
    Draw_TransPic (150,60,Draw_CachePic("gfx/team/gign.lmp"));
    M_PrintWhite (-70, 200, "France`s elite Counter_Terrorist unit, the GIGN, was");
    M_PrintWhite (-70, 210, "designed to be a faast response force that could");
    M_PrintWhite (-70, 220, "decisively react to any large-scale terrorist incident.");
    M_PrintWhite (-70, 230, "Consisting of no more than 100 men, the GIGN has earned");
    M_PrintWhite (-70, 240, "its reputation through a history of successful ops.");

}

  M_PrintWhite (-30, 11, "CHOOSE A CLASS");

  M_PrintWhite (-30, 39, "SEAL TEAM 6");
  M_PrintWhite (-30, 59, "GSG 9");
  M_PrintWhite (-30, 79, "SAS");
  M_PrintWhite (-30, 99, "GIGN");

M_DrawTransPic (250, 200,Draw_CachePic("gfx/help1.lmp") );

M_DrawTransPic (-50, 32 + m_counter_cursor * 20,Draw_CachePic( va("gfx/menudot.lmp") ) );
// Entar - end new code
}


void M_Counter_Key (int key)
{
	switch (key)
	{
	/*
	case K_ESCAPE:
		key_dest = key_game;
		m_state = m_none;
		cls.demonum = m_save_demonum;
		if (cls.demonum != -1 && !cls.demoplayback && cls.state != ca_connected)
			CL_NextDemo ();
		break;
	*/
	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_counter_cursor >= COUNTER_ITEMS)
			m_counter_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_counter_cursor < 0)
			m_counter_cursor = COUNTER_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		key_dest = key_game;
		m_state = m_none;
		switch (m_counter_cursor)
		{
		case 0:
		    Cbuf_AddText ("impulse 94\n");
			break;

		case 1:
			Cbuf_AddText ("impulse 95\n");
			break;
		case 2:
			Cbuf_AddText ("impulse 96\n");
			break;

		case 3:
			Cbuf_AddText ("impulse 97\n");
			break;

		}
	}
}

//=============================================================================
/* MAPS MENU by Crow_bar */
#include <pspiofilemgr.h>

#define MAPS_INPAGE  25
#define MAPS_MAX     256
#define MAPS_MAXNAME 32

int		maps_cursor;
int     maps_stage;
int     maps_num;
int     maps_maxlen;
char    maps_list[MAPS_MAX][MAPS_MAXNAME];


void M_ScanMaps ()
{
    maps_num = 0;
	SceUID dir = sceIoDopen(va("%s/maps", com_gamedir));
	if(dir < 0)
	{
		return;
	}

	SceIoDirent dirent;
    memset(&dirent, 0, sizeof(SceIoDirent));
	while(sceIoDread(dir, &dirent) > 0)
	{
		if(dirent.d_name[0] == '.')
		{
			continue;
		}
		if(!strcasecmp(COM_FileExtension (dirent.d_name),"bsp"))
	    {
		  COM_StripExtension (dirent.d_name, maps_list[maps_num]);

		  int cur_len = strlen(maps_list[maps_num]);
		  if(maps_maxlen < cur_len)
			 maps_maxlen = cur_len;

          maps_num++;
        }
        memset(&dirent, 0, sizeof(SceIoDirent));
    }
	sceIoDclose(dir);

}

void M_Menu_Maps_f (void)
{
	m_entersound = true;
	m_state = m_maps;
	key_dest = key_menu;
	M_ScanMaps ();
}

void M_Maps_Draw (void)
{
	int		i;
	int		f;
	qpic_t	*p, *background;
	background = Draw_CachePic ("gfx/conback.tga");
    M_DrawPic (0, 0, background);

    if((!maps_list[0]) && (maps_num == 0))
    {
       M_PrintWhite (64, 64 + 8,"Maps not found");
       return;
	}

    M_DrawTextBox (56, 56, maps_maxlen, MAPS_INPAGE);

	for (i = 0 ; i < MAPS_INPAGE; i++)
	{
		M_PrintWhite (64, 64 + 8*(i), maps_list[i+maps_stage]);
    }

// line cursor
	M_DrawCharacter (56, 64 + maps_cursor * 8, 12+((int)(realtime*4)&1));
}

void M_Maps_Key (int k)
{
	switch (k)
	{
	case K_ESCAPE:
		M_Menu_Main_f ();
		break;

	case K_ENTER:
		m_state = m_none;
		key_dest = key_game;
		Cbuf_AddText ("disconnect\n");
		Cbuf_AddText (va("map %s\n", maps_list[maps_cursor+maps_stage]));
		return;

	case K_UPARROW:
	case K_LEFTARROW:
		S_LocalSound ("misc/menu1.wav");

		maps_cursor--;
	    if(maps_cursor < 0)//hh
	    {
			maps_cursor = 0;
			if(maps_stage > 0)
		       maps_stage--;
        }

		break;

	case K_DOWNARROW:
	case K_RIGHTARROW:
		S_LocalSound ("misc/menu1.wav");

	    if(maps_cursor >= maps_num-1)
		   break;

	    maps_cursor++;
	    if(maps_cursor > MAPS_INPAGE-1)
	    {
		   maps_cursor = MAPS_INPAGE-1;
		   if(maps_stage + maps_cursor < maps_num-1)
		      maps_stage++;
        }
		break;
	}
}

//=============================================================================
/* MULTIPLAYER MENU */

int	m_multiplayer_cursor;

#define	MULTIPLAYER_ITEMS	4


void M_Menu_MultiPlayer_f (void)
{
	key_dest = key_menu;
	m_state = m_multiplayer;
	m_entersound = true;
}


void M_MultiPlayer_Draw (void)
{
	int		f;
	qpic_t	*p;


//M_DrawTransPic (76, 11,Draw_CachePic("gfx/mp.lmp"));

    M_PrintWhite(-50,50,"Join Game");
	M_PrintWhite(-50,60,"Create Game");
	M_PrintWhite(-50,70,"Setup");
	M_PrintWhite(-50,80,"Disconnect");

	
	switch(m_multiplayer_cursor)
	{
		case 0:
		M_Print(-50,50,"Join Game");
		break;
		case 1:
		M_Print(-50,60,"Create Game");
		break;
		case 2:
		M_Print(-50,70,"Setup");
		break;
		case 3:
		M_Print(-50,80,"Disconnect");
		break;
	}
	f = (int)(host_time * 10)%6;

	if (serialAvailable || ipxAvailable || tcpipAvailable)
		return;
}


void M_MultiPlayer_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		M_Menu_Main_f ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_multiplayer_cursor >= MULTIPLAYER_ITEMS)
			m_multiplayer_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_multiplayer_cursor < 0)
			m_multiplayer_cursor = MULTIPLAYER_ITEMS - 1;
		break;

	case K_ENTER:
		m_entersound = true;
		switch (m_multiplayer_cursor)
		{
		case 0:
			if (serialAvailable || ipxAvailable || tcpipAvailable)
				M_Menu_LanConfig_f ();
			break;

		case 1:
			if (serialAvailable || ipxAvailable || tcpipAvailable)
				M_Menu_LanConfig_f ();
			break;

		case 2:
			M_Menu_Setup_f ();
			break;

		case 3:
		Datagram_Shutdown();

		}
	}
}


//=============================================================================
/* SETUP MENU */

int		setup_cursor = 3; // с каого места начать курсор 0 1 2 3 
int		setup_cursor_table[] = {40, 56, 72, 156}; //0 1 2 3 

char	setup_hostname[16];
char	setup_myname[16];
int		setup_oldtop;
int		setup_oldbottom;
int		setup_top;
int		setup_bottom;

#define	NUM_SETUP_CMDS	4
extern int totalAccessPoints;
extern int accessPointNumber[100];
char    setup_accesspoint[64];

void M_Menu_Setup_f (void)
{
	key_dest = key_menu;
	m_state = m_setup;
	m_entersound = true;
	Q_strcpy(setup_myname, cl_name.string);
	Q_strcpy(setup_hostname, hostname.string);
	//setup_top = setup_oldtop = ((int)cl_color.value) >> 4;
	//setup_bottom = setup_oldbottom = ((int)cl_color.value) & 15;
#ifdef PSP
	if(totalAccessPoints)
	{
	    sceUtilityGetNetParam(accessPointNumber[(int)accesspoint.value], 0, (netData*)setup_accesspoint);
	}
#endif
}

void M_Setup_Draw (void)
{
	int offset = 0;



#ifdef PSP
	offset = 16;

	M_Print (64, 40, "Access Point");
	M_PrintWhite (168, 40, setup_accesspoint);
#endif
	M_Print (64, 40+offset, "Hostname");
	M_PrintWhite (168, 56, setup_hostname);

	M_Print (64, 56+offset, "Your name");
	M_PrintWhite (168, 56+offset, setup_myname);

	M_PrintWhite (72, 140+offset, "Accept Changes");
	M_DrawCharacter (56, setup_cursor_table [setup_cursor], 12+((int)(realtime*4)&1));

#ifndef PSP
	offset = 0;
#else
	offset = 1;
#endif

	if (setup_cursor == 0+offset)
		M_DrawCharacter (168 + 8*strlen(setup_hostname), setup_cursor_table [setup_cursor], 10+((int)(realtime*4)&1));

	if (setup_cursor == 1+offset)
		M_DrawCharacter (168 + 8*strlen(setup_myname), setup_cursor_table [setup_cursor], 10+((int)(realtime*4)&1));
}


void M_Setup_Key (int k)
{
	int	l;
	int	offset = 0;

	switch (k)
	{
	case K_ESCAPE:
		M_Menu_MultiPlayer_f ();
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		setup_cursor--;
		if (setup_cursor < 0)
			setup_cursor = NUM_SETUP_CMDS-1;
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		setup_cursor++;
		if (setup_cursor >= NUM_SETUP_CMDS)
			setup_cursor = 0;
		break;

	case K_LEFTARROW:
#ifdef PSP
		if (setup_cursor == 0)
		{
			S_LocalSound ("misc/menu3.wav");
			if(accesspoint.value > 1)
			{
				Cvar_SetValue("accesspoint", accesspoint.value-1);
				sceUtilityGetNetParam(accessPointNumber[(int)accesspoint.value], 0, (netData*)setup_accesspoint);			
			}
		}
		offset = 1;
#endif
		if (setup_cursor < 2+offset)
			return;
		S_LocalSound ("misc/menu3.wav");
		if (setup_cursor == 2+offset)
			setup_top = setup_top - 1;
		if (setup_cursor == 3+offset)
			setup_bottom = setup_bottom - 1;
		break;
	case K_RIGHTARROW:
#ifdef PSP
		if (setup_cursor == 0)
		{
			S_LocalSound ("misc/menu3.wav");
			if(accesspoint.value < totalAccessPoints)
			{
				Cvar_SetValue("accesspoint", accesspoint.value+1);
				sceUtilityGetNetParam(accessPointNumber[(int)accesspoint.value], 0, (netData*)setup_accesspoint);
			}
		}

		offset = 1;
#endif

		if (setup_cursor < 2+offset)
			return;
		/*
forward:
		S_LocalSound ("misc/menu3.wav");
		if (setup_cursor == 2+offset)
			setup_top = setup_top + 1;
		if (setup_cursor == 3+offset)
			setup_bottom = setup_bottom + 1;
		*/
		break;

	case K_INS:
#ifdef PSP
		offset = 1;
#endif
		if (setup_cursor == 0+offset)
		{
			M_Menu_OSK_f(setup_hostname, setup_hostname, 16);
			break;
		}

		if (setup_cursor == 1+offset)
		{
			M_Menu_OSK_f(setup_myname, setup_myname,16);
			break;
		}
		break;


	case K_ENTER:
#ifdef PSP
		offset = 1;
#endif
	//	if (setup_cursor == 0 || setup_cursor == 0+offset)
	//		return;

		//if (setup_cursor == 2+offset || setup_cursor == 3+offset)
		//	goto forward;

		if (setup_cursor < 3)
			break;

		// setup_cursor == 4 (OK)
		if (Q_strcmp(cl_name.string, setup_myname) != 0)
			Cbuf_AddText ( va ("name \"%s\"\n", setup_myname) );
		if (Q_strcmp(hostname.string, setup_hostname) != 0)
			Cvar_Set("hostname", setup_hostname);
		//if (setup_top != setup_oldtop || setup_bottom != setup_oldbottom)
		//	Cbuf_AddText( va ("colora %i %i\n", setup_top, setup_bottom) );
		m_entersound = true;
		M_Menu_MultiPlayer_f ();
		break;

	case K_BACKSPACE:
#ifdef PSP
		offset = 1;
#endif
		if (setup_cursor == 0+offset)
		{
			if (strlen(setup_hostname))
				setup_hostname[strlen(setup_hostname)-1] = 0;
		}

		if (setup_cursor == 1+offset)
		{
			if (strlen(setup_myname))
				setup_myname[strlen(setup_myname)-1] = 0;
		}
		break;

	default:
		if (k < 32 || k > 127)
			break;

#ifdef PSP
		offset = 1;
#endif

		if (setup_cursor == 0+offset)
		{
			l = strlen(setup_hostname);
			if (l < 15)
			{
				setup_hostname[l+1] = 0;
				setup_hostname[l] = k;
			}
		}
		if (setup_cursor == 1+offset)
		{
			l = strlen(setup_myname);
			if (l < 15)
			{
				setup_myname[l+1] = 0;
				setup_myname[l] = k;
			}
		}
	}

	if (setup_top > 13)
		setup_top = 0;
	if (setup_top < 0)
		setup_top = 13;
	if (setup_bottom > 13)
		setup_bottom = 0;
	if (setup_bottom < 0)
		setup_bottom = 13;
}

//=============================================================================
/* NET MENU */

int	m_net_cursor;
int m_net_items;
int m_net_saveHeight;

char *net_helpMessage [] =
{
/* .........1.........2.... */
  "                        ",
  " Two computers connected",
  "   through two modems.  ",
  "                        ",

  "                        ",
  " Two computers connected",
  " by a null-modem cable. ",
  "                        ",

  " Novell network LANs    ",
  " or Windows 95 DOS-box. ",
  "                        ",
  "(LAN=Local Area Network)",

  " Commonly used to play  ",
  " over the Internet, but ",
  " also used on a Local   ",
  " Area Network.          "
};

void M_Menu_Net_f (void)
{
	key_dest = key_menu;
	m_state = m_net;
	m_entersound = true;
	m_net_items = 4;

	if (m_net_cursor >= m_net_items)
		m_net_cursor = 0;
	m_net_cursor--;
	M_Net_Key (K_DOWNARROW);
}


void M_Net_Draw (void)
{
	int		f;
	qpic_t	*p;



	f = 32;

	if (serialAvailable)
	{
		p = Draw_CachePic ("gfx/netmen1.lmp");
	}
	else
	{
#ifdef WIN32
		p = NULL;
#else
		p = Draw_CachePic ("gfx/dim_modm.lmp");
#endif
	}

	if (p)
		M_DrawTransPic (72, f, p);

	f += 19;

	if (serialAvailable)
	{
		p = Draw_CachePic ("gfx/netmen2.lmp");
	}
	else
	{
#ifdef WIN32
		p = NULL;
#else
		p = Draw_CachePic ("gfx/dim_drct.lmp");
#endif
	}

	if (p)
		M_DrawTransPic (72, f, p);

	f += 19;
	if (ipxAvailable)
		p = Draw_CachePic ("gfx/netmen3.lmp");
	else
		p = Draw_CachePic ("gfx/dim_ipx.lmp");
	M_DrawTransPic (72, f, p);

	f += 19;
	if (tcpipAvailable)
		p = Draw_CachePic ("gfx/netmen4.lmp");
	else
		p = Draw_CachePic ("gfx/dim_tcp.lmp");
	M_DrawTransPic (72, f, p);

	if (m_net_items == 5)	// JDC, could just be removed
	{
		f += 19;
		p = Draw_CachePic ("gfx/netmen5.lmp");
		M_DrawTransPic (72, f, p);
	}

	f = (320-26*8)/2;
	M_DrawTextBox (f, 134, 24, 4);
	f += 8;
	M_Print (f, 142, net_helpMessage[m_net_cursor*4+0]);
	M_Print (f, 150, net_helpMessage[m_net_cursor*4+1]);
	M_Print (f, 158, net_helpMessage[m_net_cursor*4+2]);
	M_Print (f, 166, net_helpMessage[m_net_cursor*4+3]);

	f = (int)(host_time * 10)%6;
	M_DrawTransPic (54, 32 + m_net_cursor * 20,Draw_CachePic( va("gfx/menudot.lmp") ) );
}


void M_Net_Key (int k)
{
again:
	switch (k)
	{
	case K_ESCAPE:
		M_Menu_MultiPlayer_f ();
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		if (++m_net_cursor >= m_net_items)
			m_net_cursor = 0;
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		if (--m_net_cursor < 0)
			m_net_cursor = m_net_items - 1;
		break;

	case K_ENTER:
		m_entersound = true;

		switch (m_net_cursor)
		{
		case 0:
			break;

		case 1:
			break;

		case 2:
			M_Menu_LanConfig_f ();
			break;

		case 3:
			M_Menu_LanConfig_f ();
			break;

		case 4:
// multiprotocol
			break;
		}
	}

	if (m_net_cursor == 0 && !serialAvailable)
		goto again;
	if (m_net_cursor == 1 && !serialAvailable)
		goto again;
	if (m_net_cursor == 2 && !ipxAvailable)
		goto again;
	if (m_net_cursor == 3 && !tcpipAvailable)
		goto again;
}

//=============================================================================
/* OPTIONS MENU */
#define	SLIDER_RANGE	10
#define NUM_SUBMENU 2
#ifdef PSP_HARDWARE_VIDEO
enum 
{
	OPT_CUSTOMIZE = 0,
	OPT_CONSOLE,
	OPT_DEFAULTS,
	OPT_SUBMENU,
	OPTIONS_ITEMS
};
enum 
{
	OPT_SUBMENU_0 = OPT_SUBMENU,	
	OPT_GAMMA,			
	OPT_SNDVOL,	
	OPT_SHADOWS,
    OPTIONS_ITEMS_0
};
enum 
{
	OPT_SUBMENU_1 = OPT_SUBMENU,
    //OPT_GAP_1,
	OPT_IN_SPEED,
	OPT_IN_TOLERANCE,
	OPT_IN_ACCELERATION,	
	OPT_INVMOUSE,	
	OPT_MOUSELOOK,
	OPT_MOUSESTAFE,	
	OPT_IN_X_ADJUST,
	OPT_IN_Y_ADJUST,
    OPTIONS_ITEMS_1
};
#else
enum 
{
	OPT_CUSTOMIZE = 0,
	OPT_CONSOLE,
	OPT_DEFAULTS,
	OPT_SUBMENU,
	OPTIONS_ITEMS
};
enum 
{
	OPT_SUBMENU_0 = OPT_SUBMENU,	
	OPT_GAMMA,			
	OPT_SNDVOL,		
    OPTIONS_ITEMS_0
};
enum 
{
	OPT_SUBMENU_1 = OPT_SUBMENU,
    //OPT_GAP_1,
	OPT_IN_SPEED,
	OPT_IN_TOLERANCE,
	OPT_IN_ACCELERATION,	
	OPT_INVMOUSE,	
	OPT_MOUSELOOK,
	OPT_MOUSESTAFE,
	OPT_IN_X_ADJUST,
	OPT_IN_Y_ADJUST,	
    OPTIONS_ITEMS_1
};
#endif	

int	options_cursor;
int m_submenu = 0;

void M_Menu_Options_f (void)
{
	key_dest = key_menu;
	m_state = m_options;
	m_entersound = true;
}

void M_AdjustSliders (int dir)
{
	S_LocalSound ("misc/menu3.wav");

	switch (options_cursor)
	{
		case OPT_SUBMENU:
	        m_submenu += dir;
	        if (m_submenu > NUM_SUBMENU-1)
	        	m_submenu = 0;
	        else if (m_submenu < 0)
	        	m_submenu = NUM_SUBMENU-1;
	        break;	
	}
	
    if (m_submenu == 0)
    {
    	switch (options_cursor)
        {
			/*
			case OPT_CRHAIR:
				crosshair.value += dir * 1;
				if(crosshair.value > 6)
					crosshair.value = 6;
				if(crosshair.value < 0)
					crosshair.value = 0;
				Cvar_SetValue ("crosshair", crosshair.value);
				break;
			*/
				/*
			case OPT_CRCOLOR:	
				crosshaircolor.value += dir * 10;
				if (crosshaircolor.value < 0)
					crosshaircolor.value = 0;
				if (crosshaircolor.value > 255)
					crosshaircolor.value = 255;
				Cvar_SetValue ("crosshaircolor", crosshaircolor.value);
				break;
				*/
			case OPT_GAMMA:	// gamma
				v_gamma.value -= dir * 0.05;
				if (v_gamma.value < 0.5)
					v_gamma.value = 0.5;
				if (v_gamma.value > 1)
					v_gamma.value = 1;
				Cvar_SetValue ("gamma", v_gamma.value);
				break;
			case OPT_SNDVOL:	// sfx volume
				volume.value += dir * 0.1;
				if (volume.value < 0)
					volume.value = 0;
				if (volume.value > 1)
					volume.value = 1;
				Cvar_SetValue ("volume", volume.value);
				break;
			case OPT_SHADOWS:	// shadows
				Cvar_SetValue ("r_shadows", !r_shadows.value);
				break;	
				
				/*
			case OPT_QMB:
				if(qmb_particles.value == 1)
					qmb_particles.value = 0;
				else
					qmb_particles.value = 1;
				Cvar_SetValue ("qmb_particles", qmb_particles.value);
				Cvar_SetValue ("r_part_explosions",  qmb_particles.value);
				Cvar_SetValue ("r_part_trails",      qmb_particles.value);
				Cvar_SetValue ("r_part_sparks",      qmb_particles.value);
				Cvar_SetValue ("r_part_spikes",      qmb_particles.value);
				Cvar_SetValue ("r_part_gunshots",    qmb_particles.value);
				Cvar_SetValue ("r_part_blood",       qmb_particles.value);
				Cvar_SetValue ("r_part_telesplash",  qmb_particles.value);
				Cvar_SetValue ("r_part_blobs",       qmb_particles.value);
				Cvar_SetValue ("r_part_lavasplash",  qmb_particles.value);
				Cvar_SetValue ("r_part_flames",      qmb_particles.value);
				Cvar_SetValue ("r_part_lightning",   qmb_particles.value);
				Cvar_SetValue ("r_part_flies",       qmb_particles.value);
				Cvar_SetValue ("r_part_muzzleflash", qmb_particles.value);
				break;
				*/
        }
    }
    else if (m_submenu == 1)
    {
       	switch (options_cursor)
        {

       		case OPT_IN_SPEED:	// mouse speed
				in_sensitivity.value += dir * 0.5;
				if (in_sensitivity.value < 1)
					in_sensitivity.value = 1;
				if (in_sensitivity.value > 11)
					in_sensitivity.value = 11;
				Cvar_SetValue ("sensitivity", in_sensitivity.value);
				break;
       		
       		case OPT_IN_TOLERANCE:	// mouse tolerance
				in_tolerance.value += dir * 0.05;
				if (in_tolerance.value < 0)
					in_tolerance.value = 0;
				if (in_tolerance.value > 1)
					in_tolerance.value = 1;
				Cvar_SetValue ("tolerance", in_tolerance.value);
				break;

       		case OPT_IN_ACCELERATION:	// mouse tolerance
				in_acceleration.value -= dir * 0.25;
				if (in_acceleration.value < 0.5)
					in_acceleration.value = 0.5;
				if (in_acceleration.value > 2)
					in_acceleration.value = 2;
				Cvar_SetValue ("acceleration", in_acceleration.value);
				break;
				
			case OPT_IN_X_ADJUST:	
				in_x_axis_adjust.value += dir*5;
				if (in_x_axis_adjust.value < -127)
					in_x_axis_adjust.value = -127;
				if (in_x_axis_adjust.value > 127)
					in_x_axis_adjust.value = 127;
				Cvar_SetValue ("in_x_axis_adjust", in_x_axis_adjust.value);
				break;
				
			case OPT_IN_Y_ADJUST:	
				in_y_axis_adjust.value += dir*5;
				if (in_y_axis_adjust.value < -127)
					in_y_axis_adjust.value = -127;
				if (in_y_axis_adjust.value > 127)
					in_y_axis_adjust.value = 127;
				Cvar_SetValue ("in_y_axis_adjust", in_y_axis_adjust.value);
				break;
								
			case OPT_INVMOUSE:	// invert mouse
				Cvar_SetValue ("m_pitch", -m_pitch.value);
				break;
			//case OPT_NOMOUSE:	// disable mouse
			//	Cvar_SetValue ("in_disable_analog", !in_disable_analog.value);
			//	break;
			case OPT_MOUSESTAFE:
				Cvar_SetValue ("in_analog_strafe", !in_analog_strafe.value);
				break;
			case OPT_MOUSELOOK:
				Cvar_SetValue ("in_analog_freelook", !in_analog_freelook.value);
				break;
        }	
    }    
}



void M_DrawSlider (int x, int y, float range)
{
	int	i;

	if (range < 0)
		range = 0;
	if (range > 1)
		range = 1;
	M_DrawCharacter (x-8, y, 128);
	for (i=0 ; i<SLIDER_RANGE ; i++)
		M_DrawCharacter (x + i*8, y, 129);
	M_DrawCharacter (x+i*8, y, 130);
	M_DrawCharacter (x + (SLIDER_RANGE-1)*8 * range, y, 131);
}

void M_Options_Draw (void)
{
	float	 r;
	qpic_t	*p;
	float d;


	M_Print (16, 32+(OPT_CUSTOMIZE*8), "    Customize controls");
	M_Print (16, 32+(OPT_CONSOLE*8),   "         Go to console");
	M_Print (16, 32+(OPT_DEFAULTS*8),  "   Default  Config ");

	switch (m_submenu) {
        case 0:    
		//	M_Print (16, 32+(OPT_CRHAIR*8), 	   "            Crosshair");
		//	r = crosshair.value * 0.16;
		//	M_DrawSlider (220, 32+(OPT_CRHAIR*8), r);
			
		//	M_Print (16, 32+(OPT_CRCOLOR*8),   "         Crosshair Color");
			//r = crosshaircolor.value;
		//	Draw_Fill(350, 32+(OPT_CRCOLOR*8),32,8, r);
		
			M_Print (16, 32+(OPT_GAMMA*8), 	   "            Brightness");
			r = (1.0 - v_gamma.value) / 0.5;
			M_DrawSlider (220, 32+(OPT_GAMMA*8), r);
	
		
			M_Print (16, 32+(OPT_SNDVOL*8),   "          Sound Volume");
			r = volume.value;
			M_DrawSlider (220, 32+(OPT_SNDVOL*8), r);
			
			M_Print (16, 32+(OPT_SHADOWS*8),   "          Shadows");
			M_DrawCheckbox (220, 32+(OPT_SHADOWS*8), r_shadows.value);
		//	M_Print (16, 32+(OPT_QMB*8),   "          QMB");
			//M_DrawCheckbox (220, 32+(OPT_QMB*8), qmb_particles.value);
		
            break;
        case 1:
		    
			M_Print (16, 32+(OPT_IN_SPEED*8), 		 "           A-Nub Speed");

			r = (in_sensitivity.value - 1)/10;
			M_DrawSlider (220, 32+(OPT_IN_SPEED*8), r);

			M_Print (16, 32+(OPT_IN_ACCELERATION*8), "    A-Nub Acceleration");
			r = 1.0f -((in_acceleration.value - 0.5f)/1.5f);
			M_DrawSlider (220, 32+(OPT_IN_ACCELERATION*8), r);

			M_Print (16, 32+(OPT_IN_TOLERANCE*8), 	 "      A-Nub Tollerance");
			r = (in_tolerance.value )/1.0f;
			M_DrawSlider (220, 32+(OPT_IN_TOLERANCE*8), r);
		
			M_Print (16, 32+(OPT_IN_X_ADJUST*8), 	 "         Adjust Axis X");	
			r = (128+in_x_axis_adjust.value)/255;
			M_DrawSlider (220, 32+(OPT_IN_X_ADJUST*8), r);

			M_Print (16, 32+(OPT_IN_Y_ADJUST*8), 	 "         Adjust Axis Y");	
			r = (128+in_y_axis_adjust.value)/255;
			M_DrawSlider (220, 32+(OPT_IN_Y_ADJUST*8), r);
		
		
		#ifdef PSP
			M_Print (16, 32+(OPT_INVMOUSE*8),        "          Invert A-Nub");
		#else
			M_Print (16, 32+(OPT_INVMOUSE*8),        "          Invert Mouse");
		#endif
			M_DrawCheckbox (220, 32+(OPT_INVMOUSE*8), m_pitch.value < 0);
		
			M_Print (16, 32+(OPT_MOUSELOOK*8),       "            A-Nub Look");
			M_DrawCheckbox (220, 32+(OPT_MOUSELOOK*8), in_analog_freelook.value);

		////	M_Print (16, 32+(OPT_NOMOUSE*8),         "         Disable A-Nub");
		//	M_DrawCheckbox (220, 32+(OPT_NOMOUSE*8), in_disable_analog.value );
		
			M_Print (16, 32+(OPT_MOUSESTAFE*8),		 "         A-Nub Stafing");
			M_DrawCheckbox (220, 32+(OPT_MOUSESTAFE*8), in_analog_strafe.value );
			break;
	}
	
	M_PrintWhite (16, 32+(OPT_SUBMENU*8),	 "        Select Submenu");
    switch (m_submenu)
        {
        case 0:
            M_PrintWhite (220, 32+(OPT_SUBMENU*8), "More options");         
            break;
        case 1:
            M_PrintWhite (220, 32+(OPT_SUBMENU*8), "Less options");         
            break;
        default:
            break;
        }                         	
	// Cursor
	M_DrawCharacter (200, 32 + options_cursor*8, 12+((int)(realtime*4)&1));
}


void M_Options_Key (int k)
{
	switch (k)
	{
	case K_ESCAPE:
		M_Menu_Main_f ();
		break;

	case K_ENTER:
		m_entersound = true;
		switch (options_cursor)
		{
		case OPT_CUSTOMIZE:
			M_Menu_Keys_f ();
			break;
		case OPT_CONSOLE:
			m_state = m_none;
			Con_ToggleConsole_f ();
			break;
		case OPT_DEFAULTS:
			Cbuf_AddText ("exec cfg/default.cfg\n");
			break;
		default:
			M_AdjustSliders (1);
			break;
		}
		return;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		options_cursor--;
		if (options_cursor < 0) {
			if (m_submenu == 0)
			    options_cursor = OPTIONS_ITEMS_0-1;
	        if (m_submenu == 1)
			    options_cursor = OPTIONS_ITEMS_1-1;
		}
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		options_cursor++;
        if (m_submenu == 0)
            if (options_cursor >= OPTIONS_ITEMS_0)
			    options_cursor = 0;

        if (m_submenu == 1)
            if (options_cursor >= OPTIONS_ITEMS_1)
			    options_cursor = 0;
		break;

	case K_LEFTARROW:
		M_AdjustSliders (-1);
		break;

	case K_RIGHTARROW:
		M_AdjustSliders (1);
		break;
	}
}

//=============================================================================
/* KEYS MENU */

char *bindnames[][2] =
{
{"+attack", 		"Attack"},
{"impulse 121", 		"Change weapon"},
{"+jump", 			"Jump / Swim up"},
{"+forward", 		"Walk forward"},
{"+back", 			"Walk backward"},
{"+left", 			"Turn left"},
{"+right", 			"Turn right"},
{"+moveleft", 		"Step left"},
{"+moveright", 		"Step right"},
{"+strafe", 		"Sidestep"},
{"+lookup", 		"Look up"},
{"+lookdown", 		"Look down"},
{"centerview", 		"Center view"},
#ifdef PSP
{"+mlook", 			"Analog nub look"},
#else
{"+mlook", 			"Mouse look"},
{"+klook", 			"Keyboard look"},
#endif
{"+moveup",			"Swim up"},
{"+movedown",		"Swim down"},
{"impulse 105", 		"Buy Menu"},
{"+use", 		"Secondary Attack"},
{"+crouch", 		"Crouch"},
};

#define	NUMCOMMANDS	(sizeof(bindnames)/sizeof(bindnames[0]))

int		keys_cursor;
int		bind_grab;

void M_Menu_Keys_f (void)
{
	key_dest = key_menu;
	m_state = m_keys;
	m_entersound = true;
}


void M_FindKeysForCommand (char *command, int *twokeys)
{
	int		count;
	int		j;
	int		l;
	char	*b;

	twokeys[0] = twokeys[1] = -1;
	l = strlen(command);
	count = 0;

	for (j=0 ; j<256 ; j++)
	{
		b = keybindings[j];
		if (!b)
			continue;
		if (!strncmp (b, command, l) )
		{
			twokeys[count] = j;
			count++;
			if (count == 2)
				break;
		}
	}
}

void M_UnbindCommand (char *command)
{
	int		j;
	int		l;
	char	*b;

	l = strlen(command);

	for (j=0 ; j<256 ; j++)
	{
		b = keybindings[j];
		if (!b)
			continue;
		if (!strncmp (b, command, l) )
			Key_SetBinding (j, "");
	}
}


void M_Keys_Draw (void)
{
	int		i, l;
	int		keys[2];
	char	*name;
	int		x, y;
	qpic_t	*p;



#ifdef PSP
	if (bind_grab)
		M_Print (12, 32, "Press a button for this action");
	else
		M_Print (18, 32, "Press CROSS to change, SQUARE to clear");
#else
	if (bind_grab)
		M_Print (12, 32, "Press a key or button for this action");
	else
		M_Print (18, 32, "Enter to change, backspace to clear");
#endif

// search for known bindings
	for (i=0 ; i<NUMCOMMANDS ; i++)
	{
		y = 48 + 8*i;

		M_Print (16, y, bindnames[i][1]);

		l = strlen (bindnames[i][0]);

		M_FindKeysForCommand (bindnames[i][0], keys);

		if (keys[0] == -1)
		{
			M_Print (140, y, "???");
		}
		else
		{
			name = Key_KeynumToString (keys[0]);
			M_Print (140, y, name);
			x = strlen(name) * 8;
			if (keys[1] != -1)
			{
				M_Print (140 + x + 8, y, "or");
				M_Print (140 + x + 32, y, Key_KeynumToString (keys[1]));
			}
		}
	}

	if (bind_grab)
		M_DrawCharacter (130, 48 + keys_cursor*8, '=');
	else
		M_DrawCharacter (130, 48 + keys_cursor*8, 12+((int)(realtime*4)&1));
}


void M_Keys_Key (int k)
{
	char	cmd[80];
	int		keys[2];

	if (bind_grab)
	{	// defining a key
		S_LocalSound ("misc/menu1.wav");
		if (k == K_ESCAPE)
		{
			bind_grab = false;
		}
		else if (k != '`')
		{
			sprintf (cmd, "bind \"%s\" \"%s\"\n", Key_KeynumToString (k), bindnames[keys_cursor][0]);
			Cbuf_InsertText (cmd);
		}

		bind_grab = false;
		return;
	}

	switch (k)
	{
	case K_ESCAPE:
		M_Menu_Options_f ();
		break;

	case K_LEFTARROW:
	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		keys_cursor--;
		if (keys_cursor < 0)
			keys_cursor = NUMCOMMANDS-1;
		break;

	case K_DOWNARROW:
	case K_RIGHTARROW:
		S_LocalSound ("misc/menu1.wav");
		keys_cursor++;
		if (keys_cursor >= NUMCOMMANDS)
			keys_cursor = 0;
		break;

	case K_ENTER:		// go into bind mode
		M_FindKeysForCommand (bindnames[keys_cursor][0], keys);
		S_LocalSound ("misc/menu2.wav");
		if (keys[1] != -1)
			M_UnbindCommand (bindnames[keys_cursor][0]);
		bind_grab = true;
		break;

	case K_BACKSPACE:		// delete bindings
	case K_DEL:				// delete bindings
		S_LocalSound ("misc/menu2.wav");
		M_UnbindCommand (bindnames[keys_cursor][0]);
		break;
	}
}

//=============================================================================
/* VIDEO MENU */

void M_Menu_Video_f (void)
{
	key_dest = key_menu;
	m_state = m_video;
	m_entersound = true;
}


void M_Video_Draw (void)
{
	(*vid_menudrawfn) ();
}


void M_Video_Key (int key)
{
	(*vid_menukeyfn) (key);
}

//=============================================================================
/* HELP MENU */

void M_Menu_Help_f (void)
{
	key_dest = key_menu;
	m_state = m_help;
	m_entersound = true;
}



void M_Help_Draw (void)
{
	 // Draw_Fill(180,130,80,20,100);
	  M_PrintWhite (25, 60, "By st1x51");
	  M_PrintWhite (25, 70, "Check for updates: vk.com/cscapsp");
}


void M_Help_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		M_Menu_Main_f ();
		break;
	}

}

//=============================================================================
/* QUIT MENU */

int		msgNumber;
int		m_quit_prevstate;
qboolean	wasInMenus;

void M_Menu_Quit_f (void)
{
	if (m_state == m_quit)
		return;
	wasInMenus = (key_dest == key_menu);
	key_dest = key_menu;
	m_quit_prevstate = m_state;
	m_state = m_quit;
	m_entersound = true;
	msgNumber = rand()&7;
}


void M_Quit_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
	case 'n':
	case 'N':
		if (wasInMenus)
		{
			m_state = m_quit_prevstate;
			m_entersound = true;
		}
		else
		{
			key_dest = key_game;
			m_state = m_none;
		}
		break;

	case 'Y':
	case 'y':
#ifdef PSP
	case K_ENTER:
#endif
		key_dest = key_console;
		Host_Quit_f ();
		break;

	default:
		break;
	}

}


void M_Quit_Draw (void)
{
	if (wasInMenus)
	{
		m_state = m_quit_prevstate;
		m_recursiveDraw = true;
		M_Draw ();
		m_state = m_quit;
	}

#ifdef WIN32
	M_DrawTextBox (0, 0, 38, 23);
	M_PrintWhite (16, 12,  "  Quake version 1.09 by id Software\n\n");
	M_PrintWhite (16, 28,  "Programming        Art \n");
	M_Print (16, 36,  " John Carmack       Adrian Carmack\n");
	M_Print (16, 44,  " Michael Abrash     Kevin Cloud\n");
	M_Print (16, 52,  " John Cash          Paul Steed\n");
	M_Print (16, 60,  " Dave 'Zoid' Kirsch\n");
	M_PrintWhite (16, 68,  "Design             Biz\n");
	M_Print (16, 76,  " John Romero        Jay Wilbur\n");
	M_Print (16, 84,  " Sandy Petersen     Mike Wilson\n");
	M_Print (16, 92,  " American McGee     Donna Jackson\n");
	M_Print (16, 100,  " Tim Willits        Todd Hollenshead\n");
	M_PrintWhite (16, 108, "Support            Projects\n");
	M_Print (16, 116, " Barrett Alexander  Shawn Green\n");
	M_PrintWhite (16, 124, "Sound Effects\n");
	M_Print (16, 132, " Trent Reznor and Nine Inch Nails\n\n");
	M_PrintWhite (16, 140, "Quake is a trademark of Id Software,\n");
	M_PrintWhite (16, 148, "inc., (c)1996 Id Software, inc. All\n");
	M_PrintWhite (16, 156, "rights reserved. NIN logo is a\n");
	M_PrintWhite (16, 164, "registered trademark licensed to\n");
	M_PrintWhite (16, 172, "Nothing Interactive, Inc. All rights\n");
	M_PrintWhite (16, 180, "reserved. Press y to exit\n");
#elif defined PSP
	M_Print (64, 100,	"  Press CROSS to quit,  ");
	M_Print (64, 108,	" or CIRCLE to continue. ");
#else
	M_DrawTextBox (56, 76, 24, 4);
	M_Print (64, 84,  quitMessage[msgNumber*4+0]);
	M_Print (64, 92,  quitMessage[msgNumber*4+1]);
	M_Print (64, 100, quitMessage[msgNumber*4+2]);
	M_Print (64, 108, quitMessage[msgNumber*4+3]);
#endif
}

//=============================================================================
/* OSK IMPLEMENTATION */
#define CHAR_SIZE 8
#define MAX_Y 8
#define MAX_X 12

#define MAX_CHAR_LINE 36
#define MAX_CHAR      72

int  osk_pos_x = 0;
int  osk_pos_y = 0;
int  max_len   = 0;
int  m_old_state = 0;

char* osk_out_buff = NULL;
char  osk_buffer[128];

char *osk_text [] = 
	{ 
		" 1 2 3 4 5 6 7 8 9 0 - = ` ",
		" q w e r t y u i o p [ ]   ",
		"   a s d f g h j k l ; ' \\ ",
		"     z x c v b n m   , . / ",
		"                           ",
		" ! @ # $ % ^ & * ( ) _ + ~ ",
		" Q W E R T Y U I O P { }   ",
		"   A S D F G H J K L : \" | ",
		"     Z X C V B N M   < > ? "
	};

char *osk_help [] = 
	{ 
		"CONFIRM: ",
		" SQUARE  ",
		"CANCEL:  ",
		" CIRCLE  ",
		"DELETE:  ",
		" TRIAGLE ",
		"ADD CHAR:",
		" CROSS   ",
		""
	};

void M_Menu_OSK_f (char *input, char *output, int outlen)
{
	key_dest = key_menu;
	m_old_state = m_state;
	m_state = m_osk;
	m_entersound = false;
	max_len = outlen;
	strncpy(osk_buffer,input,max_len);
	osk_buffer[outlen] = '\0';
	osk_out_buff = output; 
}

void Con_OSK_f (char *input, char *output, int outlen)
{
	max_len = outlen;
	strncpy(osk_buffer,input,max_len);
	osk_buffer[outlen] = '\0';
	osk_out_buff = output; 
}


void M_OSK_Draw (void)
{
#ifdef PSP
	int x,y;
	int i;
	
	char *selected_line = osk_text[osk_pos_y]; 
	char selected_char[2];
	
	selected_char[0] = selected_line[1+(2*osk_pos_x)];
	selected_char[1] = '\0';
	if (selected_char[0] == ' ' || selected_char[0] == '\t') 
		selected_char[0] = 'X';
		
	y = 20;
	x = 16;

	M_DrawTextBox (10, 10, 		     26, 10);
	M_DrawTextBox (10+(26*CHAR_SIZE),    10,  10, 10);
	M_DrawTextBox (10, 10+(10*CHAR_SIZE),36,  3);
	
	for(i=0;i<=MAX_Y;i++) 
	{
		M_PrintWhite (x, y+(CHAR_SIZE*i), osk_text[i]);
		if (i % 2 == 0)
			M_Print      (x+(27*CHAR_SIZE), y+(CHAR_SIZE*i), osk_help[i]);
		else			
			M_PrintWhite (x+(27*CHAR_SIZE), y+(CHAR_SIZE*i), osk_help[i]);
	}
	
	int text_len = strlen(osk_buffer);
	if (text_len > MAX_CHAR_LINE) {
		
		char oneline[MAX_CHAR_LINE+1];
		strncpy(oneline,osk_buffer,MAX_CHAR_LINE);
		oneline[MAX_CHAR_LINE] = '\0';
		
		M_Print (x+4, y+4+(CHAR_SIZE*(MAX_Y+2)), oneline );
		
		strncpy(oneline,osk_buffer+MAX_CHAR_LINE, text_len - MAX_CHAR_LINE);
		oneline[text_len - MAX_CHAR_LINE] = '\0';
		
		M_Print (x+4, y+4+(CHAR_SIZE*(MAX_Y+3)), oneline );
		M_PrintWhite (x+4+(CHAR_SIZE*(text_len - MAX_CHAR_LINE)), y+4+(CHAR_SIZE*(MAX_Y+3)),"_");
	}
	else {
		M_Print (x+4, y+4+(CHAR_SIZE*(MAX_Y+2)), osk_buffer );
		M_PrintWhite (x+4+(CHAR_SIZE*(text_len)), y+4+(CHAR_SIZE*(MAX_Y+2)),"_");
	}
	M_Print      (x+((((osk_pos_x)*2)+1)*CHAR_SIZE), y+(osk_pos_y*CHAR_SIZE), selected_char);

#endif
}

void M_OSK_Key (int key)
{
#ifdef PSP
	switch (key)
	{
	case K_RIGHTARROW:
		osk_pos_x++;
		if (osk_pos_x > MAX_X)
			osk_pos_x = MAX_X;
		break;
	case K_LEFTARROW:
		osk_pos_x--;
		if (osk_pos_x < 0)
			osk_pos_x = 0;
		break;
	case K_DOWNARROW:
		osk_pos_y++;
		if (osk_pos_y > MAX_Y)
			osk_pos_y = MAX_Y;
		break;
	case K_UPARROW:
		osk_pos_y--;
		if (osk_pos_y < 0)
			osk_pos_y = 0;
		break;
	case K_ENTER: 
		if (max_len > strlen(osk_buffer)) {
			char *selected_line = osk_text[osk_pos_y]; 
			char selected_char[2];
			
			selected_char[0] = selected_line[1+(2*osk_pos_x)];
			
			if (selected_char[0] == '\t')
				selected_char[0] = ' ';
			
			selected_char[1] = '\0';
			strcat(osk_buffer,selected_char);		
		}
		break;
	case K_DEL:
		if (strlen(osk_buffer) > 0) {
			osk_buffer[strlen(osk_buffer)-1] = '\0';	
		}
		break;
	case K_INS:
		strncpy(osk_out_buff,osk_buffer,max_len);
		
		m_state = m_old_state;
		break;
	case K_ESCAPE:
		m_state = m_old_state;
		break;
	default:
		break;
	}
#endif		
}

void Con_OSK_Key (int key)
{
#ifdef PSP
	switch (key)
	{
	case K_RIGHTARROW:
		osk_pos_x++;
		if (osk_pos_x > MAX_X)
			osk_pos_x = MAX_X;
		break;
	case K_LEFTARROW:
		osk_pos_x--;
		if (osk_pos_x < 0)
			osk_pos_x = 0;
		break;
	case K_DOWNARROW:
		osk_pos_y++;
		if (osk_pos_y > MAX_Y)
			osk_pos_y = MAX_Y;
		break;
	case K_UPARROW:
		osk_pos_y--;
		if (osk_pos_y < 0)
			osk_pos_y = 0;
		break;
	case K_ENTER: 
		if (max_len > strlen(osk_buffer)) {
			char *selected_line = osk_text[osk_pos_y]; 
			char selected_char[2];
			
			selected_char[0] = selected_line[1+(2*osk_pos_x)];
			
			if (selected_char[0] == '\t')
				selected_char[0] = ' ';
			
			selected_char[1] = '\0';
			strcat(osk_buffer,selected_char);		
		}
		break;
	case K_DEL:
		if (strlen(osk_buffer) > 0) {
			osk_buffer[strlen(osk_buffer)-1] = '\0';	
		}
		break;
	case K_INS:
		strncpy(osk_out_buff,osk_buffer,max_len);
		Con_SetOSKActive(false);
		break;
	case K_ESCAPE:
		Con_SetOSKActive(false);
		break;
	default:
		break;
	}
#endif		
}	


//=============================================================================
/* LAN CONFIG MENU */

int		lanConfig_cursor = -1;
int		lanConfig_cursor_table [] = {72, 92, 124};
#define NUM_LANCONFIG_CMDS	3

int 	lanConfig_port;
char	lanConfig_portname[6];
char	lanConfig_joinname[22];

void M_Menu_LanConfig_f (void)
{
	key_dest = key_menu;
	m_state = m_lanconfig;
	m_entersound = true;
	if (lanConfig_cursor == -1)
	{
		if (JoiningGame && TCPIPConfig)
			lanConfig_cursor = 2;
		else
			lanConfig_cursor = 1;
	}
	if (StartingGame && lanConfig_cursor == 2)
		lanConfig_cursor = 1;
	lanConfig_port = DEFAULTnet_hostport;
	sprintf(lanConfig_portname, "%u", lanConfig_port);

	m_return_onerror = false;
	m_return_reason[0] = 0;
}

void M_LanConfig_Draw (void)
{
	qpic_t	*p;
	int		basex;
	char	*startJoin;
	char	*protocol;
	
	if (StartingGame)
		startJoin = "New Game";
	else
		startJoin = "Join Game";
	if (IPXConfig)
		protocol = "IPX";
	else
		protocol = "TCP/IP";
	M_Print (basex, 32, va ("%s - %s", startJoin, protocol));
	basex += 8;

	M_Print (basex, 52, "Address:");
	if (IPXConfig)
		M_Print (basex+9*8, 52, my_ipx_address);
	else
		M_Print (basex+9*8, 52, my_tcpip_address);

	M_Print (basex, lanConfig_cursor_table[0], "Port");
	M_DrawTextBox (basex+8*8, lanConfig_cursor_table[0]-8, 6, 1);
	M_Print (basex+9*8, lanConfig_cursor_table[0], lanConfig_portname);

	if (JoiningGame)
	{
		M_Print (basex, lanConfig_cursor_table[1], "Search for adhoc games...");
		M_Print (basex, 108, "Join game at:");
		M_DrawTextBox (basex+8, lanConfig_cursor_table[2]-8, 22, 1);
		M_Print (basex+16, lanConfig_cursor_table[2], lanConfig_joinname);
	}
	else
	{
		M_DrawTextBox (basex, lanConfig_cursor_table[1]-8, 2, 1);
		M_Print (basex+8, lanConfig_cursor_table[1], "OK");
	}

	M_DrawCharacter (basex-8, lanConfig_cursor_table [lanConfig_cursor], 12+((int)(realtime*4)&1));

	if (lanConfig_cursor == 0)
		M_DrawCharacter (basex+9*8 + 8*strlen(lanConfig_portname), lanConfig_cursor_table [0], 10+((int)(realtime*4)&1));

	if (lanConfig_cursor == 2)
		M_DrawCharacter (basex+16 + 8*strlen(lanConfig_joinname), lanConfig_cursor_table [2], 10+((int)(realtime*4)&1));

	if (*m_return_reason)
		M_PrintWhite (basex, 148, m_return_reason);
}


void M_LanConfig_Key (int key)
{
	int		l;

	switch (key)
	{
	case K_ESCAPE:
		//M_Menu_Net_f ();
		M_Menu_MultiPlayer_f();
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		lanConfig_cursor--;
		if (lanConfig_cursor < 0)
			lanConfig_cursor = NUM_LANCONFIG_CMDS-1;
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		lanConfig_cursor++;
		if (lanConfig_cursor >= NUM_LANCONFIG_CMDS)
			lanConfig_cursor = 0;
		break;

	case K_INS:
		if (lanConfig_cursor == 0)
		{
			M_Menu_OSK_f(lanConfig_portname, lanConfig_portname, 6);
			break;
		}

		if (lanConfig_cursor == 2)
		{
			M_Menu_OSK_f(lanConfig_joinname, lanConfig_joinname, 22);
			break;
		}
		break;

	case K_ENTER:
		if (lanConfig_cursor == 0)
			break;

		m_entersound = true;

		M_ConfigureNetSubsystem ();

		if (lanConfig_cursor == 1)
		{
			if (StartingGame)
			{
				M_Menu_GameOptions_f ();
				break;
			}
			M_Menu_Search_f();
			break;
		}

		if (lanConfig_cursor == 2)
		{
			m_return_state = m_state;
			m_return_onerror = true;
			key_dest = key_game;
			m_state = m_none;
			Cbuf_AddText ( va ("connect \"%s\"\n", lanConfig_joinname) );
			break;
		}

		break;

	case K_BACKSPACE:
		if (lanConfig_cursor == 0)
		{
			if (strlen(lanConfig_portname))
				lanConfig_portname[strlen(lanConfig_portname)-1] = 0;
		}

		if (lanConfig_cursor == 2)
		{
			if (strlen(lanConfig_joinname))
				lanConfig_joinname[strlen(lanConfig_joinname)-1] = 0;
		}
		break;

	default:
		if (key < 32 || key > 127)
			break;

		if (lanConfig_cursor == 2)
		{
			l = strlen(lanConfig_joinname);
			if (l < 21)
			{
				lanConfig_joinname[l+1] = 0;
				lanConfig_joinname[l] = key;
			}
		}

		if (key < '0' || key > '9')
			break;
		if (lanConfig_cursor == 0)
		{
			l = strlen(lanConfig_portname);
			if (l < 5)
			{
				lanConfig_portname[l+1] = 0;
				lanConfig_portname[l] = key;
			}
		}
	}

	if (StartingGame && lanConfig_cursor == 2)
	{
		if (key == K_UPARROW)
			lanConfig_cursor = 1;
		else
			lanConfig_cursor = 0;
	}

	l =  Q_atoi(lanConfig_portname);
	if (l > 65535)
		l = lanConfig_port;
	else
		lanConfig_port = l;
	sprintf(lanConfig_portname, "%u", lanConfig_port);
}

//=============================================================================
/* GAME OPTIONS MENU */

typedef struct
{
	char	*name;
	char	*description;
} level_t;


level_t		levels[] =
{
	{"cs_italy", "Hostage Rescue", },
	{"cs_office", "Hostage Rescue"},			
	{"de_dust", "Bomb"},
	{"de_dust2", "Bomb"},
	{"cs_mansion", "Hostage Rescue"},
	{"de_cbble", "Bomb"},	
	{"de_nuke", "Bomb"},	
};

//MED 01/06/97 added hipnotic levels
level_t     hipnoticlevels[] =
{
};

//PGM 01/07/97 added rogue levels
//PGM 03/02/97 added dmatch level
level_t		roguelevels[] =
{
};

typedef struct
{
	char	*description;
	int		firstLevel;
	int		levels;
} episode_t;

episode_t	episodes[] =
{
	{"MAPS", 0, 6},

};

//MED 01/06/97  added hipnotic episodes
episode_t   hipnoticepisodes[] =
{
   {"MAPS", 0, 6},

};

//PGM 01/07/97 added rogue episodes
//PGM 03/02/97 added dmatch episode
episode_t	rogueepisodes[] =
{
	{"MAPS", 0, 6},

};

int	startepisode;
int	startlevel;
int maxplayers;
qboolean m_serverInfoMessage = false;
double m_serverInfoMessageTime;

void M_Menu_GameOptions_f (void)
{
	key_dest = key_menu;
	m_state = m_gameoptions;
	m_entersound = true;
	if (maxplayers == 0)
		maxplayers = svs.maxclients;
	if (maxplayers < 2)
		maxplayers = svs.maxclientslimit;
}


int gameoptions_cursor_table[] = {40, 56, 64,72,88};
#define	NUM_GAMEOPTIONS	5
int		gameoptions_cursor;

void M_GameOptions_Draw (void)
{
	qpic_t	*p;
	int		x;

	p = Draw_CachePic ("gfx/p_multi.lmp");
	M_DrawPic ( (320-p->width)/2, 4, p);

	M_DrawTextBox (152, 32, 11, 1);
	M_Print (160, 40, "Start Server");

	M_Print (0, 56, "     Max players");
	M_Print (160, 56, va("%i", maxplayers) );

	M_Print (0, 64, "     FriendlyFire");

		char *msg;

		switch((int)teamplay.value)
		{
			case 0: msg = "Off"; break;
			case 1: msg = "On"; break;
		}
		M_Print (160, 64, msg);


	M_Print (0, 72, "     Map Time");
	if (timelimit.value == 0)
		M_Print (160, 72, "none");
	else
		M_Print (160, 72, va("%i minutes", (int)timelimit.value));

	M_Print (0, 88, "     Map Name");
      M_Print (160, 88, levels[episodes[startepisode].firstLevel + startlevel].name);


// line cursor
	M_DrawCharacter (144, gameoptions_cursor_table[gameoptions_cursor], 12+((int)(realtime*4)&1));

	if (m_serverInfoMessage)
	{
		if ((realtime - m_serverInfoMessageTime) < 5.0)
		{

		}
		else
		{
			m_serverInfoMessage = false;
		}
	}
}


void M_NetStart_Change (int dir)
{
	int count;

	switch (gameoptions_cursor)
	{
	case 1:
		maxplayers += dir;
		if (maxplayers > svs.maxclientslimit)
		{
			maxplayers = svs.maxclientslimit;
			m_serverInfoMessage = true;
			m_serverInfoMessageTime = realtime;
		}
		if (maxplayers < 2)
			maxplayers = 2;
		break;

	case 2:


		Cvar_SetValue ("teamplay", teamplay.value + dir);
		if (teamplay.value > 2)
			Cvar_SetValue ("teamplay", 1);
		else if (teamplay.value < 1)
			Cvar_SetValue ("teamplay", 1);
		break;

	case 3:
		Cvar_SetValue ("timelimit", timelimit.value + dir*5);
		if (timelimit.value > 60)
			Cvar_SetValue ("timelimit", 0);
		if (timelimit.value < 0)
			Cvar_SetValue ("timelimit", 60);
		break;

	case 4:
		startlevel += dir;
			count = episodes[startepisode].levels;

		if (startlevel < 0)
			startlevel = count - 1;

		if (startlevel >= count)
			startlevel = 0;
		break;
	}
}

void M_GameOptions_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		M_Menu_Main_f ();
		break;

	case K_UPARROW:
		S_LocalSound ("misc/menu1.wav");
		gameoptions_cursor--;
		if (gameoptions_cursor < 0)
			gameoptions_cursor = NUM_GAMEOPTIONS-1;
		break;

	case K_DOWNARROW:
		S_LocalSound ("misc/menu1.wav");
		gameoptions_cursor++;
		if (gameoptions_cursor >= NUM_GAMEOPTIONS)
			gameoptions_cursor = 0;
		break;

	case K_LEFTARROW:
		if (gameoptions_cursor == 0)
			break;
		S_LocalSound ("misc/menu3.wav");
		M_NetStart_Change (-1);
		break;

	case K_RIGHTARROW:
		if (gameoptions_cursor == 0)
			break;
		S_LocalSound ("misc/menu3.wav");
		M_NetStart_Change (1);
		break;

	case K_ENTER:
		S_LocalSound ("misc/menu2.wav");
		if (gameoptions_cursor == 0)
		{
			if (sv.active)
				Cbuf_AddText ("disconnect\n");
			Cbuf_AddText ("listen 0\n");	// so host_netport will be re-examined
			Cbuf_AddText ( va ("maxplayers %u\n", maxplayers) );
			SCR_BeginLoadingPlaque ();

				Cbuf_AddText ( va ("map %s\n", levels[episodes[startepisode].firstLevel + startlevel].name) );

			return;
		}

		M_NetStart_Change (1);
		break;
	}
}

//=============================================================================
/* SEARCH MENU */

qboolean	searchComplete = false;
double		searchCompleteTime;

void M_Menu_Search_f (void)
{
	key_dest = key_menu;
	m_state = m_search;
	m_entersound = false;
	slistSilent = true;
	slistLocal = false;
	searchComplete = false;
	NET_Slist_f();
}


void M_Search_Draw (void)
{
	int x;
	x = (320/2) - ((12*8)/2) + 4;
	M_DrawTextBox (x-8, 32, 12, 1);
	M_Print (x, 40, "Searching...");

	if(slistInProgress)
	{
		NET_Poll();
		return;
	}

	if (! searchComplete)
	{
		searchComplete = true;
		searchCompleteTime = realtime;
	}
	if (hostCacheCount)
	{
		M_Menu_ServerList_f ();
		return;
	}

	M_PrintWhite ((320/2) - ((22*8)/2), 64, "No servers found");
	if ((realtime - searchCompleteTime) < 3.0)
		return;


	M_Menu_LanConfig_f ();
}


void M_Search_Key (int key)
{
}

 //=============================================================================
/* Network Menu */

int	m_networks_cursor;

#define	NETWORKS_ITEMS	2


void M_Menu_Networks_f (void)
{
	key_dest = key_menu;
	m_state = m_networks;
	m_entersound = true;
}


void M_Networks_Draw (void)
{
	int		f;
	qpic_t	*p;

	if (serialAvailable || ipxAvailable || tcpipAvailable){
            M_Menu_MultiPlayer_f ();
		return;}
	f = (int)(host_time * 10)%6;
	M_PrintWhite(-50,200,"Adhoc"); 
	M_PrintWhite(200,200,"Press X to ON");
	M_DrawCheckbox (100, 200, tcpipAvailable && tcpipAdhoc);
}

void M_Networks_Key (int key)
{
	switch (key)
	{
	case K_ESCAPE:
		M_Menu_Main_f ();
		break;
	case K_ENTER:
		m_entersound = true;
		            Datagram_Shutdown();

			tcpipAvailable = !tcpipAvailable;


			if(tcpipAvailable && sceWlanDevIsPowerOn())
			{
				tcpipAdhoc = true;
				net_driver_to_use = 1;
				Datagram_Init();
			}
			break;
		if (serialAvailable || ipxAvailable || tcpipAvailable || tcpipAdhoc)
			M_Menu_MultiPlayer_f ();
		break;
	}
}
//=============================================================================
/* SLIST MENU */

int		slist_cursor;
qboolean slist_sorted;

void M_Menu_ServerList_f (void)
{
	key_dest = key_menu;
	m_state = m_slist;
	m_entersound = true;
	slist_cursor = 0;
	m_return_onerror = false;
	m_return_reason[0] = 0;
	slist_sorted = false;
}


void M_ServerList_Draw (void)
{
	int		n;
	char	string [64];
	qpic_t	*p;

	if (!slist_sorted)
	{
		if (hostCacheCount > 1)
		{
			int	i,j;
			hostcache_t temp;
			for (i = 0; i < hostCacheCount; i++)
				for (j = i+1; j < hostCacheCount; j++)
					if (strcmp(hostcache[j].name, hostcache[i].name) < 0)
					{
						Q_memcpy(&temp, &hostcache[j], sizeof(hostcache_t));
						Q_memcpy(&hostcache[j], &hostcache[i], sizeof(hostcache_t));
						Q_memcpy(&hostcache[i], &temp, sizeof(hostcache_t));
					}
		}
		slist_sorted = true;
	}

    M_DrawCharacter (0, 32 + slist_cursor*8, 12+((int)(realtime*4)&1));

	for (n = 0; n < hostCacheCount; n++)
	{
		if (hostcache[n].maxusers)
			sprintf(string, "%-15.15s %-15.15s %2u/%2u\n", hostcache[n].name, hostcache[n].map, hostcache[n].users, hostcache[n].maxusers);
		else
			sprintf(string, "%-15.15s %-15.15s\n", hostcache[n].name, hostcache[n].map);
		M_Print (16, 32 + 8*n, string);
	}

	if (*m_return_reason)
		M_PrintWhite (16, 148, m_return_reason);
}


void M_ServerList_Key (int k)
{
	switch (k)
	{
	case K_ESCAPE:
		M_Menu_LanConfig_f ();
		break;

	case K_SPACE:
		M_Menu_Search_f ();
		break;

	case K_UPARROW:
	case K_LEFTARROW:
		S_LocalSound ("misc/menu1.wav");
		slist_cursor--;
		if (slist_cursor < 0)
			slist_cursor = hostCacheCount - 1;
		break;

	case K_DOWNARROW:
	case K_RIGHTARROW:
		S_LocalSound ("misc/menu1.wav");
		slist_cursor++;
		if (slist_cursor >= hostCacheCount)
			slist_cursor = 0;
		break;

	case K_ENTER:
		S_LocalSound ("misc/menu2.wav");
		m_return_state = m_state;
		m_return_onerror = true;
		slist_sorted = false;
		key_dest = key_game;
		m_state = m_none;
		Cbuf_AddText ( va ("connect \"%s\"\n", hostcache[slist_cursor].cname) );
		break;

	default:
		break;
	}

}

//=============================================================================
/* Menu Subsystem */


void M_Init (void)
{
	Cmd_AddCommand ("togglemenu", M_ToggleMenu_f);
	Cmd_AddCommand ("menu_maps", M_Menu_Maps_f); //Crow_bar. maplist
	Cmd_AddCommand ("menu_main", M_Menu_Main_f);
	Cmd_AddCommand ("menu_singleplayer", M_Menu_SinglePlayer_f);
	Cmd_AddCommand ("menu_multiplayer", M_Menu_MultiPlayer_f);
	Cmd_AddCommand ("menu_team", M_Menu_Team_f);
	Cmd_AddCommand ("menu_ingame", M_Menu_Ingame_f);
	Cmd_AddCommand ("menu_setup", M_Menu_Setup_f);
	Cmd_AddCommand ("menu_options", M_Menu_Options_f);
	Cmd_AddCommand ("menu_keys", M_Menu_Keys_f);
	Cmd_AddCommand ("menu_video", M_Menu_Video_f);
	Cmd_AddCommand ("help", M_Menu_Help_f);
	Cmd_AddCommand ("menu_quit", M_Menu_Quit_f);
	Cmd_AddCommand ("menu_buyt", M_Menu_Buyt_f);
	Cmd_AddCommand ("menu_buyct", M_Menu_Buyct_f);
	Cmd_AddCommand ("menu_bot", M_Menu_Bots_f);
}


void M_Draw (void)
{
	if (m_state == m_none || key_dest != key_menu)
		return;

	if (!m_recursiveDraw)
	{
		scr_copyeverything = 1;

		if (scr_con_current)
		{
			Draw_ConsoleBackground (vid.height);
			VID_UnlockBuffer ();
			S_ExtraUpdate ();
			VID_LockBuffer ();
		}
		else
			Draw_FadeScreen ();

		scr_fullupdate = 0;
	}
	else
	{
		m_recursiveDraw = false;
	}

	switch (m_state)
	{
	case m_none:
		break;
	case m_maps: //Crow_bar. maplist	
	M_Maps_Draw ();
	break;
	
	case m_main:
		M_Main_Draw ();
		break;

	case m_singleplayer:
		M_SinglePlayer_Draw ();
		break;


	case m_buyt:
		M_Buyt_Draw ();
		break;

	case m_pistolst:
		M_Pistolst_Draw ();
		break;

	case m_smgst:
		M_Smgst_Draw ();
		break;

	case m_buyct:
		M_Buyct_Draw ();
		break;

	case m_networks:
		M_Networks_Draw ();
		break;

	case m_pistolsct:
		M_Pistolsct_Draw ();
		break;


	case m_smgsct:
		M_Smgsct_Draw ();
		break;


	case m_shotguns:
		M_Shotguns_Draw ();
		break;

	case m_riflest:
		M_Riflest_Draw ();
		break;
	case m_riflesct:
		M_Riflesct_Draw ();
		break;
	case m_machineguns:
		M_Machineguns_Draw ();
		break;

	case m_equipment:
		M_Equipment_Draw ();
		break;

	case m_ingame:
		M_Ingame_Draw ();
		break;

	case m_chat:
		M_Chat_Draw ();
		break;

	case m_orders:
		M_Orders_Draw ();
		break;

	case m_team:
		M_Team_Draw ();
		break;
	case m_counter:
		M_Counter_Draw ();
		break;
	case m_terror:
		M_Terror_Draw ();
		break;
	case m_taunts:
		M_Taunts_Draw ();
		break;
	case m_bots:
		M_Bots_Draw ();
		break;
	case m_multiplayer:
		M_MultiPlayer_Draw ();
		break;

	case m_setup:
		M_Setup_Draw ();
		break;

	case m_net:
		M_Net_Draw ();
		break;

	case m_options:
		M_Options_Draw ();
		break;

	case m_keys:
		M_Keys_Draw ();
		break;

	case m_video:
		M_Video_Draw ();
		break;

	case m_help:
		M_Help_Draw ();
		break;

	case m_quit:
		M_Quit_Draw ();
		break;

	case m_lanconfig:
		M_LanConfig_Draw ();
		break;

	case m_gameoptions:
		M_GameOptions_Draw ();
		break;

	case m_search:
		M_Search_Draw ();
		break;
		
	case m_slist:
		M_ServerList_Draw ();
		break;
		
	case m_osk:
		M_OSK_Draw();
		break;
	}

	if (m_entersound)
	{
		S_LocalSound ("misc/menu2.wav");
		m_entersound = false;
	}

	VID_UnlockBuffer ();
	S_ExtraUpdate ();
	VID_LockBuffer ();
}


void M_Keydown (int key)
{
	switch (m_state)
	{
	case m_none:
		return;
		
	case m_maps: //Crow_bar. maplist
	M_Maps_Key (key);
	return;
	
	case m_main:
		M_Main_Key (key);
		return;

	case m_singleplayer:
		M_SinglePlayer_Key (key);
		return;

	case m_taunts:
		M_Taunts_Key (key);
		return;

	case m_buyt:
		M_Buyt_Key (key);
		return;
	case m_pistolst:
		M_Pistolst_Key (key);
		return;
	case m_smgst:
		M_Smgst_Key (key);
		return;

	case m_buyct:
		M_Buyct_Key (key);
		return;
	case m_pistolsct:
		M_Pistolsct_Key (key);
		return;
	case m_smgsct:
		M_Smgsct_Key (key);
		return;

	case m_shotguns:
		M_Shotguns_Key (key);
		return;

	case m_riflest:
		M_Riflest_Key (key);
		return;

	case m_riflesct:
		M_Riflesct_Key (key);
		return;
	case m_machineguns:
		M_Machineguns_Key (key);
		return;

	case m_ingame:
		M_Ingame_Key (key);
		return;

	case m_bots:
		M_Bots_Key (key);
		return;
		
	case m_equipment:
		M_Equipment_Key (key);
		return;

	case m_chat:
		M_Chat_Key (key);
		return;

	case m_orders:
		M_Orders_Key (key);
		return;

	case m_team:
		M_Team_Key (key);
		return;
	case m_counter:
		M_Counter_Key (key);
		return;
	case m_terror:
		M_Terror_Key (key);
		return;


	case m_networks:
		M_Networks_Key (key);
		break;

	case m_multiplayer:
		M_MultiPlayer_Key (key);
		return;

	case m_setup:
		M_Setup_Key (key);
		return;

	case m_net:
		M_Net_Key (key);
		return;

	case m_options:
		M_Options_Key (key);
		return;

	case m_keys:
		M_Keys_Key (key);
		return;

	case m_video:
		M_Video_Key (key);
		return;

	case m_help:
		M_Help_Key (key);
		return;

	case m_quit:
		M_Quit_Key (key);
		return;


	case m_lanconfig:
		M_LanConfig_Key (key);
		return;

	case m_gameoptions:
		M_GameOptions_Key (key);
		return;

	case m_search:
		M_Search_Key (key);
		break;

	case m_slist:
		M_ServerList_Key (key);
		return;
		
	case m_osk:
		M_OSK_Key(key);	


	}
}


void M_ConfigureNetSubsystem(void)
{
// enable/disable net systems to match desired config

	Cbuf_AddText ("stopdemo\n");

	if (IPXConfig || TCPIPConfig)
		net_hostport = lanConfig_port;
}

