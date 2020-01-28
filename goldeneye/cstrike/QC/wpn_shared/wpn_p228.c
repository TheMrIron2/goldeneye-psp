void()anim_p228_attack=
{
	if(self.weaponframe == 25)
	{
			self.state = 0;
			self.wepsequence = 0;
			self.weaponframe = 0;
			return;
	}
	self.weaponframe += 1;
	self.think = anim_p228_attack;
	self.nextthink = time + 0.025;
}
void() P228_Reload=
{
	if(self.weaponframe == 95) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}	
	if(self.weaponframe == 25)
		sound (self, CHAN_AUTO, "weapons/p228_clipout.wav", 1, ATTN_NORM);
	if(self.weaponframe == 47)
		sound (self, CHAN_AUTO, "weapons/p228_clipin.wav", 1, ATTN_NORM);
	if(self.weaponframe == 80)
		sound (self, CHAN_AUTO, "weapons/p228_slidepull.wav", 1, ATTN_NORM);	
	self.weaponframe += 1;
	self.think = P228_Reload;
	self.nextthink = time + 0.03;
}
void()P228_Attack=
{
	if(self.p228clip == 0)
	{
		ReloadWeaponP228(0);
		return;
	}
	self.attack_finished = time + 0.15;
	self.wepsequence = 2;
	self.weaponframe = 0;
	anim_p228_attack();
	DefaultFire(1, 32, "weapons/p228-1.wav");
	self.p228clip -=1;
	self.p228_fired +=1;
}