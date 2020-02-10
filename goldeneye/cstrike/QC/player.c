void()PutClientCTInServer;
void()PutClientTInServer;
.float anim_time;
.float anim_end; 
void () SetClientFrame =
{
    if (self.anim_time > time)
        return; //don't call every frame, if it is the animations will play too fast
	
    self.anim_time = time + 0.015;
	if (self.frame < self.anim_end)
	{   // continue an animation
		self.frame = self.frame + 1;
		return;
	}
	
    if (self.velocity_x || self.velocity_y)
    {   // because i am so lazy
		if(self.button3)
		{
			self.sequence = 6;
			self.frame = 0;
			self.anim_end = 30;	
			return;
		}
		self.sequence = 3;
        self.frame = 0;
        self.anim_end = 22;
    }
    else
    { 
		if(self.button3)
		{
			self.sequence = 7;
			self.frame = 0;
			self.anim_end = 30;	
			return;
		}
		self.sequence = 0;
        self.frame = 0;
        self.anim_end = 30;
    }

};
void()PlayerRespawn=
{
	if(self.team == CT_SIDE)	
		PutClientCTInServer();
	else
		PutClientTInServer();
}
void() PlayerDead =
{
	self.nextthink = -5;
	self.deadflag = DEAD_DEAD;
	PlayerRespawn();
};
void()	die_anim1=[1, die_anim2]{self.nextthink = time + 0.05;};
void()	die_anim2=[2, die_anim3]{self.nextthink = time + 0.05;};
void()	die_anim3=[3, die_anim4]{self.nextthink = time + 0.05;};
void()	die_anim4=[4, die_anim5]{self.nextthink = time + 0.05;};
void()	die_anim5=[5, die_anim6]{self.nextthink = time + 0.05;};
void()	die_anim6=[6, die_anim7]{self.nextthink = time + 0.05;};
void()	die_anim7=[7, die_anim8]{self.nextthink = time + 0.05;};
void()	die_anim8=[8, die_anim9]{self.nextthink = time + 0.05;};
void()	die_anim9=[9, die_anim10]{self.nextthink = time + 0.05;};
void()	die_anim10=[10, die_anim11]{self.nextthink = time + 0.05;};
void()	die_anim11=[11, die_anim12]{self.nextthink = time + 0.05;};
void()	die_anim12=[12, die_anim13]{self.nextthink = time + 0.05;};
void()	die_anim13=[13, die_anim14]{self.nextthink = time + 0.05;};
void()	die_anim14=[14, die_anim15]{self.nextthink = time + 0.05;};
void()	die_anim15=[15, die_anim16]{self.nextthink = time + 0.05;};
void()	die_anim16=[16, die_anim17]{self.nextthink = time + 0.05;};
void()	die_anim17=[17, die_anim18]{self.nextthink = time + 0.05;};
void()	die_anim18=[18, die_anim19]{self.nextthink = time + 0.05;};
void()	die_anim19=[19, die_anim20]{self.nextthink = time + 0.05;};
void()	die_anim20=[20, die_anim21]{self.nextthink = time + 0.05;};
void()	die_anim21=[21, die_anim22]{self.nextthink = time + 0.05;};
void()	die_anim22=[22, die_anim23]{self.nextthink = time + 0.05;};
void()	die_anim23=[23, die_anim24]{self.nextthink = time + 0.05;};
void()	die_anim24=[24, die_anim25]{self.nextthink = time + 0.05;};
void()	die_anim25=[25, die_anim26]{self.nextthink = time + 0.05;};
void()	die_anim26=[26, die_anim27]{self.nextthink = time + 0.05;};
void()	die_anim27=[27, PlayerDead]{};
void()DeathSound=

{	local float r;
	r = floor(random()*4);
	if (r == 0)
		sound (self, CHAN_VOICE, "player/die1.wav", 1, ATTN_NORM);
	else if (r == 1)
		sound (self, CHAN_VOICE, "player/die2.wav", 1, ATTN_NORM);
	else if (r == 2)
		sound (self, CHAN_VOICE, "player/die3.wav", 1, ATTN_NORM);
	else if (r == 3)
		sound (self, CHAN_VOICE, "player/death6.wav", 1, ATTN_NORM);
}
void () PlayerDie =
{
	self.deadflag = DEAD_DEAD;
	self.items = 0; 
    self.view_ofs = '0 0 -8';
    self.angles_x = self.angles_z = 0;
    self.solid = SOLID_NOT;
    self.movetype = MOVETYPE_TOSS;
    self.flags = self.flags - (self.flags & FL_ONGROUND);
    if (self.velocity_z < 10)
        self.velocity_z = self.velocity_z + random()*300;
	self.sequence = 22;
	self.frame = 0;
	die_anim1();
	DeathSound();
};
