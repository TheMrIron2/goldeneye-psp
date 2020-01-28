void()anim_phantom_attack=
{
	if(self.weaponframe == 40)
	{
		self.state = 0;
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_phantom_attack;
	self.nextthink = time + 0.03;
}
void()Phantom_Reload=
{
	if(self.weaponframe == 15)
		sound (self, CHAN_AUTO, "weapons/galil_clipout.wav", 1, ATTN_NORM);	
	if(self.weaponframe == 47)
		sound (self, CHAN_AUTO, "weapons/galil_clipin.wav", 1, ATTN_NORM);	
	if(self.weaponframe == 65)
		sound (self, CHAN_AUTO, "weapons/galil_boltpull.wav", 1, ATTN_NORM);	
	if(self.weaponframe == 33)
	{
		self.state = 0;
		UpdateWeapon();
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = Phantom_Reload;
	self.nextthink = time + 0.04;
}

void()Phantom_PrimaryAttack=
{
	if(self.phantomclip <= 0)
	{
		ReloadPhantom(0); 
		return;
	}
	self.wepsequence = 1;
	self.weaponframe = 0;
	self.phantomclip -= 1;
	self.phantom_fired += 1;
	anim_phantom_attack();
	DefaultFire(1,29,"weapons/galil-1.wav");
	self.attack_finished = time + 0.1;	
}