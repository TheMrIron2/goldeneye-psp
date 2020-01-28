/*
Crow_bar 2012(c).
Beams, rope, wire physics
*/

#define MAX_LINEP 256

//types
#define TYPEBEAM 1
#define TYPEROPE 2
#define TYPEWIRE 3

void R_InitLinep(void);
void R_ClearLinep(void);
void R_ShutdownLinep(void);
void R_DrawLinep(void);
int R_AddNewLinep(int type, int seg, float amp, unsigned int color, int texindex, vec3_t start, vec3_t end);

