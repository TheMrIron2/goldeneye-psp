void() func_illusionary =
{
void() func_illusionary =
{
	self.angles = '0 0 0';
	self.solid = SOLID_NOT;
	if(self.parent)
	{
	   self.movetype = MOVETYPE_COMPOUND;
	   self.aiment = find(world, targetname, self.parent);
	   setmodel (self, self.model);
	}
	else
	{
       self.movetype = MOVETYPE_NONE;
       setmodel (self, self.model);
       makestatic (self);
	}
};
};
void() func_wall_use =
{	
	self.frame = 1 - self.frame;
};

void() func_wall =
{
	self.angles = '0 0 0';
	self.movetype = MOVETYPE_PUSH;
	self.solid = SOLID_BSP;
	self.use = func_wall_use;
	setmodel (self, self.model);
};
float BRK_BLOCKABLE	= 1;
float BRK_STARTOFF	= 2;
float BRK_ONEHIT		= 4;
.float	color;
.float  material;
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
	matRocks
};
.string pain_sound1;
.string pain_sound2;
.string pain_sound3;
.string  die_sound1;
.string  die_sound2;
.string  gib_model1;
.string  gib_model2;
.string  gib_model3;

void () blocker_use =
{
    if ( !self.state )
    {
      self.state = 1;
      setorigin( self, self.origin - '8000 8000 8000' );
    }
    else
    {
      self.state = 0;
      setorigin( self, self.origin + '8000 8000 8000' );
    }
};
   
void(entity attacker, float damage)	breakable_pain =
{
    float r;
    r = randomlong(0,2);
    if(r == 0 )
		sound(self, CHAN_AUTO, self.pain_sound1, 1, ATTN_NORM);
	else if(r == 1)
		sound(self, CHAN_AUTO, self.pain_sound2, 1, ATTN_NORM);
	else if(r == 2)
		sound(self, CHAN_AUTO, self.pain_sound3, 1, ATTN_NORM);

	// check if one hit is required to break it
	if(self.spawnflags & BRK_ONEHIT)
		self.health = self.max_health;
};

void() breakable_die=
{
	local entity new;
	local vector tmpvec;
	local vector tmpmin, tmpmax, tmpsize;
	local float r;

	while(self.color > 0)
	{
		new = spawn();
		new.origin = self.origin;
		if(random() < 0.33)
			setmodel (new, self.gib_model1 );
		else if(random() < 0.5)
			setmodel (new, self.gib_model2 );
		else
			setmodel (new, self.gib_model3 );
		if (self.material == matComputer)
			new.bodygroup = randomlong(0,14);
		if (self.material == matMetal)
			new.bodygroup = randomlong(0,12);
		setsize (new, '0 0 0', '0 0 0');
		new.velocity_x = 70 * crandom();
		new.velocity_y = 70 * crandom();
		new.velocity_z = 140 + 70 * random();
		new.movetype = MOVETYPE_BOUNCE;
		new.solid = SOLID_BBOX;
		new.avelocity_x = random()*600;
		new.avelocity_y = random()*600;
		new.avelocity_z = random()*600;
		new.nextthink = time + 2 + random()*3;
		new.think = SUB_Remove;

		self.absmin = self.origin + self.mins;
		self.absmax = self.origin + self.maxs;
		tmpvec_x = self.absmin_x + (random() * (self.absmax_x - self.absmin_x));
		tmpvec_y = self.absmin_y + (random() * (self.absmax_y - self.absmin_y));
		tmpvec_z = self.absmin_z + (random() * (self.absmax_z - self.absmin_z));

		setorigin(new, tmpvec);

		self.color = self.color - 1;
	}

    r = randomlong(0,1);
    if(r == 0)
		sound(self, CHAN_VOICE, self.die_sound1, 1, ATTN_NORM);
	else if(r == 1)
		sound(self, CHAN_VOICE, self.die_sound2, 1, ATTN_NORM);

	remove(self);
};

void () blocker_use;
void() func_breakable =
{
	local vector tmpvec;
	local float tmpflt;

	self.movetype = MOVETYPE_PUSH;
	self.solid = SOLID_BSP;
	//self.mdl = self.model;
	setmodel (self, self.model);
	setsize (self, self.mins, self.maxs);
	setorigin (self, self.origin);

	if(self.health > 0)
	{
		if(!self.color)
		{
			tmpvec = self.maxs - self.mins;
			tmpvec = tmpvec * 0.031; //(divide by about 32)
			if(tmpvec_x < 1)
				tmpvec_x = 1;
			if(tmpvec_y < 1)
				tmpvec_y = 1;
			if(tmpvec_z < 1)
				tmpvec_z = 1;
			self.color = tmpvec_x * tmpvec_y * tmpvec_z;
		}
		else if(self.color == -1)
			self.color = 0;
		if(self.color > 16) // max number of chunks
			self.color = 16;
		self.takedamage = DAMAGE_YES;
		self.max_health = self.health;
		self.th_die  = breakable_die;
		self.th_pain = breakable_pain;
	}

// setup as either togglable
	self.use = 	blocker_use;
	if ( self.spawnflags & BRK_STARTOFF )
	{
		self.state = 0;
		setorigin( self, self.origin + '8000 8000 8000' );
	}
	else
	{
		self.state = 1;
	}

	self.pain_sound1 = string_null;
	self.pain_sound2 = string_null;
	self.pain_sound3 = string_null;
	self.die_sound1  = string_null;
	self.die_sound2  = string_null;
	self.gib_model1  = string_null;
	self.gib_model2  = string_null;
	self.gib_model3  = string_null;
	switch(self.material)
	{
		case matGlass:
			self.pain_sound1 = "break/glass1.wav";
			self.pain_sound2 = "break/glass2.wav";
			self.pain_sound3 = "break/glass3.wav";
			self.die_sound1  = "break/bustglass1.wav";
			self.die_sound2  = "break/bustglass2.wav";
			self.gib_model1  = "progs/glass1.mdl";
			self.gib_model2  = "progs/glass2.mdl";
			self.gib_model3  = "progs/glass3.mdl";
		break;
		case matWood: //undone
			self.pain_sound1 = "break/wood1.wav";
			self.pain_sound2 = "break/wood2.wav";
			self.pain_sound3 = "break/wood3.wav";
			self.die_sound1  = "break/bustcrate1.wav";
			self.die_sound2  = "break/bustcrate2.wav";
			self.gib_model1  = "progs/Rubble1.mdl";
			self.gib_model2  = "progs/Rubble2.mdl";
			self.gib_model3  = "progs/Rubble3.mdl";
		break;
		case matMetal:
			self.pain_sound1 = "break/metal1.wav";
			self.pain_sound2 = "break/metal2.wav";
			self.pain_sound3 = "break/metal3.wav";
			self.die_sound1  = "break/bustmetal1.wav";
			self.die_sound2  = "break/bustmetal2.wav";
			self.gib_model1  = "progs/metalplategibs.mdl";
			self.gib_model2  = "progs/metalplategibs.mdl";
			self.gib_model3  = "progs/metalplategibs.mdl";
		break;
		case matComputer:
			self.pain_sound1 = "buttons/spark5.wav";
			self.pain_sound2 = "buttons/spark6.wav";
			self.pain_sound3 = "buttons/spark5.wav";
			self.die_sound1  = "break/bustmetal1.wav";
			self.die_sound2  = "break/bustmetal2.wav";
			self.gib_model1  = "progs/computergibs.mdl";
			self.gib_model2  = "progs/computergibs.mdl";
			self.gib_model3  = "progs/computergibs.mdl";
		break;
		default:
			self.pain_sound1 = "break/concrete1.wav";
			self.pain_sound2 = "break/concrete2.wav";
			self.pain_sound3 = "break/concrete3.wav";
			self.die_sound1  = "break/bustconcrete1.wav";
			self.die_sound2  = "break/bustconcrete2.wav";
			self.gib_model1  = "progs/Rubble1.mdl";
			self.gib_model2  = "progs/Rubble2.mdl";
			self.gib_model3  = "progs/Rubble3.mdl";
		break;
	}
	//precache res
    precache_sound( self.pain_sound1 );
	precache_sound( self.pain_sound2 );
	precache_sound( self.pain_sound3 );
	precache_sound( self.die_sound1 );
	precache_sound( self.die_sound2 );
	precache_model( self.gib_model1 );
	precache_model( self.gib_model2 );
	precache_model( self.gib_model3 );
};

float SF_CONVEYOR_VISUAL   = 1;
float SF_CONVEYOR_NOTSOLID = 2;

void(float speedd) UpdateSpeed =
{
	if ( speedd < 0 )
	{
		self.rendercolor_x = 1;
    }
	else
    {
		self.rendercolor_x = 0;
    }
	// Encode it as an integer with 4 fractional bits
	local float speedCode;
	speedCode = (fabs(speedd) * 16.0);

	self.rendercolor_y = (speedCode / 255);
	self.rendercolor_z = (speedCode * 16);
};

void() conveyor_use =
{
    self.speed = 0 - self.speed;
	UpdateSpeed( self.speed );
};

void() func_conveyor =
{
    SetMovedir();

	self.movetype = MOVETYPE_PUSH;	// so it doesn't get pushed by anything
	self.solid = SOLID_BSP;
	setmodel (self, self.model);
	
	if ( self.speed == 0 )
		self.speed = 100;

	if ( !(self.spawnflags & SF_CONVEYOR_VISUAL))
		self.flags = self.flags | FL_CONVEYOR;

	// HACKHACK - This is to allow for some special effects
	if ( self.spawnflags & SF_CONVEYOR_NOTSOLID )
	{
		self.solid = SOLID_NOT;
		self.skin  = 0;		// Don't want the engine thinking we've got special contents on this brush
	}

    self.use = conveyor_use;

    UpdateSpeed(self.speed);
};
void()func_rot_button_use=
{
	SUB_UseTargets();
}
#define SF_ROTBUTTON_NOTSOLID 1
/*
	Tricky way to make it work
	Because i am lazy
*/
void()func_rot_button=
{
	self.movetype = MOVETYPE_PUSH;	// so it doesn't get pushed by anything
	self.solid = SOLID_TRIGGER;
	setmodel (self, self.model);
	self.use = func_rot_button_use;
	self.touch = func_rot_button_use;
	self.useflags = self.useflags | PL_SHORTUSE;
}