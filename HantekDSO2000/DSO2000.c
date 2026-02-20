#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DSO2000.h"
#include "scpi.h"

/// <summary>
/// Boolean strings
/// </summary>
char* boolstr[] = { "OFF", "ON" };

/// <summary>
/// Enable or disable 20MHz bandwidth limit of the specified channel.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <param name="limit">Bandwith limit status</param>
void setBandwidthLimit(ViSession device, int channelN, DSO_BOOL limit) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:BWL %s\n", channelN, boolstr[limit]);
	scpi_write(device, command);
}

/// <summary>
/// Query 20MHz bandwidth limit status of the specified channel.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <returns>Bandwith limit status</returns>
DSO_BOOL getBandwithlimit(ViSession device, int channelN) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:BWL?\n", channelN);
	ViPBuf read_buffer[PACKET_SIZE];
	size_t bytes_read = 0;
	scpi_write_and_read(device, command, read_buffer, PACKET_SIZE, bytes_read);
	read_buffer[bytes_read] = '\0';
	int status;
	(void)sscanf(read_buffer, "%d", &status);
	return (DSO_BOOL)status;
}

/// <summary>
/// Used by
/// <see cref="setCoupling"/>
/// and
/// <see cref="getCoupling"/>
/// </summary>
char* modes[] = { "AC", "DC", "GND" };

/// <summary>
/// Set the connection of the specified channel signal.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <param name="mode">Coupling mode</param>
void setCoupling(ViSession device, int channelN, DSO_COUPLING mode) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:COUP %s\n", channelN, modes[(int)mode]);
	scpi_write(device, command);
}

/// <summary>
/// Query the connection of the specified channel signal.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <returns>Coupling mode</returns>
DSO_COUPLING getCoupling(ViSession device, int channelN) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:COUP?\n", channelN);
	ViPBuf read_buffer[PACKET_SIZE];
	size_t bytes_read = 0;
	scpi_write_and_read(device, command, read_buffer, PACKET_SIZE, bytes_read);
	read_buffer[bytes_read] = '\0';
	for (size_t i = 0; i < sizeof(modes); i++) {
		if (strcmp(read_buffer, modes[i]) == 0) return (DSO_COUPLING)i;
	}
	return (DSO_COUPLING)-1;
}

/// <summary>
/// Enable or disable the specified channel.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <param name="enabled">Enabled status</param>
void setChannelEnabled(ViSession device, int channelN, DSO_BOOL enabled) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:DISP %s\n", channelN, boolstr[enabled]);
	scpi_write(device, command);
}

/// <summary>
/// Query enabled status of the specified channel.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <returns>Enabled status</returns>
DSO_BOOL getChannelEnabled(ViSession device, int channelN) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:DISP?\n", channelN);
	ViPBuf read_buffer[PACKET_SIZE];
	size_t bytes_read = 0;
	scpi_write_and_read(device, command, read_buffer, PACKET_SIZE, bytes_read);
	read_buffer[bytes_read] = '\0';
	int status;
	(void)sscanf(read_buffer, "%d", &status);
	return (DSO_BOOL)status;
}


/// <summary>
/// Set inversion the specified channel.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <param name="inverted">Inverted status</param>
void setChannelInvert(ViSession device, int channelN, DSO_BOOL inverted) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:INV %s\n", channelN, boolstr[inverted]);
	scpi_write(device, command);
}

/// <summary>
/// Query inverted status of the specified channel.
/// </summary>
/// <param name="device">Device connection session handle</param>
/// <param name="channelN">Channel number</param>
/// <returns>Inverted status</returns>
DSO_BOOL getChannelInvert(ViSession device, int channelN) {
	char command[PACKET_SIZE];
	snprintf(command, PACKET_SIZE, ":CHANnel%d:INV?\n", channelN);
	ViPBuf read_buffer[PACKET_SIZE];
	size_t bytes_read = 0;
	scpi_write_and_read(device, command, read_buffer, PACKET_SIZE, bytes_read);
	read_buffer[bytes_read] = '\0';
	int status;
	(void)sscanf(read_buffer, "%d", &status);
	return (DSO_BOOL)status;
}

/// <summary>
/// Used by
/// <see cref="setCoupling"/>
/// and
/// <see cref="getCoupling"/>
/// </summary>
char* offset_suffix[] = { "V", "mV" };

/// <summary>
/// Parse waveform data header of first <c>:WAV:DATA:ALL?</c> call
/// </summary>
/// <param name="bytes">Raw data</param>
/// <param name="header">Pointer to header</param>
void readDataHeader(void *bytes, WaveformDataHeader* header) {
	char *data = (char*)bytes;
	if (data[0] != '#' || data[1] != '9') {
		printf("invalid header %c%c\n", data[0], data[1]);
	}
	(void)sscanf(data +   2, "%9llu", &header->header.thisBytes);
	(void)sscanf(data +  11, "%9llu", &header->header.totalBytes);
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

/// <summary>
/// Print waveform data header to stdout
/// </summary>
/// <param name="header">Pointer to header</param>
void printDataHeader(WaveformDataHeader* header) {
	printf("Header length: %llu\n", header->header.thisBytes);
	printf("Total data:    %llu\n", header->header.totalBytes);
	printf("Transmitted:   %llu\n", header->header.bytesTransmitted);
	printf("Running:       %d\n", header->running);
	printf("Trigger:       %d\n", header->trigger);
	printf("Channel 1 (%s)\n  volts/bit: %e\n  offset:    %hd\n",
		header->enabledC1 ? "enabled" : "disabled",
		header->voltageC1,
		header->offsetC1
	);
	printf("Channel 2 (%s)\n  volts/bit: %e\n  offset:    %hd\n",
		header->enabledC2 ? "enabled" : "disabled",
		header->voltageC2,
		header->offsetC2
	);
	printf("Channel 3 (%s)\n  volts/bit: %e\n  offset:    %hd\n",
		header->enabledC3 ? "enabled" : "disabled",
		header->voltageC3,
		header->offsetC3
	);
	printf("Channel 4 (%s)\n  volts/bit: %e\n  offset:    %hd\n",
		header->enabledC4 ? "enabled" : "disabled",
		header->voltageC4,
		header->offsetC4
	);
	printf("Sample rate multiplier: %u\n", header->sampleMultiple);
	printf("Sample rate:  %e\n", header->sampleRate);
	printf("Trigger time: %e\n", header->triggerTime);
	printf("Start time:   %e\n", header->startTime);
	printf("\n");
}
