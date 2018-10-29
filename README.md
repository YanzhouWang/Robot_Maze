# Robot_Maze_Code
This code currently takes direction data from an XBee-based controller and writes the command into a .txt file in a SD card. Once a directional command is received, the correponding motors turn in a predetermined direction. The command data is then written in a text file (RECORD.TXT).

The recorded directions is then analyzed to combine directions and turn durations in the same and opposite directions, as a means to eliminate redundant commands. The optimized path is then written in another text file (ANALYZED.TXT) for further processing.

The program finally reads the analyzed commands record and their duration information, and passes it to the motor, essentially recreating the past path automatically without redundancy.
