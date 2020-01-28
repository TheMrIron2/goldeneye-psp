void()UpdateWeapon;
void()anim_p99_attack=
{
	if(self.weaponframe == 45)
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.state = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_p99_attack;
	self.nextthink = time + 0.01;
}
void() P99_Reload=
{
	if(self.weaponframe == 99)
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}
	if(self.weaponframe  == 82)
		sound (self, CHAN_AUTO, "weapons/usp_sliderelease.wav", 1, ATTN_NORM);
	if(self.weaponframe  == 17)
		sound (self, CHAN_AUTO, "weapons/usp_clipout.wav", 1, ATTN_NORM);	
	if(self.weaponframe  == 40)
		sound (self, CHAN_AUTO, "weapons/usp_clipin.wav", 1, ATTN_NORM);		
	self.weaponframe += 1;
	self.think = P99_Reload;
	self.nextthink = time + 0.015;
} 

void()P99_Attack=
{
	if(self.p99clip == 0)
	{
		ReloadWeaponP99(0);
		return;
	}
	self.attack_finished = time + 0.15;
	self.wepsequence = 2;
	self.weaponframe = 1;
	anim_p99_attack();
	DefaultFire(1, 30, "weapons/usp1.wav");

	self.p99clip -=1;
	self.p99_fired +=1;
}