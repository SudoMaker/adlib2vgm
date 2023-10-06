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

#include "VgmOpl.hpp"

const unsigned char VgmOpl::op_table[9] = {0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12};

VgmOpl::VgmOpl(const std::string &filename) {
	file.open(filename, O_CREAT|O_TRUNC|O_WRONLY, 0644);

	uint8_t buf[128] = "Vgm ";

	auto *buf32 = (uint32_t *)buf;

	buf32[0x08 / 4] = 0x00000151;

	buf32[0x34 / 4] = 0x0000004c;

	if (!global_use_opl3) {
		buf32[0x50 / 4] = 3579545;
		settype(TYPE_OPL2);
	} else {
		buf32[0x5c / 4] = 14318180;
		settype(TYPE_OPL3);
	}

	buffer.insert(buffer.end(), buf, buf+sizeof(buf));
}

void VgmOpl::write(int reg, int val) {
	if (buffered_sleep_samples) {
		uint8_t buf[3] = {0x61};
		*((uint16_t *)&buf[1]) = buffered_sleep_samples;
		buffer.insert(buffer.end(), buf, buf+sizeof(buf));
		buffered_sleep_samples = 0;
	}

	uint8_t buf[3] = {0x5a, (uint8_t)reg, (uint8_t)val};

	if (currType == TYPE_OPL3) {
		if (currChip == 0) {
			buf[0] = 0x5e;
			if (global_verbose)
				printf("OPL3 C0 write: 0x%02x 0x%02x\n", reg, val);

		} else {
			buf[0] = 0x5f;
			if (global_verbose)
				printf("OPL3 C1 write: 0x%02x 0x%02x\n", reg, val);

		}
	} else {
		if (global_verbose)
			printf("OPL2 write: 0x%02x 0x%02x\n", reg, val);
	}

	buffer.insert(buffer.end(), buf, buf+sizeof(buf));
}

void VgmOpl::insert_sleep(uint16_t samples) {
	sample_count += samples;

	if (buffered_sleep_samples + samples <= UINT16_MAX) {
		buffered_sleep_samples += samples;
	} else {
		uint8_t buf[3] = {0x61};
		*((uint16_t *)&buf[1]) = buffered_sleep_samples;
		buffer.insert(buffer.end(), buf, buf+sizeof(buf));

		buffered_sleep_samples = samples;
	}
}

void VgmOpl::save() {
	if (buffered_sleep_samples) {
		uint8_t buf[3] = {0x61};
		*((uint16_t *)&buf[1]) = buffered_sleep_samples;
		buffer.insert(buffer.end(), buf, buf+sizeof(buf));
		buffered_sleep_samples = 0;
	}

	// EoF offset
	*((uint32_t *)&buffer[0x4]) = buffer.size() - 4;

	// Total # samples
	*((uint32_t *)&buffer[0x18]) = sample_count;

	file.write_all(buffer);
}
