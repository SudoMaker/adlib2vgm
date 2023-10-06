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

#include "CommonIncludes.hpp"
#include "VgmOpl.hpp"
#include "Formats/Formats.hpp"

bool global_use_opl3;
bool global_verbose;
bool global_debug;

VgmOpl thisopl;

struct format {
    std::string name;
    std::function<std::unique_ptr<CPlayer>()> player;
};

std::unordered_map<std::string, struct format> format_list = {
	{ "CMF", { "Creative Music File Format by Creative Technology",
               []{return std::make_unique<CcmfPlayer>(&thisopl);} } },
	{ "RAD", { "Reality ADlib Tracker by Reality",
               []{return std::make_unique<Crad2Player>(&thisopl);} } },
	{ "D00", { "EdLib by Vibrants",
               []{return std::make_unique<Cd00Player>(&thisopl);} } },
	{ "DRO", { "DOSBox Raw OPL Format",
               []{return std::make_unique<CdroPlayer>(&thisopl);} } },
	{ "DRO2",{ "DOSBox Raw OPL Format",
               []{return std::make_unique<Cdro2Player>(&thisopl);} } },
	{ "RIX", { "Softstar RIX OPL Music Format",
               []{return std::make_unique<CrixPlayer>(&thisopl);} } },
	{ "LAA", { "Lucas Arts Adlib Music",
               []{return std::make_unique<CmidPlayer>(&thisopl);} } },
	{ "SCI", { "Sierra AdLib Music",
               []{return std::make_unique<CmidPlayer>(&thisopl);} } },
	{ "MID", { "MIDI Audio File Format",
               []{return std::make_unique<CmidPlayer>(&thisopl);} } },
	{ "MUS", { "AdLib MIDI Music Format by Ad Lib Inc.",
               []{return std::make_unique<CmusPlayer>(&thisopl);} } },
	{ "LDS", { "Loudness Sound System by Andras Molnar",
               []{return std::make_unique<CldsPlayer>(&thisopl);} } },
	{ "ROL", { "AdLib Visual Composer by Ad Lib Inc. (needs standard.bnk, too)",
               []{return std::make_unique<CrolPlayer>(&thisopl);} } },
	{ "A2M", { "AdLib Tracker 2",
               []{return std::make_unique<Ca2mLoader>(&thisopl);} } },
	{ "S3M", { "Scream Tracker 3 AdLib",
               []{return std::make_unique<Cs3mPlayer>(&thisopl);} } },
};

int main(int argc, char **argv) {
	cxxopts::Options options("adlib2vgm", "adlib2vgm - Converts adlib file formats to VGM format.");

	options.add_options()
		("h,help", "Show this help")
		("l,list-formats", "List supported formats / file extensions")
		("v,verbose", "Verbose output during conversion")
		("d,debug", "Show input format debug information")
		("m,mode", "Generation mode (opl2/opl3), not supported by all formats", cxxopts::value<std::string>()->default_value("opl2"))
		("f,format", "Override auto format detection", cxxopts::value<std::string>()->default_value(""))
		("i,in", "Input file", cxxopts::value<std::string>())
		("o,out", "Output file", cxxopts::value<std::string>())
		;

	std::string mode, input_file, output_file, format, format_name;

	try {
		auto cmd = options.parse(argc, argv);

		if (cmd.count("help")) {
			std::cout << options.help();
			return 0;
		}

		if (cmd.count("verbose")) global_verbose = true;
		if (cmd.count("debug"))   global_debug   = true;

		if (cmd.count("list-formats")) {
			puts("Supported formats / file extensions:");
			for (auto &it : format_list) {
                std::cout << it.first.c_str() << " - " 
                          << it.second.name   << "\n";
			}
			return 0;
		}

		if (!cmd.count("in") || !cmd.count("out")) {
			puts("error: please supply both input file and output file.");
			std::cout << options.help();
			return 1;
		}

		mode = cmd["mode"].as<std::string>();
		format = cmd["format"].as<std::string>();
		input_file = cmd["in"].as<std::string>();
		output_file = cmd["out"].as<std::string>();
	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << "\n";
		std::cout << options.help();
		return 1;
	}

	std::unique_ptr<CPlayer> player;
	global_use_opl3 = mode == "opl3";
	thisopl = {output_file};

	if (format.empty()) {
		auto ext = std::filesystem::path(input_file).extension().string();

		if (ext.size() > 1) {
			ext.erase(0, 1);
			format = ext;
		}
	}

	if (!format.empty()) {
		for (auto &it : format) {
			if (isalpha(it)) {
				it = toupper(it);
			}
		}

		auto it = format_list.find(format);

		if (it != format_list.end()) {
			player = it->second.player();
            format_name = it->second.name;
		} else {
			printf("error: file format `%s' is unsupported!\n", format.c_str());
			return 2;
		}
	} else {
		puts("error: please supply a file with proper extension, or use the -f option.");
		return 2;
	}

    std::cout << "trying " << format << ": " << format_name << "\n";

	if (!player->load(input_file, CProvider_Filesystem())) {
		puts("error: failed to open file");
		exit(2);
	}

	do {
		if (global_verbose) puts("tick");
		double hertz = player->getrefresh();
		double sleep_samples = 44100 / hertz;
		thisopl.insert_sleep(round(sleep_samples));
	} while (player->update());

	thisopl.save();

	puts("Done.");

	return 0;
}
