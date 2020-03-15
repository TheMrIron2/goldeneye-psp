/*
============
PlaceItem

plants the object on the floor
============
*/

void() PlaceItem =
{
	self.flags = FL_ITEM;		// make extra wide
	self.solid = SOLID_TRIGGER;
	self.movetype = MOVETYPE_TOSS;	
	self.velocity = '0 0 0';
	self.origin_z = self.origin_z + 10;
	if (!droptofloor())
	{
		bprint ("Item fell out of level at ");
		bprint (vtos(self.origin));
		bprint ("\n");
		remove(self);
		return;
	}
};

/*
============
StartItem

Sets the clipping size and plants the object on the floor
============
*/
void() StartItem =
{
	self.nextthink = time + 0.2;	// items start after other solids
	self.think = PlaceItem;
};
void() RespawnItem =
{
	self.model = self.oldmodel;          
	self.solid = SOLID_TRIGGER;     
	setorigin (self, self.origin);
};
void()item_touch=
{
	entity oldself,oldother;
	if(other.classname != "player")
		return;
	if(other.state > 0 || time < other.attack_finished)
		return;
	self.solid = SOLID_NOT;	
	self.oldmodel = self.model;
	self.model = string_null;
	oldself = self;
	if(self.netname == "weapon_dd44")
	{
		if(other.items != other.items | IT_DD44)
			other.dd44clip = 8;
		other.ammo_dd44 += 16;
		other.weapon = IT_DD44;
		self = other;
		UpdateWeapon();
		self = oldself;
	}
	if(self.netname == "weapon_kf7")
	{
		if(other.items != other.items | IT_KF7)
			other.kf7clip = 30;
		other.ammo_kf7 += 60;
		other.weapon = IT_KF7;
		self = other;
		UpdateWeapon();
		self = oldself;
	}
	if(self.netname == "weapon_cougar")
	{
		if(other.items != other.items | IT_MAGNUM)
			other.magnumclip = 6;
		other.ammo_magnum += 30;
		other.weapon = IT_MAGNUM;
		self = other;
		UpdateWeapon();
		self = oldself;
	}
	if(self.netname == "weapon_phantom")
	{
		if(other.items != other.items | IT_PHANTOM)
			other.phantomclip = 50;
		other.ammo_phantom += 100;
		other.weapon = IT_PHANTOM;
		self = other;
		UpdateWeapon();
		self = oldself;
	}
	if(self.netname == "weapon_ar33")
	{
		if(other.items != other.items | IT_AR33)
			other.ar33clip = 30;
		other.ammo_ar33 += 90;
		other.weapon = IT_AR33;
		self = other;
		UpdateWeapon();
		self = oldself;
	}
	self.nextthink = time + 15;
	self.think = RespawnItem;
}
void() weapon_dd44 =
{
	setmodel (self, "progs/w_dd44.mdl");
	self.netname = "weapon_dd44";
	self.touch = item_touch;
	setsize (self, '-4 -4 -4', '4 4 4');
	StartItem ();
};
void() weapon_kf7 =
{
	setmodel (self, "progs/w_kf7.mdl");
	self.netname = "weapon_kf7";
	self.touch = item_touch;
	setsize (self, '-4 -4 -4', '4 4 4');
	StartItem ();
};
void() weapon_cougar =
{
	setmodel (self, "progs/w_magnum.mdl");
	self.netname = "weapon_cougar";
	self.touch = item_touch;
	setsize (self, '-4 -4 -4', '4 4 4');
	StartItem ();
};
void() weapon_phantom =
{
	setmodel (self, "progs/w_phantom.mdl");
	self.netname = "weapon_phantom";
	self.touch = item_touch;
	setsize (self, '-4 -4 -4', '4 4 4');
	StartItem ();
};
void() weapon_ar33 =
{
	setmodel (self, "progs/w_ar33.mdl");
	self.netname = "weapon_ar33";
	self.touch = item_touch;
	setsize (self, '-4 -4 -4', '4 4 4');
	StartItem ();
};