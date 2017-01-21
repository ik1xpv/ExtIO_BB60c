#ifndef EXTIO_BB60C_H
#define EXTIO_BB60C_H
#define VERNUM "0.8"
/*
ExtIO_dll specification by Alberto Di Bene I2PHD
ExtIO_BB60 from Signal Hound http://www.signalhound.com/sigdownloads/Other/HDSDR-SignalHound.zip
HDSDR ExtIO_dll from http://hdsdr.de/download/ExtIO/ExtIO_Demo_102.zip

	- mods by Oscar Steila IK1XPV 
	- buffers allocation and dimension to reach 40Msps operation.
	- added sampling rate selection from HDSDR GUI. 
	- 
*/

#include "LC_ExtIO_Types.h"

#ifndef EXTIO_EXPORTS
#define EXT_API __declspec(dllexport)
#else
#define EXT_API __declspec(dllimport)
#endif

#ifdef __cplusplus 
extern "C" { 
#endif

EXT_API bool __stdcall InitHW(char *name, char *model, int &hwtype);
EXT_API bool __stdcall OpenHW();
EXT_API void __stdcall CloseHW();
EXT_API int __stdcall StartHW64(int64_t extLOfreq);
EXT_API void __stdcall StopHW();
EXT_API void __stdcall SetCallback(pfnExtIOCallback funcptr);
EXT_API int __stdcall SetHWLO64(int64_t extLOfreq);
EXT_API int __stdcall GetStatus();

EXT_API void __stdcall RawDataReady(long samprate, void *Ldata, void *Rdata, int numsamples);
EXT_API long __stdcall GetHWLO();             
EXT_API long __stdcall GetHWSR();

EXT_API int __stdcall GetAttenuators(int idx, float *attenuation);
EXT_API int __stdcall GetActualAttIdx();
EXT_API int __stdcall SetAttenuator(int idx);

EXT_API int __stdcall ExtIoGetSrates(int srate_idx, double * samplerate);
EXT_API int __stdcall ExtIoSetSrate(int srate_idx);
EXT_API int __stdcall ExtIoGetActualSrateIdx(void);
EXT_API long __stdcall ExtIoGetBandwidth(int srate_idx);

#ifdef __cplusplus
} // extern "C"
#endif

#endif