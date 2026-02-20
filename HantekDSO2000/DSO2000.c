#include "DSO2000.h"

#include <stdio.h>
#include <stdlib.h>

#include "scpi.h"

/**
Enable or disable 20MHz bandwidth limit
Acts as 20MHz low-pass filter
@param device Device connection session handle
@param channelN Channel number
@param limit Limit on or off
*/
void setBandwidthLimit(ViSession device, int channelN, DSO_BOOL limit) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHAN%d:BWL %d\n", channelN, (int)limit);
	scpi_write(device, command);
}

/**
Query 20MHz bandwidth limit status
Acts as 20MHz low-pass filter
@param device device connection session handle
@param channelN channel number
*/
DSO_BOOL getBandwithlimit(ViSession device, int channelN) {
	ViPBuf read_buffer[PACKET_SIZE];
	size_t bytes_read = 0;
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHAN%d:BWL? \n", channelN);
	scpi_write_and_read(device, command, read_buffer, PACKET_SIZE, bytes_read);
	read_buffer[bytes_read] = '\0';
	int status;
	(void)sscanf(read_buffer, "%d", &status);
	return (DSO_BOOL)status;
}

/**
First call to 
*/
void readDataHeader(void *bytes, WaveformDataHeader* header) {
	char *data = (char*)bytes;
	if (data[0] != '#' || data[1] != '9') {
		printf("invalid header %c%c\n", data[0], data[1]);
	}
	(void)sscanf(data +   2, "%9llu", &header->header.dataLength);
	(void)sscanf(data +  11, "%9llu", &header->header.byteLength);
	(void)sscanf(data +  20, "%9llu", &header->header.bytesTransmitted);
	(void)sscanf(data +  29,   "%1d", &header->running);
	(void)sscanf(data +  30,   "%1d", &header->trigger);
	(void)sscanf(data +  31,   "%4d", &header->offsetC1);
	(void)sscanf(data +  35,   "%4d", &header->offsetC2);
	(void)sscanf(data +  39,   "%4d", &header->offsetC3);
	(void)sscanf(data +  43,   "%4d", &header->offsetC4);
	(void)sscanf(data +  47,  "%8lf", &header->voltageC1);
	(void)sscanf(data +  55,  "%8lf", &header->voltageC2);
	(void)sscanf(data +  63,  "%8lf", &header->voltageC3);
	(void)sscanf(data +  71,  "%8lf", &header->voltageC4);
	(void)sscanf(data +  79,   "%1d", &header->enabledC1);
	(void)sscanf(data +  80,   "%1d", &header->enabledC2);
	(void)sscanf(data +  81,   "%1d", &header->enabledC3);
	(void)sscanf(data +  82,   "%1d", &header->enabledC4);
	(void)sscanf(data +  83,  "%9lf", &header->sampleRate);
	(void)sscanf(data +  92,   "%6d", &header->sampleMultiple);
	(void)sscanf(data +  98,  "%9lf", &header->triggerTime);
	(void)sscanf(data + 107,  "%9lf", &header->startTime);
	// Renormalize voltage levels
	header->voltageC1 = (header->voltageC1 / 12) / 1e-317;
	header->voltageC2 = (header->voltageC2 / 12) / 1e-317;
	header->voltageC3 = (header->voltageC3 / 12) / 1e-317;
	header->voltageC4 = (header->voltageC4 / 12) / 1e-317;
}

void printDataHeader(WaveformDataHeader* header) {
	printf("Header length:     %llu\n", header->header.dataLength);
	printf("Header bytes:      %llu\n", header->header.byteLength);
	printf("Transmitted:       %llu\n", header->header.bytesTransmitted);
	printf("\n");
	printf("Running:           %d\n", header->running);
	printf("Trigger:           %d\n", header->trigger);
	printf("\n");
	printf("C1 enabled:        %d\n", header->enabledC1);
	printf("C1 voltage:        %e\n", header->voltageC1);
	printf("C1 offset:         %hd\n", header->offsetC1);
	printf("\n");
	printf("C2 enabled:        %d\n", header->enabledC2);
	printf("C2 voltage:        %e\n", header->voltageC2);
	printf("C2 offset:         %hd\n", header->offsetC2);
	printf("\n");
	printf("C3 enabled:        %d\n", header->enabledC3);
	printf("C3 voltage:        %e\n", header->voltageC3);
	printf("C3 offset:         %hd\n", header->offsetC3);
	printf("\n");
	printf("C4 enabled:        %d\n", header->enabledC4);
	printf("C4 voltage:        %e\n", header->voltageC4);
	printf("C4 offset:         %hd\n", header->offsetC4);
	printf("\n");
	printf("Sample rate:       %e\n", header->sampleRate);
	printf("Sample multiplier: %u\n", header->sampleMultiple);
	printf("Trigger time:      %e\n", header->triggerTime);
	printf("Start time:        %e\n", header->startTime);
	printf("\n");
}