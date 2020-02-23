void() CheckImpulses;
void() WeaponFrameAll;
void() UpdateWeapon;
void() SecondaryAttack;
.float crouch_time;
.float crouch_stuck;
void() OLD_CheckBuyZone=
{
	local string pszSpawnClass;
	if(self.m_iTeam == TERRORIST)
		pszSpawnClass = "info_player_deathmatch";
	else if(self.m_iTeam == CT)
		pszSpawnClass = "info_player_start";
	if(pszSpawnClass)
	{
		local entity pSpot;
		pSpot = find(world,classname,pszSpawnClass);
		while(pSpot)
		{
			pSpot = find(pSpot,classname,pszSpawnClass);
			if(vlen(pSpot.origin - self.origin) < 200)
			{
				stuffcmd(self,"buyicon 1 \n");
				self.m_bInBuyZone = 1;
			}
		}
	}
}
void()CheckBuyZone=
{
	if(self.m_bInBuyZone == 1)
		stuffcmd(self,"buyicon 1 \n");
	else
		stuffcmd(self,"buyicon 0 \n");
}
void() Show_Menu_Team=
{
	stuffcmd (self, "menu_team \n");
	self.m_iMenu = Menu_ChooseTeam;
}

void()TraceTexture=
{
	//============================texture trace test================================
    local vector start;
    local vector end;
    local vector end_player;
    
	end_player = self.origin;

	// Straight down
	end_player_z = end_player_z - 64;
	
	traceline (self.origin, end_player, TRUE, self);
	if (trace_fraction == 1.0)
		return;

	start = trace_endpos + trace_plane_normal * (-8);
	end = trace_endpos + trace_plane_normal * 8;

    local string trace_texture;
	trace_texture = tracematerial( trace_ent, start, end );
	if(trace_texture == "d" || trace_texture == "D" )
	{
    	sound_step1 = "player/pl_dirt1.wav";
    	sound_step2 = "player/pl_dirt2.wav";
    	sound_step3 = "player/pl_dirt3.wav";
    	sound_step4 = "player/pl_dirt4.wav";
    }
    else if(trace_texture == "v" || trace_texture == "V" )
    {
    	sound_step1 = "player/pl_duct1.wav";
    	sound_step2 = "player/pl_duct2.wav";
    	sound_step3 = "player/pl_duct3.wav";
    	sound_step4 = "player/pl_duct4.wav";
	}
	else if(trace_texture == "g" || trace_texture == "G")
	{
    	sound_step1 = "player/pl_grate1.wav";
    	sound_step2 = "player/pl_grate2.wav";
    	sound_step3 = "player/pl_grate3.wav";
    	sound_step4 = "player/pl_grate4.wav";
	}
	else if(trace_texture == "m" || trace_texture == "M")
	{
    	sound_step1 = "player/pl_metal1.wav";
    	sound_step2 = "player/pl_metal2.wav";
    	sound_step3 = "player/pl_metal3.wav";
    	sound_step4 = "player/pl_metal4.wav";
	}
	else if(trace_texture == "t" || trace_texture == "T")
	{
    	sound_step1 = "player/pl_tile1.wav";
    	sound_step2 = "player/pl_tile2.wav";
    	sound_step3 = "player/pl_tile3.wav";
    	sound_step4 = "player/pl_tile4.wav";
	}
	else if(trace_texture == "n" || trace_texture =="N")
	{
    	sound_step1 = "player/pl_snow1.wav";
    	sound_step2 = "player/pl_snow2.wav";
    	sound_step3 = "player/pl_snow3.wav";
    	sound_step4 = "player/pl_snow4.wav";
	}
	else 
	{
    	sound_step1 = "player/pl_step1.wav";
    	sound_step2 = "player/pl_step2.wav";
    	sound_step3 = "player/pl_step3.wav";
    	sound_step4 = "player/pl_step4.wav";
	}	
}
void()PlayerCrouching =
{
	if ( (self.health <= 0) )
	{
		return;
	}
	
	setsize (self, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	self.hull = 4;
	self.crouch_time = time;
    self.crouch_stuck = 0;
};

void()PlayerUnCrouching =
{
	tracearea ((self.origin), (self.origin + '0 0 36'), VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, FALSE, self);

	if ( (trace_fraction < 1) )
	{
		self.crouch_stuck = 1;
		return ;
	}

	setsize (self, VEC_HULLHL_MIN, VEC_HULLHL_MAX);
	self.hull = 2;
	self.crouch_time = time;
};

void()PlayerCrouch =
{
   if ( (self.crouch_time && (self.crouch_time < time)) )
   {
      if ( (self.hull == 4) )
	  {
         self.crouch_stuck = 0;
         self.view_ofs_z = self.view_ofs_z - 5;
		 if ( (self.view_ofs_z < 8) )
		 {
            self.view_ofs_z = 12;
		    self.crouch_time = 0;
         }
		 else
		 {
            self.crouch_time = (time + (0.05 / 4));
         }
      }
	  else
	  {
         self.view_ofs_z = self.view_ofs_z + 5;
		 if ((self.view_ofs_z > 28))
		 {
            self.view_ofs_z = 28;
	        self.crouch_time = 0;
         }
		 else
		 {
            self.crouch_time = (time + (0.05 / 4));
         }
      }
   }
   if ( self.button3 )
   {
      PlayerCrouching ();
      self.button3 = 0;
   }
   else
   {
      if ((self.hull == 4) || self.crouch_stuck )
	  {
         PlayerUnCrouching ();
      }
   }

};

void() PlayerJump =
{
	
	if (self.flags & FL_WATERJUMP)
		return;
	
	if (self.waterlevel >= 2)
	{
		if (self.watertype == CONTENT_WATER)
			self.velocity_z = 150;
		else if (self.watertype == CONTENT_SLIME)
			self.velocity_z = 80;
		else
			self.velocity_z = 50;
		return;
	}

	if (!(self.flags & FL_ONGROUND))
		return;

	if ( !(self.flags & FL_JUMPRELEASED) )
		return;		// don't pogo stick

	self.flags = self.flags - (self.flags & FL_JUMPRELEASED);
	self.flags = self.flags - FL_ONGROUND;	// don't stairwalk
	
	self.button2 = 0;
	self.velocity_z = self.velocity_z + 300;
};
//Called when a client issues the 'kill' command
 void() ClientKill = {};
 //Called when a client connected to the server
 void() ClientConnect = 
 {
	bprint (self.netname);
	bprint (" entered the game\n");
	ClientInRankings();
};
 //Called when a client disconnects from the server
 void() ClientDisconnect = 
 {
 	bprint (self.netname);
	bprint (" left the game\n");
	ClientDisconnected();
	if(self.team == CT_SIDE)
		m_iNumCT -= 1;
	if(self.team == T_SIDE)
		m_iNumTerrorist -= 1;
 };
 void() JoiningThink =
 {
	if (m_pIntroCamera && time >= self.m_fIntroCamTime)
	{
		 m_pIntroCamera = find(m_pIntroCamera,classname, "info_intermission");
		if (!m_pIntroCamera)
		{
			m_pIntroCamera = find(world,classname, "info_intermission");
		}

		local entity Target = find(world,targetname,m_pIntroCamera.target);

		if (Target)
		{
			vector CamAngles = vectoangles(normalize(Target.origin - m_pIntroCamera.origin));

			CamAngles_x = -CamAngles_x;
			setorigin(self, m_pIntroCamera.origin);

			self.angles = CamAngles;
			self.v_angle = self.angles;

			self.velocity = '0 0 0';
			self.punchangle = '0 0 0';

			self.fixangle = 1;
			self.view_ofs = '0 0 0';
			self.m_fIntroCamTime = time + 6;
		}
		else
			m_pIntroCamera = world;
}
 };
 //Called every frame, before physics.
void() PlayerPreThink = 
{
	if(self.deadflag == DEAD_DEAD)
		return;
	if(self.weapon == IT_AWP)
		stuffcmd(self,"crosshair 0\n");
	else
		stuffcmd(self,"crosshair 1\n");
	if (BotPreFrame()) // FrikBot
		return;
	WaterMove ();
	SetClientFrame();
	if (self.m_iJoiningState != JOINED)
		JoiningThink();
	TraceTexture();
	if (self.button1)
    {
        PlayerUse();
	}
	else
		self.flags = self.flags | FL_USERELEASED;
	if (self.button2)
	{
		PlayerJump ();
	}
	else
		self.flags = self.flags | FL_JUMPRELEASED;
	PlayerCrouch();
};
.float nextfootstep;

void() playerfootstep =
{
	local float r;

	if (self.deadflag) 
		return;
	if (!(self.flags & FL_ONGROUND))
		return;
	if (vlen(self.velocity) < 100)
		return;

	if (time < self.nextfootstep)
		return;

	self.nextfootstep = time + 0.4;

	r = floor(random()*4);
	// ATTN_IDLE to make them short range
	if (r == 0)
	{
		if(sound_step1)
	    	sound(self, CHAN_AUTO, sound_step1, 0.5, ATTN_IDLE);
    }
	else if (r == 1)
	{
		if(sound_step2)
	    	sound(self, CHAN_AUTO, sound_step2, 0.5, ATTN_IDLE);
    }
	else if (r == 2)
	{
		if(sound_step3)
	    	sound(self, CHAN_AUTO, sound_step3, 0.5, ATTN_IDLE);
    }
	else if (r == 3)
	{
		if(sound_step4)
	    	sound(self, CHAN_AUTO, sound_step4, 0.5, ATTN_IDLE);
    }
};
void() PlayerUse =
{
    local entity pTrain;
	local entity to_use;
	pTrain = world;
	
	if ( !(self.flags & FL_USERELEASED) )
		return;		// don't pogo stick

    self.flags = self.flags - (self.flags & FL_USERELEASED);

	to_use = findradius (self.origin, 64);
	if(to_use != world)
	{
	   if((to_use.useflags & PL_SHORTUSE) || (to_use.useflags & PL_LONGUSE))
          SUB_FireTargetsEnt(to_use);
	}
	self.button1 = 0;
};
 //Called every frame, AFTER physics.
 void() PlayerPostThink = 
 {
	if (BotPostFrame()) // FrikBot
		return;
    if(self.deadflag == DEAD_DEAD)
		return;
	CheckImpulses();
	/*
	CheckBuyZone();
	// Reset this.. css style
	self.m_bInBuyZone = 0;
	if(!m_bMapHasBuyZone)
		OLD_CheckBuyZone();
	*/
 	if(self.m_iMenu == Menu_ChooseTeam)
	{
		self.angles = '0 0 0';
		self.fixangle = TRUE;
	}
	WeaponFrameAll();
	playerfootstep();
};
/*
============
SelectSpawnPoint

Returns the entity to spawn at
============
*/
entity() SelectSpawnPoint =
{

	local	entity spot;
	local	entity thing;
	local	float  pcount;

	spot = lastspawn;
	while (1)
	{
		spot = find(spot, classname, "info_player_deathmatch");
		if (spot != world)
		{
			if (spot == lastspawn)
				return lastspawn;
			pcount = 0;
			thing = findradius(spot.origin, 64);
			while(thing)
			{
				if (thing.classname == "player")
					pcount = pcount + 1;
				thing = thing.chain;
			}
			if (pcount == 0)
			{
				lastspawn = spot;
				return spot;
			}
		}
	}
	spot = find (world, classname, "info_player_start");
	if (!spot)
		error ("PutClientInServer: no info_player_start on level");
	
	return spot;
};
//Called to spawn the clients player entity
void() PutClientInServer =
{
	self.solid = SOLID_NOT;
	self.takedamage = DAMAGE_NO;
	self.velocity = '0 0 0';
	self.punchangle = '0 0 0';
	self.m_iJoiningState = READINGLTEXT;
	self.health = 1;
	self.m_iTeam = UNASSIGNED;
	self.fixangle = 1;
	setmodel (self, "progs/player.mdl");
	self.think = Show_Menu_Team;
	self.nextthink = time + 2;
	
	local entity Target;
	Target = find(world,classname,"info_intermission");
	m_pIntroCamera = Target;
	if(m_pIntroCamera)
	{	
		Target = find(world,targetname,m_pIntroCamera.target);
	}
	if(m_pIntroCamera && Target)
	{
		local vector CamAngles;
		CamAngles = vectoangles(normalize(Target.origin - m_pIntroCamera.origin));
		CamAngles_x = -CamAngles_x;
		setorigin(self,m_pIntroCamera.origin);
		self.angles = CamAngles;
		self.v_angle = self.angles;
		self.m_fIntroCamTime = time + 6;
		self.view_ofs = '0 0 0';
	}
	else
	{
		self.v_angle = '0 0 0';
		self.angles = v_forward;
	}
	
};

void() ResetAmmo =
{
	self.ammo_dd44 = 0;
	self.dd44clip = 0;
	self.famasburst = 0;
	self.ammo_magnum = 0;
	self.magnumclip = 0;
	self.ammo_m3 = 0;
	self.m3clip = 0;
	self.ammo_kf7 = 0;
	self.kf7clip = 0;
	self.ammo_awp = 0;
	self.awpclip = 0;
	self.ammo_phantom = 0;
	self.phantomclip = 0;
	self.ammo_aug = 0;
	self.augclip = 0;
	self.ammo_famas = 0;
	self.famasclip = 0;
	self.ammo_ar33 = 0;
	self.ar33clip = 0;
	self.he_grenades = 0;
	//reset fired bullets
	self.dd44_fired = 0;
	self.magnum_fired = 0;
	self.m3_fired = 0;
	self.kf7_fired = 0;
	self.awp_fired = 0;
	self.phantom_fired = 0;
	self.aug_fired = 0;
	self.famas_fired = 0;
	self.ar33_fired = 0;
}
void()Decal_Hack=
{
	WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
	WriteByte (MSG_BROADCAST, TE_GUNSHOT);
	WriteCoord (MSG_BROADCAST, self.origin_x);
	WriteCoord (MSG_BROADCAST, self.origin_y);
	WriteCoord (MSG_BROADCAST, self.origin_z);
	WriteShort (MSG_BROADCAST, etoi(self));	
}
void() PutClientCTInServer =
{
	local	entity spot;
	spot = SelectSpawnPoint ();
	self.classname = "player";
	self.health = 100;
	self.takedamage = DAMAGE_AIM;
	self.solid = SOLID_SLIDEBOX;
	self.movetype = MOVETYPE_WALK;
	self.flags = FL_CLIENT;
	self.team = CT_SIDE;
	self.p99clip = 7;
	self.ammo_p99 = 21;
	ResetAmmo();
	self.th_die = PlayerDie;
	self.deadflag = DEAD_NO;
	self.origin = spot.origin + '0 0 10';
	self.angles = spot.angles;
	self.fixangle = TRUE;
	setmodel (self, "progs/007.mdl");
	self.hull = 2;
	self.state = 0;
	self.view_ofs = DEFAULT_VIEWHEIGHT;
	setsize(self,VEC_HULLHL_MIN,VEC_HULLHL_MAX);
	self.iSlot = SECONDARY;
	self.weapon = IT_P99;
	self.items = self.items | IT_P99 | IT_KNIFE;
	m_iNumCT +=1;
	self.m_iJoiningState = JOINED;
	self.m_iMenu = Menu_OFF;
	self.fov = 90;
	stuffcmd(self,"fov 90\n");
	stuffcmd(self,"scope 0\n");
	UpdateWeapon();
	Decal_Hack();
}
void() PutClientTInServer =
{
	local	entity spot;
	spot = SelectSpawnPoint ();
	self.classname = "player";
	self.health = 100;
	self.takedamage = DAMAGE_AIM;
	self.solid = SOLID_SLIDEBOX;
	self.movetype = MOVETYPE_WALK;
	self.flags = FL_CLIENT;
	self.team = T_SIDE;
	self.p99clip = 7;
	self.ammo_p99 = 21;
	ResetAmmo();
	self.th_die = PlayerDie;
	self.deadflag = DEAD_NO;
	self.origin = spot.origin + '0 0 10';
	self.angles = spot.angles;
	self.fixangle = TRUE;
	setmodel (self, "progs/ivan.mdl");
	self.hull = 2;
	self.state = 0;
	self.view_ofs = DEFAULT_VIEWHEIGHT;
	setsize(self,VEC_HULLHL_MIN,VEC_HULLHL_MAX);
	self.iSlot = SECONDARY;
	self.weapon = IT_P99; 
	self.items = self.items | IT_P99 | IT_KNIFE;
	m_iNumTerrorist +=1;
	self.m_iJoiningState = JOINED;
	self.m_iMenu = Menu_OFF;
	self.fov = 90;
	stuffcmd(self,"fov 90\n");
	stuffcmd(self,"scope 0\n");
	UpdateWeapon();
	Decal_Hack();
}
void() info_intermission=
{};
void() info_player_start=
{};
void() info_player_deathmatch=
{};
void() info_target=
{};
