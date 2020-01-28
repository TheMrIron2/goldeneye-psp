/*
Rotating doors are based on "Chris '06 Hinge Doors".
Doors needs to be made like in HalfLife using origin brush.
Swings from both sides, avelocity overshot issue resolved using .ltime.
/*
-=-=-=-
Defines
-=-=-=-
*/
.float RSTATE, reverse;
void() door_open;

// ------------------------------------------------
float(entity targ) infrontofdoor =
// ------------------------------------------------
{
	float sign = 1;
	float loX = self.mins_x + self.origin_x;
	float loY = self.mins_y + self.origin_y;

	float hiX = self.maxs_x + self.origin_x;
	float hiY = self.maxs_y + self.origin_y;

	float momentArmX = targ.origin_x - self.origin_x;
	float momentArmY = targ.origin_y - self.origin_y;
	if (loX > targ.origin_x)
	{
		if (targ.origin_y < loY)
		{
			if (fabs(momentArmY) > fabs(momentArmX))
				sign = (momentArmY < 0) ? 1 : -1;
			else
				sign = (momentArmX > 0) ? 1 : -1;
		}
		else if (targ.origin_y > hiY)
		{
			if (fabs(momentArmY) > fabs(momentArmX))
				sign = (momentArmY < 0) ? 1 : -1;
			else
				sign = (momentArmX < 0) ? 1 : -1;
		}
		else
			sign = (momentArmY < 0) ? 1 : -1;
	}
	else
	{
		if (targ.origin_x <= hiX)
		{
			if (targ.origin_y < loY)
				sign = (momentArmX > 0) ? 1 : -1;
			else if (targ.origin_y > hiY)
				sign = (momentArmX < 0) ? 1 : -1;
		}
		else if (targ.origin_y < loY)
		{
			if (fabs(momentArmY) > fabs(momentArmX))
				sign = (momentArmY > 0) ? 1 : -1;
			else
				sign = (momentArmX > 0) ? 1 : -1;
		}
		else if (targ.origin_y > hiY)
		{
			if (fabs(momentArmY) > fabs(momentArmX))
				sign = (momentArmY > 0) ? 1 : -1;
			else
				sign = (momentArmX < 0) ? 1 : -1;
		}
		else
			sign = (momentArmY > 0) ? 1 : -1;
	}
	if(sign == 1)
		return TRUE;
	else
		return FALSE;
};

// ------------------------------------------------
void() func_door_rotating =
// ------------------------------------------------
{
   if (self.sounds == 0)//no sound fx
   {
      precache_sound ("misc/null.wav");
      self.noise1 = "misc/null.wav";
      self.noise2 = "misc/null.wav";
      self.noise3 = "misc/null.wav";
   }
   if (self.sounds == 1)//generic
   {
      precache_sound ("door/open_generic.wav");
      precache_sound ("door/close_generic.wav");
      precache_sound ("door/squeek_generic.wav");
      self.noise1 = "door/open_generic.wav";
      self.noise2 = "door/close_generic.wav";
      self.noise3 = "door/squeek_generic.wav";
   }
   if (self.sounds == 2)//metal light door
   {
      precache_sound ("door/open_lmetal.wav");
      precache_sound ("door/close_lmetal.wav");
      precache_sound ("door/squeek_generic.wav");
      self.noise1 = "door/open_lmetal.wav";
      self.noise2 = "door/close_lmetal.wav";
      self.noise3 = "door/squeek_generic.wav";
   }
   if (self.sounds == 3)//metal heavy door
   {
      precache_sound ("door/open_metal.wav");
      precache_sound ("door/close_metal.wav");
      precache_sound ("door/squeek_metal.wav");
      self.noise1 = "door/open_metal.wav";
      self.noise2 = "door/close_metal.wav";
      self.noise3 = "door/squeek_metal.wav";
   }
   if (self.sounds == 4)//wood door
   {
      precache_sound ("door/open_wood.wav");
      precache_sound ("door/close_wood.wav");
      precache_sound ("door/squeek_wood.wav");
      self.noise1 = "door/open_wood.wav";
      self.noise2 = "door/close_wood.wav";
      self.noise3 = "door/squeek_wood.wav";

   }
       
	  self.solid = SOLID_BSP;
	  self.movetype = MOVETYPE_PUSH;
	  setorigin (self, self.origin);
	  setmodel (self, self.model);
	  self.classname = "func_door_rotating";
	  setsize (self, self.mins, self.maxs);
	  self.RSTATE = 0;

   if (!self.targetname)
      self.touch = door_open;
   
   self.nextthink = self.ltime + 9999999999999999999;
   self.think = door_open;
   
   if (!self.wait)
      self.wait = 0.1;
   
   if (self.targetname)
      self.use = door_open;
   
   self.avelocity = '0 0 0';

};

// ------------------------------------------------
void() door_closed =
// ------------------------------------------------
{
   self.avelocity = '0 0 0';
   self.RSTATE = 0;
   self.nextthink = self.ltime + 9999999999999999999;
   sound (self, CHAN_VOICE, self.noise2, 1, ATTN_IDLE);
   self.touch = door_open;

};

// ------------------------------------------------
void() door_closing =
// ------------------------------------------------
{
   self.nextthink = self.ltime + 1;
   self.think = door_closed;

   if (!self.reverse)
      self.avelocity = '0 -90 0';
   else
      self.avelocity = '0 90 0';

   sound (self, CHAN_VOICE, self.noise3, 1, ATTN_IDLE); 
};

// ------------------------------------------------
void() door_opened =
// ------------------------------------------------
{

   if (!self.targetname)
   {
      self.avelocity = '0 0 0';
      self.nextthink = self.ltime + 2;
      self.think = door_closing;
   }
};

// ------------------------------------------------
void() door_opening =
// ------------------------------------------------
{
   self.nextthink = self.ltime + 1;
   self.think = door_opened;

   if (!self.reverse)
      self.avelocity = '0 90 0';
   else
      self.avelocity = '0 -90 0';
};

// ------------------------------------------------
void() door_open =
// ------------------------------------------------
{

    self.avelocity = '0 0 0';
      self.touch = SUB_Null;

	if (!infrontofdoor(other))
		self.reverse = 1;

	if (infrontofdoor(other))
		self.reverse = 0;

   if (self.RSTATE == 0)
   {
      self.RSTATE = 1;
      door_opening ();
      sound (self, CHAN_BODY, self.noise1, 1, ATTN_IDLE);
   }

};