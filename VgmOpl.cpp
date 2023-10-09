/*
    This file is part of adlib2vgm.

    Copyright (C) 2021 ReimuNotMoe <reimu@sudomaker.com>
    Copyright (C) 2023 Ivo van Poorten <ivopvp@gmail.com>

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
#include "utf8.h"

#define OPL2_CLOCK 3579545
#define OPL3_CLOCK 14318180

#define vgm_header_size   0x80
#define vgm_eof_offset    0x04
#define vgm_gdm_offset    0x14
#define vgm_total_samples 0x18
#define vgm_loop_offset   0x1c
#define vgm_loop_samples  0x20
#define vgm_opl2_clock    0x50
#define vgm_opl3_clock    0x5c
#define vgm_version       0x08
#define vgm_data_offset   0x34

#define vgm_cmd_write_ym3812       0x5a
#define vgm_cmd_write_ymf262_port0 0x5e
#define vgm_cmd_write_ymf262_port1 0x5f
#define vgm_cmd_wait_n_samples     0x61
#define vgm_cmd_wait_735_samples   0x62
#define vgm_cmd_wait_882_samples   0x63
#define vgm_cmd_end_of_sound_data  0x66
#define vgm_cmd_wait_n1_samples    0x70	/* 0x70-0x7f: 0x7n --> n+1 samples */

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

VgmOpl::VgmOpl() {
	uint8_t buf[vgm_header_size] = "Vgm ";

	write32le(&buf[vgm_version], 0x151);
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

void VgmOpl::store_sleep(uint16_t &n_samples) {
	if (!n_samples) {
		return;
	} else if (n_samples <= 16) {
		buffer.push_back(vgm_cmd_wait_n1_samples + n_samples - 1);
	} else if (n_samples <= 32) {
		buffer.push_back(vgm_cmd_wait_n1_samples + 16 -1);
		buffer.push_back(vgm_cmd_wait_n1_samples + n_samples - 16 - 1);
	} else if (n_samples == 735) {
		buffer.push_back(vgm_cmd_wait_735_samples);
	} else if (n_samples >= 735+1 && n_samples <= 735+16) {
		buffer.push_back(vgm_cmd_wait_735_samples);
		buffer.push_back(vgm_cmd_wait_n1_samples + n_samples - 735 - 1);
	} else if (n_samples == 882) {
		buffer.push_back(vgm_cmd_wait_882_samples);
	} else if (n_samples >= 882+1 && n_samples <= 882+16) {
		buffer.push_back(vgm_cmd_wait_882_samples);
		buffer.push_back(vgm_cmd_wait_n1_samples + n_samples - 882 - 1);
	} else {
		buffer.push_back(vgm_cmd_wait_n_samples);
		buffer.push_back(n_samples & 0xff);
		buffer.push_back(n_samples >> 8);
	}
	n_samples = 0;
}

void VgmOpl::write(int reg, int val) {
	if (buffered_sleep_samples) store_sleep(buffered_sleep_samples);

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

	if ((int)buffered_sleep_samples + samples > UINT16_MAX)
		store_sleep(buffered_sleep_samples);
	buffered_sleep_samples += samples;
}

void VgmOpl::append_as_u16string(std::string line) {
	uint8_t buf[sizeof(char16_t)];

	std::u16string utf16line = utf8::utf8to16(line);

	for (char16_t c : utf16line) {
		write16le(&buf[0], c);
		buffer.insert(buffer.end(), buf, buf + sizeof(buf));
	}
	buffer.push_back(0);
	buffer.push_back(0);
}

int VgmOpl::save(std::string &filename) {
	if (buffered_sleep_samples) store_sleep(buffered_sleep_samples);
	buffer.push_back(vgm_cmd_end_of_sound_data);

	write32le(&buffer[vgm_total_samples], sample_count);

	unsigned int gd3_start = buffer.size();

	uint8_t buf[12] = "Gd3 ";

	write32le(&buf[4], 0x0100);		// verion 1.0
	buffer.insert(buffer.end(), buf, buf+sizeof(buf));

	append_as_u16string(title);		// Title ENG
	append_as_u16string("");		// Title JAP
	append_as_u16string("");		// Game name ENG
	append_as_u16string("");		// Game name JAP
	append_as_u16string("");		// System name ENG
	append_as_u16string("");		// System name JAP
	append_as_u16string(author);		// Author ENG
	append_as_u16string("");		// Author JAP
	append_as_u16string("");		// Date
	append_as_u16string("adlib2vgm");	// Converter
	append_as_u16string(desc);		// Notes

	unsigned int gd3_length = buffer.size() - gd3_start - 12;
	write32le(&buffer[gd3_start+8], gd3_length);

	write32le(&buffer[vgm_gdm_offset], gd3_start - vgm_gdm_offset);
	write32le(&buffer[vgm_eof_offset], buffer.size() - vgm_eof_offset);

	if (loop) {
		write32le(&buffer[vgm_loop_offset], vgm_header_size - vgm_loop_offset);
		write32le(&buffer[vgm_loop_samples], sample_count);
	}

	std::ofstream *file = new std::ofstream(filename, std::ios::out | std::ios::trunc | std::ios::binary);
	if (!file->good()) {
		std::cerr << "error opening " << filename << "\n";
		return EXIT_FAILURE;
	}

	file->write(reinterpret_cast<const char *>(&buffer[0]), buffer.size());
	if (!file->good()) {
		std::cerr << "error writing to " << filename << "\n";
		return EXIT_FAILURE;
	}

	file->close();
	if (!file->good()) {
		std::cerr << "error closing " << filename << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void VgmOpl::set_author(std::string s) {
	author = s;
}

void VgmOpl::set_title(std::string s) {
	title = s;
}

void VgmOpl::set_desc(std::string s) {
	desc = s;
}

void VgmOpl::set_loop(bool enable) {
	loop = enable;
}

// vi: ts=8 sw=8 noet
