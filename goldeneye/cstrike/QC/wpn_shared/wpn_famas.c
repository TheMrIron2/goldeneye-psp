void()anim_famas_attack=
{
	if(self.weaponframe == 30)
	{
		self.state = 0;
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_famas_attack;
	self.nextthink = time + 0.01;
}
void()anim_burst_famas_attack=
{
	if(self.weaponframe == 30)
	{
		self.state = 0;
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}		
	if(self.weaponframe == 1)	
	{
		if(self.famasclip >= 3)
		{
			self.famasclip -=1;
			self.famas_fired +=1;
			DefaultFire(1,29,string_null);
		}
	}
	if(self.weaponframe == 2)
	{
		if(self.famasclip >= 2)
		{
			self.famasclip -=1;
			self.famas_fired +=1;
			DefaultFire(1,29,string_null);
		}
	}
	if(self.weaponframe == 3)
	{
		if(self.famasclip >= 1)
		{
			self.famasclip -=1;
			self.famas_fired +=1;
			DefaultFire(1,29,string_null);
		}
	}
	self.weaponframe += 1;
	self.think = anim_burst_famas_attack;
	self.nextthink = time + 0.02;
}
void()Famas_Reload=
{
	if(self.weaponframe == 15)
		sound (self, CHAN_AUTO, "weapons/famas_clipout.wav", 1, ATTN_NORM);	
	if(self.weaponframe == 47)
		sound (self, CHAN_AUTO, "weapons/famas_clipin.wav", 1, ATTN_NORM);	
	if(self.weaponframe == 65)
		sound (self, CHAN_AUTO, "weapons/famas_boltslap.wav", 1, ATTN_NORM);	
	if(self.weaponframe == 90)
	{
		self.state = 0;
		UpdateWeapon();
		self.wepsequence = 0;
		self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = Famas_Reload;
	self.nextthink = time + 0.035;
}

void()Famas_PrimaryAttack=
{
	if(self.famasburst == 0)
	{
		if(self.famasclip <= 0)
		{
			ReloadFamas(0); 
			return;
		}
		self.state = ATTACK;
		self.wepsequence = 3;
		self.weaponframe = 0;
		self.famasclip -= 1;
		self.famas_fired += 1;
		anim_famas_attack();
		DefaultFire(1,29,"weapons/famas-1.wav");
		self.attack_finished = time + 0.0825;	
	}
	if(self.famasburst == 1)
	{
		if(self.famasclip <= 0)
		{
			ReloadFamas(2); 
			return;
		}
		self.state = ATTACK;
		sound (self, CHAN_AUTO, "weapons/famas-burst.wav", 1, ATTN_NORM);	
		self.wepsequence = 3;
		self.weaponframe = 0;
		anim_burst_famas_attack();
		self.attack_finished = time + 0.3825;	
	}
}