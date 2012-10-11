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
#include <fcntl.h>
#include <iostream>
#include <sys/stat.h>

const unsigned EventHunter::G_PAN       = 0x0;
const unsigned EventHunter::G_SWIPE     = 0x1;
const unsigned EventHunter::G_FLICK     = 0x2;
const unsigned EventHunter::G_PINCH     = 0x3;
const unsigned EventHunter::G_SPREAD    = 0x4;
const unsigned EventHunter::G_SWIPING   = 0x5;
const unsigned EventHunter::G_ZOOMING   = 0x6;
const unsigned EventHunter::G_FLICKING  = 0x7;

const unsigned EventHunter::BREADTHWISE = 0x0;
const unsigned EventHunter::LENGTHWAYS  = 0x1;

EventHunter::EventHunter(const char * eventInterface, int centerX, int centerY, unsigned delay, unsigned orientation, unsigned step, unsigned stepCount, int speedFactor, int logLevel):
    mLogLevel(logLevel),
    mEventInterface(eventInterface),
    mDelay(delay),
    mOrientation(orientation),
    mStep(step),
    mStepCount(stepCount),
    mSpeedFactor(speedFactor),
    mIsFirstEvent(true)
{
    mEventFD = open(mEventInterface, O_RDWR|O_SYNC); 
    if(mEventFD <= 0){
        std::cerr << "Fail to open input event interface." << std::endl;
    }

    int length = mStep * mStepCount;
    if (orientation == BREADTHWISE) {
        mPosX_a = centerX - length;
        mPosX_b = centerX + length;
        mPosY_a = centerY;
        mPosY_b = centerY;
    } else {
        mPosX_a = centerX;
        mPosX_b = centerX;
        mPosY_a = centerY - length;
        mPosY_b = centerY + length;
    }
}

EventHunter::~EventHunter()
{
    close(mEventFD);
}

void EventHunter::simWriteEvent(unsigned short type, unsigned short code, int value) 
{
    static struct input_event event;
    event.type = type;

    event.code = code;
    event.value = value;
    gettimeofday(&event.time, NULL);
    if (write(mEventFD,&event,sizeof(event))!=sizeof(event)) {
        std::cerr << "event write error" << std::endl;
    }
}

void EventHunter::simMoveREL(int posX, int posY)
{
    static struct input_event emptyEvent;

    simWriteEvent(EV_REL, REL_X, posX);

    simWriteEvent(EV_REL, REL_Y, posY);

    write(mEventFD, &emptyEvent, sizeof(emptyEvent));
}

void EventHunter::simMTMoveABS_ProtoB(int posX, int posY, int slot_id, int contact_id, bool touchDown)
{
    simWriteEvent(EV_ABS, ABS_MT_SLOT, slot_id);

    if (touchDown) {
        simWriteEvent(EV_ABS, ABS_MT_TRACKING_ID, contact_id);
    }
    
    //simWriteEvent(EV_ABS, ABS_MT_TOUCH_MAJOR, 4);

    simWriteEvent(EV_ABS, ABS_MT_POSITION_X, posX);

    simWriteEvent(EV_ABS, ABS_MT_POSITION_Y, posY);
    
    //simWriteEvent(EV_ABS, ABS_MT_PRESSURE, 52);

    // Conform to old single point protocol
    simWriteEvent(EV_ABS, ABS_X, posX);
    simWriteEvent(EV_ABS, ABS_Y, posY);
    //simWriteEvent(EV_ABS, ABS_PRESSURE, 52);

    simWriteEvent(EV_SYN, SYN_REPORT, 0);
        
    if (mIsFirstEvent) {
        mIsFirstEvent = false;
       //mFirstEventTS.tv_sec = mFirstEventTS.tv_nsec = 0;
       mFirstEventTV.tv_sec = mFirstEventTV.tv_usec = 0;
       //clock_gettime(CLOCK_MONOTONIC, &mFirstEventTS);
       gettimeofday(&mFirstEventTV, NULL);
    }
}

void EventHunter::simMTEND_ProtoB(int slot_id)
{
    simWriteEvent(EV_ABS, ABS_MT_SLOT, slot_id);

    simWriteEvent(EV_ABS, ABS_MT_TRACKING_ID, -1);

    simWriteEvent(EV_SYN, SYN_REPORT, 0);
}

void EventHunter::readEvent()
{
    struct input_event ev;
    struct timeval timeStamp;

    bool tsFlag = false;
    
    while (1) {
        if(read(mEventFD, &ev, sizeof(struct input_event)) <= 0) {
            std::cerr << "Event read error." << std::endl;
            return;
        }

        timeStamp.tv_sec = ev.time.tv_sec;
        timeStamp.tv_usec = ev.time.tv_usec;
        if (ev.type == EV_REL) {
            switch(ev.code) {
                case REL_X:
                    if (2==mLogLevel||tsFlag) {
                        log("REL x", ev);
                        tsFlag = false;
                    }
                    break;
                case REL_Y:
                    if (2==mLogLevel||tsFlag) {
                        log("REL y", ev);
                        tsFlag = false;
                    }
                    break;
                case REL_Z:
                    if (2==mLogLevel||tsFlag) {
                        log("REL z", ev);
                        tsFlag = false;
                    }
                    break;
                default:
                    if (2==mLogLevel||tsFlag) {
                        log("REL UNKNOWN", ev);
                        tsFlag = false;
                    }
                    break;
            }
        } else if (ev.type == EV_ABS) {
            switch(ev.code) {
                case ABS_X:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS x", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_Y:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS y", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_Z:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS z", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_PRESSURE:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS PRESSURE", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_MT_POSITION_X:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS MT_X", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_MT_POSITION_Y:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS MT_Y", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_MT_SLOT:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS MT_SLOT", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_MT_TRACKING_ID:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS MT_TRACKING_ID", ev);
                        tsFlag = false;
                    }
                    break;
                case ABS_MT_TOUCH_MAJOR:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS MT_TOUCH_MAJOR", ev);
                        tsFlag = false;
                    }                                                                                                                                                                                                                       
                    break;
                case ABS_MT_PRESSURE:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS MT_PRESSURE", ev);
                        tsFlag = false;
                    }
                    break;
                default:
                    if (2==mLogLevel||tsFlag) {
                        log("ABS UNKNOWN", ev);
                        tsFlag = false;
                    }
                    break;
            }
        } else if (ev.type == EV_KEY) {
            switch(ev.code) {
                case BTN_LEFT:
                    log("KEY", ev);
                    if (1==ev.value) {
                        tsFlag = true;
                    } else if (0==ev.value) {
                        tsFlag = false;
                    }
                    break;
                default:
                    if (2==mLogLevel||tsFlag) {
                        log("KEY", ev);
                        break;
                    }
            }
        } else if (ev.type == EV_SYN) {
            switch(ev.code) {
                case SYN_REPORT:
                    log("SYN REPORT", ev);
                    break;
                case SYN_MT_REPORT:
                    log("SYN MT_REPORT", ev);
                    break;
                default:
                    log("SYN", ev);
                    break;
            }
        } else {
            if (2==mLogLevel||tsFlag) {
                log("ELSE", ev);
            }
        }
    }
}

void EventHunter::simKeyEvent(int key, int value)
{
    static struct input_event emptyEvent;

    simWriteEvent(EV_KEY, key, value);
    
    write(mEventFD,&emptyEvent,sizeof(emptyEvent));
}
    
void EventHunter::playGesture(unsigned gesture)
{
    int pos_x = mPosX_a;
    int pos_y = mPosY_a;

    switch (gesture) {
        case G_PAN:
            sim1Point(mStep, false, true, 1000000);
            break;
        case G_SWIPE:
            sim1Point(mStep, false, true);
            break;
        case G_FLICK:
            sim1Point(mStep, true, true);
            break;
        case G_PINCH:
            sim2Points(mStep, true);
            break;
        case G_SPREAD:
            sim2Points(-1*mStep, true);
            break;
        case G_SWIPING:
            while(1) {
                sim1Point(mStep, false, false);
                mStep *= -1;
            }
            break;
        case G_FLICKING:
            while(1) {
                sim1Point(mStep, true, true);
                mPosX_a = pos_x;
                mPosY_a = pos_y;
            }
            break;
        case G_ZOOMING:
            while(1) {
                mStep *= -1;
                sim2Points(mStep, false);
            }
            break;
        default: break;
    }
    //std::cout << "First Event at: " << mFirstEventTS.tv_sec*1000 + mFirstEventTS.tv_nsec/1000000 << std::endl;
    std::cout << " [Info]: Gesture started at " << mFirstEventTV.tv_sec << " - " << mFirstEventTV.tv_usec << std::endl;
}

void EventHunter::sim1Point(int step, bool speedup, bool touchUp, unsigned touchUpDelay)
{
    if (mOrientation == BREADTHWISE) {
        simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, true);
        for (unsigned i = 2; i < mStepCount; i++) {
            mPosX_a += step;
            simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, false);
            if (speedup) {
                step = step + mSpeedFactor;
            }
            usleep(mDelay);
        }
    } else {
        simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, true);
        for (unsigned i = 1; i < mStepCount; i++) {
            mPosY_a += step;
            simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, false);
            if (speedup) {
                step = step + mSpeedFactor;
            }
            usleep(mDelay);
        }
    }

    if (touchUp) {
        usleep(touchUpDelay);
        simMTEND_ProtoB(0);
    }
}

void EventHunter::sim2Points(int step, bool touchUp)
{
    if (mOrientation == BREADTHWISE) {
        simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, true);
        simMTMoveABS_ProtoB(mPosX_b, mPosY_b, 1, 4, true);
        for (unsigned i = 1; i < mStepCount; i++) {
            mPosX_a += step;
            mPosX_b -= step;
            simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, false);
            simMTMoveABS_ProtoB(mPosX_b, mPosY_b, 1, 4, false);
            usleep(mDelay);
        }
    } else {
        simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, true);
        simMTMoveABS_ProtoB(mPosX_b, mPosY_b, 1, 4, true);
        for (unsigned i = 1; i < mStepCount; i++) {
            mPosY_a += step;
            mPosY_b -= step;
            simMTMoveABS_ProtoB(mPosX_a, mPosY_a, 0, 2, false);
            simMTMoveABS_ProtoB(mPosX_b, mPosY_b, 1, 4, false);
            usleep(mDelay);
        }
    }

    if (touchUp) {
        simMTEND_ProtoB(0);
        simMTEND_ProtoB(1);
    }
}

inline void EventHunter::log(std::string pre, struct input_event & ev)
{
    std::cout << "[Event]: <" << pre << "> " << ev.code << ":" << ev.value << " at " << ev.time.tv_sec << " - " << ev.time.tv_usec << std::endl;
}
