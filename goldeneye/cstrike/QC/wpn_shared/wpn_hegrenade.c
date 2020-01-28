float() crandom;
void() BounceSound=
{
	local float r;
	r = randomfloat(0,2);
	if (self.dmg > 50)
	{
		sound(self, CHAN_VOICE, "weapons/he_bounce-1.wav", 0.25, ATTN_NORM);
		return;
	}
	
	switch (r)
	{
		case 0: sound(self, CHAN_VOICE, "weapons/grenade_hit1.wav", 0.25, ATTN_NORM); break;
		case 1: sound(self, CHAN_VOICE, "weapons/grenade_hit2.wav", 0.25, ATTN_NORM); break;
		case 2: sound(self, CHAN_VOICE, "weapons/grenade_hit3.wav", 0.25, ATTN_NORM); break;
	}
}
float m_iBounceCount;
void() BounceTouch=
{
	// don't hit the guy that launched this grenade
	if (other == self.owner)
		return;
	
	if(other.classname == "func_breakable")
	{
		self.velocity *= -2;
		return;		
	}
	
	if (self.flags & FL_ONGROUND)
	{
		// add a bit of static friction
		self.velocity = self.velocity * 0.8;
	}
	else
	{
		if (m_iBounceCount < 5)
		{
			// play bounce sound
			BounceSound();
		}

		if (m_iBounceCount >= 10)
		{
			self.groundentity = world;
			self.flags |= FL_ONGROUND;
			self.velocity = '0 0 0';
		}
		++m_iBounceCount;
	}
}
entity fexplo,exp,smoke;
void() smoke_amim =
{
	if(smoke.frame == 15)
	{
		SUB_Remove();
	}
	smoke.frame += 1;
	smoke.think = smoke_amim;
	smoke.nextthink = time + 0.1;
}
void()Smoke3_C=
{
	smoke = spawn();
	smoke.owner = self;
	smoke.movetype = MOVETYPE_NONE;
	smoke.velocity = '0 0 0';
	setsize (smoke, '0 0 0', '0 0 0');	
	smoke.origin = self.origin;
	smoke.origin_z -= 5;
	smoke.touch = SUB_Null;
	setmodel (smoke, "sprites/steam1.spr");
	smoke.solid = SOLID_NOT;
	smoke.frame = 1;
	
	smoke_amim();
}
void() eexplo =
{
	if(exp.frame == 23)
	{
		SUB_Remove();
		return;
	}
	exp.frame += 1;
	exp.think = eexplo;
	exp.nextthink = time + 0.1;
}
void() fexplo_anim =
{
	if(fexplo.frame == 29)
	{
		SUB_Remove();
	}
	fexplo.frame += 1;
	fexplo.think = fexplo_anim;
	fexplo.nextthink = time + 0.1;
}
void() BecomeExplosion =
{
	fexplo = spawn();
	fexplo.owner = self;
	fexplo.movetype = MOVETYPE_NONE;
	fexplo.velocity = '0 0 0';
	setsize (fexplo, '0 0 0', '0 0 0');	
	fexplo.origin = self.origin;
	fexplo.origin_z += 20 ;
	fexplo.touch = SUB_Null;
	setmodel (fexplo, "sprites/fexplo.spr");
	fexplo.solid = SOLID_NOT;
	fexplo.frame = 1;
	fexplo_anim();
};
void() BecomeExplosion2 =
{
	exp = spawn();
	exp.owner = self;
	exp.movetype = MOVETYPE_NONE;
	exp.velocity = '0 0 0';
	setsize (exp, '0 0 0', '0 0 0');	
	exp.origin = fexplo.origin;
	exp.origin_x += randomfloat(-64,64);
	exp.origin_y += randomfloat(-64,64);
	exp.origin_z += randomfloat(30,35);
	exp.touch = SUB_Null;
	setmodel (exp, "sprites/eexplo.spr");
	exp.solid = SOLID_NOT;
	exp.frame = 1;
	eexplo();
};
void() GrenadeExplode =
{
	_RadiusDamage(self,self.owner,self.dmg,world,0);
	//RadiusFlash(self.origin, self, self.owner, 4, 0, 0);
	WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
	WriteByte (MSG_BROADCAST, TE_EXPLOSION);
	WriteCoord (MSG_BROADCAST, self.origin_x);
	WriteCoord (MSG_BROADCAST, self.origin_y);
	WriteCoord (MSG_BROADCAST, self.origin_z);
	sound(self, CHAN_VOICE, "weapons/explode3.wav", 0.5, ATTN_NORM);
	
	BecomeExplosion();
	BecomeExplosion2();
	self.nextthink = time + 0.55;
	self.think = Smoke3_C;
	SUB_Remove();
};
void()ThrowGrenade=
{
	local entity he;
	local vector angThrow,vecSrc;
	local float flVel;
	angThrow = self.v_angle;
	if (angThrow_x < 0)
		angThrow_x = -10 + angThrow_x * ((90 - 10) / 90);
	else
		angThrow_x = -10 + angThrow_x * ((90 + 10) / 90);
	flVel = (90 - angThrow_x) * 6;
	if (flVel > 750)
		flVel = 750;
	makevectors(angThrow);
	vecSrc = self.origin + self.view_ofs + v_forward*16;
	m_iBounceCount = 0;
	self.he_grenades -=1;
	he = spawn ();
	he.owner = self;
	he.movetype = MOVETYPE_BOUNCE;
	he.solid = SOLID_BBOX;
	he.velocity = v_forward * flVel + v_up * 50 + crandom()*v_right*10 + crandom()*v_up*10 + v_forward * fabs(self.velocity_y);
	he.avelocity = '150 150 150';
	he.angles = vectoangles(he.velocity);
	setmodel (he, "progs/w_grenade.mdl");
	setsize (he, '0 0 0', '0 0 0');	
	he.classname = "grenade";
	setorigin (he, vecSrc);
	he.touch = BounceTouch;
	he.dmg = 100;
	he.nextthink = time + 1.5;
	he.think = GrenadeExplode;
}
void() he_anim_attack=
{
	if(self.weaponframe == 75)
		{
			self.weaponframe = 1;
			self.state = 0;
			self.items = self.items - (self.items & IT_HEGRENADE);
			ChangeWeapon();
			return;
		}
	if(self.weaponframe == 63)
		ThrowGrenade();
	self.weaponframe += 1;
	self.think = he_anim_attack;
	self.nextthink = time + 0.03;
}

void()HE_Attack=
{
			self.weaponframe = 1;
			self.state = ATTACK;
			he_anim_attack();
			self.attack_finished = time + 0.2;
}
