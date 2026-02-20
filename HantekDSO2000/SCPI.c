#include "scpi.h"

ViStatus scpi_write(ViSession device, ViConstString command) {
    return viPrintf(device, command);
}

ViStatus scpi_read(ViSession device, ViPBuf buffer, ViUInt32 length, ViPUInt32 count) {
    return viRead(device, buffer, length, count);
}

ViStatus scpi_write_and_read(ViSession device, ViConstString command, ViPBuf buffer, ViUInt32 length, ViPUInt32 count) {
    scpi_write(device, command);
    return scpi_read(device, buffer, length, count);
}