#ifndef BB_API_H
#define BB_API_H

#if defined(_WIN32) || defined(_WIN64)
    #ifdef BB_EXPORTS
        #define BB_API __declspec(dllexport)
    #else
        #define BB_API __declspec(dllimport)
    #endif
#else // Linux
    #define BB_API
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define BB_DEPRECATED(comment) __declspec(deprecated(comment))
#elif  defined(__GNUC__)
    #define BB_DEPRECATED(comment) __attribute__((deprecated))
#else
    #define BB_DEPRECATED(comment) comment
#endif

#define BB_TRUE                  1
#define BB_FALSE                 0

// Device type
#define BB_DEVICE_NONE           0
#define BB_DEVICE_BB60A          1
#define BB_DEVICE_BB60C          2

// Frequencies in Hz
// RT = Real-Time
// TG = Time-Gate
#define BB_MAX_DEVICES           8

// BB60A/C
#define BB60_MIN_FREQ            9.0e3    
#define BB60_MAX_FREQ            6.4e9
#define BB60_MAX_SPAN            (BB60_MAX_FREQ - BB60_MIN_FREQ)

// Frequencies specified in Hz
#define BB_MIN_SPAN              20.0       
#define BB_MIN_BW                0.602006912
#define BB_MAX_BW                10100000.0
#define BB_MIN_RT_RBW            2465.820313      
#define BB_MAX_RT_RBW            631250.0 
#define BB_MIN_RT_SPAN           200.0e3
#define BB60A_MAX_RT_SPAN        20.0e6
#define BB60C_MAX_RT_SPAN        27.0e6
#define BB_MIN_SWEEP_TIME        0.00001 // 10us
#define BB_MAX_SWEEP_TIME        1.0 // 1s
#define BB_MIN_USB_VOLTAGE       4.4f

#define BB_AUTO_ATTEN           -1.0
#define BB_MAX_REFERENCE         50.0 // dBM
#define BB_MAX_ATTENUATION       30.0 // dB

#define BB_MIN_DECIMATION        1    // 2^0
#define BB_MAX_DECIMATION        8192 // 2^13

// Gain can be integers between -1 and MAX
#define BB_AUTO_GAIN            -1
#define BB60_MAX_GAIN            3
#define BB60C_MAX_GAIN           3

#define BB_IDLE                 -1
#define BB_SWEEPING              0x0
#define BB_REAL_TIME             0x1      
#define BB_STREAMING             0x4
#define BB_RAW_PIPE              BB_STREAMING // use BB_STREAMING
#define BB_AUDIO_DEMOD           0x7
#define BB_TG_SWEEPING           0x8

#define BB_NO_SPUR_REJECT        0x0     
#define BB_SPUR_REJECT           0x1     

#define BB_LOG_SCALE             0x0
#define BB_LIN_SCALE             0x1
#define BB_LOG_FULL_SCALE        0x2
#define BB_LIN_FULL_SCALE        0x3

#define BB_RBW_SHAPE_NUTTALL     0x0
#define BB_RBW_SHAPE_FLATTOP     0x1
#define BB_RBW_SHAPE_CISPR       0x2

// Deprecated
#define BB_NATIVE_RBW            BB_RBW_SHAPE_NUTTALL
#define BB_NON_NATIVE_RBW        BB_RBW_SHAPE_FLATTOP

#define BB_MIN_AND_MAX           0x0  
#define BB_AVERAGE               0x1

#define BB_LOG                   0x0 
#define BB_VOLTAGE               0x1
#define BB_POWER                 0x2
#define BB_SAMPLE                0x3

#define BB_NUTALL                0x0
#define BB_BLACKMAN              0x1  
#define BB_HAMMING               0x2
#define BB_FLAT_TOP              0x3

#define BB_DEMOD_AM              0x0
#define BB_DEMOD_FM              0x1  
#define BB_DEMOD_USB             0x2  
#define BB_DEMOD_LSB             0x3  
#define BB_DEMOD_CW              0x4  

// Streaming flags
#define BB_STREAM_IQ             0x0
#define BB_STREAM_IF             0x1
#define BB_DIRECT_RF             0x2 // BB60C only
#define BB_TIME_STAMP            0x10

#define BB_NO_TRIGGER            0x0      
#define BB_VIDEO_TRIGGER         0x1      
#define BB_EXTERNAL_TRIGGER      0x2

#define BB_TRIGGER_RISING        0x0
#define BB_TRIGGER_FALLING       0x1

#define BB_PORT1_AC_COUPLED      0x00
#define BB_PORT1_DC_COUPLED      0x04
#define BB_PORT1_INT_REF_OUT     0x00
#define BB_PORT1_EXT_REF_IN      0x08
#define BB_PORT1_OUT_AC_LOAD     0x10
#define BB_PORT1_OUT_LOGIC_LOW   0x14
#define BB_PORT1_OUT_LOGIC_HIGH  0x1C

#define BB_PORT2_OUT_LOGIC_LOW            0x00
#define BB_PORT2_OUT_LOGIC_HIGH           0x20
#define BB_PORT2_IN_TRIGGER_RISING_EDGE   0x40
#define BB_PORT2_IN_TRIGGER_FALLING_EDGE  0x60

// TG Notify Flags
#define TG_THRU_0DB (0x1)
#define TG_THRU_20DB (0x2)

#define TG_REF_UNUSED (0)
#define TG_REF_INTERNAL_OUT (1)
#define TG_REF_EXTERNAL_IN (2) // TG124 only

typedef struct bbIQPacket {
    float *iqData; 
    int iqCount; 
    int *triggers;
    int triggerCount;
    int purge;
    int dataRemaining;
    int sampleLoss;
    int sec;
    int nano;
} bbIQPacket;

// Status Codes
// Errors are negative and suffixed with 'Err'
// Errors stop the flow of execution, warnings do not
typedef enum bbStatus {
    // Configuration Errors
    bbInvalidModeErr             = -112,
    bbReferenceLevelErr          = -111,
    bbInvalidVideoUnitsErr       = -110,
    bbInvalidWindowErr           = -109,
    bbInvalidBandwidthTypeErr    = -108,
    bbInvalidSweepTimeErr        = -107,
    bbBandwidthErr               = -106,
    bbInvalidGainErr             = -105,
    bbAttenuationErr             = -104,
    bbFrequencyRangeErr          = -103,
    bbInvalidSpanErr             = -102,
    bbInvalidScaleErr            = -101,
    bbInvalidDetectorErr         = -100,

    // General Errors
    bbLibusbError                = -18,
    bbNotSupportedErr            = -17,
    bbTrackingGeneratorNotFound  = -16,

    bbUSBTimeoutErr              = -15,
    bbDeviceConnectionErr        = -14,
    bbPacketFramingErr           = -13,
    bbGPSErr                     = -12,
    bbGainNotSetErr              = -11,
    bbDeviceNotIdleErr           = -10,
    bbDeviceInvalidErr           = -9,
    bbBufferTooSmallErr          = -8,
    bbNullPtrErr                 = -7,
    bbAllocationLimitErr         = -6,
    bbDeviceAlreadyStreamingErr  = -5,
    bbInvalidParameterErr        = -4,
    bbDeviceNotConfiguredErr     = -3,
    bbDeviceNotStreamingErr      = -2,
    bbDeviceNotOpenErr           = -1,

    // No Error
    bbNoError                    = 0,

    // Warnings/Messages
    bbAdjustedParameter          = 1,
    bbADCOverflow                = 2,
    bbNoTriggerFound             = 3,
    bbClampedToUpperLimit        = 4,
    bbClampedToLowerLimit        = 5,
    bbUncalibratedDevice         = 6,
    bbDataBreak                  = 7,
    bbUncalSweep                 = 8
} bbStatus;

#ifdef __cplusplus 
extern "C" { 
#endif

BB_API bbStatus bbGetSerialNumberList(int serialNumbers[8], int *deviceCount);
BB_API bbStatus bbOpenDeviceBySerialNumber(int *device, int serialNumber);

BB_API bbStatus bbOpenDevice(int *device); // Open the first device found
BB_API bbStatus bbCloseDevice(int device);	

BB_API bbStatus bbConfigureAcquisition(int device, unsigned int detector, unsigned int scale);    
BB_API bbStatus bbConfigureCenterSpan(int device, double center, double span);
BB_API bbStatus bbConfigureLevel(int device, double ref, double atten);
BB_API bbStatus bbConfigureGain(int device, int gain);
BB_API bbStatus bbConfigureSweepCoupling(int device, double rbw, double vbw, double sweepTime, 
                                         unsigned int rbwShape, unsigned int rejection); 
BB_API bbStatus bbConfigureProcUnits(int device, unsigned int units); 
BB_API bbStatus bbConfigureIO(int device, unsigned int port1, unsigned int port2);
BB_API bbStatus bbConfigureDemod(int device, int modulationType, double freq, float IFBW, 
                                 float audioLowPassFreq, float audioHighPassFreq, float FMDeemphasis);
BB_API bbStatus bbConfigureIQ(int device, int downsampleFactor, double bandwidth);
BB_API bbStatus bbConfigureRealTime(int device, double frameScale, int frameRate);
BB_API bbStatus bbConfigureRealTimeOverlap(int device, double advanceRate);

BB_API bbStatus bbInitiate(int device, unsigned int mode, unsigned int flag);

BB_API bbStatus bbFetchTrace_32f(int device, int arraySize, float *min, float *max);
BB_API bbStatus bbFetchTrace(int device, int arraySize, double *min, double *max);
BB_API bbStatus bbFetchRealTimeFrame(int device, float *sweep, float *frame);
BB_API bbStatus bbFetchAudio(int device, float *audio);
BB_DEPRECATED("This function is deprecated and will be removed in future versions. Contact Signal Hound for more information")
BB_API bbStatus bbFetchRaw(int device, float *buffer, int triggers[64]);
BB_API bbStatus bbGetIQ(int device, bbIQPacket *pkt);

BB_API bbStatus bbQueryTraceInfo(int device, unsigned int *traceLen, double *binSize, double *start);
BB_API bbStatus bbQueryRealTimeInfo(int device, int *frameWidth, int *frameHeight);
BB_API bbStatus bbQueryRealTimePoi(int device, double *poi);
BB_DEPRECATED("This function is deprecated and will be removed in future versions. Contact Signal Hound for more information")
BB_API bbStatus bbQueryTimestamp(int device, unsigned int *seconds, unsigned int *nanoseconds);
BB_API bbStatus bbQueryStreamInfo(int device, int *return_len, double *bandwidth, int *samples_per_sec);

BB_API bbStatus bbAbort(int device);
BB_API bbStatus bbPreset(int device);
BB_API bbStatus bbPresetFull(int *device);
BB_API bbStatus bbSelfCal(int device);
BB_API bbStatus bbSyncCPUtoGPS(int comPort, int baudRate);

BB_API bbStatus bbGetDeviceType(int device, int *type);
BB_API bbStatus bbGetSerialNumber(int device, unsigned int *sid);
BB_API bbStatus bbGetFirmwareVersion(int device, int *version);
BB_API bbStatus bbGetDeviceDiagnostics(int device, float *temperature, float *usbVoltage, float *usbCurrent);

BB_API bbStatus bbAttachTg(int device);
BB_API bbStatus bbIsTgAttached(int device, bool *attached);
BB_API bbStatus bbConfigTgSweep(int device, int sweepSize, bool highDynamicRange, bool passiveDevice);
BB_API bbStatus bbStoreTgThru(int device, int flag);
BB_API bbStatus bbSetTg(int device, double frequency, double amplitude);
BB_API bbStatus bbGetTgFreqAmpl(int device, double *frequency, double *amplitude);
BB_API bbStatus bbSetTgReference(int device, int reference);

BB_API const char* bbGetAPIVersion();
BB_API const char* bbGetProductID();
BB_API const char* bbGetErrorString(bbStatus status);

// Convenience function for converting digital 32-bit float samples to signed shorts
// dst[i] = (short)(src[i] * 2^scaleFactor)
BB_API void bbConvert_32f16s(const float *src, short *dst, int scaleFactor, int len);
// dst[i] = (float)(src[i] * 2^scaleFactor)
BB_API void bbConvert_16s32f(const short *src, float *dst, int scaleFactor, int len);

BB_DEPRECATED("This function is deprecated and will be removed in future versions. Contact Signal Hound for more information")
BB_API bbStatus bbConfigureWindow(int device, unsigned int window);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BB_API_H
