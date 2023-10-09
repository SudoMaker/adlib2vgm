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

#include "CommonIncludes.h"
#include "FakeAdplug/FakeAdplug.h"
#include <fstream>

class VgmOpl: public Copl {
public:
	VgmOpl();

	~VgmOpl() override = default;

	void init() override {}
	void write(int reg, int val) override;
	void insert_sleep(uint16_t samples);
	int save(std::string &filename);

	void set_author(std::string author);
	void set_title(std::string title);
	void set_desc(std::string desc);
	void set_loop(bool enable);

protected:
	void write16le(uint8_t *buffer, uint32_t value);
	void write32le(uint8_t *buffer, uint32_t value);
	void store_sleep(uint16_t &n_samples);
	void append_as_u16string(std::string line);

	static const unsigned char	op_table[9];

	uint32_t sample_count = 0;
	std::vector<uint8_t> buffer;
	uint16_t buffered_sleep_samples = 0;
	std::string author, title, desc;
	bool loop;
};

// vi: ts=8 sw=8 noet
