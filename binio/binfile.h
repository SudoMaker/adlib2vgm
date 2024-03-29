/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * binfile.h - Binary file I/O
 * Copyright (C) 2002, 2003 Simon Peter <dn.tlp@gmx.net>
 */

#ifndef H_BINIO_BINFILE
#define H_BINIO_BINFILE

#include <stdio.h>

#include "binio.h"

class binfbase: virtual public binio
{
public:
  typedef enum {
    Append	= 1 << 0,
    NoCreate	= 1 << 1
  } ModeFlags;

  typedef int Mode;

  binfbase();
  ~binfbase() override;

  virtual void open(const char *filename, const Mode mode) = 0;
#if BINIO_ENABLE_STRING
  virtual void open(const std::string &filename, const Mode mode) = 0;
#endif
  void close();

  void seek(long pos, Offset offs = Set) override;
  long pos() override;

protected:
  FILE *f;
};

class binifstream: public binistream, virtual public binfbase
{
public:
  binifstream();
  binifstream(const char *filename, const Mode mode = NoCreate);
#if BINIO_ENABLE_STRING
  binifstream(const std::string &filename, const Mode mode = NoCreate);
#endif

  ~binifstream() override;

  void open(const char *filename, const Mode mode = NoCreate) override;
#if BINIO_ENABLE_STRING
  void open(const std::string &filename, const Mode mode = NoCreate) override;
#endif

protected:
  Byte getByte() override;
};

class binofstream: public binostream, virtual public binfbase
{
public:
  binofstream();
  binofstream(const char *filename, const Mode mode = 0);
#if BINIO_ENABLE_STRING
  binofstream(const std::string &filename, const Mode mode = 0);
#endif

  ~binofstream() override;

  void open(const char *filename, const Mode mode = 0) override;
#if BINIO_ENABLE_STRING
  void open(const std::string &filename, const Mode mode = 0) override;
#endif

protected:
  void putByte(Byte b) override;
};

class binfstream: public binifstream, public binofstream
{
public:
  binfstream();
  binfstream(const char *filename, const Mode mode = 0);
#if BINIO_ENABLE_STRING
  binfstream(const std::string &filename, const Mode mode = 0);
#endif

  ~binfstream() override;

  void open(const char *filename, const Mode mode = 0) override;
#if BINIO_ENABLE_STRING
  void open(const std::string &filename, const Mode mode = 0) override;
#endif
};

#endif
