#pragma once

#include <visa.h>

ViStatus scpi_write(ViSession device, ViConstString command);
ViStatus scpi_read(ViSession device, ViPBuf buffer, ViUInt32 length, ViPUInt32 count);
ViStatus scpi_write_and_read(ViSession device, ViConstString command, ViPBuf buffer, ViUInt32 length, ViPUInt32 count);