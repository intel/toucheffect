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

#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <signal.h>

#include "gestureSim.h"

#define     MOVE_RIGHT  0x0
#define     MOVE_LEFT   0x2


const unsigned   GestureSim::QUICK_DELAY = 1;
const unsigned   GestureSim::NORMAL_DELAY = 20;
const unsigned   GestureSim::SLOW_DELAY = 50;

const unsigned   GestureSim::BREADTHWISE = 0x0;
const unsigned   GestureSim::LENGTHWAYS = 0x1;

const unsigned   GestureSim::G_SWIPE = 0x0;
const unsigned   GestureSim::G_SWIPING = 0x1;
const unsigned   GestureSim::G_PAN = 0x2;

bool GestureSim::mQuitFlag = false;

GestureSim::GestureSim(pid_t customerPid, int step):mCustomerPid(customerPid),mStep(step)
{
    int eventnum = 0, errornum = 0, majornum = 0, minornum = 0;

    mXWindowDisplay = XOpenDisplay(NULL);
    if (mXWindowDisplay == NULL) {
        std::cerr << "[Error]: Can't Open the Dispaly." << std::endl;
        exit(-1);
    }

    if (!XTestQueryExtension(mXWindowDisplay, &eventnum, &errornum, &majornum, &minornum)) {
        std::cerr << "[Error]: XServer " << DisplayString(mXWindowDisplay) << " doesn't support the XTest extensions!" << std::endl;
        exit(-1);
    }

    XSync(mXWindowDisplay, True);
    mScreen = DefaultScreen(mXWindowDisplay);
    mFirstEventTimeStamp = (struct timeval *)malloc(sizeof(struct timeval));
    mLastEventTimeStamp = (struct timeval *)malloc(sizeof(struct timeval));
    mFirstEventFlag = true;
}

GestureSim::~GestureSim()
{
    if (mXWindowDisplay!=NULL) {
        XSync(mXWindowDisplay, False);
        XCloseDisplay(mXWindowDisplay);
        mXWindowDisplay = NULL;
    }

    free(mLastEventTimeStamp);
    free(mFirstEventTimeStamp);
}

void GestureSim::stop()
{
    mQuitFlag = true;
}

void GestureSim::panAndSwipe(bool isSwipe, int & posX, int & posY, int direction, unsigned delay, bool mouseDown, bool mouseUp)
{
    int step = 0;
    
    // Click Mouse (left button)
    if (mouseDown) {
        XTestFakeButtonEvent(mXWindowDisplay, 1, True, CurrentTime);
        XFlush(mXWindowDisplay);
    }
        
    XTestFakeMotionEvent(mXWindowDisplay, mScreen, posX, posY, CurrentTime);
    XSync(mXWindowDisplay, False);

    if (mFirstEventFlag) {
        gettimeofday(mFirstEventTimeStamp, NULL);
        mFirstEventFlag = false;
    }

    if (direction == (BREADTHWISE|MOVE_RIGHT)) {
        for (int i=1; i<20; i++) {
            XTestFakeMotionEvent(mXWindowDisplay, mScreen, posX+step, posY, delay);
            XFlush(mXWindowDisplay);
            step += mStep;
        }
        posX += step - mStep;
    } else if (direction == (BREADTHWISE|MOVE_LEFT)){
        for (int i=1; i<20; i++) {
            XTestFakeMotionEvent(mXWindowDisplay, mScreen, posX-step, posY, delay);
            XFlush(mXWindowDisplay);
            step += mStep;
        }
        posX -= step - mStep;
    } else if (direction == (LENGTHWAYS|MOVE_RIGHT)) {
        for (int i=1; i<20; i++) {
            XTestFakeMotionEvent(mXWindowDisplay, mScreen, posX, posY+step, delay);
            XFlush(mXWindowDisplay);
            step += mStep;
        }
        posY += step - mStep;
    } else if (direction == (LENGTHWAYS|MOVE_LEFT)) {
        for (int i=1; i<20; i++) {
            XTestFakeMotionEvent(mXWindowDisplay, mScreen, posX, posY-step, delay);
            XFlush(mXWindowDisplay);
            step += mStep;
        }
        posY -= step - mStep;
    }
    
    XSync(mXWindowDisplay, False);
    gettimeofday(mLastEventTimeStamp, NULL);

    if (!isSwipe) {
        sleep(1);
    }

    if (mCustomerPid != 0) { 
        kill(mCustomerPid, 12); // Defined siginal sent to customer to inform that one round of gesture ends.
    }
    
    // Release mouse (left button)
    if (mouseUp) {
        XTestFakeButtonEvent(mXWindowDisplay, 1, False, CurrentTime);
        XFlush(mXWindowDisplay);
    }
}

void GestureSim::swiping(int & posX, int & posY, unsigned orientation, unsigned delay, struct timespec * tsInterval)
{
    panAndSwipe(true, posX, posY, MOVE_RIGHT|orientation, delay, true, false);
    while (!mQuitFlag) { 
        panAndSwipe(true, posX, posY, MOVE_LEFT|orientation, delay, false, false);
        nanosleep(tsInterval, NULL);
        panAndSwipe(true, posX, posY, MOVE_RIGHT|orientation, delay, false, false);
        nanosleep(tsInterval, NULL);
    }
    panAndSwipe(true, posX, posY, MOVE_LEFT|orientation, delay, false, true);
}

void GestureSim::sim(unsigned gesture, unsigned orientation, unsigned delay, unsigned timeInterval)
{
    int posX, posY;

    struct timespec s;
    s.tv_sec = timeInterval/1000;
    s.tv_nsec = timeInterval%1000 * 1000000L;

    posX = DisplayWidth(mXWindowDisplay, mScreen)/2;
    posY = DisplayHeight(mXWindowDisplay, mScreen)/2 + 70;

    // reset focus to be at center
    XTestFakeMotionEvent(mXWindowDisplay, mScreen, posX, posY, CurrentTime);
    //XTestFakeButtonEvent(mXWindowDisplay, 1, True, CurrentTime);
    //XTestFakeButtonEvent(mXWindowDisplay, 1, False, CurrentTime);
    XFlush(mXWindowDisplay);
    sleep(1);

    switch (gesture) {
    case G_SWIPE:
        panAndSwipe(true, posX, posY, MOVE_RIGHT|orientation, delay, true, true);
        break;
    case G_SWIPING:
        swiping(posX, posY, orientation, delay, &s);
        break;
    case G_PAN:
        panAndSwipe(false, posX, posY, MOVE_RIGHT|orientation, delay, true, true);
        break;
    default: break;
    }
    
    // Send the 'ESC' key event 
    //XTestFakeKeyEvent(mXWindowDisplay, 9, True, CurrentTime);
    //XTestFakeKeyEvent(mXWindowDisplay, 9, False, CurrentTime);

    XFlush(mXWindowDisplay);
}

void GestureSim::getFirstEventTimeStamp(struct timeval * timeStamp)
{
    timeStamp->tv_sec = mFirstEventTimeStamp->tv_sec;
    timeStamp->tv_usec = mFirstEventTimeStamp->tv_usec;
}

void GestureSim::getLastEventTimeStamp(struct timeval * timeStamp)
{
    timeStamp->tv_sec = mLastEventTimeStamp->tv_sec;
    timeStamp->tv_usec = mLastEventTimeStamp->tv_usec;
}
