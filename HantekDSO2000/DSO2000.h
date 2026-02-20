#pragma once

#include <stdint.h>
#include <visa.h>

#define PACKET_SIZE 512

/**
* Used for on/off commands
*/
typedef enum dso_bool {
	OFF = 0,
	ON = 1
} DSO_BOOL;

void setBandwidthLimit(ViSession device, int channelN, DSO_BOOL limit);
DSO_BOOL getBandwithlimit(ViSession device, int channelN);

/// <summary>
/// Channel connection coupling modes
/// </summary>
typedef enum dso_coupling{
	AC, /// The DC component of the signal under test is blocked.
	DC, /// Both the DC and AC components of the signal under test can pass.
	GND, /// Both the DC and AC components of the signal under test are blocked
	ERROR = -1 /// An error occurred.
} DSO_COUPLING;

void setCoupling(ViSession device, int channelN, DSO_COUPLING mode);
DSO_COUPLING getCoupling(ViSession device, int channelN);

/// <summary>
/// SCPI bulk data header
/// </summary>
typedef struct scpi_header{
	size_t thisBytes; /// Length of this packet
	size_t totalBytes; /// Total data length
	size_t bytesTransmitted; /// Bytes already received
} SCPIHeader;

typedef struct wav_header{
	SCPIHeader header; /// SCPI header
	int running; /// Is running
	int trigger; /// Ready to trigger
	int offsetC1; /// Channel 1 sample offset
	int offsetC2; /// Channel 2 sample offset
	int offsetC3; /// Channel 3 sample offset
	int offsetC4; /// Channel 4 sample offset
	double voltageC1; /// Channel 1 denormalized volts/sample
	double voltageC2; /// Channel 2 denormalized volts/sample
	double voltageC3; /// Channel 3 denormalized volts/sample
	double voltageC4; /// Channel 4 denormalized volts/sample
	int enabledC1; /// Channel 1 enabled
	int enabledC2; /// Channel 2 enabled
	int enabledC3; /// Channel 3 enabled
	int enabledC4; /// Channel 4 enabled
	double sampleRate; /// Samples/s
	int sampleMultiple; /// Sample rate multiplier
	double triggerTime; /// Trigger offset from center
	double startTime; /// Data packet offset from start
} WaveformDataHeader;

typedef struct wav_data{
	SCPIHeader header; /// SCPI header
	size_t dataLength; /// Packet data length
	int8_t* data; /// Raw samples
} WaveformDataPacket;

void readDataHeader(void *bytes, WaveformDataHeader* header);
void printDataHeader(WaveformDataHeader* header);

void setChannelInvert(ViSession device, int channelN, DSO_BOOL inverted);

typedef enum {
	D_4K   = 4000,
	D_40K  = 40000,
	D_400K = 400000,
	D_4M   = 4000000,
	D_8M   = 8000000,
} DSOStorageDepth;

int getStorageDepth(ViSession device);
void setStorageDepth(ViSession device, DSOStorageDepth depth);
