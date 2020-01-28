void()anim_magnum_attack=
{
	if(self.weaponframe == 79)
	{
		self.state = 0;
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_magnum_attack;
	self.nextthink = time + 0.02;
}
void() Magnum_Reload=
{
	if(self.weaponframe == 79) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}	
	if(self.weaponframe == 14)
		sound (self, CHAN_AUTO, "weapons/de_clipout.wav", 1, ATTN_NORM);
	if(self.weaponframe == 34)
		sound (self, CHAN_AUTO, "weapons/de_clipin.wav", 1, ATTN_NORM);
	self.weaponframe += 1;
	self.think = Magnum_Reload;
	self.nextthink = time + 0.03;
}
void()MAGNUM_Attack=
{
	if(self.magnumclip == 0)
	{
		ReloadWeaponMagnum(0);
		return;
	}
	self.attack_finished = time + 1;
	self.wepsequence = 2;
	self.weaponframe = 0;
	anim_magnum_attack();
	DefaultFire(1, 47, "weapons/deagle-1.wav");
	self.magnumclip -=1;
	self.magnum_fired +=1;
}