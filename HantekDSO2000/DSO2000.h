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

/**
* Used for on/off commands
*/
typedef enum dso_coupling{
	AC,
	DC,
	GND
} DSO_COUPLING;

typedef struct scpi_header{
	size_t dataLength;
	size_t byteLength;
	size_t bytesTransmitted;
} SCPIHeader;

typedef struct wav_header{
	SCPIHeader header;
	int running;
	int trigger;
	int offsetC1;
	int offsetC2;
	int offsetC3;
	int offsetC4;
	double voltageC1;
	double voltageC2;
	double voltageC3;
	double voltageC4;
	int enabledC1;
	int enabledC2;
	int enabledC3;
	int enabledC4;
	double sampleRate;
	int sampleMultiple;
	double triggerTime;
	double startTime;
} WaveformDataHeader;

typedef struct wav_data{
	SCPIHeader header;
	void* data;
} WaveformDataPacket;

void readDataHeader(void *bytes, WaveformDataHeader* header);
void printDataHeader(WaveformDataHeader* header);

typedef enum {
	D_4K   = 4000,
	D_40K  = 40000,
	D_400K = 400000,
	D_4M   = 4000000,
	D_8M   = 8000000,
} DSOStorageDepth;

int getStorageDepth(ViSession device);
void setStorageDepth(ViSession device, DSOStorageDepth depth);
