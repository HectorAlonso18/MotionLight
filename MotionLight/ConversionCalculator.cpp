#include "ConversionCalculator.h"
#include <iostream>
#ifndef PI
#define PI 3.14159265358979323846
#endif

ConversionCalculator::ConversionCalculator():
    velocity(1.0f), acceleration(1.0f), jerk(1.0f), length(1.0f), time(1.0f), raw_velocity(1.0f), raw_acceleration(1.0f), raw_length(1.0f),
raw_jerk(1.0f),raw_time(1.0f),raw_angular_velocity(1.0f){


}

void ConversionCalculator::ConvertLength(LengthUnits inputUnits) {
    switch (inputUnits)
    {
    case CENTIMETER:
        this->length = this->raw_length* .01f; 
        break;
    case KILOMETER:
        this->length =  this->raw_length*1000.0f; 
        break;
    case YARD:
        this->length = this->raw_length *.9144f; 
        break;
    case MILE:
        this->length = this->raw_length * 1609.35f;
        break;
    case FOOT:
        this->length = this->raw_length * .3048f;
        break;
    case INCH:
        this->length = this->raw_length * .0254f;
        break;
    default:
        this->length = raw_length; 
        break;
    }
}

void ConversionCalculator::PopLength(LengthUnits inputUnits){
    switch (inputUnits)
    {
    case CENTIMETER:
        this->raw_length = this->length / .01f;
        break;
    case KILOMETER:
        this->raw_length = this->length / 1000.0f;
        break;
    case YARD:
        this->raw_length = this->length / .9144f;
        break;
    case MILE:
        this->raw_length = this->length / 1609.35f;
        break;
    case FOOT:
        this->raw_length = this->length / .3048f;
        break;
    case INCH:
        this->raw_length = this->length / .0254f;
        break;
    default:
        break;
    }
  
}

void ConversionCalculator::ConvertTime(TimeUnits inputUnits) {
    switch (inputUnits)
    {
    case MILLIS:
        this->time = this->raw_time / 1000.0f; 
        break;
    case HOURS:
        this->time = this->raw_time * 3600.0f; 
        break;
    case MINUTES:
        this->time = this->raw_time * 60.0f; 
        break;
    default:
        break;
    }
}

void ConversionCalculator::PopTime(TimeUnits inputUnits) {
    switch (inputUnits)
    {
    case MILLIS:
        this->raw_time = this->time * 1000.0f;
        break;
    case HOURS:
        this->raw_time = this->time / 3600.0f;
        break;
    case MINUTES:
        this->raw_time = this->time / 60.0f;
        break;
    default:
        break;
    }
}
void ConversionCalculator::ConvertVelocity(VelocityUnits inputUnits) {
    switch (inputUnits)
    {
    case CENTIMETERS_SECOND:
        this->velocity = raw_velocity * .01f; 
        break;
    case YARDS_SECOND:
        this->velocity = raw_velocity * .91444f; 
        break;
    case FOOTS_SECOND:
        this->velocity = raw_velocity * .3048f; 
        break;
    case INCHES_SECOND:
        this->velocity = raw_velocity * .0254f; 
        break;
    case KILOMETERS_HOUR:
        this->velocity = (raw_velocity * 1000.0f) / 3600.0f; 
        break;
    case MILES_HOUR:
        this->velocity = (raw_velocity * 1609.35f) / 3600.0f;
        break;
    default:
        break;
    }

}

void ConversionCalculator::PopVelocity(VelocityUnits inputUnits) {
    switch (inputUnits)
    {
    case CENTIMETERS_SECOND:
        this->raw_velocity = velocity / .01f;
        break;
    case YARDS_SECOND:
        this->raw_velocity = velocity / .91444f;
        break;
    case FOOTS_SECOND:
        this->raw_velocity = velocity / .3048f;
        break;
    case INCHES_SECOND:
        this->raw_velocity = velocity / .0254f;
        break;
    case KILOMETERS_HOUR:
        this->raw_velocity =  (velocity*3600.0f)/1000.0f;
        break;
    case MILES_HOUR:
        this->raw_velocity = (velocity*3600.0f)/1609.35f;
        break;
    default:
        break;
    }
}

void ConversionCalculator::ConvertAccel(AccelerationUnits inputUnits) {
    switch (inputUnits)
    {
    case CENTIMETERS_SECOND2:
        this->acceleration = raw_acceleration * .01f;
        break;
    case YARDS_SECOND2:
        this->acceleration = raw_acceleration * .91444f;
        break;
    case FOOTS_SECOND2:
        this->acceleration = raw_acceleration * .3048f;
        break;
    case INCHES_SECOND2:
        this->acceleration = raw_acceleration * .0254f;
        break;
    case KILOMETERS_HOUR2:
        this->acceleration = (raw_acceleration * 1000.0f) / (3600.0f*3600.0f);
        break;
    case MILES_HOUR2:
        this->acceleration = (raw_acceleration * 1609.35f) / (3600.0f*3600.0f);
        break;
    default:
        break;
    }
}

void ConversionCalculator::PopAccel(AccelerationUnits inputUnits) {
    switch (inputUnits)
    {
    case CENTIMETERS_SECOND2:
        this->raw_acceleration = acceleration / .01f;
        break;
    case YARDS_SECOND2:
        this->raw_acceleration = acceleration / .91444f;
        break;
    case FOOTS_SECOND2:
        this->raw_acceleration = acceleration / .3048f;
        break;
    case INCHES_SECOND2:
        this->raw_acceleration = acceleration / .0254f;
        break;
    case KILOMETERS_HOUR2:
        this->raw_acceleration = (acceleration*3600.0f*3600.0f)/1000.0f;
        break;
    case MILES_HOUR2:
        this->raw_acceleration = (acceleration*3600.0f*3600.0f) / 1609.35f;
        break;
    default:
        break;
    }
}

void ConversionCalculator::ConvertJerk(JerkUnits inputUnits) {
    switch (inputUnits)
    {
    case CENTIMETERS_SECOND3:
        this->jerk = raw_jerk * .01f;
        break;
    case YARDS_SECOND3:
        this->jerk = raw_jerk * .91444f;
        break;
    case FOOTS_SECOND3:
        this->jerk = raw_jerk * .3048f;
        break;
    case INCHES_SECOND3:
        this->jerk = raw_jerk * .0254f;
        break;
    case KILOMETERS_HOUR3:
        this->jerk = (raw_jerk * 1000.0f) / (3600.0f*3600.0f*3600.0f);
        break;
    case MILES_HOUR3:
        this->jerk = (raw_jerk * 1609.35f) / (3600.0f*3600.0f*3600.0f);
        break;
    default:
        break;
    }
}

void ConversionCalculator::PopJerk(JerkUnits inputUnits) {
    switch (inputUnits)
    {
    case CENTIMETERS_SECOND3:
        this->raw_jerk = jerk / .01f;
        break;
    case YARDS_SECOND3:
        this->raw_jerk = jerk / .91444f;
        break;
    case FOOTS_SECOND3:
        this->raw_jerk = jerk / .3048f;
        break;
    case INCHES_SECOND3:
        this->raw_jerk = jerk / .0254f;
        break;
    case KILOMETERS_HOUR3:
        this->raw_jerk = (jerk *3600.0f * 3600.0f * 3600.0f) / 1000.0f;
        break;
    case MILES_HOUR3:
        this->raw_jerk = (jerk * 3600.0f * 3600.0f * 3600.0f) / 1609.35f  ;
        break;
    default:
        break;
    }
}

void ConversionCalculator::ConvertRPMtoMS(AngularUnits inputUnits, float diameter) {
    switch (inputUnits)
    {
    case RPM: this->velocity = (raw_angular_velocity * PI * diameter) / 60.0f; 
        break;
    case RADS:
        this->velocity = raw_angular_velocity * (diameter / 2.0f); 
        break;
    case DEGS:
        this->velocity = (raw_angular_velocity * PI * diameter) / 360.0f; 
        break;
    default:
        this->velocity = 0; 
        break;
    }
}
