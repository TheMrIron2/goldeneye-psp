void() precaches;
void() LightStyles_setup;
void() main = {};
void()CheckRules=
{

}

void() worldspawn = 
{
	lastspawn = world;
	precaches();
	LightStyles_setup();
	CheckRules();
	BotInit();
};
void() SetNewParms = {};
void() SetChangeParms = {};
void() StartFrame = 
{
	BotFrame();
};
 
void() precaches =
{
	//models
	precache_model("progs/player.mdl");
	precache_model("progs/v_p99.mdl");
	precache_model("progs/v_slappers.mdl"); 
	precache_model("progs/v_dd44.mdl");
	precache_model("progs/v_magnum.mdl");
	precache_model("progs/w_magnum.mdl");
	precache_model("progs/w_dd44.mdl");
	precache_model("progs/007.mdl"); 
	precache_model("progs/ivan.mdl"); 
	
	precache_sound("weapons/357_cock1.wav");
	precache_sound("player/sprayer.wav");
	//sounds
	precache_sound("weapons/knife_slash1.wav");
	precache_sound("weapons/knife_hit1.wav");
	precache_sound("weapons/knife_hit2.wav");
	precache_sound("weapons/knife_hit3.wav");
	precache_sound("weapons/knife_hit4.wav");
	precache_sound("weapons/knife_hitwall1.wav");
	precache_sound("items/gunpickup2.wav");
	//usp
	precache_sound("weapons/usp1.wav");
	precache_sound("weapons/usp_unsil-1.wav");
	precache_sound("weapons/usp_sliderelease.wav");
	precache_sound("weapons/usp_clipin.wav");
	precache_sound("weapons/usp_clipout.wav");
	precache_sound("weapons/usp_silencer_on.wav");
	precache_sound("weapons/usp_silencer_off.wav");
	//flock
	precache_sound("weapons/glock18-1.wav");
	precache_sound("weapons/glock18-2.wav");
	precache_sound("weapons/clipout1.wav");
	precache_sound("weapons/clipin1.wav");
	precache_sound("weapons/sliderelease1.wav");
	precache_sound("weapons/slideback1.wav");
	//deagle
	precache_sound("weapons/de_clipin.wav");
	precache_sound("weapons/de_clipout.wav");
	precache_sound("weapons/deagle-1.wav");
	//grenade
	precache_sound("weapons/grenade_hit1.wav");
	precache_sound("weapons/grenade_hit2.wav");
	precache_sound("weapons/grenade_hit3.wav");
	precache_sound("weapons/he_bounce-1.wav");
	//m3
	precache_sound("weapons/m3-1.wav");
	precache_sound("weapons/m3_insertshell.wav");
	precache_sound("weapons/m3_pump.wav");
	//ak47
	precache_model("progs/v_kf7.mdl");
	precache_model("progs/w_kf7.mdl");
	precache_sound("weapons/ak47-1.wav");
	precache_sound("weapons/ak47_clipout.wav");
	precache_sound("weapons/ak47_clipin.wav");
	//awp
	precache_sound("weapons/awp1.wav");
	precache_sound("weapons/zoom.wav");
	precache_sound("weapons/boltup.wav");
	precache_sound("weapons/boltpull1.wav");
	precache_sound("weapons/boltdown.wav");
	precache_sound("weapons/awp_clipin.wav");
	precache_sound("weapons/awp_clipout.wav");
	//galil
	precache_model("progs/v_phantom.mdl");
	precache_model("progs/w_phantom.mdl");
	precache_sound("weapons/galil-1.wav");
	precache_sound("weapons/galil_clipout.wav");
	precache_sound("weapons/galil_clipin.wav");
	precache_sound("weapons/galil_boltpull.wav");
	//aug
	precache_sound("weapons/aug-1.wav");
	precache_sound("weapons/aug_boltpull.wav");
	precache_sound("weapons/aug_clipout.wav");
	precache_sound("weapons/aug_clipin.wav");
	precache_sound("weapons/aug_boltslap.wav");
	//famas
	precache_sound("weapons/famas-1.wav");
	precache_sound("weapons/famas-burst.wav");
	precache_sound("weapons/famas_boltslap.wav");
	precache_sound("weapons/famas_clipin.wav");
	precache_sound("weapons/famas_clipout.wav");
	//m4a1
	precache_model("progs/v_ar33.mdl");
	precache_model("progs/w_ar33.mdl");
	precache_sound("weapons/m4a1-1.wav");
	precache_sound("weapons/m4a1_clipout.wav");
	precache_sound("weapons/m4a1_clipin.wav");
	precache_sound("weapons/m4a1_boltpull.wav");
	precache_sound("weapons/m4a1_silencer_on.wav");
	precache_sound("weapons/m4a1_silencer_off.wav");
	precache_sound("weapons/m4a1_unsil-1.wav");
	//p228
	precache_sound("weapons/p228-1.wav");
	precache_sound("weapons/p228_clipout.wav");
	precache_sound("weapons/p228_clipin.wav");
	precache_sound("weapons/p228_sliderelease.wav");
	precache_sound("weapons/p228_slidepull.wav");
	//elite
	precache_sound("weapons/elite_fire.wav");
	precache_sound("weapons/elite_reloadstart.wav");
	precache_sound("weapons/elite_leftclipin.wav");
	precache_sound("weapons/elite_clipout.wav");
	precache_sound("weapons/elite_sliderelease.wav");
	precache_sound("weapons/elite_rightclipin.wav");
	precache_sound("weapons/elite_deploy.wav");
	//footsteps
	precache_sound ("player/pl_step1.wav");
	precache_sound ("player/pl_step2.wav");
	precache_sound ("player/pl_step3.wav");
	precache_sound ("player/pl_step4.wav");

	precache_sound ("player/pl_metal1.wav");
	precache_sound ("player/pl_metal2.wav");
	precache_sound ("player/pl_metal3.wav");
	precache_sound ("player/pl_metal4.wav");
	
	precache_sound ("player/pl_dirt1.wav");
	precache_sound ("player/pl_dirt2.wav");
	precache_sound ("player/pl_dirt3.wav");
	precache_sound ("player/pl_dirt4.wav");

	precache_sound ("player/pl_duct1.wav");
	precache_sound ("player/pl_duct2.wav");
	precache_sound ("player/pl_duct3.wav");
	precache_sound ("player/pl_duct4.wav");

	precache_sound ("player/pl_grate1.wav");
	precache_sound ("player/pl_grate2.wav");
	precache_sound ("player/pl_grate3.wav");
	precache_sound ("player/pl_grate4.wav");


	precache_sound ("player/pl_tile1.wav");
	precache_sound ("player/pl_tile2.wav");
	precache_sound ("player/pl_tile3.wav");
	precache_sound ("player/pl_tile4.wav");
	precache_sound ("player/pl_tile5.wav");
	
	precache_sound ("player/pl_snow1.wav");
	precache_sound ("player/pl_snow2.wav");
	precache_sound ("player/pl_snow3.wav");
	precache_sound ("player/pl_snow4.wav");
	//death
	precache_sound ("player/die1.wav");
	precache_sound ("player/die2.wav");
	precache_sound ("player/die3.wav");
	precache_sound ("player/death6.wav");
	precache_sound("weapons/explode3.wav");
	precache_model("sprites/pistol_smoke1.spr");
	precache_model("sprites/fexplo.spr");
	precache_model("sprites/eexplo.spr");
	precache_model("sprites/steam1.spr");
};
