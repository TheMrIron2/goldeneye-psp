#define AMBIENT_SOUND_STATIC			0	// medium radius attenuation
#define AMBIENT_SOUND_EVERYWHERE		1
#define AMBIENT_SOUND_SMALLRADIUS		2
#define AMBIENT_SOUND_MEDIUMRADIUS		4
#define AMBIENT_SOUND_LARGERADIUS		8
#define AMBIENT_SOUND_START_SILENT		16
#define AMBIENT_SOUND_NOT_LOOPING		32
float m_flAttenuation,m_fActive,m_fLooping;
.float Radius,roomtype;
void()PrecacheSounds=
{
	precache_sound(self.message);
	if (!(self.spawnflags & AMBIENT_SOUND_START_SILENT))
	{
		// start the sound ASAP
		if (m_fLooping)
		{
			m_fActive = TRUE;
		}
	}
	if (m_fActive)
	{
		ambientsound(self.origin,self.message,1,m_flAttenuation);
	}
}
//incomplete,do we need a use?	
void()ambient_generic=
{
	if (self.spawnflags & AMBIENT_SOUND_EVERYWHERE)
	{
		m_flAttenuation = ATTN_NONE;
	}
	else if (self.spawnflags & AMBIENT_SOUND_SMALLRADIUS)
	{
		m_flAttenuation = ATTN_IDLE;
	}
	else if (self.spawnflags & AMBIENT_SOUND_MEDIUMRADIUS)
	{
		m_flAttenuation = ATTN_STATIC;
	}
	else if (self.spawnflags & AMBIENT_SOUND_LARGERADIUS)
	{
		m_flAttenuation = ATTN_NORM;
	}
	else
	{
		// if the designer didn't set a sound attenuation, default to one.
		m_flAttenuation = ATTN_STATIC;
	}
	
	if(!self.message)
	{
		bprint("Empty ambient at: ",vtos(self.origin),"\n");
		self.nextthink = time + 0.1;
		self.think = SUB_Remove;
		return;
	}
	
	self.solid = SOLID_NOT;
	self.movetype = MOVETYPE_NONE;
	m_fActive = FALSE;

	if (self.spawnflags & AMBIENT_SOUND_NOT_LOOPING)
		m_fLooping = FALSE;
	else
		m_fLooping = TRUE;
	
	PrecacheSounds();
}

float (entity ent, entity enttarget)sound_inrange =
{
    local vector vecSpot1;
	local vector vecSpot2;
	local vector vecRange;
	local float flRange;

    vecSpot1 = ent.origin + ent.view_ofs;
    vecSpot2 = enttarget.origin + enttarget.view_ofs;

	vecRange = vecSpot1 - vecSpot2;
	flRange = vlen(vecRange);
/*
    traceline (vecSpot1, vecSpot2, TRUE, self);

	// check if line of sight crosses water boundary, or is blocked
	if ((trace_inopen && trace_inwater) || (trace_fraction != 1))
		return -1;

	// calc range from sound entity to player
	vecRange = trace_endpos - vecSpot1;
	flRange = vlen(vecRange);
*/
	return flRange;
};

void() radius_player_find =
{
	local entity client;
	local float res;
	
	client = checkclient ();
	if(!client)
		return;
	msg_entity = client;
    res = sound_inrange(self, client);
    if(res <= self.Radius)
    {
       WriteByte (MSG_ONE,  SVC_ROOMTYPE);
       WriteShort(MSG_ONE, self.roomtype);
    }
	self.nextthink = time + 0.75;
	self.think = radius_player_find;
};

void() env_sound =
{
    self.classname = "env_sound";
	self.nextthink = time + 0.75;
    self.think = radius_player_find;
};