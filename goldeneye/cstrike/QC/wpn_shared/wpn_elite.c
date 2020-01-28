void()anim_elite_attack=
{
	if(self.weaponframe == 40)
	{
			self.state = 0;
			self.wepsequence = 0;
			self.weaponframe = 0;
			return;
	}
	self.weaponframe += 1;
	self.think = anim_elite_attack;
	self.nextthink = time + 0.025;
}
void() ELITE_Reload=
{
	if(self.weaponframe == 137)
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}
	if(self.weaponframe == 6)
		sound (self, CHAN_AUTO, "weapons/elite_reloadstart.wav", 1, ATTN_NORM);
	if(self.weaponframe == 44)
		sound (self, CHAN_AUTO, "weapons/elite_leftclipin.wav", 1, ATTN_NORM);
	if(self.weaponframe == 73)
		sound (self, CHAN_AUTO, "weapons/elite_clipout.wav", 1, ATTN_NORM);
	if(self.weaponframe == 82)
		sound (self, CHAN_AUTO, "weapons/elite_sliderelease.wav", 1, ATTN_NORM);
	if(self.weaponframe == 112)
		sound (self, CHAN_AUTO, "weapons/elite_rightclipin.wav", 1, ATTN_NORM);
	if(self.weaponframe == 125)
		sound (self, CHAN_AUTO, "weapons/elite_sliderelease.wav", 1, ATTN_NORM);
	self.weaponframe += 1;
	self.think = ELITE_Reload;
	self.nextthink = time + 0.03;
}
void()ELITE_Attack=
{
	if(self.eliteclip == 0)
	{
		ReloadWeaponELITE(0);
		return;
	}
	self.attack_finished = time + 0.15;
	if(self.eliteclip % 2 == 0)
	{
		self.wepsequence = 2;
	}
	else
	{
		self.wepsequence = 8;
	}
	self.weaponframe = 0;
	anim_elite_attack();
	DefaultFire(1, 32, "weapons/elite_fire.wav");
	self.eliteclip -=1;
	self.elite_fired +=1;
}
