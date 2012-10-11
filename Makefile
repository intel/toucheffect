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

all: build

build: buildSIM buildEFH buildEVH 
	@(rm -rf ./bin)
	@(mkdir ./bin)
	@(cp src/gestureSim/gestureSim ./bin)
	@(cp src/effectHunter/effectHunter ./bin)
	@(cp src/eventHunter/eventHunter ./bin)

buildSIM:
	@(cd src/gestureSim && $(MAKE) && cd ../../)

buildEFH:
	@(cd src/effectHunter && $(MAKE) && cd ../../)

buildEVH:
	@(cd src/eventHunter && $(MAKE) && cd ../../)

clean: 
	@(rm -rf ./bin)
	@(cd src/gestureSim && $(MAKE) clean && cd ../../)
	@(cd src/effectHunter && $(MAKE) clean && cd ../../)
	@(cd src/eventHunter && $(MAKE) clean && cd ../../)
