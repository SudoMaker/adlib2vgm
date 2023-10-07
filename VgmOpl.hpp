/*
    This file is part of adlib2vgm.

    Copyright (C) 2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "CommonIncludes.hpp"
#include "FakeAdplug/FakeAdplug.hpp"
#include <fstream>

class VgmOpl: public Copl {
public:
	// Who cares copying the string one more time? Sorry, I care.
	// We can't expect everyone to correctly write C++ !!!
	// We also can't expect everyone to know how to use CMake in 2021 !!!
	VgmOpl() = default;

	VgmOpl(const std::string &filename);

	~VgmOpl() override = default;

	void init() override {}
	void write(int reg, int val) override;
	void insert_sleep(uint16_t samples);
	void save();

protected:
	static const unsigned char	op_table[9];

	std::ofstream *file;
	uint32_t sample_count = 0;
	std::vector<uint8_t> buffer;
	uint16_t buffered_sleep_samples = 0;
};
