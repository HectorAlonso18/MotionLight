---
sidebar_label: Implementations in Different Programming Language
sidebar_position: 4
---

# Implementation in Different Programming Language

In this tutorial, you will see various implementations of MotionLight using different programming languages. The goal is to provide dynamic examples of how to work with the ``motion.light`` files, showcasing the versatility of MotionLight across different coding environments.

For more information about the ``motion.light`` files click [here](/MotionLight/docs/Tutorials/How_to_use_motion_light#the-motionlight-file).

--- 

## C++ 
### With a function
The first approach in ``C++`` involves creating a function that returns a velocity vector from the ``motion.light`` file and stores it in a new vector. This function allows for easy access to the velocity data for further processing or simulation. Below is a basic implementation:

```cpp title="main.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

std::vector<float> get_motion_light_vector(const char* motion_file_name) {
    std::ifstream motion_file(motion_file_name); 
    if (!motion_file.is_open()) {
        std::cerr << "Error opening the file" << "\n";
        std::cerr << "Are you sure that the file is on your project?" << "\n";
        return { 0 };
    }

    std::string line; 
    std::vector<float> velocities; 
    int index = 0; 

    while (std::getline(motion_file, line, '\n')) {
        //Ignoring the first line
        if (index > 0) {
            velocities.push_back(std::stod(line));
        }
        index++;
    }

    return velocities; 
}

int main(){
    std::vector<float> my_velocities = get_motion_light_vector("motion.light.txt"); 

    for (int i = 0; i < my_velocities.size(); i++) {
        std::cout << "Vel [" << i << "]: " << my_velocities[i]<<"\n";
    }
}
```
### Using struct/class
This other example is using a struct to interact with the ``motion.light`` files and store the information in variables.
In this example, we are reading the first line of the file to take the motion profile parameters. 

```cpp title= "main.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


enum LinearProfiler {
    TRAPEZOIDAL,
    SCURVE
};

struct Profile {
    LinearProfiler profiler_type=TRAPEZOIDAL; 
    float MaxVelocity=0; 
    float MaxAcceleration=0; 
    float MaxDesacceleration=0; 
    float MaxJerk=0; 
    float TargetDistance=0; 
    float SampleTimeSec=0; 
    std::vector<float> Velocities; 

    void ReadMotionLightFile(const char* motion_file_name) {
        std::ifstream motion_file(motion_file_name);
        if (!motion_file.is_open()) {
            std::cerr << "Error opening the file" << "\n";
            std::cerr << "Are you sure that the file is on your project?" << "\n";
        }

        std::string line;
        std::vector<float> file_velocities;
        std::vector<float> parameters; 
 

        for (int i = 0; i<6; i++) {
            std::getline(motion_file, line, ',');
            parameters.push_back(std::stod(line));
        }

        std::getline(motion_file, line); 
        parameters.push_back(std::stod(line));
        
        
        while (std::getline(motion_file, line, '\n')) {
            file_velocities.push_back(std::stod(line));
        }

        profiler_type = static_cast<LinearProfiler>(parameters[0]); 
        MaxVelocity = parameters[1];
        MaxAcceleration = parameters[2];
        MaxDesacceleration = parameters[3];
        MaxJerk = parameters[4];
        TargetDistance = parameters[5];
        SampleTimeSec = parameters[6];
        this->Velocities = file_velocities; 

    }

}; 

int main(){
    //Creating the object
    Profile my_profiler;

    my_profiler.ReadMotionLightFile("motion.light.txt"); 

    
    std::cout << "\nProfiler type: " << my_profiler.profiler_type; 
    std::cout << "\nMax Velocity: " << my_profiler.MaxVelocity; 
    std::cout << "\nMax Acceleration: " << my_profiler.MaxAcceleration;
    std::cout << "\nMax Desacceleration: " << my_profiler.MaxDesacceleration;
    std::cout << "\nMax Jerk: " << my_profiler.MaxJerk;
    std::cout << "\nDistance: " << my_profiler.TargetDistance; 
    std::cout << "\n Sample time: " << my_profiler.SampleTimeSec; 
    
 
    for (int i = 0; i < my_profiler.Velocities.size(); i++) {
        std::cout <<"\n Vel [" << i << "]: " << my_profiler.Velocities[i];
    }

    return 0; 
}
```

---

## Java
The first approach in Java involves creating a method that returns a velocity vector from the motion.light file and stores it in a new list. This method allows for easy access to the velocity data for further processing or simulation. Below is a basic implementation:

``` js
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class Main {
    public static List<Float> getMotionLightVector(String motionFileName) {
        List<Float> velocities = new ArrayList<>();

        try (BufferedReader br = new BufferedReader(new FileReader(motionFileName))) {
            String line;
            int index = 0;

            while ((line = br.readLine()) != null) {
                // Ignoring the first line
                if (index > 0) {
                    velocities.add(Float.parseFloat(line));
                }
                index++;
            }
        } catch (IOException e) {
            System.err.println("Error opening the file");
            System.err.println("Are you sure that the file is in your project?");
            return new ArrayList<>();
        }

        return velocities;
    }

    public static void main(String[] args) {
        List<Float> myVelocities = getMotionLightVector("motion.light.txt");

        for (int i = 0; i < myVelocities.size(); i++) {
            System.out.println("Vel [" + i + "]: " + myVelocities.get(i));
        }
    }
}

```
---

## Python
In ``Python`` we can create a list to store the velocities from ``motion.light`` files. 

```python
def get_motion_light_vector(motion_file_name):
    velocities = []
    
    try:
        with open(motion_file_name, 'r') as motion_file:
            for index, line in enumerate(motion_file):
                # Ignoring the first line
                if index > 0:
                    velocities.append(float(line.strip()))
    except IOError:
        print("Error opening the file")
        print("Are you sure that the file is in your project?")
        return [0]

    return velocities


my_velocities = get_motion_light_vector("motion.light.txt")

for velocity in my_velocities: 
    print(velocity)

```