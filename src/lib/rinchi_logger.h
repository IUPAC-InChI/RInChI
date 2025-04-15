#ifndef IUPAC_RINCHI_LOGGER_HEADER_GUARD
#define IUPAC_RINCHI_LOGGER_HEADER_GUARD

#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include "rinchi_utils.h"

namespace rinchi {

/**
  Log messages to an internal list. Used by RInChI readers and writers to log warnings.
  **/
class RInChILogger {
private:
	StringVector m_msg_list;
public:
	void clear();
	void log(const std::string& msg);

	// All log messages separated by linefeeds.
	std::string all_msgs();
};


/**
  Default instance that will be used to log warnings.
  **/
RInChILogger& default_warning_logger();

}
#endif
