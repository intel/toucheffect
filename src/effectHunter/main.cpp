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
#include "effectHunter.h"

typedef struct{
    int winId;
    int mode;
    int sampleCount;
} ParamEffect;

void printUsage();
void * startEffectHunter(void * param);

int main(int argc, char *argv[])
{
    long    winId = 0;
    int     sampleCount = 20;
    bool    winIdSet = false;
    int     mode = SAMPLE_FPS;
    const std::string version = "1.00"; 
   
    std::string temp;
    for (int index=1; index < argc; index++) {
        temp = argv[index];
        if (temp[0] == '-' && temp.length()>1) {
            switch (temp[1]) {
                case 'w':
                  if (index+1 < argc) {
                        winId = strtol(argv[index+1], NULL, 16);
                        winIdSet = true;
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 'c':
                  if (index+1 < argc) {
                        sampleCount = atoi(argv[index+1]);
                        if (sampleCount < 0) {
                            std::cout << "SampleCount must be a positive number." << std::endl;
                            printUsage();
                            return 1;
                        }                                                                                                                                                                                  
                    } else {
                        printUsage();
                        return 1;
                    }
                    break;
                case 't':
                    mode = SAMPLE_TIMESTAMP;
                    break;
                case 'h':
                    printUsage();
                    return 0;
                case 'v':
                    std::cout << "effectHunter version: " << version << std::endl;
                    return 0;
                default:
                    printUsage();
            }
        }
    }
                    
    if (!winIdSet) {
        std::cout << "Window id not set, will use root window for sampling" << std::endl;
    }

    ParamEffect paramEffect;
    paramEffect.sampleCount = sampleCount;
    paramEffect.mode = mode;
    paramEffect.winId = winId;   

    if (mode == SAMPLE_TIMESTAMP) { 
        pthread_t ptEffectHunter;

        int ret = pthread_create(&ptEffectHunter, NULL, startEffectHunter, &paramEffect);
        if (ret != 0) {
            std::cerr << "Start effectHunter error" << std::endl;
        }

        if (sampleCount!=0) {
            sleep(sampleCount);
            ret = pthread_kill(ptEffectHunter, 12);
            if (ret != 0)
            {
                std::cerr << "Thread cancel failed" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        ret = pthread_join(ptEffectHunter, NULL);
        if (ret != 0)
        {
            std::cerr << "Thread join failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (mode == SAMPLE_FPS) {
        startEffectHunter(&paramEffect);
    }
    
    return 0;
}

void printUsage()
{
    std::cout << "\n[Usage]: ./effectHunter [-w winId] [-t] [-c sampleCount] [-h]\n";
    std::cout << "-w    Set the window id of the widget to be sampled, you can obtain it with xwininfo\n";
    std::cout << "-t    Set the sampling mode to be TimeStamp sampling, default mode is FPS sampling\n";
    std::cout << "-c    Set the count of sampling, also means the TimeOut (s) in TimeStamp sampling mode, default is 20\n";
    std::cout << "-h    Show this help message\n";
    std::cout << "-v    Show version information\n";
    std::cout << "______________________________________________________________________________\n";
    std::cout << "effectHunter is a tool used for collecting FPS and TimeStamp info for the movement effect of an active widget.\n\n";
}

void * startEffectHunter(void * param)
{
    ParamEffect * paramX = (ParamEffect *)param;
    EffectHunter * effectHunter = EffectHunter::genInstance(paramX->winId, paramX->mode, paramX->sampleCount);
    std::cout << " [Info]: Start sampling ..." << std::endl;
    effectHunter->collectFPS();
    delete effectHunter;
    return NULL;
}

