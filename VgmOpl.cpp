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

#include "VgmOpl.h"

#define OPL2_CLOCK 3579545
#define OPL3_CLOCK 14318180

#define vgm_header_size   0x80
#define vgm_eof_offset    0x04
#define vgm_total_samples 0x18
#define vgm_opl2_clock    0x50
#define vgm_opl3_clock    0x5c
#define vgm_version       0x08
#define vgm_data_offset   0x34

#define vgm_cmd_write_ym3812       0x5a
#define vgm_cmd_write_ymf262_port0 0x5e
#define vgm_cmd_write_ymf262_port1 0x5f
#define vgm_cmd_wait_n_samples     0x61
#define vgm_cmd_end_of_sound_data  0x66

const unsigned char VgmOpl::op_table[9] = {
	0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12
};

void VgmOpl::write16le(uint8_t *buffer, uint32_t value) {
	buffer[0] = (value      ) & 0xff;
	buffer[1] = (value >>  8) & 0xff;
}

void VgmOpl::write32le(uint8_t *buffer, uint32_t value) {
	buffer[0] = (value      ) & 0xff;
	buffer[1] = (value >>  8) & 0xff;
	buffer[2] = (value >> 16) & 0xff;
	buffer[3] = (value >> 24) & 0xff;
}

VgmOpl::VgmOpl(const std::string &filename) {
	file = new std::ofstream(filename, std::ios::out | std::ios::trunc | std::ios::binary);

	uint8_t buf[vgm_header_size] = "Vgm ";

	write32le(&buf[vgm_version],     0x151);
	write32le(&buf[vgm_data_offset], vgm_header_size - vgm_data_offset);

	if (!global_use_opl3) {
		write32le(&buf[vgm_opl2_clock], OPL2_CLOCK);
		settype(TYPE_OPL2);
	} else {
		write32le(&buf[vgm_opl3_clock], OPL3_CLOCK);
		settype(TYPE_OPL3);
	}

	buffer.insert(buffer.end(), buf, buf+sizeof(buf));
}

void VgmOpl::store_sleep(uint16_t n_samples) {
	uint8_t buf[3] = { vgm_cmd_wait_n_samples };
	write16le(&buf[1], n_samples);
	buffer.insert(buffer.end(), buf, buf+sizeof(buf));
}

void VgmOpl::write(int reg, int val) {
	if (buffered_sleep_samples) {
		store_sleep(buffered_sleep_samples);
		buffered_sleep_samples = 0;
	}

	uint8_t buf[3] = { vgm_cmd_write_ym3812, (uint8_t)reg, (uint8_t)val };

	if (currType == TYPE_OPL3) {
		buf[0] = currChip ? vgm_cmd_write_ymf262_port1
				  : vgm_cmd_write_ymf262_port0;
		if (global_verbose)
			printf("OPL3 C%d write: 0x%02x 0x%02x\n",
					currChip, reg, val);
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
		store_sleep(buffered_sleep_samples);
		buffered_sleep_samples = samples;
	}
}

void VgmOpl::save() {
	if (buffered_sleep_samples) {
		store_sleep(buffered_sleep_samples);
		buffered_sleep_samples = 0;
		buffer.push_back(vgm_cmd_end_of_sound_data);
	}

	write32le(&buffer[vgm_eof_offset], buffer.size() - vgm_eof_offset);
	write32le(&buffer[vgm_total_samples], sample_count);

	file->write(reinterpret_cast<const char *>(&buffer[0]), buffer.size());
}

// vi: ts=8 sw=8 noet
