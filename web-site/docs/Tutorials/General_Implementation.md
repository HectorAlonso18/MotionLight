---
sidebar_label: General Implementation 
sidebar_position: 3
---
# General implementation
MotionLight can be applied across various systems to run motion profiles, so having a general solution is crucial. Below is a generalized pseudocode about how to work with the ``motion.light.txt`` files. 

For more information about ``motion.light`` file click [here](/MotionLight/docs/Tutorials/How_to_use_motion_light#the-motionlight-file)

1. You can create a function to read ``motion.light`` files
```c title="Pseudocode"
FUNCTION GetMotionLightVector(motion_file_name)

    // Open the file using the provided file name
    Open motion_file using motion_file_name

    // Check if the file is successfully opened
    IF motion_file is not open THEN
        PRINT "Error: unable to open file"
        RETURN empty vector
    END IF

    // Initialize an empty vector to store velocities
    velocities_vector = EMPTY VECTOR

    // Initialize an index to track line number
    line_index = 0

    // Read the file line by line
    WHILE there are lines left in the file DO
        Read current line

        // Ignore the first line
        IF line_index > 0 THEN
            // Convert the line to a float value
            velocity_value = Convert current line to float

            // Add the velocity value to the vector
            Append velocity_value to velocities_vector
        END IF

        // Increment the line index
        line_index = line_index + 1
    END WHILE

    // Return the velocities vector
    RETURN velocities_vector

END FUNCTION
```
2. Then just invoke the function and store the results in a vector or array. 
```c title="Pseudocode"
MAIN
    // Load the velocities from the motion light file into a vector
    velocity_vector = GetMotionLightVector("my_motion.light.txt"); 

    // Iterate through each velocity in the vector
    FOR EACH velocity IN velocity_vector
        // Move the motor using the corresponding velocity
        move_motor(velocity)
        
        // Wait for the sample time before moving to the next velocity
        // For delay time use the same value from your simulation!
        DELAY: sample time 

    ENDFOR

END
```
:::tip
Its highly recommend that you delay time be equal to the same sample time that you used in the MotionLight Simulation.
:::

3. You may want to convert the motion.light file´s velocity to a motor velocity unit, like: RPM, rad/s, etc. 
```c title="Pseudocode"
MAIN
    // Load the velocities from the motion light file into a vector
    velocity_vector = GetMotionLightVector("my_motion.light.txt"); 
    
    // Motor diameter in meters
    Diameter = 1

    // Iterate through each velocity in the vector
    FOR EACH velocity IN velocity_vector
        
        // Example: Transform velocity in m/s to motor RPMs
        motor_velocity = (velocity * 60) / (PI * Diameter)

        // Move the motor using the corresponding velocity in RPM
        move_motor(motor_velocity)
        
        // Wait for the sample time before moving to the next velocity
        // For delay time use the same value from your simulation!
        DELAY: sample time 

    ENDFOR

END
```

Good 😊, with this generic example you are able to implement the MotionLight's files to your specific implementation. However, feel free to take a look to the other tutorials to see different specific implementations. 

On the other hand, you can always copy the velocity vector from the MotionLight´s terminal to your code. 

![Terminal](/img/Terminal.png)
