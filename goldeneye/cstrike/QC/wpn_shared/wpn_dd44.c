
void()anim_dd44_attack=
{
	if(self.weaponframe == 29)
		{
			self.wepsequence = 0;
			self.weaponframe = 0;
			self.state = 0;
			return;
		}
	self.weaponframe += 1;
	self.think = anim_dd44_attack;
	self.nextthink = time + 0.01;
}
void() DD44_Reload=
{
	if(self.weaponframe == 91) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}

	if(self.weaponframe  == 16)
		sound (self, CHAN_AUTO, "weapons/clipout1.wav", 1, ATTN_NORM);	
	if(self.weaponframe  == 48)
		sound (self, CHAN_AUTO, "weapons/clipin1.wav", 1, ATTN_NORM);		
	if(self.weaponframe  == 62)
		sound (self, CHAN_AUTO, "weapons/sliderelease1.wav", 1, ATTN_NORM);			
	self.weaponframe += 1;
	self.think = DD44_Reload;
	self.nextthink = time + 0.02;
}
void()DD44_Attack=
{
	if(self.dd44clip == 0)
	{
		ReloadWeaponDD44(0); 
		return;
	}
	self.attack_finished = time + 0.3;
	DefaultFire(1,21, "weapons/glock18-2.wav" );
	self.wepsequence = 1;
	self.weaponframe = 0;
	anim_dd44_attack();
	self.dd44clip -=1;
	self.dd44_fired +=1;	
}