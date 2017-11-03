#pragma once

typedef long long SIM_TIME;
typedef unsigned int USERID;
typedef enum 
{
	UID_INVALID = -1,
	UID_LBSP0 = 0,
	UID_USER_MIN,
	UID_USER_MAX = 0X7F00,
	UID_LBS_MIN,
	UID_LBS_MAX = 0X7FFF
};

#define PROTOCOL_NAME_HSLPO "HSLPO"
#define PROTOCOL_NAME_BSW "BSW"
#define PROTOCOL_NAME_MHLPP "MHLPP"
#define PROTOCOL_NAME_APTCARD "APTCARD"
#define PROTOCOL_NAME_SLPD "SLPD"

#define HOST_SPEED_PEDESTRIANS_MIN 1.8
#define HOST_SPEED_PEDESTRIANS_MAX 5.4
#define HOST_SPEED_CAR_MIN 10
#define HOST_SPEED_CAR_MAX 50

#define SERVER_NODE_COUNT 1
#define DEFAULT_TIME_OUT 400000

#define DEFAULT_TRUST_VALUE 80

#define DEFAULT_HIGH_TRUST_VALUE (100-(int)(ORIGINAL_HOST_MOVING_COUNT * (100-DEFAULT_TRUST_VALUE) / HOST_COUNT_MOVING + 0.5))

#define DEFAULT_LOW_TRUST_VALUE 40
#define K_ANONYMITY_COUNT 400

#define LAST_FRIEND_SURRING_RADIUS (K_ANONYMITY_COUNT*WIFI_RANGE)