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
    assert parser.get_option("bool") == False


def test_bool_option_missing():
    parser = clio.ArgParser()
    parser.add_flag("bool")
    parser.parse(["foo", "bar"])
    assert parser.get_option("bool") == False


def test_bool_option_longform():
    parser = clio.ArgParser()
    parser.add_flag("bool")
    parser.parse(["--bool"])
    assert parser.get_option("bool") == True


def test_bool_option_shortform():
    parser = clio.ArgParser()
    parser.add_flag("bool", "b")
    parser.parse(["-b"])
    assert parser.get_option("bool") == True


# --------------------------------------------------------------------------
# String options.
# --------------------------------------------------------------------------


def test_string_option_empty():
    parser = clio.ArgParser()
    parser.add_str_option("string", "default")
    parser.parse([])
    assert parser.get_option("string") == "default"


def test_string_option_missing():
    parser = clio.ArgParser()
    parser.add_str_option("string", "default")
    parser.parse(["foo", "bar"])
    assert parser.get_option("string") == "default"


def test_string_option_longform():
    parser = clio.ArgParser()
    parser.add_str_option("string", "default")
    parser.parse(["--string", "value"])
    assert parser.get_option("string") == "value"


def test_string_option_shortform():
    parser = clio.ArgParser()
    parser.add_str_option("string", "default", "s")
    parser.parse(["-s", "value"])
    assert parser.get_option("string") == "value"


def test_string_option_missing_value():
    parser = clio.ArgParser()
    parser.add_str_option("string", "default")
    with pytest.raises(SystemExit):
        parser.parse(["--string"])


# --------------------------------------------------------------------------
# Integer options.
# --------------------------------------------------------------------------


def test_int_option_empty():
    parser = clio.ArgParser()
    parser.add_int_option("int", 101)
    parser.parse([])
    assert parser.get_option("int") == 101


def test_int_option_missing():
    parser = clio.ArgParser()
    parser.add_int_option("int", 101)
    parser.parse(["foo", "bar"])
    assert parser.get_option("int") == 101


def test_int_option_longform():
    parser = clio.ArgParser()
    parser.add_int_option("int", 101)
    parser.parse(["--int", "202"])
    assert parser.get_option("int") == 202


def test_int_option_shortform():
    parser = clio.ArgParser()
    parser.add_int_option("int", 101, "i")
    parser.parse(["-i", "202"])
    assert parser.get_option("int") == 202


def test_int_option_missing_value():
    parser = clio.ArgParser()
    parser.add_str_option("int", 101)
    with pytest.raises(SystemExit):
        parser.parse(["--int"])


def test_int_option_invalid_value():
    parser = clio.ArgParser()
    parser.add_int_option("int", 101)
    with pytest.raises(SystemExit):
        parser.parse(["--int", "foo"])


# --------------------------------------------------------------------------
# Float options.
# --------------------------------------------------------------------------


def test_float_option_empty():
    parser = clio.ArgParser()
    parser.add_float_option("float", 1.1)
    parser.parse([])
    assert parser.get_option("float") == 1.1


def test_float_option_missing():
    parser = clio.ArgParser()
    parser.add_float_option("float", 1.1)
    parser.parse(["foo", "bar"])
    assert parser.get_option("float") == 1.1


def test_float_option_longform():
    parser = clio.ArgParser()
    parser.add_float_option("float", 1.1)
    parser.parse(["--float", "2.2"])
    assert parser.get_option("float") == 2.2


def test_float_option_shortform():
    parser = clio.ArgParser()
    parser.add_float_option("float", 1.1, "f")
    parser.parse(["-f", "2.2"])
    assert parser.get_option("float") == 2.2


def test_float_option_missing_value():
    parser = clio.ArgParser()
    parser.add_str_option("float", 1.1)
    with pytest.raises(SystemExit):
        parser.parse(["--float"])


def test_float_option_invalid_value():
    parser = clio.ArgParser()
    parser.add_int_option("float", 1.1)
    with pytest.raises(SystemExit):
        parser.parse(["--float", "foo"])


# --------------------------------------------------------------------------
# Multiple options.
# --------------------------------------------------------------------------


def test_multi_options_empty():
    parser = clio.ArgParser()
    parser.add_flag("bool1")
    parser.add_flag("bool2", "b")
    parser.add_str_option("string1", "default1")
    parser.add_str_option("string2", "default2", "s")
    parser.add_int_option("int1", 101)
    parser.add_int_option("int2", 202, "i")
    parser.add_float_option("float1", 1.1)
    parser.add_float_option("float2", 2.2, "f")
    parser.parse([])
    assert parser.get_option("bool1") == False
    assert parser.get_option("bool2") == False
    assert parser.get_option("string1") == "default1"
    assert parser.get_option("string2") == "default2"
    assert parser.get_option("int1") == 101
    assert parser.get_option("int2") == 202
    assert parser.get_option("float1") == 1.1
    assert parser.get_option("float2") == 2.2


def test_multi_options_longform():
    parser = clio.ArgParser()
    parser.add_flag("bool1")
    parser.add_flag("bool2", "b")
    parser.add_str_option("string1", "default1")
    parser.add_str_option("string2", "default2", "s")
    parser.add_int_option("int1", 101)
    parser.add_int_option("int2", 202, "i")
    parser.add_float_option("float1", 1.1)
    parser.add_float_option("float2", 2.2, "f")
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
    assert parser.get_option("bool1") == True
    assert parser.get_option("bool2") == True
    assert parser.get_option("string1") == "value1"
    assert parser.get_option("string2") == "value2"
    assert parser.get_option("int1") == 303
    assert parser.get_option("int2") == 404
    assert parser.get_option("float1") == 3.3
    assert parser.get_option("float2") == 4.4


def test_multi_options_shortform():
    parser = clio.ArgParser()
    parser.add_flag("bool1")
    parser.add_flag("bool2", "b")
    parser.add_str_option("string1", "default1")
    parser.add_str_option("string2", "default2", "s")
    parser.add_int_option("int1", 101)
    parser.add_int_option("int2", 202, "i")
    parser.add_float_option("float1", 1.1)
    parser.add_float_option("float2", 2.2, "f")
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
    assert parser.get_option("bool1") == True
    assert parser.get_option("bool2") == True
    assert parser.get_option("string1") == "value1"
    assert parser.get_option("string2") == "value2"
    assert parser.get_option("int1") == 303
    assert parser.get_option("int2") == 404
    assert parser.get_option("float1") == 3.3
    assert parser.get_option("float2") == 4.4


# --------------------------------------------------------------------------
# Condensed short-form options.
# --------------------------------------------------------------------------


def test_condensed_options():
    parser = clio.ArgParser()
    parser.add_flag("bool", "b")
    parser.add_str_option("string", "default", "s")
    parser.add_int_option("int", 101, "i")
    parser.add_float_option("float", 1.1, "f")
    parser.parse(["-bsif", "value", "202", "2.2"])
    assert parser.get_option("bool") == True
    assert parser.get_option("string") == "value"
    assert parser.get_option("int") == 202
    assert parser.get_option("float") == 2.2


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
    assert len(parser.get_args()) == 2
    assert parser.get_args()[0] == "foo"
    assert parser.get_args()[1] == "bar"


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
    assert len(parser.get_args()) == 3


# --------------------------------------------------------------------------
# Commands
# --------------------------------------------------------------------------


def test_command_absent():
    parser = clio.ArgParser()
    cmdparser = parser.add_command("cmd", lambda p: None, "helptext")
    parser.parse([])
    assert parser.has_cmd() == False


def test_command_present():
    parser = clio.ArgParser()
    cmdparser = parser.add_command("cmd", lambda p: None, "helptext")
    parser.parse(["cmd"])
    assert parser.has_cmd() == True
    assert parser.get_cmd() == "cmd"
    assert parser.get_cmd_parser() == cmdparser


def test_command_with_options():
    parser = clio.ArgParser()
    cmdparser = parser.add_command("cmd", lambda p: None, "helptext")
    cmdparser.add_flag("bool")
    cmdparser.add_str_option("string", "default")
    cmdparser.add_int_option("int", 101)
    cmdparser.add_float_option("float", 1.1)
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
    assert len(cmdparser.get_args()) == 2
    assert cmdparser.get_option("string") == "value"
    assert cmdparser.get_option("int") == 202
    assert cmdparser.get_option("float") == 2.2
