#ifdef MSVC
#pragma region InChI-Trust Licence
/*
 * Reaction International Chemical Identifier (RInChI)
 * Version 1
 * Software version 1.00
 * 2022-01-14
 * 
 * The RInChI library and programs are free software developed under the
 * auspices of the International Union of Pure and Applied Chemistry (IUPAC).
 * 
 * IUPAC/InChI-Trust Licence No.1.0 for the 
 * Reaction International Chemical Identifier (RInChI) Software version 1.0
 * Copyright (C) IUPAC and InChI Trust Limited
 * 
 * This library is free software; you can redistribute it and/or modify it 
 * under the terms of the IUPAC/InChI Trust InChI Licence No.1.0, 
 * or any later version.
 * 
 * Please note that this library is distributed WITHOUT ANY WARRANTIES 
 * whatsoever, whether expressed or implied.  See the IUPAC/InChI Trust
 * Licence for the International Chemical Identifier (InChI) Software
 * ("IUPAC/InChI-Trust InChI Licence No. 1.0" in "LICENCE.TXT")
 * for more details.
 * 
 * You should have received a copy of the IUPAC/InChI Trust InChI 
 * Licence No. 1.0 with this library; if not, please write to:
 * 
 *     The InChI Trust
 *     8 Cavendish Avenue
 *     Cambridge CB1 7US
 *     UK
 *
 * or email to: alan@inchi-trust.org.
 *
 */
#pragma endregion
#endif

/**
  This shows how to use the RInChI call interface from C. The C interface
  defined in "rinchi_lib.h" will be used.
  The program will process a given small RXN file and output its RInChI string
  and key.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rinchi_lib.h"

#define RXN_BUF_SIZE 4000
#define RINCHI_STRING_BUF_SIZE 32000

void read_rxn_text_from_file(const char* filename, char* buffer)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Cannot open %s.\n", filename);
        exit(1);
    }
    /* Check that we can hold all file data in the 'rxn_text' buffer. */
    long file_size = -1;
    if (fseek(fp, 0L, SEEK_END) == 0) {
        file_size = ftell(fp);
        if (file_size == -1) {
            fprintf(stderr, "ERROR: Unable to get file size of %s.\n", filename);
            exit(1);
        }
        else {
            if (file_size > RXN_BUF_SIZE) {
                fprintf(stderr, "ERROR: File %sis too large to fit in text buffer. Please increase RXN_BUF_SIZE.\n", filename);
                exit(2);
            }
        }
    }
    /* Read all data from file. */
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "ERROR: Failed to reset file %s.\n", filename);
        exit(3);
    }
    size_t data_length = fread(buffer, sizeof(char), file_size, fp);
    int file_error = ferror(fp);
    if ( file_error != 0 ) {
        fprintf(stderr, "ERROR: Error %d reading file %s.\n", file_error, filename);
        exit(4);
    }
    buffer[data_length] = '\0'; /* Doubly ensure that string is terminated. */
    fclose(fp);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "ERROR: Expected an RXN file name as first parameter.\n");
        fprintf(stderr, "       NOTE: This program can read a file of maximum %d bytes.\n", RXN_BUF_SIZE);
        exit(1);
    }
    const char* rxn_filename = argv[1];

    /** Read RXN file into a char buffer. **/
    char rxn_text[RXN_BUF_SIZE + 1];
    read_rxn_text_from_file(rxn_filename, &rxn_text[0]);

    /**
     * Call RInChI C interface to calculate RInChI string and auxinfo
     * from file contents.
     **/
    int return_code;
    const char* rinchi_string_ptr;
    const char* rinchi_auxinfo_ptr;
    return_code = rinchilib_rinchi_from_file_text("RXN", &rxn_text[0], false, &rinchi_string_ptr, &rinchi_auxinfo_ptr);
    if (return_code != RETURN_CODE_SUCCESS) {
        fprintf(stderr, "ERROR: rinchi_from_file_text() failed with error message:\n%s", rinchilib_latest_err_msg());
        exit(10);
    }
    printf("%s\n%s\n", rinchi_string_ptr, rinchi_auxinfo_ptr);

    /**
      * Must copy the result, as the next call to the RInChI lib will
      * invalidate the 'rinchi_string' pointer (shared output buffer).
      **/
    if (strlen(rinchi_string_ptr) > RINCHI_STRING_BUF_SIZE) {
        fprintf(stderr, "ERROR: RInChI string is too long for buffer. Please increase RINCHI_STRING_BUF_SIZE.\n");
        exit(11);
    }
    char rinchi_string[RINCHI_STRING_BUF_SIZE + 1];
    strcpy(&rinchi_string[0], rinchi_string_ptr);

    /**
     * Call again, this time calculating the RInChI key from the already-calculating RInChI string.
     **/
    const char* rinchi_key;
    return_code = rinchilib_rinchikey_from_rinchi(&rinchi_string[0], "LONG", &rinchi_key);
    if (return_code != RETURN_CODE_SUCCESS) {
        fprintf(stderr, "ERROR: rinchikey_from_rinchi() failed with error message:\n%s\n", rinchilib_latest_err_msg());
        exit(10);
    }
    printf("%s\n", rinchi_key);
    return_code = rinchilib_rinchikey_from_rinchi(&rinchi_string[0], "WEB", &rinchi_key);
    if (return_code != RETURN_CODE_SUCCESS) {
        fprintf(stderr, "ERROR: rinchikey_from_rinchi() failed with error message:\n%s\n", rinchilib_latest_err_msg());
        exit(10);
    }
    printf("%s\n", rinchi_key);
}
