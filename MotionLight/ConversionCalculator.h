#pragma once

enum LengthUnits
{
    CENTIMETER = 0,
    KILOMETER,
    YARD,
    MILE,
    FOOT,
    INCH,
    METER
};

enum TimeUnits
{
    MILLIS=0,
    MINUTES,
    HOURS
};

enum VelocityUnits
{
    CENTIMETERS_SECOND=0,
    YARDS_SECOND,
    FOOTS_SECOND,
    INCHES_SECOND,
    KILOMETERS_HOUR,
    MILES_HOUR,
};

enum AccelerationUnits {
    CENTIMETERS_SECOND2 = 0,
    YARDS_SECOND2,
    FOOTS_SECOND2,
    INCHES_SECOND2,
    KILOMETERS_HOUR2,
    MILES_HOUR2,
};

enum JerkUnits {
    CENTIMETERS_SECOND3 = 0,
    YARDS_SECOND3,
    FOOTS_SECOND3,
    INCHES_SECOND3,
    KILOMETERS_HOUR3,
    MILES_HOUR3,
};

enum AngularUnits {
    RPM =0,
    RADS,
    DEGS
};


struct ConversionCalculator {
public: 
    float velocity;
    float acceleration;
    float length;
    float jerk;
    float time;

    float raw_velocity;
    float raw_acceleration;
    float raw_length;
    float raw_jerk;
    float raw_time;

    float raw_angular_velocity;  

    ConversionCalculator(); 

public:
    void ConvertLength(LengthUnits inputUnits);
    void ConvertTime(TimeUnits inputUnits); 
    void ConvertVelocity(VelocityUnits inputUnits);
    void ConvertAccel(AccelerationUnits inputUnits);
    void ConvertJerk(JerkUnits inputUnits);
    void ConvertRPMtoMS(AngularUnits inputUnits, float diameter);

    void PopLength(LengthUnits inputUnits);
    void PopTime(TimeUnits inputUnits);
    void PopVelocity(VelocityUnits inputUnits);
    void PopAccel(AccelerationUnits inputUnits);
    void PopJerk(JerkUnits inputUnits);

};
