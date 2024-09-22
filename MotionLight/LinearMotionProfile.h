#pragma once
#include <vector>

namespace LinearProfile {

    struct ProfileConstraint {
        float maxVelocity;
        float maxAcceleration;
        float maxDeceleration;
        float maxJerk;
        float distance_target; 

        ProfileConstraint(float maxVel, float maxAccel, float maxDecel, float maxJerk, float idistance);
        ProfileConstraint(float maxVel, float maxAccel, float maxJerk,float idistance);
        ~ProfileConstraint() = default;

    protected:
     
        ProfileConstraint() = default;

        friend class LinearMotionProfile;
    };

    /**
     * @brief An abstract class which acts as a base of all linear motion profile classes
     *
     */
    class LinearMotionProfile {
    protected:
        ProfileConstraint constraint;
        float distance{ 0.0 };
        bool isReversed{ false };

        std::vector<float> timePhase;
        std::vector<float> distPhase;
        std::vector<float> velPhase;
        std::vector<float> accPhase;
        std::vector<float> jerkPhase;

    public:
        /**
         * @brief Constructs a new Linear Motion Profile object
         *
         */
        LinearMotionProfile() = default;

        /**
         * @brief Destroys the Linear Motion Profile object
         *
         */
        ~LinearMotionProfile() = default;

        /**
         * @brief sets the target distance
         *
         * @param iDistance new target distance
         */
        virtual void setDistance(float iDistance) = 0;

        /**
         * @brief sets chassis constraints
         *
         * @param iConstraint new constraint
         */
        virtual void setConstraint(ProfileConstraint iConstraint) = 0;

        /**
         * @brief Gets the total time to run the profile
         *
         * @return QTime total time to run the profile
         */
        virtual float getTotalTime() const = 0;

        /**
         * @brief Gets the target position at a given time
         *
         * @param time time step to query
         * @return QLength the distance travelled at the target time
         */
        virtual float getPosition(float time) const = 0;

        /**
         * @brief Gets the target velocity at a given time
         *
         * @param time time step to query
         * @return QSpeed target velocity at the target time
         */
        virtual float getVelocity(float time) const = 0;

        /**
         * @brief Gets the target acceleration at a given time
         *
         * @param time time step to query
         * @return target acceleration at the target time
         */
        virtual float getAcceleration(float time) const = 0;

        
    };

 
    class TrapezoidalMotionProfile : public LinearMotionProfile {
    private:
        float min3Stage = 0;

    public:
        TrapezoidalMotionProfile(ProfileConstraint iConstraint);
        TrapezoidalMotionProfile();
        ~TrapezoidalMotionProfile() = default;

        void setDistance(float iDistance) override;
        void setConstraint(ProfileConstraint iConstraint) override;

        float getTotalTime() const override;
        float getPosition(float time) const override;
        float getVelocity(float time) const override;
        float getAcceleration(float time) const override;
    
    };

 
    class SCurveMotionProfile : public LinearMotionProfile {
        float fullDist = 0 ;
        float minDist = 0;
        bool fullAccel{ true };

    public:
        SCurveMotionProfile(ProfileConstraint iConstraint);
        SCurveMotionProfile() = default;
        ~SCurveMotionProfile() = default;

        void setDistance(float iDistance) override;
        void setConstraint(ProfileConstraint iConstraint) override;

        float getTotalTime() const override;
        float getPosition(float time) const override;
        float getVelocity(float time) const override;
        float getAcceleration(float time) const override;

    };


}
        
