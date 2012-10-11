#                                                                                                                                                                                                                                           
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

CC=g++
CFLAGS=-c -Wall -g -O3 -D_THREAD_SAFE -static 
LDFLAGS= -static -lrt 
SOURCES=main.cpp eventHunter.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=eventHunter

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) *.o
