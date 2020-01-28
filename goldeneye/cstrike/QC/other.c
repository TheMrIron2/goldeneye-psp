.string texture;

void() infodecal_ =
{
	WriteByte  (MSG_ALL, SVC_BSPDECAL);
	WriteString(MSG_ALL, self.texture);
	WriteCoord (MSG_ALL, self.origin_x);
	WriteCoord (MSG_ALL, self.origin_y);
	WriteCoord (MSG_ALL, self.origin_z);

	setsize (self, '-16 -16 0', '16 16 56');
};
void() CSprayCan = 
{
	local vector org,end;
	makevectors(self.v_angle);
	org = self.origin + self.view_ofs;
	end = org + v_forward * 128;
	traceline(org,end,0,self);
	WriteByte  (MSG_ALL, SVC_BSPDECAL);
	WriteString(MSG_ALL, "{lambda06");
	WriteCoord (MSG_ALL, trace_endpos_x);
	WriteCoord (MSG_ALL, trace_endpos_y);
	WriteCoord (MSG_ALL, trace_endpos_z);
	sound(self, CHAN_VOICE, "player/sprayer.wav", 1, ATTN_NORM);
}