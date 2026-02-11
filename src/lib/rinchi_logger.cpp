#ifdef MSVC
#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../LICENCE.txt.
#pragma endregion
#endif

#include <rinchi_logger.h>

#include <sstream>

namespace rinchi {

void RInChILogger::clear()
{
	m_msg_list.clear();
}

void RInChILogger::log(const std::string& msg)
{
	m_msg_list.push_back(msg);
}

std::string RInChILogger::all_msgs()
{
	std::stringstream result;
	for (StringVector::const_iterator s = m_msg_list.begin(); s != m_msg_list.end(); s++)
		result << *s << "\n";

	return result.str();
}

RInChILogger& default_warning_logger()
{
	static RInChILogger _default_instance = RInChILogger();
	return _default_instance;
}

} // end of namespace
