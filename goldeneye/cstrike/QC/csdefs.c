vector  DEFAULT_VIEWHEIGHT = '0 0 28';
vector  VEC_DUCK_VIEW = '0 0 12';
vector  VEC_DUCK_HULL_MIN = '-16 -16 -18';
vector  VEC_DUCK_HULL_MAX =  '16 16 18';

#define AUTOAIM_2DEGREES  0.0348994967025
#define AUTOAIM_5DEGREES  0.08715574274766
#define AUTOAIM_8DEGREES  0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669
// func breakable
#define SF_BREAK_TRIGGER_ONLY		1	// may only be broken by trigger
#define	SF_BREAK_TOUCH			2	// can be 'crashed through' by running player (plate glass)
#define SF_BREAK_PRESSURE		4	// can be broken by a player standing on it
#define SF_BREAK_CROWBAR	256	// instant break if hit with crowbar
// screen fade flags
float FFADE_IN		    = 0;		// Just here so we don't pass 0 into the function
float FFADE_OUT		    = 1;		// Fade out (not in)
float FFADE_MODULATE	= 2;		// Modulate (don't blend)
float FFADE_STAYOUT	    = 4;		// ignores the duration, stays faded out until new ScreenFade message received
float T_SIDE = 1;
float CT_SIDE = 2;

float m_iNumTerrorist,m_iNumCT;
.float menu_team_on;
.float ammo_p99,p99clip,p99_fired;
.float ammo_dd44,dd44clip,dd44_fired;
.float ammo_magnum,magnumclip,magnum_fired;
.float ammo_m3,m3clip,m3_fired;
.float ammo_kf7,kf7clip,kf7_fired;
.float ammo_awp,awpclip,scope,awp_fired;
.float ammo_phantom,phantomclip,phantom_fired;
.float ammo_aug,augclip,aug_fired;
.float ammo_famas,famasclip,famasburst,famas_fired;
.float ammo_ar33,ar33clip,ar33_fired;
.float ammo_p228,p228clip,p228_fired;
.float ammo_elite,eliteclip,elite_fired;
.float he_grenades;
.float attack_finished;
.float iSlot;
.float m_iMenu,m_iTeam;
enum
{
	PRIMARY,
	SECONDARY,
	KNIFE,
	GRENADES
};
enum
{
	JOINED,
	SHOWLTEXT,
	READINGLTEXT,
	SHOWTEAMSELECT,
	PICKINGTEAM,
	GETINTOGAME
};
enum
{
	Menu_OFF,
	Menu_ChooseTeam,
	Menu_IGChooseTeam,
	Menu_ChooseAppearance,
	Menu_Buy
};
enum
{
	UNASSIGNED,
	TERRORIST,
	CT,
	SPECTATOR
};
enum
{
	ASSAULTSUIT_PRICE   = 1000,
	FLASHBANG_PRICE     = 200,
	HEGRENADE_PRICE     = 300,
	SMOKEGRENADE_PRICE  = 300,
	KEVLAR_PRICE        = 650,
	HELMET_PRICE        = 350,
	NVG_PRICE           = 1250,
	DEFUSEKIT_PRICE     = 200,
};
enum 
{
	AK47_PRICE      = 2500,
	AWP_PRICE       = 4750,
	DEAGLE_PRICE    = 650,
	G3SG1_PRICE     = 5000,
	SG550_PRICE     = 4200,
	GLOCK18_PRICE   = 400,
	M249_PRICE      = 5750,
	M3_PRICE        = 1700,
	M4A1_PRICE      = 3100,
	AUG_PRICE       = 3500,
	MP5NAVY_PRICE   = 1500,
	P228_PRICE      = 600,
	P90_PRICE       = 2350,
	UMP45_PRICE     = 1700,
	MAC10_PRICE     = 1400,
	SCOUT_PRICE     = 2750,
	SG552_PRICE     = 3500,
	TMP_PRICE       = 1250,
	USP_PRICE       = 500,
	ELITE_PRICE     = 800,
	FIVESEVEN_PRICE = 750,
	XM1014_PRICE    = 3000,
	GALIL_PRICE     = 2000,
	FAMAS_PRICE     = 2250,
	SHIELDGUN_PRICE = 2200,
};
enum 
{
	matGlass = 0,
	matWood,
	matMetal,
	matFlesh,
	matCinderBlock,
	matCeilingTile,
	matComputer,
	matUnbreakableGlass,
	matRocks,
	matNone,
	matLastMaterial,
};
.float m_iJoiningState;
entity m_pIntroCamera;
.float m_fIntroCamTime;
//for flashbang
.float m_blindUntilTime;
.float m_blindStartTime;
.float m_blindHoldTime;
.float m_blindFadeTime;
.float m_blindAlpha;
.float throwmode;
.float m_flNextSecondaryAttack;
float MaxSpreadY,MaxSpreadX,SpreadX,SpreadY;
float LeftSpread;
float IT_KNIFE = 1;
float IT_P99 = 2;
float IT_DD44 = 4;
float IT_MAGNUM = 8;
float IT_HEGRENADE = 16;
float IT_M3 = 32;
float IT_KF7 = 64;
float IT_AWP = 128;
float IT_PHANTOM = 256;
float IT_AUG = 512;
float IT_FAMAS = 1024;
float IT_AR33 = 2048;
float IT_P228 = 4096;
float IT_ELITE = 8192;
.float state;
float RELOADING = 1;
float SILENCER  = 2;
float ATTACK 	= 3;
//footsteps
string sound_step1;
string sound_step2;
string sound_step3;
string sound_step4;

float m_bMapHasBuyZone,buyTime;
.float m_bInBuyZone;
.float fov;
.float direction,wp;
float m_flNextDecalTime; 

/*
-----------------------------------------
min

Returns the lesser of two (or more) numbers

-----------------------------------------
*/
float (float a, float b) mathlib_min =
{
	if (a<b)
		return a;
	else
		return b;
};

