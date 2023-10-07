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

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <filesystem>

#include <cmath>
#include <cassert>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>

#define stricmp			strcasecmp
#define AdPlug_LogWrite(...)		if (global_debug) printf(__VA_ARGS__)
#define AdPlug_LogError(...)		fprintf(stderr, __VA_ARGS__)

extern bool global_use_opl3;
extern bool global_verbose;
extern bool global_debug;

// portable unused
#define UNUSED(x) (void)(x)
