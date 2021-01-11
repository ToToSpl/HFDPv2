#ifndef HFDP_PACKET_CONSTANTS
#define HFDP_PACKET_CONSTANTS

//BIG SIZES
#define HFDP_START_PLACE 42
#define IEEE_SIZE 24
#define RADIOTAP_SIZE 25
#define CUT_RADIOTAP_SIZE 18
#define PATTERN_OFFSET CUT_RADIOTAP_SIZE + 16

//OFFSETS
#define ID_OFFSET 0
#define FLAGS_OFFSET 1
#define RSSI_OFFSET 2
#define REMAC_OFFSET 3
#define SIZE_OFFSET 9
#define DATA_OFFSET 11

//DATA SIZES
#define HEADER_SIZE 11
#define ID_SIZE 1
#define FLAGS_SIZE 1
#define RSSI_SIZE 1
#define REMAC_SIZE 6
#define SIZE_SIZE 2

//when using fec, first 4 bits are used to determine packet index
//FLAGS
#define HEARTBEAT 0x01
#define EMPTY_1 0x02
#define EMPTY_2 0x04
#define PACKET_END 0x08
#define FRACTURED_PACKET 0x10
#define SEND_TO_SLAVE 0x20
#define RESEND 0x40
#define ENCODED 0x80
#define FEC_INDEX_MASK 0x0F

//current working header for alfa awus036nh

//header taken from Packetspammer by Andy Green <andy@warmcat.com>
//i treat this as magic spell
const u_int8_t u8aRadiotapHeader[] = {
	0x00, 0x00, // <-- radiotap version
	0x19, 0x00, // <- radiotap header length
	0x6f, 0x08, 0x00, 0x00, // <-- bitmap
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <-- timestamp
	0x00, // <-- flags
	0x16, // <-- rate 22*500kbs
	0x71, 0x09, 0xc0, 0x00, // <-- channel
	0x00, // <-- antsignal
	0x00, // <-- antnoise
	0x00, // <-- antenna
};

const u_int8_t u8aIeeeHeader_beacon[] = {
	0x08, 0x01, 0x00, 0x00, // frame control field (2 bytes), duration (2 bytes)
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff,// 1st byte of IEEE802.11 RA (mac) must be 0xff or something odd, otherwise strange things happen. second byte is the port (will be overwritten later)
	0x13, 0x22, 0x33, 0x44, 0x55, 0x66, // mac
	0x13, 0x22, 0x33, 0x44, 0x55, 0x66, // mac
	0x10, 0x86, // IEEE802.11 seqnum, (will be overwritten later by Atheros firmware/wifi chip)
};

#define MAC_OFFSET 10
#define MAC_SIZE 6

#endif