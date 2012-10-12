# Copyright 2012, Intel Corporation

# This program file is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; version 2 of the License.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
 
# You should have received a copy of the GNU General Public License
# along with this program in a file named COPYING; if not, write to the
# Free Software Foundation, Inc,
# 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA
# or just google for it.
 
# Authors:
#  Chen Guobing <guobing.chen@intel.com>



#!/bin/perl
use Getopt::Long;
use strict;

my $winId = 0;

my $help = 0;
my $versionInfo = "1.00";
my $version = 0;
my $winId = "UNSET";
my $simulator = 'X';
my $inputInterface = "UNSET";
my $mode = "FPS";
my $gesture = "SWIPING";
my $orientation = "B";
my $timeInterval = 10;
my $delay = 10;
my $sampleCount = 20;
if (@ARGV > 0) {
    GetOptions('w:s'=> \$winId,
               's:s'=> \$simulator,
               'i:s'=> \$inputInterface,
               't:i'=> \$timeInterval,
               'm:s'=> \$mode,
               'g:s'=> \$gesture,
               'o:s'=> \$orientation,
               'd:i'=> \$delay,
               'c:i'=> \$sampleCount,
               'version|v'=> \$version,
               'help|h'=> \$help);
}

if ($help) {
    &printUsage();
    exit();
}

if ($version) {
    &printVersion();
    exit();
}

if ($winId eq "UNSET") {
    &printUsage();
    exit();
} 

if ($simulator eq 'I' && $inputInterface eq "UNSET") {
    &printUsage();
    exit();
}

if ($mode ne "FPS" && $mode ne "TIMESTAMP") {
        &printUsage();
        exit();
}

mkdir("log");

print(" Sampling $winId for $mode\n\n");

$ENV{"DISPLAY"} = ":0.0";

if ($mode eq "FPS") {
    my $sim_pid;
    if ($simulator eq 'X') {
        system("./bin/gestureSim -g $gesture -o $orientation -t $timeInterval -d $delay > log/gesture.log &");
        $sim_pid = `pidof gestureSim`;
    } elsif ($simulator eq 'I') {
        system("./bin/eventHunter -i $inputInterface -g $gesture -o $orientation -d $delay > log/gesture.log &");   
        # Not accept param 'timeInterval', it will use param 'delay' for all the interval
        $sim_pid = `pidof eventHunter`;
    }
    sleep(4);
    system("./bin/effectHunter -w $winId -c $sampleCount");
    chomp($sim_pid);
    system("kill -s 9 $sim_pid");
} elsif ($mode eq "TIMESTAMP") {
    system("./bin/effectHunter -t -w $winId >log/effectHunter.log &");
    sleep(1);
    my $efh_pid = `pidof effectHunter`;
    chomp($efh_pid);
    if ($simulator eq 'X') {
        system("./bin/gestureSim -g $gesture -o $orientation -t $timeInterval -d $delay -p $efh_pid > log/gesture.log");
    } elsif ($simulator eq 'I') {
        system("./bin/eventHunter -i $inputInterface -g $gesture -o $orientation -d $delay > log/gesture.log");   
        # Not accept param 'timeInterval', it will use param 'delay' for all the interval
        system("kill -s 12 $efh_pid");
    }
    
    sleep(2);
 
    &processTSLog();
}

# Recover normal input system by fully playing one gesture
if ($simulator eq 'X') {
    system("./bin/gestureSim -g SWIPE > /dev/null");
} elsif ($simulator eq 'I') {
    system("./bin/eventHunter -i $inputInterface -g SWIPE > /dev/null");
}

###########################################################################################################################
# Main stops here                                                                                                          #
###########################################################################################################################

sub printUsage
{
    print ("Usage:\n");
    print ("perl touchEffect.pl <-w WidgetID> [-s SIMULATOR] [-i INTERFACE] [-t INTERVAL] [-d DELAY] [-m MODE] [-g GESTURE] [-o ORIENTATION] [-c COUNT]\n");
    print ("option \"w\": The id (get with xwininfo) of touch widget. You can specify either the name or the id, and id is with high priority\n");
    print ("option \"s\": The type of simulator: X - X based, I - input interface based. Default is X\n");
    print ("option \"i\": Set the input event interface for your specified device. E.X.: /dev/input/event6. Required if the simulator is set to be 'I'\n");
    print ("option \"t\": The time interval between two gestures. Default is 10 ms\n");
    print ("option \"d\": The time delay between two events. Default is 10 ms\n");
    print ("option \"m\": The mode of sampling: FPS, TIMESTAMP. Default is FPS\n");
    print ("option \"g\": The gesture type: PAN, SWIPE, SWIPING, FLICK, FLICKING, PINCH. SPREAD, ZOOMING. Default is SWIPING\n");
    print ("option \"o\": The orientation of the gesture: B - Breadthways, L - Lengthways. Default is B\n");
    print ("option \"c\": The sampling count of collection. Default is 20\n");
    print ("option \"v\": Show version info\n");
    print ("touchEffect is a tool used for measuring UI performance when playing gestures\n");
}

sub printVersion
{
    print ("touchEffect version $versionInfo\n");
}

sub processTSLog()
{
    my $efh_start_s = 0;
    my $efh_start_us = 0;
    my $efh_stop_s = 0;
    my $efh_stop_us = 0;
    my $gts_start_s = 0;
    my $gts_start_us = 0;
    my $gts_stop_s = 0;
    my $gts_stop_us = 0;

    open(EFH_HANDLER, "<log/effectHunter.log") || die("Could not open log/effectHunter.log!");

    while(<EFH_HANDLER>) {
        my($line) = $_;
        chomp($line);
        if ($line =~ /Start Refresh at (\d+) - (\d+)/) {
            print $line."\n";
            $efh_start_s = $1;
            $efh_start_us = $2;
        } elsif ($line =~ /Stop Refresh at  (\d+) - (\d+)/) {
            print $line."\n";
            $efh_stop_s = $1;
            $efh_stop_us = $2;
        }
    }
    close(EFH_HANDLER);

    open(GTS_HANDLER, "<log/gesture.log") || die("Could not open log/gesture.log!");
    while(<GTS_HANDLER>) {
        my($line) = $_;
        chomp($line);
        if ($line =~ /Gesture started at (\d+) - (\d+)/) {
            print $line."\n";
            $gts_start_s = $1;
            $gts_start_us = $2;
#        } elsif ($line =~ /Gesture ended at   (\d+) - (\d+)/) {
#            print $line."\n";
#            $gts_stop_s = $1;
#            $gts_stop_us = $2;
        }
    }
    close(GTS_HANDLER);

    my $start_latency = ($efh_start_s-$gts_start_s) + ($efh_start_us - $gts_start_us)/1000000;
#    my $stop_latency = ($efh_stop_s-$gts_stop_s) + ($efh_stop_us - $gts_stop_us)/1000000;
    
    print "\n -------------------------------------\n";
    print " Start latency: ".$start_latency." s\n";
#    print " End latency:   ".$stop_latency." s\n";

}
