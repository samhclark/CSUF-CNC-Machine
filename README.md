# CSUF-CNC-Machine
The code for the stepper motor controller (Nucleo-F767ZI) and computer interface for the 3-D Plasma CNC Machine created by Fresno State senior design students.

# CNC Control Program README

California State University, Fresno  
Lyle's College of Engineering  
Electrical and Computer Engineering Department  
ECE 186(A/B) - Senior Design(I/II)  
Fall 2018 - Spring 2019  
Plasma CNC Machine Group - Computer Engineering Team

**Instructor:**
Dr. Aaron Stillmaker

**Author:**
Sam Clark  

**Group Members:**
Jarrod Smith (Computer)  
Zachary Stewart (Computer)  
Chris Doda (Mechanical)  
Bryant Pennebaker (Mechanical)  
Elvis Pizano (Mechanical)  

## Contents

Verify that all the listed files and folders are present after extraction.
```
CNC Control Program/
|-- gui.dist/
|   |-- gui
|   |.. (many other files and folders)
|
|-- install.sh
|-- README.txt (this file)
```

## Installation


### Linux install instructions
    * Tested on Xubuntu 18.04 (recommended)
                Kubuntu 18.04

Open a terminal. The install.sh script might run by double-clicking it, but
it is likely that you will need to mark the script as executable first.
If you know how to do this graphically, you may do so. The following commands
will perform the install regardless of the installed GUI and are the
recommended procedure.  

Change directory to the extracted directory that contains this file.  
1) ```$ cd CNC\ Control\ Program/```  

Give executable permissions to install.sh  
2) ```$ chmod +x ./install.sh```  

Run install.sh  
3) ```$ ./install.sh  ```

The result will be:
    * A desktop icon that you can click to launch the program
    * A program 'cnc-control-program' that can be executed from the CLI


### Windows install instructions

The CNC Control Program will not run on Windows. Even if you get the GUI to
open up, the way Windows interacts with USB devices will prevent the program
from controlling the CNC machine.

### macOS install instructions

Not tested on macOS. However, it should run on it. Follow the same commands
as the Linux instructions.


## Usage

As of now, there is a known bug where the CNC machine must be powered on and
plugged into the control computer BEFORE the CNC Control Program starts. If
you plug in the machine after the program is running, it will not work. If you
start the program wil the machine is off, it will not work.


### Basic usage

1) Turn the CNC machine on. Give it a minute to start up and boot.
2) Plug the CNC machine into the computer with the USB cord.
3) Double-click the 'CNC Control Program' desktop icon to start the program.
4) Browse to your desired G-code file and select it.
    * Use the 'Browse' button
    * OR Enter the path directly and hit the enter key
    * OR Use 'File' > 'Open'
5) The GUI should rearrange itself to display an image of the cut superimposed
   over the cutting table.
6) If the cut and position is correct, press 'Start' to begin the cut.


### Other usage
The grid of six buttons (X,Y,Z buttons) should control the position of the
cutting head, but are finicky and don't always work. The work around is that
the machine always thinks that it boots up and (0,0,0), that is, the bottom
left corner of the displayed cut. When it is safe to do so, you can manually
move the head position and restart the machine to zero it.

The parsing code allows the image to be translated, but this feature is
unimplemented in the GUI.


## Loading program on Nucleo board
TODO: Write this part
