/*
 * Copyright 2012, Intel Corporation
 *
 * This program file is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program in a file named COPYING; if not, write to the
 * Free Software Foundation, Inc,
 * 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 * or just google for it.
 *
 * Authors:
 *  Chen Guobing <guobing.chen@intel.com>
 */

#include "eventHunter.h"
#include <iostream>
#include <cstring>
#include <cstdlib>


void printUsage();

int main(int argc, char ** argv)
{
    unsigned    step = 2;
    unsigned    stepCount = 100;
    int         speedFactor = 1;
    int         posX = 450;
    int         posY = 450;
    
    int         logLevel = 1;
    unsigned    delay = 5000; // ms
    char        eventInterface[64] = {'\0'};
    unsigned    gesture = EventHunter::G_SPREAD; 
    unsigned    orientation = EventHunter::BREADTHWISE;
    bool        isWrite = false;
    bool        isInterfaceSet = false;
    const std::string version = "1.00";

    std::string temp;

    for (int index=1; index < argc; index++) {
        temp = argv[index];
        if (temp[0] == '-' && temp.length()>1) {
            switch (temp[1]) {
                case 'i':
                    if (index +1 < argc) {
                        strncpy(eventInterface, argv[index+1], 64);
                        isInterfaceSet = true;
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'g':
                    if (index +1 < argc) {
                        isWrite = true;
                        if (strcmp(argv[index+1], "PAN")==0) {
                            gesture = EventHunter::G_PAN; 
                        } else if (strcmp(argv[index+1], "SWIPE")==0) {
                            gesture = EventHunter::G_SWIPE; 
                        } else if (strcmp(argv[index+1], "FLICK")==0) {
                            gesture = EventHunter::G_FLICK; 
                        } else if (strcmp(argv[index+1], "PINCH")==0) {
                            gesture = EventHunter::G_PINCH; 
                        } else if (strcmp(argv[index+1], "SPREAD")==0) {
                            gesture = EventHunter::G_SPREAD; 
                        } else if (strcmp(argv[index+1], "SWIPING")==0) {
                            gesture = EventHunter::G_SWIPING; 
                        } else if (strcmp(argv[index+1], "FLICKING")==0) {
                            gesture = EventHunter::G_FLICKING; 
                        } else if (strcmp(argv[index+1], "ZOOMING")==0) {
                            gesture = EventHunter::G_ZOOMING; 
                        } else {
                            printUsage();
                            return 1;
                        }
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'o':
                    if (index +1 < argc) {
                        switch ((argv[index+1])[0]) {
                            case 'B':
                                orientation = EventHunter::BREADTHWISE;
                                break;
                            case 'L': 
                                orientation = EventHunter::LENGTHWAYS;
                                break;
                            default: break;
                        }
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'd':
                    if (index+1 < argc) {
                        delay = atoi(argv[index+1])*1000;   // ms
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'p':
                    if (index+1 < argc) {
                        posX = atoi(argv[index+1]);   // x of screen center
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'q':
                    if (index+1 < argc) {
                        posY = atoi(argv[index+1]);   // x of screen center
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 's':
                    if (index+1 < argc) {
                        step = atoi(argv[index+1]);   // pixel
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'c':
                    if (index+1 < argc) {
                        stepCount = atoi(argv[index+1]);
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'x':
                    logLevel = 2;
                    break;
                case 'v':
                    std::cout << "eventHunter version: " << version << std::endl;
                    return 0;
                default:
                    break; 
            }
        }    
    }

    if (isInterfaceSet == false) {
        printUsage();
        return 1;
    }

    EventHunter eventHunter(eventInterface, posX, posY, delay, orientation, step, stepCount, speedFactor, logLevel);

    if (isWrite) {
        eventHunter.playGesture(gesture);
    } else {
        eventHunter.readEvent();
    }
   
    return 0;
}

void printUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "eventHunter -i input_event_interface [-g GESTURE] [-s STEP] [-c STEPCOUNT] [-d DELAY] [-o ORIENTATION] [-x] [-v]" << std::endl;
    std::cout << "-i    Set the input event interface for your specified device." << std::endl;
    std::cout << "-g    Set the gesture type: SWIPE, SWIPING, PAN, FLICK, FLICKING, PINCH, SPREAD, ZOOMING. Default is SWIPE" << std::endl;
    std::cout << "-s    set the step of one movement. Default is 2 px" << std::endl;
    std::cout << "-c    Set the step count during one gesture. Default is 100" << std::endl;
    std::cout << "-p    Set the x coordinate of screen center. Default is 450 px" << std::endl;
    std::cout << "-q    Set the y coordinate of screen center. Default is 450 px" << std::endl;
    std::cout << "-d    Set the delay between each movement. Default is 5 ms" << std::endl;
    std::cout << "-x    Debug information for all the events." << std::endl;
    std::cout << "-o    Set the orientation of gesture: B - Breadthwise, L - Lengthways. Default is B" << std::endl;
    std::cout << "-v    Show version information" << std::endl;
    std::cout << "Example: eventHunter -i /dev/input/event3 (means starting eventHunter by hooking to /dev/input/event3)" << std::endl;
    std::cout << "___________________________________________________________________________________" << std::endl;
    std::cout << "eventHunter is a tool used to hook to one input device for event listening/sending." << std::endl;
}
