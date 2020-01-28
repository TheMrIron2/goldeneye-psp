void()ChangeWeapon;
float()GetWeaponId;
void() CheckImpulses =
{

	if (self.impulse == 121)
		ChangeWeapon ();
	if(self.impulse == 20)
	{
		if(self.money < GLOCK18_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,GLOCK18_PRICE,BUY);
		bprint("Bought glock \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = SECONDARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_DD44;
		self.weapon = IT_DD44;
		self.dd44clip = 8;
		self.ammo_dd44 = 16;
		self.dd44_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 21)
	{
		if(self.money < USP_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,USP_PRICE,BUY);
		bprint("Bought usp \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = SECONDARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_P99;
		self.weapon = IT_P99;
		self.p99clip = 7;
		self.ammo_p99 = 21;
		self.p99_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 22)
	{
		if(self.money < DEAGLE_PRICE)
		{
			sprint(self, "Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,DEAGLE_PRICE,BUY);
		bprint("Bought Deagle \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = SECONDARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_MAGNUM;
		self.weapon = IT_MAGNUM;
		self.magnumclip = 6;
		self.ammo_magnum = 30;
		self.magnum_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 23)
	{
		if(self.money < P228_PRICE)
		{
			sprint(self, "Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,P228_PRICE,BUY);
		bprint("Bought P228 \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = SECONDARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_P228;
		self.weapon = IT_P228;
		self.p228clip = 13;
		self.ammo_p228 = 52;
		self.p228_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 25)
	{
		if(self.money < ELITE_PRICE)
		{
			sprint(self, "Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,ELITE_PRICE,BUY);
		bprint("Bought Elite \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = SECONDARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_ELITE;
		self.weapon = IT_ELITE;
		self.eliteclip = 30;
		self.ammo_elite = 120;
		self.elite_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 41)
	{
		if(self.he_grenades == 1)
		{
			goto stop;
			return;
		}
		if(self.money < HEGRENADE_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,HEGRENADE_PRICE,BUY);
		bprint("Bought HE \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.items = self.items | IT_HEGRENADE;
		self.weapon = IT_HEGRENADE;
		self.he_grenades += 1;
		UpdateWeapon();
	}
	if(self.impulse == 62)
	{
		if(self.money < M3_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,M3_PRICE,BUY);
		bprint("Bought M3 \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = PRIMARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_M3;
		self.weapon = IT_M3;
		self.ammo_m3 = 32;
		self.m3clip = 7;
		self.m3_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 43)
	{
		if(self.money < AK47_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,AK47_PRICE,BUY);
		bprint("Bought AK-47 \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = PRIMARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_KF7;
		self.weapon = IT_KF7;
		self.kf7clip = 30;
		self.ammo_kf7 = 90;
		self.kf7_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 46)
	{
		if(self.money < AWP_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,AWP_PRICE,BUY);
		bprint("Bought AWP \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = PRIMARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_AWP;
		self.weapon = IT_AWP;
		self.awpclip = 10;
		self.ammo_awp = 30;
		//self.crosshair = cvar("crosshair");
		UpdateWeapon();
	}
	if(self.impulse == 89)
	{
		if(self.money < GALIL_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,GALIL_PRICE,BUY);
		bprint("Bought Galil \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = PRIMARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_PHANTOM;
		self.weapon = IT_PHANTOM;
		self.phantomclip = 50;
		self.ammo_phantom = 100;
		UpdateWeapon();
	}
	if(self.impulse == 82)
	{
		if(self.money < AUG_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,AUG_PRICE,BUY);
		bprint("Bought AUG \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = PRIMARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_AUG;
		self.weapon = IT_AUG;
		self.augclip = 30;
		self.ammo_aug = 90;
		self.awp_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 84)
	{
		if(self.money < FAMAS_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,FAMAS_PRICE,BUY);
		bprint("Bought Famas \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = PRIMARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_FAMAS;
		self.weapon = IT_FAMAS;
		self.famasclip = 25;
		self.ammo_famas = 90;
		self.famas_fired = 0;
		UpdateWeapon();
	}
	if(self.impulse == 45)
	{
		if(self.money < M4A1_PRICE)
		{
			bprint("Not enough money \n");
			goto stop;
			return;
		}
		AddAccount(self,M4A1_PRICE,BUY);
		bprint("Bought M4A1 \n");
		sound (self, CHAN_AUTO, "items/gunpickup2.wav", 1, ATTN_NORM);
		self.iSlot = PRIMARY;
		self.items = self.items - (self.items & GetWeaponId());
		self.items = self.items | IT_AR33;
		self.weapon = IT_AR33;
		self.ar33clip = 30;
		self.ammo_ar33 = 90;
		self.ar33_fired = 0;
		UpdateWeapon();
	}
	if (self.impulse == 94)
	{
		if(self.team == T_SIDE)
			m_iNumTerrorist -= 1;
		if(m_iNumCT == 2)
		{
			bprint("Too many players in team\n");
		}
		else
		{
			bprint (self.netname);
			bprint (" Joining Counter-Terrorist team\n");
			self.team = CT_SIDE;
			self.m_iTeam = CT;
			PutClientCTInServer();
		}
	}
	if (self.impulse == 54)
	{
		if(self.team == CT_SIDE)
			m_iNumCT -= 1;
		if(m_iNumTerrorist == 2)
		{
			bprint("Too many players in team\n");
		}
		else
		{
			bprint (self.netname);
			bprint (" Joining Terrorist team\n");
			self.team = T_SIDE;
			self.m_iTeam = TERRORIST;
			PutClientTInServer();
		}
	}
	if(self.impulse == 105)
	{
		if(self.m_bInBuyZone)
		{
			if(self.team == CT_SIDE)
				stuffcmd(self,"menu_buyct \n");
			else
				stuffcmd(self,"menu_buyt \n");
		}
		else
			centerprint(self,"You are not in the buy zone\n");
	}
	if(self.impulse == 120)
		use_button();
	if(self.impulse == 122)
	{
		if(self.state > 0)
		{
			goto stop;
			return;
		}
		//todo: move this to wep_reload
		if(self.weapon == IT_P99)
		{
			ReloadWeaponP99(0);
		}
		if(self.weapon == IT_DD44)
			ReloadWeaponDD44(0);
		if(self.weapon == IT_M3)
			ReloadWeaponM3(0);
		if(self.weapon == IT_MAGNUM)
			ReloadWeaponMagnum(0);
		if(self.weapon == IT_KF7)
			ReloadKF7(0);
		if(self.weapon == IT_AWP)
			ReloadAWP(0);
		if(self.weapon == IT_PHANTOM)
			ReloadPhantom(0);
		if(self.weapon == IT_AUG)
			ReloadAug(0);
		if(self.weapon == IT_FAMAS)
			ReloadFamas(0);
		if(self.weapon == IT_AR33)
			ReloadAR33(0);
	}

	if(self.impulse == 125)
	{
		if (time < m_flNextDecalTime)
		{
			goto stop;
			return;
		}
		m_flNextDecalTime = time + 30;
		CSprayCan();
	}

	stop: self.impulse = 0;                              // Clear impulse list.
};
