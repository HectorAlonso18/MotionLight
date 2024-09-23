#include "LinearMotionProfile.h"
#include "Math.h"
#include <iostream>

struct QuadraticResult {
    float first;
    float second;
};

QuadraticResult quadraticFormula(float a, float b, float c) {
    QuadraticResult result;
    float discriminant = b * b - 4 * a * c;

    if (discriminant > 0.0f) {
        result.first = (-b + sqrt(discriminant)) / (2 * a);
        result.second = (-b - sqrt(discriminant)) / (2 * a);
    }
    else if (discriminant == 0.0f) {
        result.first = -b / (2 * a);
        result.second = result.first; // Both roots are the same for a zero discriminant
    }
    else {
        // Complex roots, set them to NaN (Not a Number)
        result.first = std::numeric_limits<float>::quiet_NaN();
        result.second = std::numeric_limits<float>::quiet_NaN();
    }

    return result;
}

namespace LinearProfile {

    ProfileConstraint::ProfileConstraint(float maxVel, float maxAccel, float maxDecel, float maxJerk, float idistance) {
        this->maxVelocity = abs(maxVel);
        this->maxAcceleration = abs(maxAccel);
        this->maxDeceleration = abs(maxDecel);
        this->maxJerk = abs(maxJerk);
        this->distance_target = idistance; 
    }

    TrapezoidalMotionProfile::TrapezoidalMotionProfile() {

    }

    TrapezoidalMotionProfile::TrapezoidalMotionProfile(ProfileConstraint iConstraint) {
        this->distPhase.resize(3);
        this->timePhase.resize(3);
        this->velPhase.resize(3);
        this->accPhase.resize(3);
        this->setConstraint(iConstraint);

    }

    void TrapezoidalMotionProfile::setConstraint(ProfileConstraint iConstraint) {
        constraint = iConstraint;
        min3Stage = constraint.maxVelocity * constraint.maxVelocity / constraint.maxAcceleration / 2 +
            constraint.maxVelocity * constraint.maxVelocity / constraint.maxDeceleration / 2;
        accPhase = { constraint.maxAcceleration, 0, -constraint.maxAcceleration };
        setDistance(constraint.distance_target);
    }

    void TrapezoidalMotionProfile::setDistance(float iDistance) {
        isReversed = (iDistance < 0.0f );
        distance = abs(iDistance);

        if (distance < min3Stage) { // 2 stage
            timePhase[0] = sqrt(distance / constraint.maxAcceleration);
            distPhase[0] = distance / 2.0f;
            velPhase[0] = 0.0f ;

            timePhase[1] = 0.0f;
            distPhase[1] = 0.0f;
            velPhase[1] = timePhase[0] * constraint.maxAcceleration;

            timePhase[2] = timePhase[0];
            distPhase[2] = distPhase[0];
            velPhase[2] = velPhase[1];
        }
        else { // full trapezoidal profile
            timePhase[0] = constraint.maxVelocity / constraint.maxAcceleration;
            distPhase[0] = 0.5f * constraint.maxVelocity * constraint.maxVelocity / constraint.maxAcceleration;
            velPhase[0] = 0.0f;

            timePhase[1] = (distance - distPhase[0] * 2.0f) / constraint.maxVelocity;
            distPhase[1] = constraint.maxVelocity * timePhase[1];
            velPhase[1] = constraint.maxVelocity;

            timePhase[2] = timePhase[0];
            distPhase[2] = distPhase[0];
            velPhase[2] = velPhase[1];
        }

        for (int i = 1; i < 3; i++) {
            timePhase[i] += timePhase[i - 1];
            distPhase[i] += distPhase[i - 1];
        }
    }

    float TrapezoidalMotionProfile::getTotalTime() const {
        return timePhase[2];
    }

    float TrapezoidalMotionProfile::getVelocity(float time) const {
        float ret;
        if (time < 0.0f || time > timePhase[2]) {
            ret = 0.0f;
        }
        else if (time < timePhase[0]) {
            ret = constraint.maxAcceleration * time;
        }
        else if (time > timePhase[1]) {
            ret = constraint.maxAcceleration * (timePhase[2] - time);
        }
        else {
            ret = constraint.maxVelocity;
        }

        return isReversed ? -ret : ret;
    }

    float TrapezoidalMotionProfile::getAcceleration(float time) const {
        float ret;
        if (time < 0.0f || time > timePhase[2]) {
            ret = 0.0f;
        }
        else if (time < timePhase[0]) {
            ret = constraint.maxAcceleration;
        }
        else if (time > timePhase[1]) {
            ret = -1.0f * constraint.maxAcceleration;
        }
        else {
            ret = 0.0f ;
        }

        return isReversed ? -1 * ret : ret;
    }

    float TrapezoidalMotionProfile::getPosition(float time) const {
        float ret;
        if (time < 0.0f ) {
            ret = 0.0f ;
        }
        else if (time > timePhase[2]) {
            ret = distance;
        }
        else if (time < timePhase[0]) {
            ret = 0.5f * constraint.maxAcceleration * time * time;
        }
        else if (time > timePhase[1]) {
            float dTime = time - timePhase[1];
            ret = distPhase[1] + velPhase[2] * dTime + 0.5f * accPhase[2] * dTime * dTime;
        }
        else {
            ret = distPhase[0] + constraint.maxVelocity * (time - timePhase[0]);
        }

        return isReversed ? -1 * ret : ret;
    }

    SCurveMotionProfile::SCurveMotionProfile(ProfileConstraint iConstraint) {
        timePhase.resize(7);
        distPhase.resize(7);
        velPhase.resize(7);
        accPhase.resize(7);
        setConstraint(iConstraint);
    }

    void SCurveMotionProfile::setConstraint(ProfileConstraint iConstraint) {
        constraint = iConstraint;
        jerkPhase = { iConstraint.maxJerk, 0.0f, -iConstraint.maxJerk, 0.0f , -iConstraint.maxJerk, 0 , iConstraint.maxJerk };

        float time = constraint.maxAcceleration / constraint.maxJerk;
        if (constraint.maxJerk * time * time >= constraint.maxVelocity) {
            fullAccel = false;
            float t1 = sqrt(constraint.maxVelocity / constraint.maxJerk);
            minDist = constraint.maxJerk * t1 * t1 * t1 * 2.0f;
            fullDist = minDist;
        }
        else {
            fullAccel = true;
            float t1 = constraint.maxAcceleration / constraint.maxJerk;
            minDist = constraint.maxJerk * t1 * t1 * t1 * 2.0f;

            float t2 = (constraint.maxVelocity - (constraint.maxJerk * t1 * t1)) / constraint.maxAcceleration;
            fullDist = (0.5f * constraint.maxJerk * t1 * t1) * t2 + 0.5f * (constraint.maxAcceleration) * t2 * t2;
            fullDist += constraint.maxVelocity * t1;
            fullDist *= 2.0f;
        }

        setDistance(constraint.distance_target);
    }

    void SCurveMotionProfile::setDistance(float iDistance) {
        isReversed = iDistance < 0.0f;
        distance = abs(iDistance);

        if (distance < minDist) { // 4 stage
            timePhase[1] = timePhase[3] = timePhase[5] = 0.0f ;
            timePhase[0] = timePhase[2] = timePhase[4] = timePhase[6] = cbrt(distance / constraint.maxJerk / 2.0f);

            distPhase[0] = distPhase[6] = timePhase[0] * timePhase[0] * timePhase[0] * constraint.maxJerk / 6.0f;
            distPhase[1] = distPhase[3] = distPhase[5] = 0.0f ;
            distPhase[2] = distPhase[4] = 0.5f * distance - distPhase[0];

            velPhase[0] = 0.0f ;
            velPhase[1] = velPhase[2] = velPhase[5] = velPhase[6] = 0.5f * constraint.maxJerk * timePhase[0] * timePhase[0];
            velPhase[3] = velPhase[4] = velPhase[1] * 2.0f;

            accPhase[0] = accPhase[3] = accPhase[4] = 0.0f;
            accPhase[1] = accPhase[2] = constraint.maxJerk * timePhase[0];
            accPhase[5] = accPhase[6] = -accPhase[1];
        }
        else if (!fullAccel) { // 5 stage
            timePhase[0] = timePhase[2] = timePhase[4] = timePhase[6] = sqrt(constraint.maxVelocity / constraint.maxJerk);
            timePhase[1] = timePhase[5] = 0.0f;
            timePhase[3] = (distance - constraint.maxVelocity * timePhase[0] * 2.0f) / constraint.maxVelocity;

            distPhase[0] = distPhase[6] = timePhase[0] * timePhase[0] * timePhase[0] * constraint.maxJerk / 6.0f;
            distPhase[1] = distPhase[5] = 0.0f;
            distPhase[2] = distPhase[4] = constraint.maxVelocity * timePhase[0] - distPhase[0];
            distPhase[3] = distance - distPhase[0] * 2.0f - distPhase[2] * 2.0f;

            velPhase[0] = 0.0f ;
            velPhase[1] = velPhase[2] = velPhase[5] = velPhase[6] = constraint.maxVelocity / 2.0f;
            velPhase[3] = velPhase[4] = constraint.maxVelocity;

            accPhase[0] = accPhase[3] = accPhase[4] = 0.0f;
            accPhase[1] = accPhase[2] = constraint.maxJerk * timePhase[0];
            accPhase[5] = accPhase[6] = -accPhase[1];
        }
        else if (distance < fullDist) { // 6 stage
            float a = constraint.maxAcceleration;
            float b = (3.0f * constraint.maxAcceleration * constraint.maxAcceleration / constraint.maxJerk);
            float c = (2.0f * constraint.maxAcceleration * constraint.maxAcceleration * constraint.maxAcceleration / constraint.maxJerk / constraint.maxJerk - distance);
            auto t2Candidate = quadraticFormula(a, b, c);

            timePhase[0] = timePhase[2] = timePhase[4] = timePhase[6] = constraint.maxAcceleration / constraint.maxJerk;
            timePhase[1] = timePhase[5] = std::max(t2Candidate.first, t2Candidate.second);
            timePhase[3] = 0.0f ;

            accPhase[0] = accPhase[3] = accPhase[4] = 0.0f;
            accPhase[1] = accPhase[2] = constraint.maxJerk * timePhase[0];
            accPhase[5] = accPhase[6] = -accPhase[1];

            velPhase[0] = 0.0f;
            velPhase[1] = velPhase[6] = 0.5f * constraint.maxJerk * timePhase[0] * timePhase[0];
            velPhase[2] = velPhase[5] = velPhase[1] + constraint.maxAcceleration * timePhase[1];
            velPhase[3] = velPhase[4] = velPhase[2] + accPhase[2] * timePhase[2] - 0.5f * constraint.maxJerk * timePhase[2] * timePhase[2];

            distPhase[0] = distPhase[6] = constraint.maxJerk * timePhase[0] * timePhase[0] * timePhase[0] / 6.0f;
            distPhase[1] = distPhase[5] = velPhase[1] * timePhase[1] + 0.5f * accPhase[1] * timePhase[1] * timePhase[1];
            distPhase[2] = distPhase[4] = velPhase[2] * timePhase[2] + 0.5f * accPhase[2] * timePhase[2] * timePhase[2] - constraint.maxJerk * timePhase[2] * timePhase[2] * timePhase[2] / 6.0f;
            distPhase[3] = 0.0f;
        }
        else { // full s curve
            velPhase[0] = 0.0f;
            accPhase[0] = 0.0f;
            timePhase[0] = constraint.maxAcceleration / constraint.maxJerk;
            distPhase[0] = constraint.maxJerk * timePhase[0] * timePhase[0] * timePhase[0] / 6.0f;

            velPhase[1] = 0.5f * constraint.maxJerk * timePhase[0] * timePhase[0];
            accPhase[1] = constraint.maxAcceleration;
            timePhase[1] = (constraint.maxVelocity - velPhase[1] * 2.0f) / constraint.maxAcceleration;
            distPhase[1] = velPhase[1] * timePhase[1] + 0.5f * accPhase[1] * timePhase[1] * timePhase[1];

            velPhase[2] = velPhase[1] + accPhase[1] * timePhase[1];
            accPhase[2] = constraint.maxAcceleration;
            timePhase[2] = timePhase[0];
            distPhase[2] = velPhase[2] * timePhase[2] + 0.5f * accPhase[2] * timePhase[2] * timePhase[2] - constraint.maxJerk * timePhase[2] * timePhase[2] * timePhase[2] / 6.0f;

            velPhase[3] = constraint.maxVelocity;
            accPhase[3] = 0.0f;
            timePhase[3] = (distance - 2.0f * (distPhase[0] + distPhase[1] + distPhase[2])) / constraint.maxVelocity;
            distPhase[3] = velPhase[3] * timePhase[3];

            velPhase[4] = constraint.maxVelocity;
            accPhase[4] = 0.0f;
            timePhase[4] = timePhase[2];
            distPhase[4] = distPhase[2];

            velPhase[5] = velPhase[2];
            accPhase[5] = -constraint.maxAcceleration;
            timePhase[5] = timePhase[1];
            distPhase[5] = distPhase[1];

            velPhase[6] = velPhase[1];
            accPhase[6] = -constraint.maxAcceleration;
            timePhase[6] = timePhase[0];
            distPhase[6] = distPhase[0];
        }

        for (int i = 1; i < 7; i++) {
            timePhase[i] += timePhase[i - 1];
            distPhase[i] += distPhase[i - 1];
        }
    }

    float SCurveMotionProfile::getTotalTime() const {
        return timePhase[6];
    }

    float SCurveMotionProfile::getPosition(float time) const {
        float pos = 0.0f ;

        if (time < 0.0f ) {
            pos = 0.0f ;
        }
        else if (time >= timePhase[6]) {
            pos = distance;
        }
        else if (time < timePhase[0]) {
            pos = constraint.maxJerk * time * time * time / 6.0f;
        }
        else {
            int index = 1;
            for (int i = 1; i < 7; i++) {
                if (time < timePhase[i]) {
                    index = i;
                    break;
                }
            }
            float dTime = time - timePhase[index - 1];
            pos = distPhase[index - 1] + velPhase[index] * dTime + 0.5f * accPhase[index] * dTime * dTime + jerkPhase[index] * dTime * dTime * dTime / 6.0f;
        }

        return isReversed ? -pos : pos;
    }

    float SCurveMotionProfile::getVelocity(float time) const {
        float vel = 0.0f ;

        if (time < 0.0f  || time >= timePhase[6]) {
            vel = 0.0f ;
        }
        else if (time < timePhase[0]) {
            vel = 0.5f * constraint.maxJerk * time * time;
        }
        else {
            int index = 1;
            for (int i = 1; i < 7; i++) {
                if (time < timePhase[i]) {
                    index = i;
                    break;
                }
            }
            float dTime = time - timePhase[index - 1];
            vel = velPhase[index] + accPhase[index] * dTime + jerkPhase[index] * dTime * dTime * 0.5f;
        }

        return isReversed ? -vel : vel;
    }

    float SCurveMotionProfile::getAcceleration(float time) const {
        float acc = 0.0f ;

        if (time < 0  || time >= timePhase[6]) {
            acc = 0.0f;
        }
        else if (time < timePhase[0]) {
            acc = constraint.maxJerk * time;
        }
        else {
            int index = 1;
            for (int i = 1; i < 7; i++) {
                if (time < timePhase[i]) {
                    index = i;
                    break;
                }
            }
            float dTime = time - timePhase[index - 1];
            acc = accPhase[index] + jerkPhase[index] * dTime;
        }

        return isReversed ? -acc : acc;
    }


}

