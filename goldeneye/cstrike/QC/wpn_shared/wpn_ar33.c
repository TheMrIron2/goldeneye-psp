void()anim_ar33_attack=
{
	if(self.weaponframe == 69)
	{
		self.state = 0;
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_ar33_attack;
	self.nextthink = time + 0.02;
}
void()AR33_PrimaryAttack=
{
	if(self.ar33clip <= 0)
	{
		ReloadAR33(0); 
		return;
	}

	self.ar33clip -= 1;
	self.ar33_fired +=1;

	self.wepsequence = 2;
	self.weaponframe = 0;
	DefaultFire(1,31,"weapons/m4a1_unsil-1.wav");
	anim_ar33_attack();
	self.attack_finished = time + 0.0875;
}

void()AR33_Reload=
{
	if(self.weaponframe == 74) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}	
	if(self.weaponframe == 25)
		sound (self, CHAN_AUTO, "weapons/m4a1_clipout.wav", 1, ATTN_NORM);
	if(self.weaponframe == 53)	
		sound (self, CHAN_AUTO, "weapons/m4a1_clipin.wav", 1, ATTN_NORM);
	if(self.weaponframe == 88)	
		sound (self, CHAN_AUTO, "weapons/m4a1_boltpull.wav", 1, ATTN_NORM);
	self.weaponframe += 1;
	self.think = AR33_Reload;
	self.nextthink = time + 0.03;	
}