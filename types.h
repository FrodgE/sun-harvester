#ifndef types_h
#define types_h

// Setting #0
typedef enum
{
    ALT_GEAR_AZI_GEAR,
    ALT_LINEAR_AZI_LINEAR,
    ALT_LINEAR_AZI_GEAR,
    ALT_GEAR_AZI_LINEAR
} altAziAssy_t;

// Setting #1
typedef enum
{
    SUN_TRACKER = 1,
    HELIOSTAT,
    SINGLE_AXIS_SUN_TRACKER
} machineType_t;

// Settings #7 and #14
typedef enum
{
    NOT_USED,
    ACUTE,
    OBTUSE
} linearAngle_t;

typedef enum
{
    ALTITUDE = 1,
    AZIMUTH
} altAz_t;

typedef enum
{
    ACTIVE_LOW,
    ACTIVE_HIGH
} activeHighLow_t;

typedef enum
{
    SLOW_ACCURATE,
    FAST_INACCURATE
} calculationSpeed_t;

#endif
