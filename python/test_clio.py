# --------------------------------------------------------------------------
# Unit tests for the clio module. Run with pytest.
# --------------------------------------------------------------------------

import clio
import pytest


# --------------------------------------------------------------------------
# Boolean options.
# --------------------------------------------------------------------------


def test_bool_option_empty():
    parser = clio.ArgParser()
    parser.add_flag("bool")
    parser.parse([])
    assert parser.get_flag("bool") == False


def test_bool_option_missing():
    parser = clio.ArgParser()
    parser.add_flag("bool")
    parser.parse(["foo", "bar"])
    assert parser.get_flag("bool") == False


def test_bool_option_longform():
    parser = clio.ArgParser()
    parser.add_flag("bool")
    parser.parse(["--bool"])
    assert parser.get_flag("bool") == True


def test_bool_option_shortform():
    parser = clio.ArgParser()
    parser.add_flag("bool b")
    parser.parse(["-b"])
    assert parser.get_flag("bool") == True


def test_bool_option_dict_syntax():
    parser = clio.ArgParser()
    parser.add_flag("bool")
    parser.parse(["--bool"])
    assert parser["bool"] == True


# --------------------------------------------------------------------------
# Boolean lists.
# --------------------------------------------------------------------------


def test_bool_list_missing():
    parser = clio.ArgParser()
    parser.add_flag_list("bool")
    parser.parse([])
    assert parser.len_list("bool") == 0


def test_bool_list_longform():
    parser = clio.ArgParser()
    parser.add_flag_list("bool")
    parser.parse(["--bool", "--bool", "--bool"])
    assert parser.len_list("bool") == 3


def test_bool_list_shortform():
    parser = clio.ArgParser()
    parser.add_flag_list("bool b")
    parser.parse(["-b", "-bb"])
    assert parser.len_list("bool") == 3


def test_bool_list_mixed():
    parser = clio.ArgParser()
    parser.add_flag_list("bool b")
    parser.parse(["--bool", "-bb"])
    assert parser.len_list("bool") == 3


# --------------------------------------------------------------------------
# String options.
# --------------------------------------------------------------------------


def test_string_option_empty():
    parser = clio.ArgParser()
    parser.add_str("string", "default")
    parser.parse([])
    assert parser.get_str("string") == "default"


def test_string_option_missing():
    parser = clio.ArgParser()
    parser.add_str("string", "default")
    parser.parse(["foo", "bar"])
    assert parser.get_str("string") == "default"


def test_string_option_longform():
    parser = clio.ArgParser()
    parser.add_str("string", "default")
    parser.parse(["--string", "value"])
    assert parser.get_str("string") == "value"


def test_string_option_shortform():
    parser = clio.ArgParser()
    parser.add_str("string s", "default")
    parser.parse(["-s", "value"])
    assert parser.get_str("string") == "value"


def test_string_option_dict_syntax():
    parser = clio.ArgParser()
    parser.add_str("string", "default")
    parser.parse(["--string", "value"])
    assert parser["string"] == "value"


def test_string_option_missing_value():
    parser = clio.ArgParser()
    parser.add_str("string", "default")
    with pytest.raises(SystemExit):
        parser.parse(["--string"])


# --------------------------------------------------------------------------
# String lists.
# --------------------------------------------------------------------------


def test_string_list_missing():
    parser = clio.ArgParser()
    parser.add_str_list("string")
    parser.parse([])
    assert parser.len_list("string") == 0


def test_string_list_longform():
    parser = clio.ArgParser()
    parser.add_str_list("string")
    parser.parse(["--string", "foo", "bar", "--string", "baz"])
    assert parser.len_list("string") == 3
    assert parser.get_str_list("string")[0] == "foo"
    assert parser.get_str_list("string")[1] == "bar"
    assert parser.get_str_list("string")[2] == "baz"


def test_string_list_shortform():
    parser = clio.ArgParser()
    parser.add_str_list("string s")
    parser.parse(["-s", "foo", "bar", "-s", "baz"])
    assert parser.len_list("string") == 3
    assert parser.get_str_list("string")[0] == "foo"
    assert parser.get_str_list("string")[1] == "bar"
    assert parser.get_str_list("string")[2] == "baz"


def test_string_list_mixed():
    parser = clio.ArgParser()
    parser.add_str_list("string s")
    parser.parse(["--string", "foo", "bar", "-s", "baz"])
    assert parser.len_list("string") == 3
    assert parser.get_str_list("string")[0] == "foo"
    assert parser.get_str_list("string")[1] == "bar"
    assert parser.get_str_list("string")[2] == "baz"


# --------------------------------------------------------------------------
# Integer options.
# --------------------------------------------------------------------------


def test_int_option_empty():
    parser = clio.ArgParser()
    parser.add_int("int", 101)
    parser.parse([])
    assert parser.get_int("int") == 101


def test_int_option_missing():
    parser = clio.ArgParser()
    parser.add_int("int", 101)
    parser.parse(["foo", "bar"])
    assert parser.get_int("int") == 101


def test_int_option_longform():
    parser = clio.ArgParser()
    parser.add_int("int", 101)
    parser.parse(["--int", "202"])
    assert parser.get_int("int") == 202


def test_int_option_shortform():
    parser = clio.ArgParser()
    parser.add_int("int i", 101)
    parser.parse(["-i", "202"])
    assert parser.get_int("int") == 202


def test_int_option_dict_syntax():
    parser = clio.ArgParser()
    parser.add_int("int", 101)
    parser.parse(["--int", "202"])
    assert parser["int"] == 202


def test_int_option_missing_value():
    parser = clio.ArgParser()
    parser.add_int("int", 101)
    with pytest.raises(SystemExit):
        parser.parse(["--int"])


def test_int_option_invalid_value():
    parser = clio.ArgParser()
    parser.add_int("int", 101)
    with pytest.raises(SystemExit):
        parser.parse(["--int", "foo"])


# --------------------------------------------------------------------------
# Integer lists.
# --------------------------------------------------------------------------


def test_int_list_missing():
    parser = clio.ArgParser()
    parser.add_int_list("int")
    parser.parse([])
    assert parser.len_list("int") == 0


def test_int_list_longform():
    parser = clio.ArgParser()
    parser.add_int_list("int")
    parser.parse(["--int", "123", "456", "--int", "789"])
    assert parser.len_list("int") == 3
    assert parser.get_str_list("int")[0] == 123
    assert parser.get_str_list("int")[1] == 456
    assert parser.get_str_list("int")[2] == 789


def test_int_list_shortform():
    parser = clio.ArgParser()
    parser.add_int_list("int i")
    parser.parse(["-i", "123", "456", "-i", "789"])
    assert parser.len_list("int") == 3
    assert parser.get_str_list("int")[0] == 123
    assert parser.get_str_list("int")[1] == 456
    assert parser.get_str_list("int")[2] == 789


def test_int_list_mixed():
    parser = clio.ArgParser()
    parser.add_int_list("int i")
    parser.parse(["--int", "123", "456", "-i", "789"])
    assert parser.len_list("int") == 3
    assert parser.get_str_list("int")[0] == 123
    assert parser.get_str_list("int")[1] == 456
    assert parser.get_str_list("int")[2] == 789


# --------------------------------------------------------------------------
# Float options.
# --------------------------------------------------------------------------


def test_float_option_empty():
    parser = clio.ArgParser()
    parser.add_float("float", 1.1)
    parser.parse([])
    assert parser.get_float("float") == 1.1


def test_float_option_missing():
    parser = clio.ArgParser()
    parser.add_float("float", 1.1)
    parser.parse(["foo", "bar"])
    assert parser.get_float("float") == 1.1


def test_float_option_longform():
    parser = clio.ArgParser()
    parser.add_float("float", 1.1)
    parser.parse(["--float", "2.2"])
    assert parser.get_float("float") == 2.2


def test_float_option_shortform():
    parser = clio.ArgParser()
    parser.add_float("float f", 1.1)
    parser.parse(["-f", "2.2"])
    assert parser.get_float("float") == 2.2


def test_float_option_dict_syntax():
    parser = clio.ArgParser()
    parser.add_float("float", 1.1)
    parser.parse(["--float", "2.2"])
    assert parser["float"] == 2.2


def test_float_option_missing_value():
    parser = clio.ArgParser()
    parser.add_float("float", 1.1)
    with pytest.raises(SystemExit):
        parser.parse(["--float"])


def test_float_option_invalid_value():
    parser = clio.ArgParser()
    parser.add_float("float", 1.1)
    with pytest.raises(SystemExit):
        parser.parse(["--float", "foo"])


# --------------------------------------------------------------------------
# Multiple options.
# --------------------------------------------------------------------------


def test_multi_options_empty():
    parser = clio.ArgParser()
    parser.add_flag("bool1")
    parser.add_flag("bool2 b")
    parser.add_str("string1", "default1")
    parser.add_str("string2 s", "default2")
    parser.add_int("int1", 101)
    parser.add_int("int2 i", 202,)
    parser.add_float("float1", 1.1)
    parser.add_float("float2 f", 2.2)
    parser.parse([])
    assert parser.get_flag("bool1") == False
    assert parser.get_flag("bool2") == False
    assert parser.get_str("string1") == "default1"
    assert parser.get_str("string2") == "default2"
    assert parser.get_int("int1") == 101
    assert parser.get_int("int2") == 202
    assert parser.get_float("float1") == 1.1
    assert parser.get_float("float2") == 2.2


def test_multi_options_longform():
    parser = clio.ArgParser()
    parser.add_flag("bool1")
    parser.add_flag("bool2 b")
    parser.add_str("string1", "default1")
    parser.add_str("string2 s", "default2")
    parser.add_int("int1", 101)
    parser.add_int("int2 i", 202,)
    parser.add_float("float1", 1.1)
    parser.add_float("float2 f", 2.2)
    parser.parse([
        "--bool1",
        "--bool2",
        "--string1", "value1",
        "--string2", "value2",
        "--int1", "303",
        "--int2", "404",
        "--float1", "3.3",
        "--float2", "4.4",
    ])
    assert parser.get_flag("bool1") == True
    assert parser.get_flag("bool2") == True
    assert parser.get_str("string1") == "value1"
    assert parser.get_str("string2") == "value2"
    assert parser.get_int("int1") == 303
    assert parser.get_int("int2") == 404
    assert parser.get_float("float1") == 3.3
    assert parser.get_float("float2") == 4.4


def test_multi_options_shortform():
    parser = clio.ArgParser()
    parser.add_flag("bool1")
    parser.add_flag("bool2 b")
    parser.add_str("string1", "default1")
    parser.add_str("string2 s", "default2")
    parser.add_int("int1", 101)
    parser.add_int("int2 i", 202,)
    parser.add_float("float1", 1.1)
    parser.add_float("float2 f", 2.2)
    parser.parse([
        "--bool1",
        "-b",
        "--string1", "value1",
        "-s", "value2",
        "--int1", "303",
        "-i", "404",
        "--float1", "3.3",
        "-f", "4.4",
    ])
    assert parser.get_flag("bool1") == True
    assert parser.get_flag("bool2") == True
    assert parser.get_str("string1") == "value1"
    assert parser.get_str("string2") == "value2"
    assert parser.get_int("int1") == 303
    assert parser.get_int("int2") == 404
    assert parser.get_float("float1") == 3.3
    assert parser.get_float("float2") == 4.4


# --------------------------------------------------------------------------
# Condensed short-form options.
# --------------------------------------------------------------------------


def test_condensed_options():
    parser = clio.ArgParser()
    parser.add_flag("bool b")
    parser.add_str("string s", "default")
    parser.add_int("int i", 101)
    parser.add_float("float f", 1.1)
    parser.parse(["-bsif", "value", "202", "2.2"])
    assert parser["bool"] == True
    assert parser["string"] == "value"
    assert parser["int"] == 202
    assert parser["float"] == 2.2


# --------------------------------------------------------------------------
# Unrecognised options.
# --------------------------------------------------------------------------


def test_unrecognised_longform_option():
    parser = clio.ArgParser()
    with pytest.raises(SystemExit):
        parser.parse(["--foo"])


def test_unrecognised_shortform_option():
    parser = clio.ArgParser()
    with pytest.raises(SystemExit):
        parser.parse(["-f"])


# --------------------------------------------------------------------------
# Positional arguments.
# --------------------------------------------------------------------------


def test_positional_args_empty():
    parser = clio.ArgParser()
    parser.parse([])
    assert parser.has_args() == False


def test_positional_args():
    parser = clio.ArgParser()
    parser.parse(["foo", "bar"])
    assert parser.has_args() == True
    assert parser.len_args() == 2
    assert parser.get_arg(0) == "foo"
    assert parser.get_arg(1) == "bar"


def test_positional_args_list_syntax():
    parser = clio.ArgParser()
    parser.parse(["foo", "bar"])
    assert parser[0] == "foo"
    assert parser[1] == "bar"


def test_positional_args_as_ints():
    parser = clio.ArgParser()
    parser.parse(["1", "11"])
    assert parser.get_args_as_ints()[0] == 1
    assert parser.get_args_as_ints()[1] == 11


def test_positional_args_as_floats():
    parser = clio.ArgParser()
    parser.parse(["1.1", "11.1"])
    assert parser.get_args_as_floats()[0] == 1.1
    assert parser.get_args_as_floats()[1] == 11.1


# --------------------------------------------------------------------------
# Option parsing switch.
# --------------------------------------------------------------------------


def test_option_parsing_switch():
    parser = clio.ArgParser()
    parser.parse(["foo", "--", "--bar", "--baz"])
    assert parser.len_args() == 3


# --------------------------------------------------------------------------
# Commands.
# --------------------------------------------------------------------------


def test_command_absent():
    parser = clio.ArgParser()
    cmdparser = parser.add_cmd("cmd", lambda p: None, "helptext")
    parser.parse([])
    assert parser.has_cmd() == False


def test_command_present():
    parser = clio.ArgParser()
    cmdparser = parser.add_cmd("cmd", lambda p: None, "helptext")
    parser.parse(["cmd"])
    assert parser.has_cmd() == True
    assert parser.get_cmd() == "cmd"
    assert parser.get_cmd_parser() == cmdparser


def test_command_with_options():
    parser = clio.ArgParser()
    cmdparser = parser.add_cmd("cmd", lambda p: None, "helptext")
    cmdparser.add_flag("bool")
    cmdparser.add_str("string", "default")
    cmdparser.add_int("int", 101)
    cmdparser.add_float("float", 1.1)
    parser.parse([
        "cmd",
        "foo", "bar",
        "--string", "value",
        "--int", "202",
        "--float", "2.2",
    ])
    assert parser.has_cmd() == True
    assert parser.get_cmd() == "cmd"
    assert parser.get_cmd_parser() == cmdparser
    assert cmdparser.has_args() == True
    assert cmdparser.len_args() == 2
    assert cmdparser["string"] == "value"
    assert cmdparser["int"] == 202
    assert cmdparser["float"] == 2.2
