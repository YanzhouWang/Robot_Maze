# EMAE 488 Course Project--Robot Maze
(Description goes here)

## Robot_Maze_Code
This code currently takes direction data from an XBee-based controller and writes the command into a .txt file in a SD card. Once a directional command is received, the correponding motors turn in a predetermined direction. The command data is then written in a text file (RECORD.TXT).

The recorded directions is then analyzed to combine directions and turn durations in the same and opposite directions, as a means to eliminate redundant commands. The optimized path is then written in another text file (ANALYZED.TXT) for further processing.

The program finally reads the analyzed commands record and their duration information, and passes it to the motor, essentially recreating the past path automatically without redundancy.

## Erase_Everything
An utility script to remove data from the SD card without having to unplug it from the Arduino SD breakout

## File_Read
Code for reading two integer data separated by a comma in a text file stored in a SD card.

## Glove_Control
Code for sending commands for the robot car.
