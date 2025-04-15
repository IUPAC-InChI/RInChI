create or replace package rinorca is

#pragma region RInChI-license
// Copyright (C) 2017 - 2024 InChI Project. All Rights Reserved.
// This file is part of the RInChI source code.
// The contents are covered by the terms of the MIT license
// included in the file ./../../../LICENCE.txt.
#pragma endregion

  -- Author  : JHJE
  -- Created : 2014-11-26 23:15:43
  -- Purpose : Wrapper for C++ RInChI library for Oracle.
  
  function FILE_FORMAT_RD return varchar2;
  function FILE_FORMAT_RXN return varchar2;
  
  /**
    Create RInChI string from an RXN or RD file in 'an_input_file'.

    When 'include_auxinfo' is <> 0 RAuxInfo will be included in the output,
    separated from the RInChI string by a linefeed.
  **/
  function rinchi_from_file(an_input_file clob, an_input_format varchar2, include_auxinfo integer default 1) return clob;
  
  /**
    Create RInChI key from an RXN or RD file in 'an_input_file'.

    'a_key_type' is a 1-letter string that determines the type of key returned: L(ong), S(hort) or W(eb).
  **/
  function rinchikey_from_file(an_input_file clob, an_input_format varchar2, a_key_type varchar2) return clob;

  /**
    Create RInChI key from an RInChI string in 'an_rinchi_string'.

    'a_key_type' is a 1-letter string that determines the type of key returned: L(ong), S(hort) or W(eb).
  **/
  function rinchikey_from_rinchi(an_rinchi_string clob, a_key_type varchar2) return clob;

  /**
    Reconstruct an RXN or RD file from RInChI and RAuxInfo. The RAuxInfo
    should be separated from the RInChI string by a linefeed, following the
    convention of the output from rinchi_from_file().

    You are allowed to pass in only RInChI strings without RAuxInfo, but in 
    that case only 0D structures can currently be re-generated.
  **/
  function file_from_rinchi(rinchi_input clob, an_output_format varchar2) return clob;

  /**
    To test the chunk transfer logic, change definitions of 
    
      MAX_PLSQL_VARCHAR2_LEN constant integer := 32000;
      subtype pls_largest_varchar2 is varchar2(32000);

    to read

      MAX_PLSQL_VARCHAR2_LEN constant integer := 8;
      subtype pls_largest_varchar2 is varchar2(8);

    and run 'test_chunk_xfer'.
  **/
  procedure test_chunk_xfer;

end rinorca;
/
create or replace package body rinorca is

  RETURN_CODE_SUCCESS constant integer := 0;

  MAX_PLSQL_VARCHAR2_LEN constant integer := 32000;
  -- PL/SQL won't allow me to declare the following using the constant above but it was the intention.
  subtype pls_largest_varchar2 is varchar2(32000);

  /** Alternative definitions to use for chunk transfer testing:
  MAX_PLSQL_VARCHAR2_LEN constant integer := 8;
  -- PL/SQL won't allow me to declare the following using the constant above but it was the intention.
  subtype pls_largest_varchar2 is varchar2(8);
  **/

  function rinorca_latest_err_msg
    return string
  is
    language c
    library rinorca_lib
    name "rinorca_latest_err_msg"
    parameters (return string);

  function rinorca_debug_input_buffer
    return string
  is
    language c
    library rinorca_lib
    name "rinorca_debug_input_buffer"
    parameters (return string);

  function rinorca_set_input_chunk (
    append_to_buffer in pls_integer,
    chunk in string
  )
    return pls_integer
  is
    language c
    library rinorca_lib
    name "rinorca_set_input_chunk"
    parameters (
      append_to_buffer int,
      chunk string,
      return int
    );

  function rinorca_rinchi_from_file (
    append_to_buffer in pls_integer,
    input_file in string,
    input_format in string,
    include_auxinfo in pls_integer,
    out_data out string
  )
    return pls_integer
  is
    language c
    library rinorca_lib
    name "rinorca_rinchi_from_file"
    parameters (
      append_to_buffer int,
      input_file string,
      input_format string,
      include_auxinfo int,
      out_data string, out_data maxlen,
      return int
    );

  function rinorca_rinchikey_from_file (
    append_to_buffer in pls_integer,
    input_file in string,
    input_format in string,
	key_type in string,
    out_data out string
  )
    return pls_integer
  is
    language c
    library rinorca_lib
    name "rinorca_rinchikey_from_file"
    parameters (
      append_to_buffer int,
      input_file string,
      input_format string,
      key_type string,
      out_data string, out_data maxlen,
      return int
    );

  function rinorca_rinchikey_from_rinchi (
    append_to_buffer in pls_integer,
    rinchi_string in string,
    key_type in string,
    out_data out string
  )
    return pls_integer
  is
    language c
    library rinorca_lib
    name "rinorca_rinchikey_from_rinchi"
    parameters (
      append_to_buffer int,
      rinchi_string string,
      key_type string,
      out_data string, out_data maxlen,
      return int
    );

  function rinorca_file_from_rinchi (
    rinchi_string in string,
    rinchi_auxinfo in string,
    output_format in string,
    out_data out string
  )
    return pls_integer
  is
    language c
    library rinorca_lib
    name "rinorca_file_from_rinchi"
    parameters (
      rinchi_string string,
      rinchi_auxinfo string,
      output_format string,
      out_data string, out_data maxlen,
      return int
    );

  procedure raise_error(err_msg varchar2)
  is
  begin
    raise_application_error(-20000, 'RINORCA-1: ' || err_msg);
  end;

  -- Test return value of function, raise appropriate error message
  -- if return value indicates failure.
  procedure check_return(return_value in pls_integer)
  is
  begin
    if return_value <> RETURN_CODE_SUCCESS then
      raise_error(rinorca_latest_err_msg());
    end if;
  end;

  /**
    Transfers the part of 'input' that is larger than 32k to
    cartridge's input buffer. Transfer is done in chunks of
    32k. The last chunk - which will be 32k or smaller - is
    returned.
  **/
  function set_gt32k_input(input in clob, total_length in pls_integer) return varchar2
  is
    data clob;
    buffer pls_largest_varchar2;
    bytes_xfrd pls_integer;
    bytes_read pls_integer;
  begin
    bytes_read := MAX_PLSQL_VARCHAR2_LEN;
    data := input;
    dbms_lob.open(data, dbms_lob.lob_readonly);
    begin
      dbms_lob.read(data, bytes_read, 1, buffer);
      check_return( rinorca_set_input_chunk(0, buffer) );

      bytes_xfrd := MAX_PLSQL_VARCHAR2_LEN;
      loop
        bytes_read := MAX_PLSQL_VARCHAR2_LEN;
        dbms_lob.read(data, bytes_read, bytes_xfrd + 1, buffer);
        bytes_xfrd := bytes_xfrd + bytes_read;
        exit when
          bytes_xfrd >= total_length;
        check_return( rinorca_set_input_chunk(1, buffer) );
      end loop;

      dbms_lob.close(data);
    exception
      when others then
        dbms_lob.close(data);
        raise;
    end;

    return buffer;
  end;

  function rinchi_from_file(an_input_file clob, an_input_format varchar2, include_auxinfo integer) return clob
  is
    total_input_length integer;
    input pls_largest_varchar2;
    append_input pls_integer := 0;
    --
    rinchi_string pls_largest_varchar2;
  begin
    if an_input_file is null then
      raise_error('NULL input.');
    end if;
    if an_input_format is null then
      raise_error('Input format undefined - NULL.');
    end if;

    total_input_length := dbms_lob.getlength(an_input_file);
    if total_input_length = 0 then
      raise_error('Empty input.');
    end if;

    if total_input_length <= MAX_PLSQL_VARCHAR2_LEN then
      append_input := 0;
      input := an_input_file;
    else
      append_input := 1;
      input := set_gt32k_input(an_input_file, total_input_length);
    end if;

    check_return(
      rinorca_rinchi_from_file(append_input, input, an_input_format, nvl(include_auxinfo, 0), rinchi_string)
    );
    return rinchi_string;
  end;

  function rinchikey_from_file(an_input_file clob, an_input_format varchar2, a_key_type varchar2) return clob
  is
    total_input_length integer;
    input pls_largest_varchar2;
    append_input pls_integer := 0;
    --
    rinchi_key pls_largest_varchar2;
  begin
    if an_input_file is null then
      raise_error('NULL input.');
    end if;
    if an_input_format is null then
      raise_error('Input format undefined - NULL.');
    end if;
    if a_key_type is null then
      raise_error('Requested key type undefined - NULL.');
    end if;

    total_input_length := dbms_lob.getlength(an_input_file);
    if total_input_length = 0 then
      raise_error('Empty input.');
    end if;

    if total_input_length <= MAX_PLSQL_VARCHAR2_LEN then
      append_input := 0;
      input := an_input_file;
    else
      append_input := 1;
      input := set_gt32k_input(an_input_file, total_input_length);
    end if;

    check_return(
      rinorca_rinchikey_from_file(append_input, input, an_input_format, a_key_type, rinchi_key)
    );
    return rinchi_key;
  end;

  function rinchikey_from_rinchi(an_rinchi_string clob, a_key_type varchar2) return clob
  is
    total_input_length integer;
    input pls_largest_varchar2;
    append_input pls_integer := 0;
    --
    rinchi_key pls_largest_varchar2;
  begin
    if an_rinchi_string is null then
      raise_error('NULL input.');
    end if;
    if a_key_type is null then
      raise_error('Requested key type undefined - NULL.');
    end if;

    total_input_length := dbms_lob.getlength(an_rinchi_string);
    if total_input_length = 0 then
      raise_error('Empty input.');
    end if;

    if total_input_length <= MAX_PLSQL_VARCHAR2_LEN then
      append_input := 0;
      input := an_rinchi_string;
    else
      append_input := 1;
      input := set_gt32k_input(an_rinchi_string, total_input_length);
    end if;

    check_return(
      rinorca_rinchikey_from_rinchi(append_input, input, a_key_type, rinchi_key)
    );
    return rinchi_key;
  end;

  function file_from_rinchi(rinchi_input clob, an_output_format varchar2) return clob
  is
    linefeed_pos pls_integer;
    the_rinchi_string pls_largest_varchar2;
    the_rauxinfo pls_largest_varchar2;
    file_text pls_largest_varchar2;
  begin
    if rinchi_input is null then
      raise_error('NULL input.');
    end if;
    if dbms_lob.getlength(rinchi_input) = 0 then
      raise_error('Empty input.');
    end if;
    if an_output_format is null then
      raise_error('Output format undefined - NULL.');
    end if;

    linefeed_pos := instr(rinchi_input, Chr(10));
    if linefeed_pos > 0 then
      the_rinchi_string := substr(rinchi_input, 1, linefeed_pos - 1);
      the_rauxinfo      := substr(rinchi_input, linefeed_pos + 1);
      -- *Could* be a stray linefeed at the end.
      if the_rauxinfo is null then
        the_rauxinfo    := ' ';
      end if;
    else
      the_rinchi_string := rinchi_input;
      -- Avoid passing NULL strings to the shared library.
      the_rauxinfo      := ' ';
    end if;

    check_return(
      rinorca_file_from_rinchi(the_rinchi_string, the_rauxinfo, an_output_format, file_text)
    );
    return file_text;
  end;

  function FILE_FORMAT_RD return varchar2
  is
  begin
    return 'RD';
  end;

  function FILE_FORMAT_RXN return varchar2
  is
  begin
    return 'RXN';
  end;

  /** --------- TEST CODE FOLLOWS -------------- **/
  
  procedure assert_equals(s varchar2, expected varchar2, err_msg varchar2)
  is
  begin
    if nvl(s, '<!blank_s_value!>') <> nvl(expected, '<!blank_expected_value!>') then
      raise_error(err_msg || ': expected "' || expected || '" but got "' || s || '".');
    end if;
  end;
  
  procedure test_chunk_xfer
  is
    total_input_length integer;
    input varchar2(4000);
    remainder pls_largest_varchar2;
  begin
    -- Basic transfer to buffer.
    check_return( rinorca_set_input_chunk(0, 'hello') );
    assert_equals(rinorca_debug_input_buffer, 'hello', 'Xfer 1');
    check_return( rinorca_set_input_chunk(1, ' world') );
    assert_equals(rinorca_debug_input_buffer, 'hello world', 'Xfer 2');

    check_return( rinorca_set_input_chunk(0, 'Reset') );
    assert_equals(rinorca_debug_input_buffer, 'Reset', 'Xfer 3');
  
    -- 1 x MAX_PLSQL_VARCHAR2_LEN + 1 input length.
    input := '123456789';
    total_input_length := length(input);
    remainder := set_gt32k_input(input, total_input_length);
    assert_equals(remainder, '9', '1 x chunk length + 1 :: remainder');
    assert_equals(rinorca_debug_input_buffer, '12345678', '1 x chunk length + 1 :: buffer');

    -- Exactly 2 x MAX_PLSQL_VARCHAR2_LEN input length.
    input := '12345678abcdefgh';
    total_input_length := length(input);
    remainder := set_gt32k_input(input, total_input_length);
    assert_equals(remainder, 'abcdefgh', '2 x chunk length :: remainder');
    assert_equals(rinorca_debug_input_buffer, '12345678', '2 x chunk length :: buffer');

    -- Exactly 3 x MAX_PLSQL_VARCHAR2_LEN input length.
    input := '12345678abcdefghABCDEFGH';
    total_input_length := length(input);
    remainder := set_gt32k_input(input, total_input_length);
    assert_equals(remainder, 'ABCDEFGH', '3 x chunk length :: remainder');
    assert_equals(rinorca_debug_input_buffer, '12345678abcdefgh', '3 x chunk length :: buffer');
    
    -- 1 x MAX_PLSQL_VARCHAR2_LEN + 5 input length.
    input := '12345678abcde';
    total_input_length := length(input);
    remainder := set_gt32k_input(input, total_input_length);
    assert_equals(remainder, 'abcde', '1 x chunk length + 5 :: remainder');
    assert_equals(rinorca_debug_input_buffer, '12345678', '1 x chunk length + 5 :: buffer');

    -- 2 x MAX_PLSQL_VARCHAR2_LEN - 1 input length.
    input := '12345678abcdefg';
    total_input_length := length(input);
    remainder := set_gt32k_input(input, total_input_length);
    assert_equals(remainder, 'abcdefg', '2 x chunk length - 1 :: remainder');
    assert_equals(rinorca_debug_input_buffer, '12345678', '2 x chunk length - 1 :: buffer');

  end;

end rinorca;
/
