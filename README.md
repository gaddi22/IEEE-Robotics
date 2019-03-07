Overview
===

This repository is a collection of work done to develop a robot to accomplish a task.  
The robot is required to:
* Work entirely autonomously.  
* Find and locate blocks in an arena.  
* Identify blocks by letter printed on them in black text.  
* Return the blocks to a location and sort by letter. Location is random.  
* Avoid objects, other than blocks

We are combining an Arduino Uno and Raspberry Pi to accomplish this task and managing state between the too to send data back and forth.  
The Arduino will be regarded as the master controller, while the Pi will operate as a slave controller used primarily for image interpretation using Pytesseract.

Arduino
---

A collection of Arduino code for the robot

Pi
---

A collection of Pi code for the robot. Written exclusively in Python

Sensor_Sharp
---

Arduino program that uses a light sensor to determine distance

Programming guideline
---

Rough outline of tasks needed to be completed in order to accomplish the goals outlined in overview

TODO.txt
---

Tasks that need attention
