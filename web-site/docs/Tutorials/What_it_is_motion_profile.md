---
sidebar_label: What is a Motion Profile? 
sidebar_position: 1
---

# Motion Profile 🤔? 
A motion profile is a common tecnique in engineering. Basically, it provides the physical motion information and depicts how the motor should behave during a trayectory. 

Motion profiles offer a tool that make machines works faster and more efficiently. In order to work, the motion profiles require kinematic information about your system, just like velocity, acceleration, jerk, target, etc. 

There are many different motion profiles. Depending on the specific application, one of the most commun application is the **point-to-point move**.  

>For a large number of applications including medical automation, scientific instrumentation, pointing systems, and many types of general automation, the point-to-point move is used more frequently than any other motion control profile. Because of this, optimization of this profile will have the largest overall impact on system performance. Point-to-point means that from a stop, the load is accelerated to a constant velocity, and then decelerated such that the final acceleration, and velocity, are zero at the moment the load arrives at the programmed destination.

[Chuck Lewin (Mathematics of Motion Control Profiles)](https://www.pmdcorp.com/resources/type/articles/get/mathematics-of-motion-control-profiles-article).

The are two main profiles used for point-to-point applications. Both profiles are the **Trapezoidal** and **S-curve profiles**. 

**Trapezoidal**
![Trapezoidal Chart](/img/Plots_trapezoidal.png)

**S-Curve**

With MotionLight, you can create, simulate, and run motion profiles. This allows you to test different profile configurations before applying them to your specific application, reducing time and increasing productivity. 


---

## Trapezoidal
The trapezoidal profile has three phases. 

1. Acceleration
2. Constant Acceleration 
3. Desacceleration

The trapezoidal profile needs to know the maximum reachable velocity and acceleration. In this profile, the [Jerk](https://en.wikipedia.org/wiki/Jerk_(physics)) is infinite.  

>The trapezoidal motion profile is arguably the most common in motion control applications, since it forms the basis of processes that require a period of constant velocity, such as dispensing, measuring, and machining.

[Danielle Collins(What is a motion profile?)](https://www.motioncontroltips.com/what-is-a-motion-profile/)

In the first phase, the load linearly increases its velocity until reaches the maximum reacheable velocity. During the second phase, velocity remains constant with zero acceleration. Finally, in the last phase (phase 3), velocity decreases until the system comes to a full stop. 

That explains why the trapezoidal profile has the shape of a trapezoid. 
 
 **Trapezoidal Profile Demostration**
<iframe width="560" height="315" src="https://www.youtube.com/embed/WZaXgi8o368" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>


---

## S-Curve
The S-Curve is ideal for smoother motion during a trayectory. In addition, S-Curve profile offers more acurrancy in specific applications. 

Why? It´s simple: The S-Curve accounts for the amount of jerk the system experiences during the trajectory, resulting in smoother velocity and acceleration transitions.

For this profile, jerk is considered constant.

Unlike the trapezoidal profile, the S-curve has 7 phases:

1. Acceleration increases until it reaches its maximum value.
2. Acceleration remains constant at its maximum.
3. Acceleration decreases until velocity becomes constant.
4. Constant velocity, with zero acceleration.
5. Acceleration decreases to reach the maximum negative value.
6. Acceleration remains constant but negative.
7. Acceleration increases as the system approaches the stop. 

---

## Trapezoidal vs S-Curve
Both motion profiles improve smoothness, reduce wear, and lower transfer times for a broad range of motion control applications. 

Trapezoidal profile is simple and useful. But it lacks smooth transitions between acceleration phases. 

On the other hand, The S curve profile solves this issue but is a more complex to implement. 

Setting the right values for velocity, acceleration, and jerk can be tedious. However, today is your day 🥳! With MotionLight, you can simulate and test these motion profiles, saving time and effort

---