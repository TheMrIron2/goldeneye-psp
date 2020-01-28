void(float cShots,vector vecSrc,vector vecDirShooting,vector vecSpread,float flDistance,float iDamage)FireBullets;
void(float damage, vector dir) TraceAttack;
vector(float flDelta) GetAutoaimVector =
{
		makevectors( self.v_angle + self.punchangle );
		return v_forward;
}
vector() GetGunPosition =
{
	local vector org;
	org = self.origin + self.view_ofs;
	return org;
}
/*
=-=-=-=-=
 Killed
=-=-=-=-=
*/
void(entity targ, entity attacker) Killed =
{
	local entity oself;
	
	if (targ.health < -99)
		targ.health = -99;		// don't let sbar look bad if a player

	targ.takedamage = DAMAGE_NO;
	targ.touch = SUB_Null;

	oself = self;
	self = targ; // self must be targ for th_die
	self.th_die ();
	self = oself;
};
/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
T_Damage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

void(entity targ, entity inflictor, entity attacker, float damage) T_Damage=
{
    local	entity	oldself;


    if (!targ.takedamage)
        return;

// used by buttons and triggers to set activator for target firing
    damage_attacker = attacker;

// figure momentum add
/*
    if ( (inflictor != world) && (targ.movetype == MOVETYPE_WALK) )
    {
        dir = targ.origin - (inflictor.absmin + inflictor.absmax) * 0.5;
        dir = normalize(dir);
        targ.velocity = targ.velocity + dir*damage*8;
    }
*/
// check for godmode
    if (targ.flags & FL_GODMODE)
        return;

// add to the damage total for clients, which will be sent as a single
// message at the end of the frame
    if (targ.flags & FL_CLIENT)
    {
        targ.dmg_take = targ.dmg_take + damage;
        targ.dmg_save = targ.dmg_save + damage;
        targ.dmg_inflictor = inflictor;
    }

// team play damage avoidance
    if (targ.team == attacker.team && targ.classname != "player")
        return;

// do the damage
    targ.health = targ.health - damage;

    if (targ.health <= 0)
    {
		 if(targ.classname != "player")
		 {
			Killed (targ, attacker);
			return;
		 }
		if(targ.team == attacker.team )
			AddAccount(attacker,-500,ADDMONEY);
		else
			AddAccount(attacker,1000,ADDMONEY);
        Killed (targ, attacker);
        return;
    }

// react to the damage
/*
    oldself = self;
    self = targ;

    if (self.th_pain)
        self.th_pain (attacker, damage);

    self = oldself;
*/
};
/*
============
CanDamage

Returns true if the inflictor can directly damage the target. Used for
explosions and melee attacks.
============
*/
float(entity targ, entity inflictor) CanDamage =
{
	// bmodels need special checking because their origin is 0,0,0
	if (targ.movetype == MOVETYPE_PUSH)
	{
		traceline(inflictor.origin, 0.5 * (targ.absmin + targ.absmax), TRUE, self);
		if (trace_fraction == 1)
			return TRUE;
		if (trace_ent == targ)
			return TRUE;
		return FALSE;
	}

	traceline(inflictor.origin, targ.origin, TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '-15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '-15 15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;
	traceline(inflictor.origin, targ.origin + '15 -15 0', TRUE, self);
	if (trace_fraction == 1)
		return TRUE;

	return FALSE;
};

void(vector vecSrc, entity pevInflictor, entity pevAttacker, float flDamage, float flRadius, entity iClassIgnore, float bitsDamageType)RadiusDamage=
{
	local entity pEntity;
	local float flAdjustedDamage,falloff;

	if(flRadius)
		falloff = flDamage / flRadius;
	else
		falloff = 1;
	
	local float bInWater = (pointcontents(vecSrc) == CONTENT_WATER);
	vecSrc_z += 1;
	
	if (!pevAttacker)
		pevAttacker = pevInflictor;
	
	pEntity = findradius(vecSrc,flRadius);
	while(pEntity)
	{
		if (pEntity != iClassIgnore)
		{
			pEntity = checkclient();
			if (!pEntity)
				return;
			
			//if (pEntity.takedamage == DAMAGE_NO || pEntity.deadflag != DEAD_NO)
			//	continue;

			if (bInWater && pEntity.waterlevel == 0)
				continue;

			if (!bInWater && pEntity.waterlevel == 3)
				continue;

			local float damageRatio = 1;
			local float length = vlen(vecSrc - pEntity.origin);
			
			flAdjustedDamage = flDamage - length * falloff;
			
			if(flAdjustedDamage < 0)
				flAdjustedDamage = 0;
			T_Damage (pEntity, pevInflictor, pevAttacker, flAdjustedDamage);
		}
		pEntity = pEntity.chain;
	}
}

void RadiusFlash(vector vecSrc, entity Inflictor, entity pevAttacker, float flDamage, float iClassIgnore, float bitsDamageType) =
{
	local float flAdjustedDamage, falloff;
	local vector vecSpot;
	local entity pEntity;
	float flRadius = 1500;
	
	if(flRadius)
		falloff = flDamage / flRadius;
	else
		falloff = 1;
	
	local float bInWater = (pointcontents(vecSrc) == CONTENT_WATER);
	vecSrc_z += 1;
	pEntity = findradius(vecSrc,1500);
	while(pEntity)
	{
		local vector vecLOS;
		local float flDot,fadeTime,fadeHold,alpha,currentHoldTime;
		pEntity = checkclient();
		if (!pEntity)
			return;
		
		if (pEntity.takedamage == DAMAGE_NO || pEntity.deadflag != DEAD_NO)
			continue;

		if (bInWater && pEntity.waterlevel == 0)
			continue;

		if (!bInWater && pEntity.waterlevel == 3)
			continue;
		vecSpot = pEntity.origin; // hz
		traceline(vecSrc,vecSpot,1,Inflictor);
		if(trace_fraction == 1.0 && trace_ent == pEntity) 
			return;
		
		traceline(vecSrc,vecSpot,1,trace_ent);		
		if(trace_fraction >= 1.0)
		{
			if(trace_startsolid)
			{
				trace_endpos = vecSrc;
				trace_fraction = 0;
			}
			flAdjustedDamage = flDamage - vlen((vecSrc - trace_endpos) * falloff);
			if(flAdjustedDamage < 0)
				flAdjustedDamage = 0;
			makevectors(pEntity.v_angle);
			vecLOS = vecSrc - (pEntity.view_ofs + pEntity.origin);
			flDot = vecLOS * v_forward;
			if(flDot < 0)
			{
				alpha = 200;
				fadeTime = flAdjustedDamage * 3;
				fadeHold = flAdjustedDamage / 1.5;
			}
			else
			{
				alpha = 255;
				fadeTime = flAdjustedDamage * 3;
				fadeHold = flAdjustedDamage / 1.5;
			}
			currentHoldTime = pEntity.m_blindStartTime + pEntity.m_blindHoldTime - time; 
			if(pEntity.m_blindStartTime != 0 && pEntity.m_blindFadeTime != 0)
			{
				if((pEntity.m_blindStartTime + pEntity.m_blindFadeTime + pEntity.m_blindHoldTime) > time)
				{
					if (pEntity.m_blindFadeTime > fadeTime)
						fadeTime = pEntity.m_blindFadeTime;

					if (pEntity.m_blindAlpha >= alpha)
						alpha = pEntity.m_blindAlpha;
				}
			}
			
			UTIL_ScreenFade(pEntity,'255 255 255', fadeTime, fadeHold, alpha, 0);
		}	
		pEntity = pEntity.chain;
	}
}
/*
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
WaterMove

Can be used for clients or monsters
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

void() WaterMove =
{
    if (self.movetype == MOVETYPE_NOCLIP)
        return;

    if (self.health < 0)
        return;

    if (self.waterlevel != 3)
    {
        self.air_finished = time + 12;
        self.dmg = 2;
    }
    else if (self.air_finished < time && self.pain_finished < time)
    {   // drown!
        self.dmg = self.dmg + 2;

        if (self.dmg > 15)
            self.dmg = 10;

        T_Damage (self, world, world, self.dmg);
        self.pain_finished = time + 1;
    }

    if (self.watertype == CONTENT_LAVA && self.dmgtime < time)
    {   // do damage
        self.dmgtime = time + 0.2;
        T_Damage (self, world, world, 6*self.waterlevel);
    }
    else if (self.watertype == CONTENT_SLIME && self.dmgtime < time)
    {   // do damage
        self.dmgtime = time + 1;
        T_Damage (self, world, world, 4*self.waterlevel);
    }
};

void(entity pevInflictor, entity pevAttacker, float flDamage, entity iClassIgnore, float bitsDamageType) _RadiusDamage =
{
	if (flDamage > 80)
		RadiusDamage(self.origin, pevInflictor, pevAttacker, flDamage, flDamage * 3.5, iClassIgnore, bitsDamageType);
	//else
	//	RadiusDamage2(pev->origin, pevInflictor, pevAttacker, flDamage, iClassIgnore, bitsDamageType);
}

void(float cShots,float damage,string soundpath)DefaultFire=
{
	local float m_iClip = self.currentammo;
	local float speed = vlen(self.velocity);
	if (m_iClip <= 0)
		return;
	if(soundpath)
		sound (self, CHAN_AUTO, soundpath, 1, ATTN_NORM);
	cShots = mathlib_min(m_iClip,cShots);
	self.currentammo -= cShots;
	m_iClip -= cShots;
	local vector vecSrc;
	local vector vecAim;
	local vector vecAcc;
	vecSrc = GetGunPosition();
	vecAcc = '0 0 0';
	vecAim = GetAutoaimVector(AUTOAIM_2DEGREES);
	if(cShots != 1)
		vecAcc = [SpreadX / 100,SpreadY / 100,0];
	if(speed)
		vecAcc += [SpreadY / 5000,SpreadY / 5000,0] * speed;
	if(cShots == 1)
		vecAcc = vecAcc / 10;
	if ( !( self.flags & FL_ONGROUND ) )
		vecAcc += [SpreadY / 5000,SpreadY / 5000,0] * 250;

	FireBullets(cShots,vecSrc,vecAim,vecAcc,8192,damage);
	if(!LeftSpread)
		LeftSpread = 1;
	self.punchangle_x -= RANDOM_LONG(SpreadY / 2,SpreadY);
	self.punchangle_y += SpreadX * RANDOM_LONG(-1,1) / 2;
	if(self.punchangle_x < -MaxSpreadY)
	{
		self.punchangle_x = -MaxSpreadY + RANDOM_LONG(-2,2);
		self.punchangle_y += SpreadX * LeftSpread; 
	}
	
	if(self.punchangle_y <= -MaxSpreadX)
		LeftSpread = 1;
	else if(self.punchangle_y > MaxSpreadX)
		LeftSpread = -1;
}