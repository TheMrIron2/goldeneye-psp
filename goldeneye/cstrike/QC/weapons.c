float() crandom =
{
	return 2*(random() - 0.5);
};

void(vector org, vector vel, float damage) SpawnBlood =
{
	particle (org, vel*0.1, 73, damage*4);
};

entity	multi_ent;
float	multi_damage;
void() ClearMultiDamage =
{
	multi_ent = world;
	multi_damage = 0;
};

void() ApplyMultiDamage =
{
	if (!multi_ent)
		return;
	T_Damage (multi_ent, self, self, multi_damage);
};

void(entity hit, float damage) AddMultiDamage =
{
	if (!hit)
		return;

	if (hit != multi_ent)
	{
		ApplyMultiDamage ();
		multi_damage = damage;
		multi_ent = hit;
	}
	else
		multi_damage = multi_damage + damage;
};
void()spark1 =
{
	if(self.frame == 29)
		SUB_Remove();
	self.frame +=1;
	self.think = spark1;
	self.nextthink = time + 0.025;
}
void(vector org) SparkTrace =
{
	local entity spark;

	spark = spawn();

	spark.owner = self;
	spark.classname = "spark";
	spark.movetype = MOVETYPE_NONE;
	spark.velocity = '0 0 0';
	spark.origin = org;
	spark.touch = SUB_Null;
	setmodel (spark, "sprites/pistol_smoke1.spr");
	spark.rendermode = 3;
	spark.solid = SOLID_NOT;
	spark.frame = 1;
	spark.think = spark1;
	spark.nextthink = time + 0.01;
};

/*
-=-=-=-=-=-=
TraceAttack
-=-=-=-=-=-=
*/
void(float damage, vector dir) TraceAttack=
{
	local	vector	vel, org;

	vel = normalize(dir + v_up*crandom() + v_right*crandom());
	vel = vel + 2*trace_plane_normal;
	vel = vel * 200;

	org = trace_endpos - dir*4;

	if (trace_ent.takedamage)
	{
		SpawnBlood (org, vel*0.2, damage);
		AddMultiDamage (trace_ent, damage);
	}
	else
	{
		WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
		WriteByte (MSG_BROADCAST, TE_GUNSHOT);
		WriteCoord (MSG_BROADCAST, org_x);
		WriteCoord (MSG_BROADCAST, org_y);
		WriteCoord (MSG_BROADCAST, org_z);
		WriteShort (MSG_BROADCAST, etoi(trace_ent));
		//SparkTrace(org);
	}
};
void(float cShots,vector vecSrc,vector vecDirShooting,vector vecSpread,float flDistance,float iDamage)FireBullets=
{
	local vector vecRight = v_right;
	local vector vecUp = v_up;
	local float x,y,z;
	local float newDamage = iDamage;

	ClearMultiDamage ();
	for(float iShot = 1; iShot <= cShots; iShot++)
	{

		do{
			x = RANDOM_LONG(-0.5,0.5) + RANDOM_LONG(-0.5,0.5);
			y = RANDOM_LONG(-0.5,0.5) + RANDOM_LONG(-0.5,0.5);
			z = x*x+y*y;
		} while(z>1);
	local vector vecDir = vecDirShooting + x * vecSpread_x * vecRight + y * vecSpread_y * vecUp;
	local vector vecEnd;
	vecEnd = vecSrc + vecDir * flDistance;
	traceline(vecSrc,vecEnd,0,self);
	if (trace_fraction != 1.0)
	{
		if(iDamage)
		{
			TraceAttack(iDamage,vecDir);
		}
		if(cShots == 1)
		{
			local float unit = iDamage;
			while(newDamage > 0)
			{
				if(pointcontents(trace_endpos + vecDir * iDamage) == CONTENT_SOLID)
					break;
				traceline(trace_endpos + vecDir * iDamage,vecSrc,0,self);
				if(trace_allsolid)
					break;
				WriteByte (MSG_BROADCAST, SVC_TEMPENTITY);
				WriteByte (MSG_BROADCAST, TE_GUNSHOT);
				WriteCoord (MSG_BROADCAST, trace_endpos_x);
				WriteCoord (MSG_BROADCAST, trace_endpos_y);
				WriteCoord (MSG_BROADCAST, trace_endpos_z);
				WriteShort (MSG_BROADCAST, etoi(trace_ent));
				newDamage /= 2;
				unit /= 2;
				vecSrc = trace_endpos;
				traceline(trace_endpos,vecEnd,0,self);
				newDamage -= vlen(trace_endpos - vecSrc) * 0.01;
				TraceAttack(newDamage,vecDir);
			}
		}
	}
	}
	ApplyMultiDamage ();
}
void()SecondaryAttack=
{
	if(time < self.m_flNextSecondaryAttack)
		return;
	if(self.weapon == IT_AWP)
	{
		if(self.state > 0)
			return;
		self.m_flNextSecondaryAttack = time + 0.3;
		if(self.fov == 90)
		{
			stuffcmd(self,"fov 40\n");
			stuffcmd(self,"scope 1\n");
			stuffcmd(self,"sensitivity 1\n");
			self.scope = 1;
			self.fov = 40;
		}
		else if(self.fov == 40)
		{
			self.fov = 10;
			stuffcmd(self,"fov 10\n");
			stuffcmd(self,"sensitivity 1\n");
		}
		else
		{
			self.fov = 90;
			stuffcmd(self,"fov 90\n");
			stuffcmd(self,"scope 0\n");
			stuffcmd(self,"sensitivity 5\n");
			self.scope = 0;
		}
		sound (self, CHAN_AUTO, "weapons/zoom.wav", 1, ATTN_NORM);
	}
	if(self.weapon == IT_AUG)
	{
		if(self.state > 0)
			return;
		self.m_flNextSecondaryAttack = time + 0.3;
		if(self.fov == 90)
		{
			stuffcmd(self,"fov 55\n");
			self.scope = 1;
			self.fov = 40;
		}
		else
		{
			self.fov = 90;
			stuffcmd(self,"fov 90\n");
			self.scope = 0;
		}
	}
	if(self.weapon == IT_FAMAS)
	{
		self.m_flNextSecondaryAttack = time + 0.3;
		if(self.famasburst  == 1)
		{
			centerprint(self,"Switched to Full Auto\n");
			self.famasburst = 0;
		}
		else
		{
			centerprint(self,"Switched to Burst Fire\n");
			self.famasburst = 1;
		}
	}
}
void() UpdateWeapon=
{
	//stuffcmd(self,"crosshair ");
	//stuffcmd(self,ftos(self.crosshair));
	//stuffcmd(self,"\n");
	if(self.weapon == IT_KNIFE)
	{
		self.weaponmodel = "progs/v_slappers.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = 0;
		self.ammo_shells = 0;
	}
	if(self.weapon == IT_P99)
	{
		self.weaponmodel = "progs/v_p99.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.p99clip;
		self.ammo_shells = self.ammo_p99;
		MaxSpreadX = 5;
		MaxSpreadY = 5;
		SpreadX = 2;
		SpreadY = 2;
	}
	if(self.weapon == IT_DD44)
	{
		self.weaponmodel = "progs/v_dd44.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.dd44clip;
		self.ammo_shells = self.ammo_dd44;
		MaxSpreadX = 2;
		MaxSpreadY = 3;
		SpreadX = 1.7;
		SpreadY = 1.7;
	}
	if(self.weapon == IT_MAGNUM)
	{
		self.weaponmodel = "progs/v_magnum.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.magnumclip;
		self.ammo_shells = self.ammo_magnum;
		MaxSpreadX = 2;
		MaxSpreadY = 5;
		SpreadX = 3.5;
		SpreadY = 3.5;
	}
	if(self.weapon == IT_P228)
	{
		//self.weaponmodel = "progs/v_p228.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.p228clip;
		self.ammo_shells = self.ammo_p228;
		MaxSpreadX = 2;
		MaxSpreadY = 2;
		SpreadX = 1.5;
		SpreadY = 1.5;
	}
	if(self.weapon == IT_ELITE)
	{
		//self.weaponmodel = "progs/v_elite.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.eliteclip;
		self.ammo_shells = self.ammo_elite;
		MaxSpreadX = 2;
		MaxSpreadY = 5;
		SpreadX = 2.5;
		SpreadY = 2.5;
	}
	if(self.weapon == IT_HEGRENADE)
	{
		//self.weaponmodel = "progs/v_hegrenade.mdl";
		self.weaponframe = 1;
		self.currentammo = self.he_grenades;
		self.ammo_shells = 0;
	}
	if(self.weapon == IT_M3)
	{
		//self.weaponmodel = "progs/v_m3.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.m3clip;
		self.ammo_shells = self.ammo_m3;
		MaxSpreadX = 2;
		MaxSpreadY = 5;
		SpreadX = 2;
		SpreadY = 2;
	}
	if(self.weapon == IT_KF7)
	{
		self.weaponmodel = "progs/v_kf7.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.kf7clip;
		self.ammo_shells = self.ammo_kf7;
		MaxSpreadX = 2;
		MaxSpreadY = 5;
		SpreadX = 2;
		SpreadY = 2;
	}
	if(self.weapon == IT_AWP)
	{
		//self.weaponmodel = "progs/v_awp.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.awpclip;
		//self.crosshair = cvar("crosshair");
		//stuffcmd(self,"crosshair 0\n");
		self.ammo_shells = self.ammo_awp;
		MaxSpreadX = 5;
		MaxSpreadY = 5;
		SpreadX = 5;
		SpreadY = 5;
	}
	if(self.weapon == IT_PHANTOM)
	{
		self.weaponmodel = "progs/v_phantom.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.phantomclip;
		self.ammo_shells = self.ammo_phantom;
		MaxSpreadX = 2;
		MaxSpreadY = 5;
		SpreadX = 1.6;
		SpreadY = 1.6;
	}
	if(self.weapon == IT_AUG)
	{
		//self.weaponmodel = "progs/v_aug.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.augclip;
		self.ammo_shells = self.ammo_aug;
		MaxSpreadX = 2;
		MaxSpreadY = 5;
		SpreadX = 2;
		SpreadY = 2;
	}
	if(self.weapon == IT_FAMAS)
	{
		//self.weaponmodel = "progs/v_famas.mdl";
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.currentammo = self.famasclip;
		self.ammo_shells = self.ammo_famas;
		MaxSpreadX = 2;
		MaxSpreadY = 5;
		SpreadX = 1.6;
		SpreadY = 1.6;
	}
	if(self.weapon == IT_AR33)
	{
		self.weaponmodel = "progs/v_ar33.mdl";
		self.wepsequence = 7;
		self.weaponframe = 0;
		self.currentammo = self.ar33clip;
		self.ammo_shells = self.ammo_ar33;
		MaxSpreadX = 5;
		MaxSpreadY = 5;
		SpreadX = 1.5;
		SpreadY = 1.5;
	}
}

void() WeaponAttack =
{
	makevectors(self.v_angle);
	if(self.weapon == IT_KNIFE)
	{
		KNIFE_PrimaryAttack();
	}
	if(self.currentammo == 0 && self.ammo_shells == 0 && self.weapon != IT_KNIFE && !self.semi)
	{
		self.semi = 1;
		sound (self, CHAN_AUTO,"weapons/357_cock1.wav", 1, ATTN_NORM);
		self.attack_finished = time + 0.2;
	}
	if(self.weapon == IT_M3)
	{
		M3_PrimaryAttack();
	}
	if(self.weapon == IT_KF7)
	{
		KF7_PrimaryAttack();
	}
	if(self.weapon == IT_PHANTOM)
	{
		Phantom_PrimaryAttack();
	}
	if(self.weapon == IT_AUG)
	{
		Aug_PrimaryAttack();
	}
	if(self.weapon == IT_FAMAS)
	{
		Famas_PrimaryAttack();
	}
	if(self.weapon == IT_AR33)
	{
		AR33_PrimaryAttack();
	}
	if(!self.semi)
	{
		self.semi = 1;
		if(self.weapon == IT_P99)
		{
			P99_Attack();
		}
		if(self.weapon == IT_DD44)
		{
			DD44_Attack();
		}
		if(self.weapon == IT_MAGNUM)
		{
			MAGNUM_Attack();
		}
		if(self.weapon == IT_P228)
		{
			P228_Attack();
		}
		if(self.weapon == IT_ELITE)
		{
			ELITE_Attack();
		}
		if(self.weapon == IT_AWP)
		{
			AWP_PrimaryAttack();
		}
		if(self.weapon == IT_HEGRENADE)
		{
			HE_Attack();
		}
	}
}
void() WeaponFrameAll=
{
	if(time < self.attack_finished || self.state > 0)
		return;
	if (!self.button0)
        self.semi = 0;
	if(self.button0)
		WeaponAttack();
	if(self.button1)
		SecondaryAttack();
}
float() GetWeaponId=
{
	if(self.iSlot == PRIMARY)
	{
		if(self.items == self.items | IT_M3)
			return IT_M3;
		else if(self.items == self.items | IT_KF7)
			return IT_KF7;
		else if(self.items == self.items | IT_AWP)
			return IT_AWP;
		else if(self.items == self.items | IT_PHANTOM)
			return IT_PHANTOM;
		else if(self.items == self.items | IT_AUG)
			return IT_AUG;
		else if(self.items == self.items | IT_FAMAS)
			return IT_FAMAS;
		else if(self.items == self.items | IT_AR33)
			return IT_AR33;
	}
	if(self.iSlot == SECONDARY)
	{
		if(self.items == self.items | IT_P99)
			return IT_P99;
		else if(self.items == self.items | IT_DD44)
			return IT_DD44;
		else if(self.items == self.items | IT_MAGNUM)
			return IT_MAGNUM;
		else if(self.items == self.items | IT_P228)
			return IT_P228;
		else if(self.items == self.items | IT_ELITE)
				return IT_ELITE;
	}
	if(self.iSlot == GRENADES)
	{
		if(self.items == self.items | IT_HEGRENADE)
			return IT_HEGRENADE;
	}

	return 0; //just shut up compiler warning
}
void() ChangeWeapon =
{
	if(time < self.attack_finished || self.state > 0)
		return;
	float skipwep;
	self.think = SUB_Null;
	if(self.scope)
	{
		stuffcmd(self,"fov 90\n");
		stuffcmd(self,"scope 0\n");
		stuffcmd(self,"sensitivity 5\n");
		self.fov = 90;
		self.scope = 0;
	}
	
	while(1)
	{
		skipwep = 0;
		if(self.weapon == IT_KNIFE)
		{
			self.weapon = IT_P99;
			if(self.ammo_p99 < 1 && self.p99clip < 1)
				skipwep = 1;
		}
		else if (self.weapon == IT_P99 )
		{
			self.weapon = IT_DD44;
			if(self.ammo_dd44 < 1 && self.dd44clip < 1)
				skipwep = 1;
		}
		else if (self.weapon == IT_DD44 )
		{
			self.weapon = IT_KF7;
			if(self.ammo_kf7 < 1 && self.kf7clip < 1)
				skipwep = 1;
		}
		else if (self.weapon == IT_KF7 )
		{
			self.weapon = IT_MAGNUM;
			if(self.ammo_magnum < 1 && self.magnumclip < 1)
				skipwep = 1;
		}
		else if (self.weapon == IT_MAGNUM )
		{
			self.weapon = IT_PHANTOM;
			if(self.ammo_phantom < 1 && self.phantomclip < 1)
				skipwep = 1;
		}
		else if (self.weapon == IT_PHANTOM )
		{
			self.weapon = IT_AR33;
			if(self.ammo_ar33 < 1 && self.ar33clip < 1)
				skipwep = 1;	
		}
		else if (self.weapon == IT_AR33 )
		{
			self.weapon = IT_KNIFE;
		}
		if(self.weapon && skipwep == 0)
		{
			UpdateWeapon();
			return;
		}
		/*
		if(self.iSlot == PRIMARY)
		{
			self.iSlot = SECONDARY;
			self.weapon = GetWeaponId();
		}
		else if(self.iSlot == SECONDARY)
		{
			self.iSlot = KNIFE;
			self.weapon = IT_KNIFE;
		}
		else if(self.iSlot == KNIFE)
		{
			self.iSlot = GRENADES;
			self.weapon = GetWeaponId();
		}
		else if(self.iSlot == GRENADES)
		{
			self.iSlot = PRIMARY;
			self.weapon = GetWeaponId();
		}
		if((self.items & self.weapon))
		{
			UpdateWeapon();
			return;
		}
		*/
	}
}
