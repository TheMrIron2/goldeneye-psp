void(float cShots,vector vecSrc,vector vecDirShooting,vector vecSpread,float flDistance,float iDamage)FireBullets;
void()anim_m3_attack=
{
	if(self.weaponframe == 37)
	{
		self.wepsequence = 0;	
		self.weaponframe = 0;
		self.state = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_m3_attack;
	self.nextthink = time + 0.03;
}
void() M3_Reload_End=
{
	if(self.weaponframe == 14)
		sound (self, CHAN_AUTO, "weapons/m3_pump.wav", 1, ATTN_NORM);
	if(self.weaponframe == 33) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = M3_Reload_End;
	self.nextthink = time + 0.03;
}
void() M3_Reload=
{
	if(self.ammo_m3 == 0)
	{	
		if(self.m3clip == m3a)
		{
			self.wepsequence = 4;
			self.weaponframe = 0;
			M3_Reload_End();
			return;
		}
	}
	if(self.m3clip == 7)
	{
		self.wepsequence = 4;
		self.weaponframe = 0;
		M3_Reload_End();
		return;
	}
	if(self.weaponframe == 12) 
		sound (self, CHAN_AUTO, "weapons/m3_insertshell.wav", 1, ATTN_NORM);
	if(self.weaponframe == 27) 
	{
		self.weaponframe = 0;
		self.m3clip += 1;
	}		
	self.weaponframe += 1;
	self.think = M3_Reload;
	self.nextthink = time + 0.02;
}
void() M3_Reload_Start=
{
	if(self.weaponframe == 15) 
	{
		self.wepsequence = 3;
		self.weaponframe = 0;
		M3_Reload();
		return;
	}		
	self.weaponframe += 1;
	self.think = M3_Reload_Start;
	self.nextthink = time + 0.02;	
}
void()M3_PrimaryAttack=
{
	local vector vecSrc,vecAcc,vecAim;
	if(self.m3clip == 0)
	{
		ReloadWeaponM3(0); 
		return;
	}
	sound (self, CHAN_AUTO, "weapons/m3-1.wav", 1, ATTN_NORM);
	self.attack_finished = time + 0.875;
	vecSrc = self.origin + self.view_ofs;
	vecAcc = '0.08716 0.04362 0.00';
	vecAim = GetAutoaimVector(AUTOAIM_2DEGREES);
	FireBullets(9,vecSrc,vecAim,vecAcc,8192,20);
	self.punchangle_x -= 5;
	self.wepsequence = 2;
	self.weaponframe = 0;
	anim_m3_attack();
	self.m3clip -=1;
	self.currentammo -= 1;
	self.m3_fired += 1;
}