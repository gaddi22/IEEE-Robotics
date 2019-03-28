Overview
===

This repository is a collection of work done to develop a robot to accomplish a task.  
The robot is required to:
* Work entirely autonomously.  
* Find and locate blocks in an arena.  
* Identify blocks by letter printed on them in black text.  
* Return the blocks to a location and sort by letter. Location is random.  
* Avoid objects, other than blocks

We are combining an Arduino Uno and Raspberry Pi to accomplish this task and managing state between the 2 to send data back and forth.  
The Arduino will be regarded as the master controller, while the Pi will operate as a slave controller used primarily for image interpretation using Pytesseract.

Arduino
---

A collection of Arduino code for the robot.

**Subdirectories:**

* main  
Main code for arduino. Implemented code will be consolidated here. We will pull code from other places, such
as 'Resources' to develop this code. This code is the official production code for our robot

* Resources
Code that is either in testing or used for reference while implementing into main

Pi
---

A collection of Pi code for the robot. Written exclusively in Python.

The most notable programs are piReadWrite and State.

* **piReadWrite**  
keeps the pi informed of who's turn it is to have access to the serial line. Manages 
state and read/writes data from and to the arduino

*Needs file to be read by json parser to be located at './resources/file.json'*

* **State**  
Holds the state value as an object for piReadWrite


Programming guideline
---

Rough outline of tasks needed to be completed in order to accomplish the goals outlined in overview

TODO.txt
---

Tasks that need attention
