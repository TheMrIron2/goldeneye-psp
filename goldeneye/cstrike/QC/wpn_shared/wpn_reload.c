void(float r_frame)ReloadWeaponP99=
{
	if(self.ammo_p99 == 0)
		return;
	if(self.p99clip == 7)
		return;
	self.state = RELOADING;
	if(self.p99_fired >= self.ammo_p99)
	{
		self.p99clip += self.ammo_p99;
		self.ammo_p99 = 0;
		UpdateWeapon();
		self.p99_fired = 0;
		self.wepsequence = 5;
		self.weaponframe = r_frame;
		P99_Reload();
		return;
	}
	self.ammo_p99 -= self.p99_fired;
	self.p99clip += self.p99_fired;
	UpdateWeapon();
	self.p99_fired = 0;
	self.wepsequence = 5;
	self.weaponframe = r_frame;
	P99_Reload();
}

void(float r_frame)ReloadWeaponDD44=
{
	if(self.ammo_dd44 == 0)
		return;
	if(self.dd44clip == 8)
		return;
	self.state = RELOADING;
	if(self.dd44_fired >= self.ammo_dd44)
	{
		self.dd44clip += self.ammo_dd44;
		self.ammo_dd44 = 0;
		UpdateWeapon();
		self.dd44_fired = 0;
		self.wepsequence = 4;
		self.weaponframe = r_frame;
		DD44_Reload();
		return;
	}
	self.ammo_dd44 -= self.dd44_fired;
	self.dd44clip += self.dd44_fired;
	UpdateWeapon();
	self.dd44_fired = 0;
	self.wepsequence = 4;
	self.weaponframe = r_frame;
	DD44_Reload();
}

//shit omg
float m3a;
void(float r_frame)ReloadWeaponM3=
{
	if(self.ammo_m3 == 0)
		return;
	if(self.m3clip == 7)
		return;
	self.state = RELOADING;
	if(self.m3_fired >= self.ammo_m3)
	{
		m3a = self.m3clip + self.ammo_m3;
		self.ammo_m3 = 0;
		UpdateWeapon();
		self.m3_fired = 0;
		self.wepsequence = 5;
		self.weaponframe = r_frame;
		M3_Reload_Start();
		return;
	}
	self.ammo_m3 -= self.m3_fired;
	UpdateWeapon();
	self.m3_fired = 0;
	self.wepsequence = 5;
	self.weaponframe = r_frame;
	M3_Reload_Start();
}

void(float r_frame)ReloadWeaponMagnum=
{
	if(self.ammo_magnum == 0)
		return;
	if(self.magnumclip == 6)
		return;
	self.state = RELOADING;
	if(self.magnum_fired >= self.ammo_magnum)
	{
		self.magnumclip += self.ammo_magnum;
		self.ammo_magnum = 0;
		UpdateWeapon();
		self.magnum_fired = 0;
		self.wepsequence = 3;
		self.weaponframe = r_frame;
		Magnum_Reload();
		return;
	}
	self.ammo_magnum -= self.magnum_fired;
	self.magnumclip += self.magnum_fired;
	UpdateWeapon();
	self.magnum_fired = 0;
	self.wepsequence = 3;
	self.weaponframe = r_frame;
	Magnum_Reload();
}

void(float r_frame)ReloadWeaponP228=
{
	if(self.ammo_p228 == 0)
		return;
	if(self.p228clip == 13)
		return;
	self.state = RELOADING;
	if(self.p228_fired >= self.ammo_p228)
	{
		self.p228clip += self.ammo_p228;
		self.ammo_p228 = 0;
		UpdateWeapon();
		self.p228_fired = 0;
		self.wepsequence = 5;
		self.weaponframe = r_frame;
		P228_Reload();
		return;
	}
	self.ammo_p228 -= self.p228_fired;
	self.p228clip += self.p228_fired;
	UpdateWeapon();
	self.p228_fired = 0;
	self.wepsequence = 5;
	self.weaponframe = r_frame;
	P228_Reload();
}

void(float r_frame)ReloadWeaponELITE=
{
	if(self.ammo_elite == 0)
		return;
	if(self.eliteclip == 30)
		return;
	self.state = RELOADING;
	if(self.elite_fired >= self.ammo_elite) 
	{
		self.eliteclip += self.ammo_elite;
		self.ammo_elite = 0;
		UpdateWeapon();
		self.elite_fired = 0;
		self.wepsequence = 14;
		self.weaponframe = r_frame;
		ELITE_Reload();
		return;
	}
	self.ammo_elite -= self.elite_fired;
	self.eliteclip += self.elite_fired;
	UpdateWeapon();
	self.elite_fired = 0;
	self.wepsequence = 14;
	self.weaponframe = r_frame;
	ELITE_Reload();
}

void(float r_frame)ReloadKF7=
{
	if(self.ammo_kf7 == 0)
		return;
	if(self.kf7clip == 30)
		return;
	self.state = RELOADING;
	if(self.kf7_fired >= self.ammo_kf7)
	{
		self.kf7clip += self.ammo_kf7;
		self.ammo_kf7 = 0;
		UpdateWeapon();
		self.kf7_fired = 0;
		self.wepsequence = 5;
		self.weaponframe = r_frame;
		KF7_Reload();
		return;
	}
	self.ammo_kf7 -= self.kf7_fired;
	self.kf7clip += self.kf7_fired;
	UpdateWeapon();
	self.kf7_fired = 0;
	self.wepsequence = 5;
	self.weaponframe = r_frame;
	KF7_Reload();
}

void(float r_frame)ReloadAWP=
{
	if(self.ammo_awp == 0)
		return;
	if(self.awpclip == 10)
		return;
	self.state = RELOADING;
	if(self.awp_fired >= self.ammo_awp)
	{
		self.awpclip += self.ammo_awp;
		self.ammo_awp = 0;
		UpdateWeapon();
		self.awp_fired = 0;
		self.wepsequence = 4;
		self.weaponframe = r_frame;
		AWP_Reload();
		return;
	}
	self.ammo_awp -= self.awp_fired;
	self.awpclip += self.awp_fired;
	UpdateWeapon();
	self.awp_fired = 0;
	self.wepsequence = 4;
	self.weaponframe = r_frame;
	AWP_Reload();
}

void(float r_frame)ReloadPhantom=
{
	if(self.ammo_phantom == 0)
		return;
	if(self.phantomclip == 50)
		return;
	self.state = RELOADING;
	if(self.phantom_fired >= self.ammo_phantom)
	{
		self.phantomclip += self.ammo_phantom;
		self.ammo_phantom = 0;
		UpdateWeapon();
		self.phantom_fired = 0;
		self.wepsequence = 4;
		self.weaponframe = r_frame;
		Phantom_Reload();
		return;
	}
	self.ammo_phantom -= self.phantom_fired;
	self.phantomclip += self.phantom_fired;
	UpdateWeapon();
	self.phantom_fired = 0;
	self.wepsequence = 4;
	self.weaponframe = r_frame;
	Phantom_Reload();
}

void(float r_frame)ReloadAug=
{
	if(self.ammo_aug == 0)
		return;
	if(self.augclip == 30)
		return;
	self.state = RELOADING;
	if(self.aug_fired >= self.ammo_aug)
	{
		self.augclip += self.ammo_aug;
		self.ammo_aug = 0;
		UpdateWeapon();
		self.aug_fired = 0;
		self.wepsequence = 1;
		self.weaponframe = r_frame;
		Aug_Reload();
		return;
	}
	self.ammo_aug -= self.aug_fired;
	self.augclip += self.aug_fired;
	UpdateWeapon();
	self.aug_fired = 0;
	self.wepsequence = 1;
	self.weaponframe = r_frame;
	Aug_Reload();
}

void(float r_frame)ReloadFamas=
{
	if(self.ammo_famas == 0)
		return;
	if(self.famasclip == 25)
		return;
	self.state = RELOADING;
	if(self.famas_fired >= self.ammo_famas)
	{
		self.famasclip += self.ammo_famas;
		self.ammo_famas = 0;
		UpdateWeapon();
		self.famas_fired = 0;
		self.wepsequence = 1;
		self.weaponframe = r_frame;
		Famas_Reload();
		return;
	}
	self.ammo_famas -= self.famas_fired;
	self.famasclip += self.famas_fired;
	UpdateWeapon();
	self.famas_fired = 0;
	self.wepsequence = 1;
	self.weaponframe = r_frame;
	Famas_Reload();
}

void(float r_frame)ReloadAR33=
{
	if(self.ammo_ar33 == 0)
		return;
	if(self.ar33clip == 30)
		return;
	self.state = RELOADING;
	if(self.ar33_fired >= self.ammo_ar33)
	{
		self.ar33clip += self.ammo_ar33;
		self.ammo_ar33 = 0;
		UpdateWeapon();
		self.ar33_fired = 0;
		self.wepsequence = 4;
		self.weaponframe = r_frame;
		AR33_Reload();
		return;
	}
	self.ammo_ar33 -= self.ar33_fired;
	self.ar33clip += self.ar33_fired;
	UpdateWeapon();
	self.ar33_fired = 0;
	self.wepsequence = 4;
	self.weaponframe = r_frame;
	AR33_Reload();
}
