package clio


import (
    "testing"
)


// -------------------------------------------------------------------------
// Boolean options.
// -------------------------------------------------------------------------


func TestBoolOptionEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool")
    parser.ParseArgs([]string{})
    if parser.GetFlag("bool") != false {
        t.Fail()
    }
}


func TestBoolOptionMissing(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool")
    parser.ParseArgs([]string{"foo", "bar"})
    if parser.GetFlag("bool") != false {
        t.Fail()
    }
}


func TestBoolOptionLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool")
    parser.ParseArgs([]string{"--bool"})
    if parser.GetFlag("bool") != true {
        t.Fail()
    }
}


func TestBoolOptionShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool b")
    parser.ParseArgs([]string{"-b"})
    if parser.GetFlag("bool") != true {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Boolean lists.
// -------------------------------------------------------------------------


func TestBoolListEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlagList("bool")
    parser.ParseArgs([]string{})
    if parser.LenList("bool") != 0 {
        t.Fail()
    }
}


func TestBoolListLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlagList("bool")
    parser.ParseArgs([]string{"--bool", "--bool", "--bool"})
    if parser.LenList("bool") != 3 {
        t.Fail()
    }
}


func TestBoolListShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlagList("bool b")
    parser.ParseArgs([]string{"-b", "-b", "-b"})
    if parser.LenList("bool") != 3 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// String options.
// -------------------------------------------------------------------------


func TestStringOptionEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStr("string", "default")
    parser.ParseArgs([]string{})
    if parser.GetStr("string") != "default" {
        t.Fail()
    }
}


func TestStringOptionMissing(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStr("string", "default")
    parser.ParseArgs([]string{"foo", "bar"})
    if parser.GetStr("string") != "default" {
        t.Fail()
    }
}


func TestStringOptionLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStr("string", "default")
    parser.ParseArgs([]string{"--string", "value"})
    if parser.GetStr("string") != "value" {
        t.Fail()
    }
}


func TestStringOptionShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStr("string s", "default")
    parser.ParseArgs([]string{"-s", "value"})
    if parser.GetStr("string") != "value" {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// String lists.
// -------------------------------------------------------------------------


func TestStringListEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStrList("str", false)
    parser.ParseArgs([]string{})
    if parser.LenList("str") != 0 {
        t.Fail()
    }
}


func TestStringListLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStrList("str", false)
    parser.ParseArgs([]string{"--str", "a", "b", "--str", "c"})
    if parser.LenList("str") != 2 {
        t.Fail()
    }
    if parser.GetStrList("str")[0] != "a" {
        t.Fail()
    }
    if parser.GetStrList("str")[1] != "c" {
        t.Fail()
    }
}


func TestStringListShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStrList("str s", false)
    parser.ParseArgs([]string{"-s", "a", "b", "-s", "c"})
    if parser.LenList("str") != 2 {
        t.Fail()
    }
    if parser.GetStrList("str")[0] != "a" {
        t.Fail()
    }
    if parser.GetStrList("str")[1] != "c" {
        t.Fail()
    }
}


func TestStringGreedyListLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStrList("str", true)
    parser.ParseArgs([]string{"--str", "a", "b", "--str", "c"})
    if parser.LenList("str") != 3 {
        t.Fail()
    }
    if parser.GetStrList("str")[0] != "a" {
        t.Fail()
    }
    if parser.GetStrList("str")[1] != "b" {
        t.Fail()
    }
    if parser.GetStrList("str")[2] != "c" {
        t.Fail()
    }
}


func TestStringGreedyListShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddStrList("str s", true)
    parser.ParseArgs([]string{"-s", "a", "b", "-s", "c"})
    if parser.LenList("str") != 3 {
        t.Fail()
    }
    if parser.GetStrList("str")[0] != "a" {
        t.Fail()
    }
    if parser.GetStrList("str")[1] != "b" {
        t.Fail()
    }
    if parser.GetStrList("str")[2] != "c" {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Integer options.
// -------------------------------------------------------------------------


func TestIntOptionEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddInt("int", 101)
    parser.ParseArgs([]string{})
    if parser.GetInt("int") != 101 {
        t.Fail()
    }
}


func TestIntOptionMissing(t *testing.T) {
    parser := NewParser("", "")
    parser.AddInt("int", 101)
    parser.ParseArgs([]string{"foo", "bar"})
    if parser.GetInt("int") != 101 {
        t.Fail()
    }
}


func TestIntOptionLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddInt("int", 101)
    parser.ParseArgs([]string{"--int", "202"})
    if parser.GetInt("int") != 202 {
        t.Fail()
    }
}


func TestIntOptionShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddInt("int i", 101)
    parser.ParseArgs([]string{"-i", "202"})
    if parser.GetInt("int") != 202 {
        t.Fail()
    }
}


func TestIntOptionNegative(t *testing.T) {
    parser := NewParser("", "")
    parser.AddInt("int", 101)
    parser.ParseArgs([]string{"--int", "-202"})
    if parser.GetInt("int") != -202 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Integer lists.
// -------------------------------------------------------------------------


func TestIntListEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddIntList("int", false)
    parser.ParseArgs([]string{})
    if parser.LenList("int") != 0 {
        t.Fail()
    }
}


func TestIntListLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddIntList("int", false)
    parser.ParseArgs([]string{"--int", "1", "2", "--int", "3"})
    if parser.LenList("int") != 2 {
        t.Fail()
    }
    if parser.GetIntList("int")[0] != 1 {
        t.Fail()
    }
    if parser.GetIntList("int")[1] != 3 {
        t.Fail()
    }
}


func TestIntListShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddIntList("int i", false)
    parser.ParseArgs([]string{"-i", "1", "2", "-i", "3"})
    if parser.LenList("int") != 2 {
        t.Fail()
    }
    if parser.GetIntList("int")[0] != 1 {
        t.Fail()
    }
    if parser.GetIntList("int")[1] != 3 {
        t.Fail()
    }
}


func TestIntGreedyListLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddIntList("int", true)
    parser.ParseArgs([]string{"--int", "1", "2", "--int", "3"})
    if parser.LenList("int") != 3 {
        t.Fail()
    }
    if parser.GetIntList("int")[0] != 1 {
        t.Fail()
    }
    if parser.GetIntList("int")[1] != 2 {
        t.Fail()
    }
    if parser.GetIntList("int")[2] != 3 {
        t.Fail()
    }
}


func TestIntGreedyListShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddIntList("int i", true)
    parser.ParseArgs([]string{"-i", "1", "2", "-i", "3"})
    if parser.LenList("int") != 3 {
        t.Fail()
    }
    if parser.GetIntList("int")[0] != 1 {
        t.Fail()
    }
    if parser.GetIntList("int")[1] != 2 {
        t.Fail()
    }
    if parser.GetIntList("int")[2] != 3 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Float options.
// -------------------------------------------------------------------------


func TestFloatOptionEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloat("float", 1.1)
    parser.ParseArgs([]string{})
    if parser.GetFloat("float") != 1.1 {
        t.Fail()
    }
}


func TestFloatOptionMissing(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloat("float", 1.1)
    parser.ParseArgs([]string{"foo", "bar"})
    if parser.GetFloat("float") != 1.1 {
        t.Fail()
    }
}


func TestFloatOptionLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloat("float", 1.1)
    parser.ParseArgs([]string{"--float", "2.2"})
    if parser.GetFloat("float") != 2.2 {
        t.Fail()
    }
}


func TestFloatOptionShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloat("float f", 1.1)
    parser.ParseArgs([]string{"-f", "2.2"})
    if parser.GetFloat("float") != 2.2 {
        t.Fail()
    }
}


func TestFloatOptionNegative(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloat("float", 1.1)
    parser.ParseArgs([]string{"--float", "-2.2"})
    if parser.GetFloat("float") != -2.2 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Float lists.
// -------------------------------------------------------------------------


func TestFloatListEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloatList("flt", false)
    parser.ParseArgs([]string{})
    if parser.LenList("flt") != 0 {
        t.Fail()
    }
}


func TestFloatListLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloatList("flt", false)
    parser.ParseArgs([]string{"--flt", "1", "2", "--flt", "3"})
    if parser.LenList("flt") != 2 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[0] != 1 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[1] != 3 {
        t.Fail()
    }
}


func TestFloatListShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloatList("flt f", false)
    parser.ParseArgs([]string{"-f", "1", "2", "-f", "3"})
    if parser.LenList("flt") != 2 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[0] != 1 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[1] != 3 {
        t.Fail()
    }
}


func TestFloatGreedyListLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloatList("flt", true)
    parser.ParseArgs([]string{"--flt", "1", "2", "--flt", "3"})
    if parser.LenList("flt") != 3 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[0] != 1 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[1] != 2 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[2] != 3 {
        t.Fail()
    }
}


func TestFloatGreedyListShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFloatList("flt f", true)
    parser.ParseArgs([]string{"-f", "1", "2", "-f", "3"})
    if parser.LenList("flt") != 3 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[0] != 1 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[1] != 2 {
        t.Fail()
    }
    if parser.GetFloatList("flt")[2] != 3 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Multiple options.
// -------------------------------------------------------------------------


func TestMultiOptionsEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool1")
    parser.AddFlag("bool2 b")
    parser.AddStr("string1", "default1")
    parser.AddStr("string2 s", "default2")
    parser.AddInt("int1", 101)
    parser.AddInt("int2 i", 202)
    parser.AddFloat("float1", 1.1)
    parser.AddFloat("float2 f", 2.2)
    parser.ParseArgs([]string{})
    if parser.GetFlag("bool1") != false {
        t.Fail()
    }
    if parser.GetFlag("bool2") != false {
        t.Fail()
    }
    if parser.GetStr("string1") != "default1" {
        t.Fail()
    }
    if parser.GetStr("string2") != "default2" {
        t.Fail()
    }
    if parser.GetInt("int1") != 101 {
        t.Fail()
    }
    if parser.GetInt("int2") != 202 {
        t.Fail()
    }
    if parser.GetFloat("float1") != 1.1 {
        t.Fail()
    }
    if parser.GetFloat("float2") != 2.2 {
        t.Fail()
    }
}


func TestMultiOptionsLongform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool1")
    parser.AddFlag("bool2 b")
    parser.AddStr("string1", "default1")
    parser.AddStr("string2 s", "default2")
    parser.AddInt("int1", 101)
    parser.AddInt("int2 i", 202)
    parser.AddFloat("float1", 1.1)
    parser.AddFloat("float2 f", 2.2)
    parser.ParseArgs([]string{
        "--bool1",
        "--bool2",
        "--string1", "value1",
        "--string2", "value2",
        "--int1", "303",
        "--int2", "404",
        "--float1", "3.3",
        "--float2", "4.4",
    })
    if parser.GetFlag("bool1") != true {
        t.Fail()
    }
    if parser.GetFlag("bool2") != true {
        t.Fail()
    }
    if parser.GetStr("string1") != "value1" {
        t.Fail()
    }
    if parser.GetStr("string2") != "value2" {
        t.Fail()
    }
    if parser.GetInt("int1") != 303 {
        t.Fail()
    }
    if parser.GetInt("int2") != 404 {
        t.Fail()
    }
    if parser.GetFloat("float1") != 3.3 {
        t.Fail()
    }
    if parser.GetFloat("float2") != 4.4 {
        t.Fail()
    }
}


func TestMultiOptionsShortform(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool1")
    parser.AddFlag("bool2 b")
    parser.AddStr("string1", "default1")
    parser.AddStr("string2 s", "default2")
    parser.AddInt("int1", 101)
    parser.AddInt("int2 i", 202)
    parser.AddFloat("float1", 1.1)
    parser.AddFloat("float2 f", 2.2)
    parser.ParseArgs([]string{
        "--bool1",
        "-b",
        "--string1", "value1",
        "-s", "value2",
        "--int1", "303",
        "-i", "404",
        "--float1", "3.3",
        "-f", "4.4",
    })
    if parser.GetFlag("bool1") != true {
        t.Fail()
    }
    if parser.GetFlag("bool2") != true {
        t.Fail()
    }
    if parser.GetStr("string1") != "value1" {
        t.Fail()
    }
    if parser.GetStr("string2") != "value2" {
        t.Fail()
    }
    if parser.GetInt("int1") != 303 {
        t.Fail()
    }
    if parser.GetInt("int2") != 404 {
        t.Fail()
    }
    if parser.GetFloat("float1") != 3.3 {
        t.Fail()
    }
    if parser.GetFloat("float2") != 4.4 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Condensed short-form options.
// -------------------------------------------------------------------------


func TestCondensedOptions(t *testing.T) {
    parser := NewParser("", "")
    parser.AddFlag("bool b")
    parser.AddStr("string s", "default")
    parser.AddInt("int i", 101)
    parser.AddFloat("float f", 1.1)
    parser.ParseArgs([]string{"-bsif", "value", "202", "2.2"})
    if parser.GetFlag("bool") != true {
        t.Fail()
    }
    if parser.GetStr("string") != "value" {
        t.Fail()
    }
    if parser.GetInt("int") != 202 {
        t.Fail()
    }
    if parser.GetFloat("float") != 2.2 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Positional arguments.
// -------------------------------------------------------------------------


func TestPositionalArgsEmpty(t *testing.T) {
    parser := NewParser("", "")
    parser.ParseArgs([]string{})
    if parser.HasArgs() != false {
        t.Fail()
    }
}


func TestPositionalArgs(t *testing.T) {
    parser := NewParser("", "")
    parser.ParseArgs([]string{"foo", "bar"})
    if parser.HasArgs() != true {
        t.Fail()
    }
    if parser.LenArgs() != 2 {
        t.Fail()
    }
    if parser.GetArg(0) != "foo" {
        t.Fail()
    }
    if parser.GetArg(1) != "bar" {
        t.Fail()
    }
    if parser.GetArgs()[0] != "foo" {
        t.Fail()
    }
    if parser.GetArgs()[1] != "bar" {
        t.Fail()
    }
}


func TestPositionalArgsAsInts(t *testing.T) {
    parser := NewParser("", "")
    parser.ParseArgs([]string{"1", "11"})
    if parser.GetArgsAsInts()[0] != 1 {
        t.Fail()
    }
    if parser.GetArgsAsInts()[1] != 11 {
        t.Fail()
    }
}


func TestPositionalArgsAsFloats(t *testing.T) {
    parser := NewParser("", "")
    parser.ParseArgs([]string{"1.1", "11.1"})
    if parser.GetArgsAsFloats()[0] != 1.1 {
        t.Fail()
    }
    if parser.GetArgsAsFloats()[1] != 11.1 {
        t.Fail()
    }
}


// -------------------------------------------------------------------------
// Commands
// -------------------------------------------------------------------------


func callback(parser *ArgParser) {}


func TestCommandAbsent(t *testing.T) {
    parser := NewParser("", "")
    parser.AddCmd("cmd", "helptext", callback)
    parser.ParseArgs([]string{})
    if parser.HasCmd() != false {
        t.Fail()
    }
}


func TestCommandPresent(t *testing.T) {
    parser := NewParser("", "")
    cmdParser := parser.AddCmd("cmd", "helptext", callback)
    parser.ParseArgs([]string{"cmd"})
    if parser.HasCmd() != true {
        t.Fail()
    }
    if parser.GetCmdName() != "cmd" {
        t.Fail()
    }
    if parser.GetCmdParser() != cmdParser {
        t.Fail()
    }
}


func TestCommandWithOptions(t *testing.T) {
    parser := NewParser("", "")
    cmdParser := parser.AddCmd("cmd", "helptext", callback)
    cmdParser.AddFlag("bool")
    cmdParser.AddStr("string", "default")
    cmdParser.AddInt("int", 101)
    cmdParser.AddFloat("float", 1.1)
    parser.ParseArgs([]string{
        "cmd",
        "foo", "bar",
        "--string", "value",
        "--int", "202",
        "--float", "2.2",
    })
    if parser.HasCmd() != true {
        t.Fail()
    }
    if parser.GetCmdName() != "cmd" {
        t.Fail()
    }
    if parser.GetCmdParser() != cmdParser {
        t.Fail()
    }
    if cmdParser.HasArgs() != true {
        t.Fail()
    }
    if cmdParser.LenArgs() != 2 {
        t.Fail()
    }
    if cmdParser.GetStr("string") != "value" {
        t.Fail()
    }
    if cmdParser.GetInt("int") != 202 {
        t.Fail()
    }
    if cmdParser.GetFloat("float") != 2.2 {
        t.Fail()
    }
}
