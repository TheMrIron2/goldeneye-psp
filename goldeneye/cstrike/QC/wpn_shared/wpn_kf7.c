void()anim_kf7_attack=
{
	if(self.weaponframe == 29)
	{
		self.state = 0;
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_kf7_attack;
	self.nextthink = time + 0.01;
}
void()KF7_PrimaryAttack=
{
	if(self.kf7clip <= 0)
	{
		ReloadKF7(0); 
		return;
	}
	self.wepsequence = 1;
	self.weaponframe = 0;
	self.kf7clip -= 1;
	self.kf7_fired += 1;
	anim_kf7_attack();
	DefaultFire(1,20,"weapons/ak47-1.wav");
	self.attack_finished = time + 0.1;
}

void()KF7_Reload=
{
	if(self.weaponframe == 72) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}	
	if(self.weaponframe == 13)
		sound (self, CHAN_AUTO, "weapons/ak47_clipout.wav", 1, ATTN_NORM);
	if(self.weaponframe == 57)	
		sound (self, CHAN_AUTO, "weapons/ak47_clipin.wav", 1, ATTN_NORM);
	self.weaponframe += 1;
	self.think = KF7_Reload;
	self.nextthink = time + 0.015;
}
