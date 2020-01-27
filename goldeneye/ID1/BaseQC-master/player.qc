/*
+------+
|Player|
+------+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+
| Scratch                        http://www.inside3d.com/qctut/scratch.shtml |
+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+
| Handle player animations and other misc player functions                   |
+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+
*/
.float anim_time; // used for animation timing
.float anim_end; // end frame for current scene
.float anim_priority; // prioritize animations
.float anim_run; // running or not

// client_t->anim_priority
float ANIM_BASIC		= 0;	// stand / run
float ANIM_PAIN			= 1;
float ANIM_ATTACK		= 2;
float ANIM_DEATH		= 3;


// running
$frame axrun1 axrun2 axrun3 axrun4 axrun5 axrun6
$frame rockrun1 rockrun2 rockrun3 rockrun4 rockrun5 rockrun6

// standing
$frame stand1 stand2 stand3 stand4 stand5
$frame axstnd1 axstnd2 axstnd3 axstnd4 axstnd5 axstnd6
$frame axstnd7 axstnd8 axstnd9 axstnd10 axstnd11 axstnd12

// pain
$frame axpain1 axpain2 axpain3 axpain4 axpain5 axpain6
$frame pain1 pain2 pain3 pain4 pain5 pain6

// death
$frame axdeth1 axdeth2 axdeth3 axdeth4 axdeth5 axdeth6
$frame axdeth7 axdeth8 axdeth9
$frame deatha1 deatha2 deatha3 deatha4 deatha5 deatha6 deatha7 deatha8
$frame deatha9 deatha10 deatha11
$frame deathb1 deathb2 deathb3 deathb4 deathb5 deathb6 deathb7 deathb8
$frame deathb9
$frame deathc1 deathc2 deathc3 deathc4 deathc5 deathc6 deathc7 deathc8
$frame deathc9 deathc10 deathc11 deathc12 deathc13 deathc14 deathc15
$frame deathd1 deathd2 deathd3 deathd4 deathd5 deathd6 deathd7
$frame deathd8 deathd9
$frame deathe1 deathe2 deathe3 deathe4 deathe5 deathe6 deathe7
$frame deathe8 deathe9

// attacks
$frame nailatt1 nailatt2
$frame light1 light2
$frame rockatt1 rockatt2 rockatt3 rockatt4 rockatt5 rockatt6
$frame shotatt1 shotatt2 shotatt3 shotatt4 shotatt5 shotatt6
$frame axatt1 axatt2 axatt3 axatt4 axatt5 axatt6
$frame axattb1 axattb2 axattb3 axattb4 axattb5 axattb6
$frame axattc1 axattc2 axattc3 axattc4 axattc5 axattc6
$frame axattd1 axattd2 axattd3 axattd4 axattd5 axattd6

void () SetClientFrame =
{

// note: call whenever weapon frames are called!

    if (self.anim_time > time)
        return; //don't call every frame, if it is the animations will play too fast

    self.anim_time = time + 0.1;

    local float anim_change, run;

    if (self.velocity_x || self.velocity_y)
        run = TRUE;
    else
        run = FALSE;

    anim_change = FALSE;


    // check for stop/go and animation transitions
    if (run != self.anim_run && self.anim_priority == ANIM_BASIC)
        anim_change = TRUE;

    if (anim_change != TRUE)
    {
        if (self.frame < self.anim_end)
        {   // continue an animation
            self.frame = self.frame + 1;
            return;
        }

        if (self.anim_priority == ANIM_DEATH)
        {
            if (self.deadflag == DEAD_DYING)
            {
                self.nextthink = -1;
                self.deadflag = DEAD_DEAD;
            }
            return;    // stay there
        }
    }


    // return to either a running or standing frame
    self.anim_priority = ANIM_BASIC;
    self.anim_run = run;


    if (self.velocity_x || self.velocity_y)
    {   // running
        self.frame = $rockrun1;
        self.anim_end = $rockrun6;
    }
    else
    {   // standing
        self.frame = $stand1;
        self.anim_end = $stand5;
    }

};

void () PlayerDie =
{
    self.view_ofs = '0 0 -8';
    self.angles_x = self.angles_z = 0;
    self.deadflag = DEAD_DYING;
    self.solid = SOLID_NOT;
    self.movetype = MOVETYPE_TOSS;
    self.flags = self.flags - (self.flags & FL_ONGROUND);

    if (self.velocity_z < 10)
        self.velocity_z = self.velocity_z + random()*300;
};
