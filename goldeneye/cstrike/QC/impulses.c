void()ChangeWeapon;
float()GetWeaponId;
void() CheckImpulses =
{
	if (self.impulse == 121)
		ChangeWeapon ();
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
			bprint (" Joining Spy team\n");
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
			bprint (" Joining Guards team\n");
			self.team = T_SIDE;
			self.m_iTeam = TERRORIST;
			PutClientTInServer();
		}
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
