#define	SF_DOOR_START_OPEN			0
#define SF_DOOR_PASSABLE			5
#define SF_DOOR_ONEWAY				16
#define	SF_DOOR_NO_AUTO_RETURN			32
#define SF_DOOR_USE_ONLY			8	// door must be opened by player's use button.
#define SF_DOOR_NOMONSTERS			512	// Monster can't open
.float movesnd,stopsnd;
enum
{
	TS_AT_TOP,
	TS_AT_BOTTOM,
	TS_GOING_UP,
	TS_GOING_DOWN
};

void() door_blocked =
{
	T_Damage (other, self, self, self.dmg);
	
// if a door has a negative wait, it would never come back if blocked,
// so let it just squash the object to death real fast
	if (self.wait >= 0)
	{
		if (self.state == TS_GOING_DOWN)
			door_go_up ();
		else
			door_go_down ();
	}
};


void() door_hit_top =
{
	sound (self, CHAN_VOICE, self.noise2, 1, ATTN_NORM);
	self.state = TS_AT_TOP;
	if (self.spawnflags & SF_DOOR_NO_AUTO_RETURN)
	{
		if (!(self.spawnflags & SF_DOOR_USE_ONLY))
			self.touch = door_touch;
	}
	else
	{	
		self.think = door_go_down;
		self.nextthink = self.ltime + self.wait;	
	}
	SUB_UseTargets();
};

void() door_hit_bottom =
{
	sound (self, CHAN_VOICE, self.noise2, 1, ATTN_NORM);
	self.state = TS_AT_BOTTOM;
	if(self.spawnflags & SF_DOOR_USE_ONLY)
	{	
			self.touch = SUB_Null;
	}
	else
	{	
		self.touch = door_touch;
	}
	SUB_UseTargets();
};

void() door_go_down =
{
	sound (self, CHAN_VOICE, self.noise1, 1, ATTN_NORM);

	self.state = TS_GOING_DOWN;
	SUB_CalcMove (self.pos1, self.speed, door_hit_bottom);
};

void() door_go_up =
{
	sound (self, CHAN_VOICE, self.noise1, 1, ATTN_NORM);
	self.state = TS_GOING_UP;
	SUB_CalcMove (self.pos2, self.speed, door_hit_top);
};

float() door_activate=
{
	if((self.spawnflags & SF_DOOR_NO_AUTO_RETURN ) && self.state == TS_AT_TOP )
	{
		// door should close
		door_go_down();
	}
	else
	{
		door_go_up();
	}

	return 1;	
}

void() door_touch =
{
	if (other.classname != "player")
		return;
	if(door_activate())
		self.touch = SUB_Null;
};

void() func_door =
{
	SetMovedir ();

	self.max_health = self.health;
	self.solid = SOLID_BSP;
	self.movetype = MOVETYPE_PUSH;
	setorigin (self, self.origin);	
	setmodel (self, self.model);
	self.classname = "door";

	self.blocked = door_blocked;
	self.use = door_activate; //weird,but working
	self.th_use = door_touch;
	if (!self.speed)
		self.speed = 100;
	if (!self.wait)
		self.wait = 3;

	self.pos1 = self.origin;
	self.pos2 = self.pos1 + ( self.movedir * ( fabs( self.movedir_x * ( self.size_x - 2 ) ) + fabs( self.movedir_y * ( self.size_y - 2 ) ) + fabs( self.movedir_z * ( self.size_z - 2 ) ) - self.lip ) );

	if (self.spawnflags & SF_DOOR_START_OPEN)
	{
		setorigin (self, self.pos2);
		self.pos2 = self.pos1;
		self.pos1 = self.origin;
	}

	self.state = TS_AT_BOTTOM;

	if(self.spawnflags & SF_DOOR_USE_ONLY )
	{
		self.touch = SUB_Null;
	}
	else
		self.touch = door_touch;

	self.touch = SUB_Null;
	switch(self.movesnd)
	{
		case 0:
		precache_sound("common/null.wav");
		self.noise1 =("common/null.wav");
		break;
		case 1:
		precache_sound("doors/doormove1.wav");
		self.noise1 =("doors/doormove1.wav");
		break;
		case 2:
		precache_sound("doors/doormove2.wav");
		self.noise1 =("doors/doormove2.wav");
		break;
		case 3:
		precache_sound("doors/doormove3.wav");
		self.noise1 =("doors/doormove3.wav");
		break;
		case 4:
		precache_sound("doors/doormove4.wav");
		self.noise1 =("doors/doormove4.wav");
		break;
		case 5:
		precache_sound("doors/doormove5.wav");
		self.noise1 =("doors/doormove5.wav");
		break;
		case 6:
		precache_sound("doors/doormove6.wav");
		self.noise1 =("doors/doormove6.wav");
		break;
		case 7:
		precache_sound("doors/doormove7.wav");
		self.noise1 =("doors/doormove7.wav");
		break;
		case 8:
		precache_sound("doors/doormove8.wav");
		self.noise1 =("doors/doormove8.wav");
		break;
		case 9:
		precache_sound("doors/doormove9.wav");
		self.noise1 =("doors/doormove9.wav");
		break;
		case 10:
		precache_sound("doors/doormove10.wav");
		self.noise1 =("doors/doormove10.wav");
		break;
		default:
		precache_sound("common/null.wav");
		self.noise1 =("common/null.wav");
		break;
	}
	switch(self.stopsnd)
	{
		case 0:
		precache_sound("common/null.wav");
		self.noise2 =("common/null.wav");
		break;
		case 1:
		precache_sound("doors/doorstop1.wav");
		self.noise2 =("doors/doorstop1.wav");
		break;
		case 2:
		precache_sound("doors/doorstop2.wav");
		self.noise2 =("doors/doorstop2.wav");
		break;
		case 3:
		precache_sound("doors/doorstop3.wav");
		self.noise2 =("doors/doorstop3.wav");
		break;
		case 4:
		precache_sound("doors/doorstop4.wav");
		self.noise2 =("doors/doorstop4.wav");
		break;
		case 5:
		precache_sound("doors/doorstop5.wav");
		self.noise2 =("doors/doorstop5.wav");
		break;
		case 6:
		precache_sound("doors/doorstop6.wav");
		self.noise2 =("doors/doorstop6.wav");
		break;
		case 7:
		precache_sound("doors/doorstop7.wav");
		self.noise2 =("doors/doorstop7.wav");
		break;
		case 8:
		precache_sound("doors/doorstop8.wav");
		self.noise2 =("doors/doorstop8.wav");
		break;
		default:
		precache_sound("common/null.wav");
		self.noise2 =("common/null.wav");
		break;
	}
};