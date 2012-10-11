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

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include "gestureSim.h"


void printUsage();
void sigQuit(int, siginfo_t *, void *);

int main(int argc, char *argv[])
{
    unsigned timeInterval = 50;
    unsigned gesture = GestureSim::G_SWIPE;
    unsigned delay = GestureSim::NORMAL_DELAY;
    unsigned orientation = GestureSim::BREADTHWISE;
    long customerPid = 0;
    int  step = 5;
    const std::string version = "1.00";
    
    std::string temp;

    if (argc < 2) {
        printUsage();
        return 1;
    }

    for (int index=1; index < argc; index++) {
        temp = argv[index];
        if (temp[0] == '-' && temp.length()>1) {
            switch (temp[1]) {
                case 't':
                    if (index+1 < argc) {
                        timeInterval = atoi(argv[index+1]);
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'd':
                    if (index+1 < argc) {
                        delay = atoi(argv[index+1]);
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 's':
                    if (index+1 < argc) {
                        step = atoi(argv[index+1]);
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'p':
                    if (index+1 < argc) {
                        customerPid = atol(argv[index+1]);
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'o':
                    if (index +1 < argc) {
                        switch ((argv[index+1])[0]) {
                            case 'B':
                                orientation = GestureSim::BREADTHWISE;
                                break;
                            case 'L': 
                                orientation = GestureSim::LENGTHWAYS;
                                break;
                            default: 
                                printUsage();
                                break;
                        }
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'g':
                    if (index +1 < argc) {
                        if (strcmp(argv[index+1], "PAN")==0) {
                            gesture = GestureSim::G_PAN; 
                        } else if (strcmp(argv[index+1], "SWIPE")==0) {                                                                                                                                                                     
                            gesture = GestureSim::G_SWIPE;
                        } else if (strcmp(argv[index+1], "SWIPING")==0) {
                            gesture = GestureSim::G_SWIPING;
                        } else {
                            printUsage();
                            return 1;
                        }
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'v':
                    std::cout << "gestureSim version: " << version << std::endl;
                    return 0;
                default:
                    printUsage();
                    return 1;
            }
        }
    }


    struct sigaction act;
    int sig = 12;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO;
    act.sa_sigaction=sigQuit;
    if (sigaction(sig, &act, NULL)<0) {
        std::cout << "[Error]: Failed to install quit signal.\n";
    }

    GestureSim gestureSim(customerPid, step);
    gestureSim.sim(gesture, orientation, delay, timeInterval);

    struct timeval timeStamp;
    gestureSim.getFirstEventTimeStamp(&timeStamp); 
    std::cout << " [Info]: Gesture started at " << timeStamp.tv_sec << " - " << timeStamp.tv_usec << std::endl;
    gestureSim.getLastEventTimeStamp(&timeStamp); 
    std::cout << " [Info]: Gesture ended at " << timeStamp.tv_sec << " - " << timeStamp.tv_usec << std::endl;

    return 0;
}

void printUsage()
{
    std::cout << "[Usage]: ./gestureSim [-g gestureType] [-t timeInterval] [-d delay] [-o orientation] [-v]\n";
    std::cout << "-g    Set the gesture type: SWIPE, SWIPING, PAN. Default is SWIPE\n";
    std::cout << "-t    Set the interval between two gestures. Default is 50 (ms)\n";
    std::cout << "-d    Set the delay between movement, it is just the opposite of movement speed. Default is 20 (ms)\n";
    std::cout << "-o    Set the orientation of gesture: B - Breadthwise, L - Lengthways. Default is B\n";
    std::cout << "-v    Show version information\n";
    std::cout << "______________________________________________________________________________\n";
    std::cout << "gestureSim is a tool used for simulating gestures like pan, swipe, swiping\n\n";
}


void sigQuit(int, siginfo_t *, void *)
{
    GestureSim::stop();
}
