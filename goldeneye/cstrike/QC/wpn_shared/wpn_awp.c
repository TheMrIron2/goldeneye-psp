void()awp_attack_anim=
{
	if(self.weaponframe == 15) 
		sound (self, CHAN_AUTO, "weapons/boltup.wav", 1, ATTN_NORM);
	if(self.weaponframe == 19) 
		sound (self, CHAN_AUTO, "weapons/boltpull1.wav", 1, ATTN_NORM);
	if(self.weaponframe == 32) 
		sound (self, CHAN_AUTO, "weapons/boltdown.wav", 1, ATTN_NORM);
	if(self.weaponframe == 41) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		self.state = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = awp_attack_anim;
	self.nextthink = time + 0.04;
}
void()AWP_Reload=
{
	if(self.weaponframe == 26)
		sound (self, CHAN_AUTO, "weapons/awp_clipout.wav", 1, ATTN_NORM);
	if(self.weaponframe == 53)
		sound (self, CHAN_AUTO, "weapons/awp_clipin.wav", 1, ATTN_NORM);
	if(self.weaponframe == 87) 
	{
		self.wepsequence = 0;
		self.weaponframe = 0;
		UpdateWeapon();
		self.state = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = AWP_Reload;
	self.nextthink = time + 0.04;
}
void()AWP_PrimaryAttack=
{
	local vector vecSrc = GetGunPosition();
	local vector vecAim = GetAutoaimVector(AUTOAIM_2DEGREES);
	local vector vecAcc = '0 0 0';
	if(self.awpclip == 0)
	{
		stuffcmd(self,"fov 90\n");
		stuffcmd(self,"scope 0\n");
		stuffcmd(self,"sensitivity 5\n");
		self.fov = 90;
		self.scope = 0;
		ReloadAWP(0); 
		return;
	}
	if(!self.flags && FL_ONGROUND)
		vecAcc = '0.85 0.85 0.85';
	else if((vlen(self.velocity) > 140) || !self.scope)
		vecAcc = '0.25 0.25 0.25';
	else if((vlen(self.velocity) > 10))
		vecAcc = '0.10 0.10 0.10';
	else 
		vecAcc = '0 0 0';
	self.state = ATTACK;
	self.awpclip -= 1;
	self.currentammo -= 1;
	self.awp_fired += 1;
	FireBullets(1,vecSrc,vecAim,vecAcc,8192,114);
	stuffcmd(self,"fov 90\n");
	stuffcmd(self,"scope 0\n");
	stuffcmd(self,"sensitivity 5\n");
	self.fov = 90;
	self.scope = 0;
	self.attack_finished = time + 1.45;
	self.wepsequence = 1;
	self.weaponframe = 0;
	sound (self, CHAN_AUTO, "weapons/awp1.wav", 1, ATTN_NORM);
	awp_attack_anim();
}