/*
    This file is part of adlib2vgm.

    Copyright (C) 2021 ReimuNotMoe <reimu@sudomaker.com>
    Copyright (C) 2023 Ivo van poorten <ivopvp@gmail.com>

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

#include "CommonIncludes.h"
#include "VgmOpl.h"
#include "Formats/Formats.h"
#include "cxxopts.h"

bool global_use_opl3;
bool global_verbose;
bool global_debug;

VgmOpl thisopl;

struct format {
	std::string name;
	std::function<std::unique_ptr<CPlayer>()> player;
};

std::vector<std::string> force_opl3 = {
	"PIS", "SNG3", "SOP"
};

std::map<std::string, struct format> format_list = {
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
	{ "MKF", { "Softstar RIX OPL Music Format in MKF container",
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
	{ "ROL", { "AdLib Visual Composer by Ad Lib Inc. (needs standard.bnk)",
               []{return std::make_unique<CrolPlayer>(&thisopl);} } },
	{ "A2M", { "AdLib Tracker 2",
               []{return std::make_unique<Ca2mLoader>(&thisopl);} } },
	{ "AS3M", { "Scream Tracker 3 AdLib",
               []{return std::make_unique<Cs3mPlayer>(&thisopl);} } },
	{ "S3M", { "Scream Tracker 3 AdLib",
               []{return std::make_unique<Cs3mPlayer>(&thisopl);} } },
	{ "RAW", { "Raw AdLib Capture",
               []{return std::make_unique<CrawPlayer>(&thisopl);} } },
	{ "U6M", { "Ultima 6 Music",
               []{return std::make_unique<Cu6mPlayer>(&thisopl);} } },
	{ "SA2", { "Surprise AdLib Tracker",
               []{return std::make_unique<Csa2Loader>(&thisopl);} } },
	{ "SAT", { "Surprise AdLib Tracker",
               []{return std::make_unique<Csa2Loader>(&thisopl);} } },
	{ "AMD", { "AMUSIC AdLib Tracker",
               []{return std::make_unique<CamdLoader>(&thisopl);} } },
	{ "IMF", { "Apogee (id/3D Realms) Music File",
               []{return std::make_unique<CimfPlayer>(&thisopl);} } },
	{ "WLF", { "Wolfenstein 3D Music File",
               []{return std::make_unique<CimfPlayer>(&thisopl);} } },
	{ "SNG", { "AdLib Tracker 1.0",
               []{return std::make_unique<CadtrackLoader>(&thisopl);} } },
	{ "BAM", { "Bob's Adlib Music Player",
               []{return std::make_unique<CbamPlayer>(&thisopl);} } },
	{ "BMF", { "Easy AdLib 1.0 (aka FMPlay) by The Brain",
               []{return std::make_unique<CxadbmfPlayer>(&thisopl);} } },
	{ "CFF", { "Boom Tracker",
               []{return std::make_unique<CcffLoader>(&thisopl);} } },
	{ "CMF2", { "SoundFX Macs Opera CMF Player",
               []{return std::make_unique<CcmfmacsoperaPlayer>(&thisopl);} } },
	{ "DTM", { "DeFy AdLib Tracker",
               []{return std::make_unique<CdtmLoader>(&thisopl);} } },
	{ "XAD", { "Lunatic Flash/Logic Design Player",
               []{return std::make_unique<CxadflashPlayer>(&thisopl);} } },
	{ "XAD2", { "Hybrid MiG-29M Player by Domark",
               []{return std::make_unique<CxadhybridPlayer>(&thisopl);} } },
	{ "XAD3", { "Hyp LKCC/SAC Player",
               []{return std::make_unique<CxadhypPlayer>(&thisopl);} } },
	{ "XAD4", { "Psi/Future Crew PGRID Player",
               []{return std::make_unique<CxadpsiPlayer>(&thisopl);} } },
	{ "XAD5", { "Ratt/GRIF Player",
               []{return std::make_unique<CxadratPlayer>(&thisopl);} } },
	{ "SNG2", { "Faust Music Tracker",
               []{return std::make_unique<CfmcLoader>(&thisopl);} } },
	{ "GOT", { "God Of Thunder Player",
               []{return std::make_unique<CgotPlayer>(&thisopl);} } },
	{ "AGD", { "HERAD Herbulot AdLib Player",
               []{return std::make_unique<CheradPlayer>(&thisopl);} } },
	{ "SDB", { "HERAD Herbulot AdLib Player",
               []{return std::make_unique<CheradPlayer>(&thisopl);} } },
	{ "SQX", { "HERAD Herbulot AdLib Player",
               []{return std::make_unique<CheradPlayer>(&thisopl);} } },
	{ "HSC", { "HSC Adlib Composer / HSC-Tracker",
               []{return std::make_unique<ChscPlayer>(&thisopl);} } },
	{ "JBM", { "Johannes Bjerregaard's JBM Adlib Music Format",
               []{return std::make_unique<CjbmPlayer>(&thisopl);} } },
	{ "KSM", { "Ken Silverman Music (requires insts.dat)",
               []{return std::make_unique<CksmPlayer>(&thisopl);} } },
	{ "MAD", { "Mlat Adlib Tracker",
               []{return std::make_unique<CmadLoader>(&thisopl);} } },
	{ "MDI", { "AdLib Visual Composer MIDIPlay File",
               []{return std::make_unique<CmdiPlayer>(&thisopl);} } },
	{ "PIS", { "Beni Tracker PIS module",
               []{return std::make_unique<CpisPlayer>(&thisopl);} } },
	{ "SNG3", { "SNGPlay File Format",
               []{return std::make_unique<CsngPlayer>(&thisopl);} } },
	{ "SOP", { "Note Sequencer v1.0 by sopepos",
               []{return std::make_unique<CsopPlayer>(&thisopl);} } },
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
		("s,subsong", "Select subsong", cxxopts::value<unsigned int>()->default_value("1"))
		("L,loop", "Enable looping")
		;

	std::string mode, input_file, output_file, format;
	unsigned int subsong;
	bool loop = false;

	try {
		auto cmd = options.parse(argc, argv);

		if (cmd.count("help")) {
			std::cout << options.help();
			return 0;
		}

		if (cmd.unmatched().size()) {
			std::cout << "error: unmatched command line arguments: ";
			for (auto u : cmd.unmatched())
				std::cout << u << " ";
			std::cout << "\n";
			return 2;
		}

		if (cmd.count("verbose")) global_verbose = true;
		if (cmd.count("debug"))   global_debug   = true;
		if (cmd.count("loop"))    loop           = true;

		if (cmd.count("list-formats")) {
			puts("Supported formats / file extensions:");
			for (auto &it : format_list) {
				std::cout << it.first.c_str() << ": "
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
		subsong = cmd["subsong"].as<unsigned int>();

	} catch (std::exception &e) {
		std::cout << "Error: " << e.what() << "\n";
		std::cout << options.help();
		return 1;
	}

	if (format.empty()) {
		auto ext = std::filesystem::path(input_file).extension().string();

		if (ext.size() > 1) {
			ext.erase(0, 1);
			format = ext;
		}
	}

	std::transform(format.begin(), format.end(), format.begin(), ::toupper);

	auto force = std::find(force_opl3.begin(), force_opl3.end(), format);
	if (force != force_opl3.end()) {
		printf("forcing OPL3 for format %s\n", format.c_str());
		mode = "opl3";
	}

	global_use_opl3 = mode == "opl3";
	printf("mode: %s\n", mode.c_str());

	thisopl = { };

	std::unique_ptr<CPlayer> player;

	auto it = format_list.find(format);

	if (it != format_list.end()) {
		printf("using loader %s: %s\n", format.c_str(), it->second.name.c_str());
		player = it->second.player();
		if (!player->load(input_file, CProvider_Filesystem())) {
			puts("error: failed to load file");
			return EXIT_FAILURE;
		}
	} else {
		bool success = false;
		printf("file format '%s' not found\n", format.c_str());
		for (auto detect : format_list) {
			printf("trying %s: %s\n", detect.first.c_str(), detect.second.name.c_str());
			player = detect.second.player();
			if (player->load(input_file, CProvider_Filesystem())) {
				puts("success!");
				success = true;
				break;
			}
		}
		if (!success) {
			puts("error: unable to load file");
			return EXIT_FAILURE;
		}
	}

	unsigned int subsongs = player->getsubsongs();

	if (subsong == 0 || subsong > subsongs) {
		printf("error: subsong out of range [1-%d]\n", subsongs);
		return EXIT_FAILURE;
	}

	player->rewind(subsong - 1);

	double sleep_samples = 0.0;

	do {
		if (global_verbose) puts("tick");
		double hertz = player->getrefresh();
		sleep_samples += 44100 / hertz;
		thisopl.insert_sleep(round(sleep_samples));
		sleep_samples -= round(sleep_samples);
	} while (player->update());

	thisopl.set_author(player->getauthor());
	thisopl.set_title(player->gettitle());
	thisopl.set_desc(player->getdesc());
	thisopl.set_loop(loop);

	int retval = thisopl.save(output_file);

	puts("Done.");

	return retval;
}

// vi: ts=8 sw=8 noet
