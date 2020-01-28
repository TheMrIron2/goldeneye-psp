
void()anim_knife_attack=
{
	if(self.weaponframe == 90)
	{
		self.state = 0;
		//self.weaponframe = 0;
		return;
	}
	self.weaponframe += 1;
	self.think = anim_knife_attack;
	self.nextthink = time + 0.001;
}
void KnifeAttack(float iDamage,float iDistance)
{
	local vector org, end;
	local float r;
	makevectors(self.v_angle);
	org = self.origin + self.view_ofs;
	end = org + v_forward * iDistance;
	traceline(org,end,0,self);
	sound (self, CHAN_AUTO, "weapons/knife_slash1.wav", 1, ATTN_NORM);
	if(trace_fraction ==1)
		return;
	r = floor(random()*4);
	if(trace_ent.takedamage)
	{
		switch(r)
		{
		case 0:
			sound (self, CHAN_AUTO, "weapons/knife_hit1.wav", 1, ATTN_NORM);
			break;
		case 1:
			sound (self, CHAN_AUTO, "weapons/knife_hit2.wav", 1, ATTN_NORM);
			break;
		case 2:
			sound (self, CHAN_AUTO, "weapons/knife_hit3.wav", 1, ATTN_NORM);
			break;
		case 3:
			sound (self, CHAN_AUTO, "weapons/knife_hit4.wav", 1, ATTN_NORM);
			break;
		}
		T_Damage (trace_ent, self, self, iDamage);
	}
	else
		sound (self, CHAN_AUTO, "weapons/knife_hitwall1.wav", 1, ATTN_NORM);
}

void()KNIFE_PrimaryAttack=
{
		self.attack_finished = time + 0.1;
		self.weaponframe = 0;
		self.state = ATTACK;
		self.wepsequence = 3;
		anim_knife_attack();
		KnifeAttack(40,48);
}