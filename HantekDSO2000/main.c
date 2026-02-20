#pragma comment(lib, "visa64")
#include <visa.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "DSO2000.h"
#include "scpi.h"

#define PACKET_SIZE 512

void print_bytes(const char* label, const unsigned char* data, size_t len) {
    printf("%s (%zu bytes):\n", label, len);

    for (size_t i = 0; i < len; i++) {
        // Print hex byte
        printf("%02X ", data[i]);

        // New line every 16 bytes
        if ((i + 1) % 16 == 0 || i == len - 1) {
            // ASCII representation
            for (int j = 0; j < 16 - (i % 16) - 1; j++) printf("   ");
            printf("| ");
            for (size_t j = i - (i % 16); j <= i; j++) {
                unsigned char c = data[j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
            for (int k = 0; k < 16 - (i % 16) - 1; k++) printf(" ");
            printf(" |\n");
        }
    }
    printf("\n");
}

int main(int argc, const char* argv[]) {
    ViStatus status;
    ViSession defaultRM, instr;

    // Open VISA resource manager
    status = viOpenDefaultRM(&defaultRM);
    if (status < VI_SUCCESS) {
        printf("Error opening RM: 0x%08x\n", status);
        return 1;
    }

    ViChar selectedDevice[VI_FIND_BUFLEN];
    // Find all test instruments
    {
    ViFindList resultList;
    ViUInt32 resultCount = 0;
    ViChar resultName[VI_FIND_BUFLEN];
    status = viFindRsrc(defaultRM, "?*INSTR", &resultList, &resultCount, resultName);
    if (status < VI_SUCCESS || resultCount == 0) {
        if (status == VI_ERROR_RSRC_NFOUND) {
            printf("Error: no devices found\n");
        }
        else {
            printf("Error finding devices: 0x%08x\n", status);
        }
        viClose(defaultRM);
        return 1;
    }
    ViChar** names = NULL;
    names = (ViChar**)malloc(sizeof(ViChar*) * resultCount);
    if (!names) {
        printf("Device array allocation failed\n");
        viClose(resultList);
        viClose(defaultRM);
        return 1;
    }
    printf("Found %d device%s:\n", resultCount, resultCount > 1 ? "s" : "");
    for (ViUInt32 i = 0; i < resultCount; i++) {
        names[i] = (ViChar*)malloc(sizeof(char) * VI_FIND_BUFLEN);
        if (!names[i]) {
            printf("Device allocation failed\n");
            goto cleanup;
        }
        memcpy(names[i], resultName, VI_FIND_BUFLEN);
        status = viFindNext(resultList, resultName);
    }
    for (ViUInt32 j = 0; j < resultCount; j++) {
        printf("  %d) %s\n", j+1, names[j]);
    }
    int selectedIndex = -1;
    if (resultCount > 1) {
        char input[32];
        while (selectedIndex < 0 || selectedIndex >= (int)resultCount) {
            printf("\nSelect device (1-%u): ", resultCount);
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("Input error.\n");
                goto cleanup;
            }

            // Remove trailing newline
            input[strcspn(input, "\n")] = '\0';

            // Try to parse as integer
            char* endptr;
            long num = strtol(input, &endptr, 10);

            if (endptr == input || *endptr != '\0') {
                printf("Invalid input. Please enter a number.\n");
                continue;
            }

            if (num < 1 || num > (int)resultCount) {
                printf("Invalid input. Please enter a number between 1 and %u.\n", resultCount);
            }

            selectedIndex = (int)num - 1;
        }
        printf("\n");
    }
    else {
        selectedIndex = 0;
    }
    memcpy(selectedDevice ,names[selectedIndex], VI_FIND_BUFLEN);
    cleanup:
    viClose(resultList);
    for (ViUInt32 i = 0; i < resultCount; i++) {
        free(names[i]);
    }
    free(names);
    }

    // Open the USBTMC instrument (replace with your actual resource string from NI MAX or Keysight Connection Expert)
    status = viOpen(defaultRM, selectedDevice, VI_NO_LOCK, 40000, &instr);
    if (status < VI_SUCCESS) {
        printf("Cannot open instrument: 0x%08x\n", status);
        viClose(defaultRM);
        return 1;
    }

    char read_buffer[PACKET_SIZE];
    ViUInt32 count;
    scpi_write_and_read(instr, "*IDN?\n", read_buffer, PACKET_SIZE, &count);
    read_buffer[count] = '\0';

    if (status >= VI_SUCCESS || status == VI_SUCCESS_TERM_CHAR) {
        printf("IDN response: %s\n", read_buffer);
    }
    else {
        printf("Read error: 0x%08x\n", status);
    }
    char wav_header[128];
    scpi_write(instr, ":WAV:DATA:ALL?\n");
    scpi_read(instr, wav_header, sizeof(wav_header), &count);
    //print_bytes(":WAV:DATA:ALL? response", wav_header, count);

    WaveformDataHeader header;
    readDataHeader(wav_header, &header);
    //printDataHeader(&header);

    FILE *csv = fopen("data.csv", "w");

    fprintf(csv, "Time_s,Volt_V\n");
    double sPerSample = 1.0 / header.sampleRate * header.sampleMultiple;

    char wave_data[4100];
    for (size_t i = 0; i < 2; i++) {
        memset(wave_data, 0x00, sizeof(wave_data));
        scpi_write(instr, ":WAV:DATA:ALL?\n");
        scpi_read(instr, wave_data, sizeof(wave_data), &count);
        for (size_t j = 29; j < count; j++) {
            fprintf(csv, "%le,%le\n",
                sPerSample * (double)(j - 29),
                header.voltageC1 * (double)((int8_t)wave_data[j] - header.offsetC1 - 2)
            );
        }
        print_bytes(":WAV:DATA:ALL? response", wave_data, count);
    }

    viClose(instr);
    viClose(defaultRM);
    return 0;
}
