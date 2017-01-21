#define _CRT_SECURE_NO_WARNINGS
#include "ExtIO_BB60c.h"

#include "bb_api.h"
#pragma comment(lib,"bb_api.lib")

#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <Windows.h>

#ifdef _DEBUG
	#define _MYDEBUG // Activate a debug console
#endif

#ifdef __cplusplus
inline void null_func(char *format, ...) { }
#define DbgEmpty null_func
#else
#define DbgEmpty { }
#endif

#ifdef  _MYDEBUG
/* Debug Trace Enabled */
#include <stdio.h>
#define DbgPrintf printf
#else
/* Debug Trace Disabled */
#define DbgPrintf DbgEmpty
#endif

#define MYIDENTIFIER "BB60c_ikxpv"
#define SETTINGS_IDENTIFIER	(MYIDENTIFIER "_" VERNUM)
static bool SDR_supports_settings = false;  // assume not supported
static bool SDR_settings_valid = false;		// assume settings are for some other ExtIO

static int handle = -1;
static int iq_len = 0;
static double bw = 0.0;
static int64_t center = (int64_t) 1.0e9; 

pfnExtIOCallback pfnCallback;

std::vector<float> iq;
std::thread loopThread;

bool stopLoopThread = false;
static std::atomic<bool> reconfigure = false;

static int		giExtSrateIdx = 8;
static unsigned gExtSampleRate = 40000000;

struct Atten {
    float displayValue;
};

static const int attenCount = 6;
static int giAttenIndex = 0;

Atten attenSettings[attenCount] = {
    { 0.0 },
    { 10.0 },
    { 20.0 },
    { 30.0 },
    { 40.0 },
    { 50.0 }
};


void loopRoutine()
{
	double newSrate = 1;
	int rateidx = 1;
	double bandwidth = 800000.0;
    stopLoopThread = false;
    reconfigure = true;
	DbgPrintf("\nloopRoutine runs");

    while(!stopLoopThread) {
        if(reconfigure) {
            bbAbort(handle);
			DbgPrintf("\nReconfigure");
            bbConfigureLevel(handle, -attenSettings[giAttenIndex].displayValue, BB_AUTO_ATTEN);
            bbConfigureCenterSpan(handle, (double)center, 1.0e3);
			bbConfigureGain(handle, BB_AUTO_GAIN);
			ExtIoGetSrates(ExtIoGetActualSrateIdx(), &newSrate);
			rateidx = 40000000 /((unsigned)(newSrate + 0.5));
			bandwidth = (double) ExtIoGetBandwidth(ExtIoGetActualSrateIdx());
			bbConfigureIQ(handle, rateidx, bandwidth); // to set sampling
            bbInitiate(handle, BB_STREAMING, BB_STREAM_IQ);
            reconfigure = false;
        }


        bbIQPacket pkt;
        pkt.iqData = &iq[0];
        pkt.iqCount = iq.size() / 2;
        pkt.triggers = 0;
        pkt.triggerCount = 0;
        pkt.purge = false;
        bbGetIQ(handle, &pkt);

		pfnCallback(iq_len , extHw_RUNNING, 0, &iq[0]);
	
    }
	DbgPrintf("\nloopRoutine ends");
}

bool __stdcall InitHW(char *name, char *model, int &hwtype)
{
#ifdef _MYDEBUG
	if (AllocConsole())
	{
		FILE* f;
		freopen_s(&f, "CONOUT$", "wt", stdout);
		SetConsoleTitle(TEXT("Debug Black Box Console ExtIO_BB60c " VERNUM ));
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
		DbgPrintf("Oscar Steila IK1XPV fecit MMXVII\n");
	}
#endif
    sprintf(name, "BB60c " VERNUM "ik1xpv");
    sprintf(model, "SH_BB60c");
    hwtype = 7;
    bbStatus status = bbOpenDevice(&handle);

    if(status != bbNoError) {
		DbgPrintf("\nHardware %s %s not ready \nExtIO_dll is not active",name ,model );
#ifdef _MYDEBUG
        return true;
#else
		return false;
#endif
    }
	iq_len = 65536; // define
    iq.resize(iq_len * 2);
	DbgPrintf("\nHardware ready");
    return true;
}

EXT_API bool __stdcall OpenHW()
{
	DbgPrintf("\nOpenHW()");
    pfnCallback(-1, extHw_READY, 0, NULL);
    return true;
}

EXT_API void __stdcall CloseHW()
{
	DbgPrintf("\nCloseHW()");
    bbCloseDevice(handle);
}

EXT_API int __stdcall StartHW64(int64_t extLOfreq)
{
	DbgPrintf("\nStartHW64");

    bbStatus status = bbInitiate(handle, BB_STREAMING, BB_STREAM_IQ);
    if(status != bbNoError) {
        return -1;
    }

    stopLoopThread = true;
    if(loopThread.joinable()) {
        loopThread.join();
    }
    bbAbort(handle);
    loopThread = std::thread(&loopRoutine);
    return iq_len;
}

EXT_API void __stdcall StopHW()
{
	DbgPrintf("\nStopHW()");
    stopLoopThread = true;
    if(loopThread.joinable()) {
        loopThread.join();
    }
    bbAbort(handle);
}

EXT_API void __stdcall SetCallback(pfnExtIOCallback funcptr)
{
	DbgPrintf("\nSetCallback()");
    pfnCallback = funcptr;
}


EXT_API int __stdcall SetHWLO64(int64_t extLOfreq) // see also SetHWLO64
{
    if(extLOfreq < 9.0e3) {
        return -1;
    }

    if(extLOfreq > 6.0e9) {
        return 1;
    }

    center = extLOfreq;
    reconfigure = true;

    return 0;
}


EXT_API int __stdcall GetStatus()
{

    return 0;
}

EXT_API void __stdcall RawDataReady(long samprate, 
                                    void *Ldata, 
                                    void *Rdata, 
                                    int numsamples)
{
    int i = 0;
}

EXT_API long __stdcall GetHWLO()
{
    return (long) center;
}


EXT_API int __stdcall ExtIoGetSrates(int srate_idx, double * samplerate)
{
	switch (srate_idx)
	{
	case 0:		*samplerate = 156250.0;		break; 
	case 1:		*samplerate = 312500.0;		break; 
	case 2:		*samplerate = 625000.0;		break; 
	case 3:		*samplerate = 1250000.0;	break; 
	case 4:		*samplerate = 2500000.0;	break; 
	case 5:		*samplerate = 5000000.0;	break; 
	case 6:		*samplerate = 10000000.0;	break; 
	case 7:		*samplerate = 20000000.0;	break; 
	case 8:		*samplerate = 40000000.0;	break; 
	default:	  return 1;	// ERROR
	}
	DbgPrintf("\n ExtIoGetSrates() srate_idx = %d samplerate = %d", srate_idx, (long)*samplerate);
	return 0;
}

EXT_API long __stdcall ExtIoGetBandwidth(int srate_idx)
{
	double newSrate = 0.0;
	long ret = -1L;
	if (0 == ExtIoGetSrates(srate_idx, &newSrate))
	{
		switch (srate_idx)
		{
		case 0:		ret = 140000L;		break;
		case 1:		ret = 250000L;		break; 
		case 2:		ret = 500000L;		break;
		case 3:		ret = 1000000L;		break;
		case 4:		ret = 2000000L;		break;
		case 5:		ret = 3750000L;		break;
		case 6:		ret = 8000000L;		break;
		case 7:		ret = 17800000L;	break;
		case 8:		ret = 27000000L;	break;
		default:	ret = -1L;			break;
		}
		DbgPrintf("\n ExtIoGetBandwidth() srate_idx = %d bandwidth = %d", srate_idx, ret);

		return (ret >= newSrate || ret <= 0L) ? -1L : ret;
	}
	return -1L;	// ERROR
}

EXT_API int __stdcall ExtIoSetSrate(int srate_idx)
{
	double newSrate = 0.0;
	if (0 == ExtIoGetSrates(srate_idx, &newSrate))
	{
		giExtSrateIdx = srate_idx;
		gExtSampleRate = (unsigned)(newSrate + 0.5);

		DbgPrintf("\ngExtSampleRate = %d   giExtSrateIdx = %d", gExtSampleRate , giExtSrateIdx);
		reconfigure = true; //reinit 

		return 0;
	}
	return 1;	// ERROR
}

EXT_API int __stdcall ExtIoGetActualSrateIdx(void)
{
	return giExtSrateIdx;
}


EXT_API long __stdcall GetHWSR()  
{
	return gExtSampleRate;
}

EXT_API int __stdcall GetAttenuators(int idx, float *attenuation)
{
    if(idx >= attenCount) {
        return -1;
    }

    *attenuation = attenSettings[idx].displayValue;
    return 0;
}

EXT_API int __stdcall GetActualAttIdx()
{
    return giAttenIndex;
}

EXT_API int __stdcall SetAttenuator(int idx)
{
    if(idx >= attenCount || idx < 0) {
        return -1;
    }

    giAttenIndex = idx;
    reconfigure = true;
    return 0;
}

//---------------------------------------------------------------------------

EXT_API int __stdcall ExtIoGetSetting(int idx, char * description, char * value)
{
	DbgPrintf("\nExtIoGetSetting()");
	switch (idx)
	{
	case 0: snprintf(description, 1024, "%s", "Identifier");		snprintf(value, 1024, "%s", SETTINGS_IDENTIFIER);	return 0;
	case 1:	snprintf(description, 1024, "%s", "SampleRateIdx");		snprintf(value, 1024, "%d", giExtSrateIdx);		    return 0;
	case 2:	snprintf(description, 1024, "%s", "AttenuationIdx");	snprintf(value, 1024, "%d", giAttenIndex);			return 0;
	default:	return -1;	// ERROR
	}
	return -1;	// ERROR
}


EXT_API void __stdcall ExtIoSetSetting(int idx, const char * value)
{
	double newSrate;
	float  newAtten = 0.0F;
	int tempInt;

	DbgPrintf("\nExtIoSetSetting()");
	// now we know that there's no need to save our settings into some (.ini) file,
	// what won't be possible without admin rights!!!,
	// if the program (and ExtIO) is installed in C:\Program files\..
	SDR_supports_settings = true;
	if (idx != 0 && !SDR_settings_valid)
		return;	// ignore settings for some other ExtIO

	switch (idx)
	{
	case 0:		SDR_settings_valid = (value && !strcmp(value, SETTINGS_IDENTIFIER));
		// make identifier version specific??? - or not ==> never change order of idx!
		break;
	case 1:		tempInt = atoi(value);
		if (0 == ExtIoGetSrates(tempInt, &newSrate))
		{
			giExtSrateIdx = tempInt;
			gExtSampleRate = (unsigned)(newSrate + 0.5);
		}
		break;
	case 2:		tempInt = atoi(value);
		if (0 == GetAttenuators(tempInt, &newAtten))
			giAttenIndex = tempInt;
		break;

	}

}
//---------------------------------------------------------------------------