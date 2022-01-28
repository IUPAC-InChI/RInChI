#ifndef IUPAC_RINCHI_MDL_MOLFILE_HEADER_GUARD
#define IUPAC_RINCHI_MDL_MOLFILE_HEADER_GUARD

#ifdef MSVC
#pragma region BSD-license
/*
 * Reaction International Chemical Identifier (RInChI)
 * Version 1
 * Software version 1.00
 * 2022-01-14
 * 
 * This source file is based on work created by Biochemfusion Holding ApS.
 * It is released under a BSD-style license.
 * 
 * Copyright (C) 2010 - 2015, Biochemfusion Holding ApS
 * (http://www.biochemfusion.com). All rights reserved.
 * 
 * Redistribution and use for any purpose in source and binary forms, with or
 * without modification, are permitted, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY Biochemfusion Holding ApS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Biochemfusion Holding ApS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma endregion
#endif

#include <string>

/** Constants defining tags in MDL molfiles. **/

namespace rinchi {

	extern const std::string MDL_TAG_V2000;
	extern const std::string MDL_TAG_V3000;

	extern const std::string MDL_TAG_MOLFILE_END;

	// V2000 constants for reading InChI-relevant data.

	extern const std::string MDL_TAG_CHARGE;
	extern const std::string MDL_TAG_RADICAL;
	extern const std::string MDL_TAG_ISOTOPE;

	extern const std::string MDL_TAG_ATOM_ALIAS;
	extern const std::string MDL_TAG_ATOM_VALUE;
	extern const std::string MDL_TAG_GROUP_ABBREVIATION;

	extern const int MDL_MAX_V2000_SGROUP_TEXT_LINE_LENGTH;
	extern const int MDL_MAX_MOLECULE_NAME_LENGTH;

	// V3000 constants.

	extern const int MDL_MAX_V3000_LINE_LENGTH;
	extern const int MDL_TAG_V30_LINE_LENGTH;
	extern const std::string MDL_TAG_V30_LINE;
	extern const std::string MDL_TAG_V30_BLOCK_CTAB_BEGIN;
	extern const std::string MDL_TAG_V30_COUNTS;
	extern const std::string MDL_TAG_V30_BLOCK_CTAB_END;
	extern const std::string MDL_TAG_V30_STEREO_CFG_KEY;

	extern const std::string MDL_TAG_V30_BLOCK_ATOM_BEGIN;
	extern const std::string MDL_TAG_V30_BLOCK_ATOM_END;
	extern const std::string MDL_TAG_V30_ATOM_KEY_CHARGE;
	extern const std::string MDL_TAG_V30_ATOM_KEY_VALENCE;
	extern const std::string MDL_TAG_V30_ATOM_KEY_ISOTOPE;
	extern const std::string MDL_TAG_V30_ATOM_KEY_RADICAL;

	extern const std::string MDL_TAG_V30_BLOCK_BOND_BEGIN;
	extern const std::string MDL_TAG_V30_BLOCK_BOND_END;

	extern const std::string MDL_TAG_V30_BLOCK_COLLECTION_BEGIN;
	extern const std::string MDL_TAG_V30_HILITE_COLLECTION_NAME;
	extern const std::string MDL_TAG_V30_COLLECTION_ATOMS;
	extern const std::string MDL_TAG_V30_BLOCK_COLLECTION_END;

} // end of namespace

#endif
