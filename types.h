#ifndef types_h
#define types_h

// Setting #0
typedef enum
{
    ALT_GEAR_AZI_GEAR,
    ALT_LINEAR_AZI_LINEAR,
    ALT_LINEAR_AZI_GEAR,
    ALT_GEAR_AZI_LINEAR,
    ALT_LINEAR2_AZI_GEAR
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

#endif
