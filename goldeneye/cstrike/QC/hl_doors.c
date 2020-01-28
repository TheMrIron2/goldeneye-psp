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
void() func_door_rotating_ =
// ------------------------------------------------
{
	switch(self.movesnd)
	{
		case 0:
		precache_sound("common/null.wav");
		self.noise1 =("common/null.wav");
		break;
		case 1:
		precache_sound("doors/doormove1.wav");
		self.noise1 =("doors/doormove1.wav");
		break;
		case 2:
		precache_sound("doors/doormove2.wav");
		self.noise1 =("doors/doormove2.wav");
		break;
		case 3:
		precache_sound("doors/doormove3.wav");
		self.noise1 =("doors/doormove3.wav");
		break;
		case 4:
		precache_sound("doors/doormove4.wav");
		self.noise1 =("doors/doormove4.wav");
		break;
		case 5:
		precache_sound("doors/doormove5.wav");
		self.noise1 =("doors/doormove5.wav");
		break;
		case 6:
		precache_sound("doors/doormove6.wav");
		self.noise1 =("doors/doormove6.wav");
		break;
		case 7:
		precache_sound("doors/doormove7.wav");
		self.noise1 =("doors/doormove7.wav");
		break;
		case 8:
		precache_sound("doors/doormove8.wav");
		self.noise1 =("doors/doormove8.wav");
		break;
		case 9:
		precache_sound("doors/doormove9.wav");
		self.noise1 =("doors/doormove9.wav");
		break;
		case 10:
		precache_sound("doors/doormove10.wav");
		self.noise1 =("doors/doormove10.wav");
		break;
		default:
		precache_sound("common/null.wav");
		self.noise1 =("common/null.wav");
		break;
	}
	switch(self.stopsnd)
	{
		case 0:
		precache_sound("common/null.wav");
		self.noise2 =("common/null.wav");
		break;
		case 1:
		precache_sound("doors/doorstop1.wav");
		self.noise2 =("doors/doorstop1.wav");
		break;
		case 2:
		precache_sound("doors/doorstop2.wav");
		self.noise2 =("doors/doorstop2.wav");
		break;
		case 3:
		precache_sound("doors/doorstop3.wav");
		self.noise2 =("doors/doorstop3.wav");
		break;
		case 4:
		precache_sound("doors/doorstop4.wav");
		self.noise2 =("doors/doorstop4.wav");
		break;
		case 5:
		precache_sound("doors/doorstop5.wav");
		self.noise2 =("doors/doorstop5.wav");
		break;
		case 6:
		precache_sound("doors/doorstop6.wav");
		self.noise2 =("doors/doorstop6.wav");
		break;
		case 7:
		precache_sound("doors/doorstop7.wav");
		self.noise2 =("doors/doorstop7.wav");
		break;
		case 8:
		precache_sound("doors/doorstop8.wav");
		self.noise2 =("doors/doorstop8.wav");
		break;
		default:
		precache_sound("common/null.wav");
		self.noise2 =("common/null.wav");
		break;
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

  // sound (self, CHAN_VOICE, self.noise3, 1, ATTN_IDLE); 
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