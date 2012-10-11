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


#ifndef PANSIM_H
#define PANSIM_H

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <sys/time.h>

class GestureSim
{
public:
    GestureSim(pid_t customerPid, int step);
    ~GestureSim();
    void sim(unsigned gestureType, unsigned orientation, unsigned delay, unsigned timeInterval);
    void getFirstEventTimeStamp(struct timeval * timeStamp);
    void getLastEventTimeStamp(struct timeval * timeStamp);
    static void stop();

    static const unsigned   QUICK_DELAY;
    static const unsigned   NORMAL_DELAY;
    static const unsigned   SLOW_DELAY;
    static const unsigned   BREADTHWISE;
    static const unsigned   LENGTHWAYS;
    static const unsigned   G_SWIPE;
    static const unsigned   G_SWIPING;
    static const unsigned   G_PAN;



private:
    Display *   mXWindowDisplay;
    int         mScreen;
    bool        mFirstEventFlag;
    pid_t       mCustomerPid;
    int         mStep;
    struct timeval *    mFirstEventTimeStamp;
    struct timeval *    mLastEventTimeStamp;
    static bool		mQuitFlag;
    void panAndSwipe(bool isSwipe, int & posX, int & posY, int direction, unsigned delay, bool mouseDown, bool mouseUp);
    void swiping(int & posX, int & posY, unsigned orientation, unsigned delay, struct timespec * tsInterval);
};

#endif
