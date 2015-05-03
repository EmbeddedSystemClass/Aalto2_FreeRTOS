/*
 * SU.h
 *
 *  Created on: Dec 1, 2014
 *      Author: nemanja
 */

#ifndef SU_H_
#define SU_H_

struct ScriptHeader_s
{
	unsigned short length;
	unsigned int start_time;
	unsigned int serial_num;
	unsigned char swv_id; // b3..b0 script tool version, b5 b4 science unit id = 0b10
	unsigned char type; //b4..b0 type, b6 b5 SU model
	unsigned char spare;
	// !!! INCONSISTENCY IN SPECIFICATION, specifies length of 12 bytes, but describes 13 bytes
};

struct TimesTableEntry_s
{
	unsigned char seconds; // 0 - 59
	unsigned char minutes; // 0 - 59
	unsigned char hours;   // 0 - 23
	unsigned char script_index; // 0x41-0x45: 5 script sequences, 0x55: EOT
};

struct ScriptSequenceEntry_s
{
	unsigned char seconds; // 0 - 59
	unsigned char minutes; // 0 - 59
	unsigned char cmd_ID;
	unsigned char length;
	unsigned char params[3];
};

struct ScriptSlot_s
{
	struct ScriptHeader_s header[7];
	struct TimesTableEntry_s times_table;
	unsigned int current_time_file_position;
	struct ScriptSequenceEntry_s sequence;
	unsigned int current_sequence_file_position;
	unsigned char current_sequence_length;

	/*uint8 times_length;
	uint8 sequence_length[5];*/
} ScriptSlot;

struct ScienceHeader_s
{
	unsigned int UTC;

	short roll;
	short pitch;
	short yaw;

	short rolldot;
	short pitchdot;
	short yawdot;

	unsigned short x;
	unsigned short y;
	unsigned short z;
};

struct ResponsePacket_s
{
	unsigned char rsp_ID;
	unsigned char seq_cnt;
	unsigned char data[172];
};

#endif /* SU_H_ */
