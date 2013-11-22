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

#ifndef __EVENTHUNTER_H
#define __EVENTHUNTER_H
#include <linux/input.h>
#include <time.h>
#include <unistd.h>
#include <string>

class EventHunter {
public:
    EventHunter(const char * eventInterface, 
                int centerX, 
                int centerY, 
                unsigned delay, 
                unsigned orientation, 
                unsigned protocolType, 
                unsigned step, 
                unsigned stepCount, 
                int speedFactor, 
                int logLevel);
    EventHunter();  // Mainly for detecting mode
    ~EventHunter();
    void simWriteEvent(unsigned short type, unsigned short code, int value);
    void simMoveREL(int posX, int posY);
    void simMTMove(int posX, int posY, int slot_id, int contact_id, bool touchDown, bool eventEnd);
    void simMTEnd(int slot_id);
    void simMTMoveProtoA(int posX, int posY, bool touchDown, bool eventEnd);
    void simMTEndProtoA();
    void simMTMoveProtoB(int posX, int posY, int slot_id, int contact_id, bool touchDown, bool eventEnd);
    void simMTEndProtoB(int slot_id);
    void simKeyEvent(int key, int value);
    void sim1Point(int step, bool speedup, bool touchUp, unsigned touchUpDelay = 1);
    void sim2Points(int step, bool touchUp);
    void playGesture(unsigned gesture);
    void detect();
    void readEvent();
    void sim1PointTouch(int posX, int posY, unsigned touchUpDelay = 1);
    void setCenterPos(int posX, int posY);
    void setOrientation(unsigned orientation);
    void setDelay(unsigned delay);
    void setInterface(const char *);

    static const unsigned G_TOUCH;        
    static const unsigned G_PAN;        
    static const unsigned G_SWIPE;      
    static const unsigned G_FLICK;      
    static const unsigned G_PINCH;      
    static const unsigned G_SPREAD;     
    static const unsigned G_SWIPING;    
    static const unsigned G_FLICKING;    
    static const unsigned G_ZOOMING;  
    static const unsigned G_TOUCH_HOME_SHORT; 
    static const unsigned G_TOUCH_HOME_LONG;  

    static const unsigned BREADTHWISE;  
    static const unsigned LENGTHWAYS;   

    static const unsigned P_TYPE_A;  
    static const unsigned P_TYPE_B;  

private:
    int mEventFD, hEventFD;
    int mLogLevel;
    const char * mEventInterface;
    int mPosX_a;
    int mPosY_a;
    int mPosX_b;
    int mPosY_b;
    int mCenterX;
    int mCenterY;
    unsigned mDelay;
    unsigned mOrientation;
    unsigned mProtocol;
    unsigned mStep;
    unsigned mStepCount;
    int mSpeedFactor;
    bool mIsFirstEvent;
    //struct timespec mFirstEventTS; 
    struct timeval  mFirstEventTV; 

    void log(std::string pre, struct input_event & ev);
    int nextContact();
};

#endif
