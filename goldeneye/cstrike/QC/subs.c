void() DelayThink =
{
	activator = self.enemy;
	SUB_UseTargets ();
	remove(self);
};

/*
==============================
SUB_UseTargets

the global "activator" should be set to the entity that initiated the firing.

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Centerprints any self.message to the activator.

Removes all entities with a targetname that match self.killtarget,
and removes them, so some events can remove other triggers.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function

==============================
*/
void() SUB_UseTargets =
{
	local entity t, stemp, otemp, act;

//
// check for a delay
//
	if (self.delay)
	{
	// create a temp object to fire at a later time
		t = spawn();
		t.classname = "DelayedUse";
		t.nextthink = time + self.delay;
		t.think = DelayThink;
		t.enemy = activator;
		t.message = self.message;
		t.killtarget = self.killtarget;
		t.target = self.target;
		return;
	}
	
	
//
// print the message
//
	if (activator.classname == "player" && self.message != "")
	{
		centerprint (activator, self.message);	
	}

//
// kill the killtagets
//
	if (self.killtarget)
	{
		t = world;
		do
		{
			t = find (t, targetname, self.killtarget);
			if (!t)
				return;
			remove (t);
		} while ( 1 );
	}
	
//
// fire targets
//
	if (self.target)
	{
		act = activator;
		t = world;
		do
		{
			t = find (t, targetname, self.target);
			if (!t)
			{
				return;
			}
			stemp = self;
			otemp = other;
			self = t;
			other = stemp;
			if (self.use != SUB_Null)
			{
				if (self.use)
					self.use ();
			}
			self = stemp;
			other = otemp;
			activator = act;
		} while ( 1 );
	}
	

};
/*
=============
SUB_CalcMove

calculate self.velocity and self.nextthink to reach dest from
self.origin traveling at speed
===============
*/
void(entity ent, vector tdest, float tspeed, void() func) SUB_CalcMoveEnt =
{
local entity	stemp;
	stemp = self;
	self = ent;

	SUB_CalcMove (tdest, tspeed, func);
	self = stemp;
};

void(vector tdest, float tspeed, void() func) SUB_CalcMove =
{
	local vector	vdestdelta;
	local float		len, traveltime;

	if (!tspeed)
		objerror("No speed is defined!");

	self.think1 = func;
	self.finaldest = tdest;
	self.think = SUB_CalcMoveDone;

	if (tdest == self.origin)
	{
		self.velocity = '0 0 0';
		self.nextthink = self.ltime + 0.1;
		return;
	}
		
// set destdelta to the vector needed to move
	vdestdelta = tdest - self.origin;
	
// calculate length of vector
	len = vlen (vdestdelta);
	
// divide by speed to get time to reach dest
	traveltime = len / tspeed;

	if (traveltime < 0.1)
	{
		self.velocity = '0 0 0';
		self.nextthink = self.ltime + 0.1;
		return;
	}
	
// set nextthink to trigger a think when dest is reached
	self.nextthink = self.ltime + traveltime;

// scale the destdelta vector by the time spent traveling to get velocity
	self.velocity = vdestdelta * (1/traveltime);	// qcc won't take vec/float	
};

/*
============
After moving, set origin to exact final destination
============
*/
void()  SUB_CalcMoveDone =
{
	setorigin(self, self.finaldest);
	self.velocity = '0 0 0';
	self.nextthink = -1;
	if (self.think1)
		self.think1();
};
/*
QuakeEd only writes a single float for angles (bad idea), so up and down are
just constant angles.
*/
vector() SetMovedir =
{
	if (self.angles == '0 -1 0')
		self.movedir = '0 0 1';
	else if (self.angles == '0 -2 0')
		self.movedir = '0 0 -1';
	else
	{
		makevectors (self.angles);
		self.movedir = v_forward;
	}
	
	self.angles = '0 0 0';
};

void UTIL_ScreenFadeBuild(vector color, float fadeTime, float fadeHold, float alpha,float flags)
{
	duration = fadeTime;
	holdTime = fadeHold;
	R = color_x;
	G = color_y;
	B = color_z;
	A = alpha;
	fadeFlags = flags;
}
void UTIL_ScreenFadeWrite(entity pEntity)
{
	if (!pEntity)
		return;
	
	msg_entity = pEntity; 
	
	WriteByte (MSG_ONE, SVC_SCREENFADE);
	WriteShort(MSG_ONE,duration);
	WriteShort(MSG_ONE,holdTime);
	WriteShort(MSG_ONE,fadeFlags);
	WriteByte(MSG_ONE,R);
	WriteByte(MSG_ONE,G);
	WriteByte(MSG_ONE,B);
	WriteByte(MSG_ONE,A);
}
void UTIL_ScreenFade(entity pEntity,vector color, float fadeTime, float fadeHold, float alpha,float flags)
{
	UTIL_ScreenFadeBuild(color, fadeTime, fadeHold, alpha,flags);
	UTIL_ScreenFadeWrite(pEntity);
}
//buttonuse
.void() th_use;
.float capenabled;
void() use_ent = 
{
	local entity oldself;
	if (trace_ent.th_use)
	{
		oldself = self;
		other = self;
		self = trace_ent;
		self.th_use();
		self = oldself;
	}
};
void() use_button = 
{
	local vector source;
	local vector org;
	makevectors(self.v_angle);
	source = self.origin + '0 0 28';
	traceline(source, source + v_forward * 70, 0, self);
	if (trace_fraction == 1)
	{
		return;
	}
	org = trace_endpos - v_forward * 4;
	if (trace_ent)
		use_ent();
};