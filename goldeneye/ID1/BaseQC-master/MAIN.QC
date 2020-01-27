/*
+----+
|Main|
+----+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+
| Scratch                                      Http://www.admdev.com/scratch |
+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+
| Contains some 'base' subroutines. As a general rule nothing in this file   |
| does much, except to setup basic variables and entities.                   |
+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+
*/
void() precaches;
void() LightStyles_setup;

 void() main = {};
 void() worldspawn = 
 {
 precaches();
 LightStyles_setup();
 };
 void() SetNewParms = {};
 void() SetChangeParms = {};
 void() StartFrame = {};
 
void() precaches =
{
//models
 precache_model ("progs/player.mdl");
 //sounds
 precache_sound("player/jump.wav");
};