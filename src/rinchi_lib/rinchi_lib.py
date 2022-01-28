#pragma region InChI-Trust Licence
#/*
# * Reaction International Chemical Identifier (RInChI)
# * Version 1
# * Software version 1.00
# * 2022-01-14
# * 
# * The RInChI library and programs are free software developed under the
# * auspices of the International Union of Pure and Applied Chemistry (IUPAC).
# * 
# * IUPAC/InChI-Trust Licence No.1.0 for the 
# * Reaction International Chemical Identifier (RInChI) Software version 1.0
# * Copyright (C) IUPAC and InChI Trust Limited
# * 
# * This library is free software; you can redistribute it and/or modify it 
# * under the terms of the IUPAC/InChI Trust InChI Licence No.1.0, 
# * or any later version.
# * 
# * Please note that this library is distributed WITHOUT ANY WARRANTIES 
# * whatsoever, whether expressed or implied.  See the IUPAC/InChI Trust
# * Licence for the International Chemical Identifier (InChI) Software
# * ("IUPAC/InChI-Trust InChI Licence No. 1.0" in "LICENCE.TXT")
# * for more details.
# * 
# * You should have received a copy of the IUPAC/InChI Trust InChI 
# * Licence No. 1.0 with this library; if not, please write to:
# * 
# *     The InChI Trust
# *     8 Cavendish Avenue
# *     Cambridge CB1 7US
# *     UK
# *
# * or email to: alan@inchi-trust.org.
# *
# */
#pragma endregion

import os
from ctypes import *

class RInChI:
	def __init__(self, lib_path = "."):
		if os.sep == "\\":
			self.lib_handle = cdll.LoadLibrary(lib_path + "/librinchi.dll")
		else:
			self.lib_handle = cdll.LoadLibrary(lib_path + "/librinchi.so.1.0.0")

		self.lib_latest_error_message = self.lib_handle.rinchilib_latest_err_msg
		self.lib_latest_error_message.restype = c_char_p

		self.lib_rinchi_from_file_text = self.lib_handle.rinchilib_rinchi_from_file_text
		self.lib_rinchi_from_file_text.argtypes = [c_char_p, c_char_p, c_bool, POINTER(c_char_p), POINTER(c_char_p)]
		self.lib_rinchi_from_file_text.restype = c_long

		self.lib_rinchikey_from_file_text = self.lib_handle.rinchilib_rinchikey_from_file_text
		self.lib_rinchikey_from_file_text.argtypes = [c_char_p, c_char_p, c_char_p, c_bool, POINTER(c_char_p)]
		self.lib_rinchikey_from_file_text.restype = c_long

		self.lib_file_text_from_rinchi = self.lib_handle.rinchilib_file_text_from_rinchi
		self.lib_file_text_from_rinchi.argtypes = [c_char_p, c_char_p, c_char_p, POINTER(c_char_p)]
		self.lib_file_text_from_rinchi.restype = c_long

		self.lib_inchis_from_rinchi = self.lib_handle.rinchilib_inchis_from_rinchi
		self.lib_inchis_from_rinchi.argtypes = [c_char_p, c_char_p, POINTER(c_char_p)]
		self.lib_inchis_from_rinchi.restype = c_long

		self.lib_rinchikey_from_rinchi = self.lib_handle.rinchilib_rinchikey_from_rinchi
		self.lib_rinchikey_from_rinchi.argtypes = [c_char_p, c_char_p, POINTER(c_char_p)]
		self.lib_rinchikey_from_rinchi.restype = c_long

	def rinchi_errorcheck(self, return_code):
		if return_code != 0:
			raise Exception (self.lib_latest_error_message().decode('utf-8'))

	def rinchi_from_file_text( self, input_format, rxnfile_data, force_equilibrium = False ):
		"""Generates RInChI string and RAuxInfo from supplied RD or RXN file text. Result is returned as a tuple."""
		result_rinchi_string  = c_char_p()
		result_rinchi_auxinfo = c_char_p()
		self.rinchi_errorcheck( self.lib_rinchi_from_file_text(input_format.encode('utf-8'), rxnfile_data.encode('utf-8'), force_equilibrium, byref(result_rinchi_string), byref(result_rinchi_auxinfo)) )
		return [str(result_rinchi_string.value.decode('utf-8')), str(result_rinchi_auxinfo.value.decode('utf-8'))]

	def rinchikey_from_file_text( self, input_format, file_text, key_type, force_equilibrium = False ):
		"""Generates RInChI key of supplied RD or RXN file text."""
		result = c_char_p()
		self.rinchi_errorcheck( self.lib_rinchikey_from_file_text(input_format.encode('utf-8'), file_text.encode('utf-8'), key_type.encode('utf-8'), force_equilibrium, byref(result)) )
		return str(result.value.decode('utf-8'))

	def file_text_from_rinchi( self, rinchi_string, rinchi_auxinfo, output_format ):
		"""Reconstructs (or attempts to reconstruct) RD or RXN file from RInChI string and RAuxInfo. RAuxInfo is not strictly required, but highly recommended."""
		result = c_char_p()
		self.rinchi_errorcheck( self.lib_file_text_from_rinchi(rinchi_string.encode('utf-8'), rinchi_auxinfo.encode('utf-8'), output_format.encode('utf-8'), byref(result)) )
		return str(result.value.decode('utf-8'))

	def inchis_from_rinchi( self, rinchi_string, rinchi_auxinfo ):
		"""Splits an RInChI string and optional RAuxInfo into components. Returns a dictionary of five lists: Direction, No-Structures, Reactants, Products, and Agents. Each Reactant, Product, and Agent list contains a set of (InChI, AuxInfo) tuples. The No-Structures list contains No-Structure counts for Reactants, Products, and Agents."""
		inchis = c_char_p()
		self.rinchi_errorcheck( self.lib_inchis_from_rinchi(rinchi_string.encode('utf-8'), rinchi_auxinfo.encode('utf-8'), byref(inchis)) )

		lines = str(inchis.value.decode('utf-8')).split("\n")
		# Get rid of trailing line, if any.
		if lines[len(lines) - 1] == "":
			lines = lines[:len(lines) - 1]
		# Sanity check: Must contain an even number of lines (direction line + No-Structure count line + n * InChI+AuxInfo line pairs).
		if len(lines) % 2 != 0:
			raise Exception ("Invalid number of lines (" + str(len(lines)) + ") produced by RInChI library function 'rinchilib_inchis_from_rinchi()'.")

		direction = lines[0]
		if direction[:2] != "D:":
			raise Exception ("Invalid direction line (must be first line received from RInChI library function 'rinchilib_inchis_from_rinchi()'.")
		direction = direction[2:]
		lines = lines[1:]

		nostruct_count_line = lines[0]
		if nostruct_count_line[:2] != "N:":
			raise Exception ("Invalid No-Structure count line (must be second line received from RInChI library function 'rinchilib_inchis_from_rinchi()'.")
		nostruct_count_line = nostruct_count_line[2:]
		nostruct_counts = [int(x) for x in nostruct_count_line.split(",")]
		lines = lines[1:]

		reactants = []
		products  = []
		agents    = []

		for i in range(0, int(len(lines) / 2)):
			component_prefix = lines[i * 2][:2]
			inchi_string     = lines[i * 2][2:]
			aux_info         = lines[i * 2 + 1][2:]
			# Sanity check: Must contain InChI + AuxInfo pairs.
			if not inchi_string.startswith("InChI"):
				raise Exception ("Invalid InChI string '" + inchi_string + "'.")
			if aux_info != "" and (not aux_info.startswith("AuxInfo")):
				raise Exception ("Invalid AuxInfo '" + aux_info + "'.")

			if component_prefix == "R:":
				reactants.append( (inchi_string, aux_info) )
			elif component_prefix == "P:":
				products.append( (inchi_string, aux_info) )
			elif component_prefix == "A:":
				agents.append( (inchi_string, aux_info) )
			else:
				raise Exception ("Unsupported component prefix '" + component_prefix + "'.")

		return {"Direction": direction, "No-Structures": nostruct_counts, "Reactants": reactants, "Products": products, "Agents": agents}

	def rinchikey_from_rinchi( self, rinchi_string, key_type ):
		"""Generates RInChI key of supplied RD or RXN file text."""
		result = c_char_p()
		self.rinchi_errorcheck( self.lib_rinchikey_from_rinchi(rinchi_string.encode('utf-8'), key_type.encode('utf-8'), byref(result)) )
		return result.value.decode('utf-8')

