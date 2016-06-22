// -------------------------------------------------------------------------
// Unit tests for the ArgParser class.
// -------------------------------------------------------------------------

import static org.junit.Assert.*;
import org.junit.Test;


public class ArgParserTest {


    // ---------------------------------------------------------------------
    // Boolean options.
    // ---------------------------------------------------------------------


    @Test
    public void testBoolOptionEmpty() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool");
        parser.parse(new String[]{});
        assertEquals(false, parser.getFlag("bool"));
    }


    @Test
    public void testBoolOptionMissing() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool");
        parser.parse(new String[]{"foo", "bar"});
        assertEquals(false, parser.getFlag("bool"));
    }


    @Test
    public void testBoolOptionLongform() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool");
        parser.parse(new String[]{"--bool"});
        assertEquals(true, parser.getFlag("bool"));
    }


    @Test
    public void testBoolOptionShortform() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool b");
        parser.parse(new String[]{"-b"});
        assertEquals(true, parser.getFlag("bool"));
    }


    // ---------------------------------------------------------------------
    // Boolean lists.
    // ---------------------------------------------------------------------


    @Test
    public void testBoolListEmpty() {
        ArgParser parser = new ArgParser();
        parser.addFlagList("bool");
        parser.parse(new String[]{});
        assertEquals(0, parser.lenList("bool"));
    }


    @Test
    public void testBoolListLongform() {
        ArgParser parser = new ArgParser();
        parser.addFlagList("bool");
        parser.parse(new String[]{"--bool", "--bool", "--bool"});
        assertEquals(3, parser.lenList("bool"));
    }


    @Test
    public void testBoolListShortform() {
        ArgParser parser = new ArgParser();
        parser.addFlagList("bool b");
        parser.parse(new String[]{"-bb", "-b"});
        assertEquals(3, parser.lenList("bool"));
    }


    // ---------------------------------------------------------------------
    // String options.
    // ---------------------------------------------------------------------


    @Test
    public void testStringOptionEmpty() {
        ArgParser parser = new ArgParser();
        parser.addStr("string", "default");
        parser.parse(new String[]{});
        assertEquals("default", parser.getStr("string"));
    }


    @Test
    public void testStringOptionMissing() {
        ArgParser parser = new ArgParser();
        parser.addStr("string", "default");
        parser.parse(new String[]{"foo", "bar"});
        assertEquals("default", parser.getStr("string"));
    }


    @Test
    public void testStringOptionLongform() {
        ArgParser parser = new ArgParser();
        parser.addStr("string", "default");
        parser.parse(new String[]{"--string", "value"});
        assertEquals("value", parser.getStr("string"));
    }


    @Test
    public void testStringOptionShortform() {
        ArgParser parser = new ArgParser();
        parser.addStr("string s", "default");
        parser.parse(new String[]{"-s", "value"});
        assertEquals("value", parser.getStr("string"));
    }


    // ---------------------------------------------------------------------
    // String lists.
    // ---------------------------------------------------------------------


    @Test
    public void testStringListEmpty() {
        ArgParser parser = new ArgParser();
        parser.addStrList("str");
        parser.parse(new String[]{});
        assertEquals(0, parser.lenList("str"));
    }


    @Test
    public void testStringListLongform() {
        ArgParser parser = new ArgParser();
        parser.addStrList("str");
        parser.parse(new String[]{"--str", "a", "b", "--str", "c"});
        assertEquals(2, parser.lenList("str"));
        assertEquals("a", parser.getStrList("str").get(0));
        assertEquals("c", parser.getStrList("str").get(1));
    }


    @Test
    public void testStringListShortform() {
        ArgParser parser = new ArgParser();
        parser.addStrList("str s");
        parser.parse(new String[]{"-s", "a", "b", "-s", "c"});
        assertEquals(2, parser.lenList("str"));
        assertEquals("a", parser.getStrList("str").get(0));
        assertEquals("c", parser.getStrList("str").get(1));
    }


    @Test
    public void testStringGreedyListLongform() {
        ArgParser parser = new ArgParser();
        parser.addStrList("str", true);
        parser.parse(new String[]{"--str", "a", "b", "--str", "c"});
        assertEquals(3, parser.lenList("str"));
        assertEquals("a", parser.getStrList("str").get(0));
        assertEquals("b", parser.getStrList("str").get(1));
        assertEquals("c", parser.getStrList("str").get(2));
    }


    @Test
    public void testStringGreedyListShortform() {
        ArgParser parser = new ArgParser();
        parser.addStrList("str s", true);
        parser.parse(new String[]{"-s", "a", "b", "-s", "c"});
        assertEquals(3, parser.lenList("str"));
        assertEquals("a", parser.getStrList("str").get(0));
        assertEquals("b", parser.getStrList("str").get(1));
        assertEquals("c", parser.getStrList("str").get(2));
    }


    // ---------------------------------------------------------------------
    // Integer options.
    // ---------------------------------------------------------------------


    @Test
    public void testIntOptionEmpty() {
        ArgParser parser = new ArgParser();
        parser.addInt("int", 101);
        parser.parse(new String[]{});
        assertEquals(101, parser.getInt("int"));
    }


    @Test
    public void testIntOptionMissing() {
        ArgParser parser = new ArgParser();
        parser.addInt("int", 101);
        parser.parse(new String[]{"foo", "bar"});
        assertEquals(101, parser.getInt("int"));
    }


    @Test
    public void testIntOptionLongform() {
        ArgParser parser = new ArgParser();
        parser.addInt("int", 101);
        parser.parse(new String[]{"--int", "202"});
        assertEquals(202, parser.getInt("int"));
    }


    @Test
    public void testIntOptionShortform() {
        ArgParser parser = new ArgParser();
        parser.addInt("int i", 101);
        parser.parse(new String[]{"-i", "202"});
        assertEquals(202, parser.getInt("int"));
    }


    @Test
    public void testIntOptionNegative() {
        ArgParser parser = new ArgParser();
        parser.addInt("int", 101);
        parser.parse(new String[]{"--int", "-202"});
        assertEquals(-202, parser.getInt("int"));
    }


    // ---------------------------------------------------------------------
    // Integer lists.
    // ---------------------------------------------------------------------


    @Test
    public void testIntListEmpty() {
        ArgParser parser = new ArgParser();
        parser.addIntList("int");
        parser.parse(new String[]{});
        assertEquals(0, parser.lenList("int"));
    }


    @Test
    public void testIntListLongform() {
        ArgParser parser = new ArgParser();
        parser.addIntList("int");
        parser.parse(new String[]{"--int", "1", "2", "--int", "3"});
        assertEquals(2, parser.lenList("int"));
        assertEquals(1, (int) parser.getIntList("int").get(0));
        assertEquals(3, (int) parser.getIntList("int").get(1));
    }


    @Test
    public void testIntListShortform() {
        ArgParser parser = new ArgParser();
        parser.addIntList("int i");
        parser.parse(new String[]{"-i", "1", "2", "-i", "3"});
        assertEquals(2, parser.lenList("int"));
        assertEquals(1, (int) parser.getIntList("int").get(0));
        assertEquals(3, (int) parser.getIntList("int").get(1));
    }


    @Test
    public void testIntGreedyListLongform() {
        ArgParser parser = new ArgParser();
        parser.addIntList("int", true);
        parser.parse(new String[]{"--int", "1", "2", "--int", "3"});
        assertEquals(3, parser.lenList("int"));
        assertEquals(1, (int) parser.getIntList("int").get(0));
        assertEquals(2, (int) parser.getIntList("int").get(1));
        assertEquals(3, (int) parser.getIntList("int").get(2));
    }


    @Test
    public void testIntGreedyListShortform() {
        ArgParser parser = new ArgParser();
        parser.addIntList("int i", true);
        parser.parse(new String[]{"-i", "1", "2", "-i", "3"});
        assertEquals(3, parser.lenList("int"));
        assertEquals(1, (int) parser.getIntList("int").get(0));
        assertEquals(2, (int) parser.getIntList("int").get(1));
        assertEquals(3, (int) parser.getIntList("int").get(2));
    }


    // ---------------------------------------------------------------------
    // Float options.
    // ---------------------------------------------------------------------


    @Test
    public void testFloatOptionEmpty() {
        ArgParser parser = new ArgParser();
        parser.addFloat("float", 1.1);
        parser.parse(new String[]{});
        assertEquals(1.1, parser.getFloat("float"), 0.001);
    }


    @Test
    public void testFloatOptionMissing() {
        ArgParser parser = new ArgParser();
        parser.addFloat("float", 1.1);
        parser.parse(new String[]{"foo", "bar"});
        assertEquals(1.1, parser.getFloat("float"), 0.01);
    }


    @Test
    public void testFloatOptionLongform() {
        ArgParser parser = new ArgParser();
        parser.addFloat("float", 1.1);
        parser.parse(new String[]{"--float", "2.2"});
        assertEquals(2.2, parser.getFloat("float"), 0.01);
    }


    @Test
    public void testFloatOptionShortform() {
        ArgParser parser = new ArgParser();
        parser.addFloat("float f", 1.1);
        parser.parse(new String[]{"-f", "2.2"});
        assertEquals(2.2, parser.getFloat("float"), 0.01);
    }


    @Test
    public void testFloatOptionNegative() {
        ArgParser parser = new ArgParser();
        parser.addFloat("float", 1.1);
        parser.parse(new String[]{"--float", "-2.2"});
        assertEquals(-2.2, parser.getFloat("float"), 0.01);
    }


    // ---------------------------------------------------------------------
    // Float lists.
    // ---------------------------------------------------------------------


    @Test
    public void testFloatListEmpty() {
        ArgParser parser = new ArgParser();
        parser.addFloatList("flt");
        parser.parse(new String[]{});
        assertEquals(0, parser.lenList("flt"));
    }


    @Test
    public void testFloatListLongform() {
        ArgParser parser = new ArgParser();
        parser.addFloatList("flt");
        parser.parse(new String[]{"--flt", "1.0", "2.0", "--flt", "3.0"});
        assertEquals(2, parser.lenList("flt"));
        assertEquals(1.0, (double) parser.getFloatList("flt").get(0), 0.01);
        assertEquals(3.0, (double) parser.getFloatList("flt").get(1), 0.01);
    }


    @Test
    public void testFloatListShortform() {
        ArgParser parser = new ArgParser();
        parser.addFloatList("flt f");
        parser.parse(new String[]{"-f", "1.0", "2.0", "-f", "3.0"});
        assertEquals(2, parser.lenList("flt"));
        assertEquals(1.0, (double) parser.getFloatList("flt").get(0), 0.01);
        assertEquals(3.0, (double) parser.getFloatList("flt").get(1), 0.01);
    }


    @Test
    public void testFloatGreedyListLongform() {
        ArgParser parser = new ArgParser();
        parser.addFloatList("flt", true);
        parser.parse(new String[]{"--flt", "1.0", "2.0", "--flt", "3.0"});
        assertEquals(3, parser.lenList("flt"));
        assertEquals(1.0, (double) parser.getFloatList("flt").get(0), 0.01);
        assertEquals(2.0, (double) parser.getFloatList("flt").get(1), 0.01);
        assertEquals(3.0, (double) parser.getFloatList("flt").get(2), 0.01);
    }


    @Test
    public void testFloatGreedyListShortform() {
        ArgParser parser = new ArgParser();
        parser.addFloatList("flt f", true);
        parser.parse(new String[]{"-f", "1.0", "2.0", "-f", "3.0"});
        assertEquals(3, parser.lenList("flt"));
        assertEquals(1.0, (double) parser.getFloatList("flt").get(0), 0.01);
        assertEquals(2.0, (double) parser.getFloatList("flt").get(1), 0.01);
        assertEquals(3.0, (double) parser.getFloatList("flt").get(2), 0.01);
    }


    // ---------------------------------------------------------------------
    // Multiple options.
    // ---------------------------------------------------------------------


    @Test
    public void testMultiOptionsEmpty() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool1");
        parser.addFlag("bool2 b");
        parser.addStr("string1", "default1");
        parser.addStr("string2 s", "default2");
        parser.addInt("int1", 101);
        parser.addInt("int2 i", 202);
        parser.addFloat("float1", 1.1);
        parser.addFloat("float2 f", 2.2);
        parser.parse(new String[]{});
        assertEquals(false, parser.getFlag("bool1"));
        assertEquals(false, parser.getFlag("bool2"));
        assertEquals("default1", parser.getStr("string1"));
        assertEquals("default2", parser.getStr("string2"));
        assertEquals(101, parser.getInt("int1"));
        assertEquals(202, parser.getInt("int2"));
        assertEquals(1.1, parser.getFloat("float1"), 0.01);
        assertEquals(2.2, parser.getFloat("float2"), 0.01);
    }


    @Test
    public void testMultiOptionsLongform() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool1");
        parser.addFlag("bool2 b");
        parser.addStr("string1", "default1");
        parser.addStr("string2 s", "default2");
        parser.addInt("int1", 101);
        parser.addInt("int2 i", 202);
        parser.addFloat("float1", 1.1);
        parser.addFloat("float2 f", 2.2);
        parser.parse(new String[]{
            "--bool1",
            "--bool2",
            "--string1", "value1",
            "--string2", "value2",
            "--int1", "303",
            "--int2", "404",
            "--float1", "3.3",
            "--float2", "4.4",
        });
        assertEquals(true, parser.getFlag("bool1"));
        assertEquals(true, parser.getFlag("bool2"));
        assertEquals("value1", parser.getStr("string1"));
        assertEquals("value2", parser.getStr("string2"));
        assertEquals(303, parser.getInt("int1"));
        assertEquals(404, parser.getInt("int2"));
        assertEquals(3.3, parser.getFloat("float1"), 0.01);
        assertEquals(4.4, parser.getFloat("float2"), 0.01);
    }


    @Test
    public void testMultiOptionsShortform() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool1");
        parser.addFlag("bool2 b");
        parser.addStr("string1", "default1");
        parser.addStr("string2 s", "default2");
        parser.addInt("int1", 101);
        parser.addInt("int2 i", 202);
        parser.addFloat("float1", 1.1);
        parser.addFloat("float2 f", 2.2);
        parser.parse(new String[]{
            "--bool1",
            "-b",
            "--string1", "value1",
            "-s", "value2",
            "--int1", "303",
            "-i", "404",
            "--float1", "3.3",
            "-f", "4.4",
        });
        assertEquals(true, parser.getFlag("bool1"));
        assertEquals(true, parser.getFlag("bool2"));
        assertEquals("value1", parser.getStr("string1"));
        assertEquals("value2", parser.getStr("string2"));
        assertEquals(303, parser.getInt("int1"));
        assertEquals(404, parser.getInt("int2"));
        assertEquals(3.3, parser.getFloat("float1"), 0.01);
        assertEquals(4.4, parser.getFloat("float2"), 0.01);
    }


    // ---------------------------------------------------------------------
    // Condensed short-form options.
    // ---------------------------------------------------------------------


    @Test
    public void testCondensedOptions() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool b");
        parser.addStr("string s", "default");
        parser.addInt("int i", 101);
        parser.addFloat("float f", 1.1);
        parser.parse(new String[]{"-bsif", "value", "202", "2.2"});
        assertEquals(true, parser.getFlag("bool"));
        assertEquals("value", parser.getStr("string"));
        assertEquals(202, parser.getInt("int"));
        assertEquals(2.2, parser.getFloat("float"), 0.01);
    }


    // ---------------------------------------------------------------------
    // Positional arguments.
    // ---------------------------------------------------------------------


    @Test
    public void testPositionalArgsEmpty() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{});
        assertEquals(false, parser.hasArgs());
    }


    @Test
    public void testPositionalArgs() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{"foo", "bar"});
        assertEquals(true, parser.hasArgs());
        assertEquals(2, parser.lenArgs(), 2);
        assertEquals("foo", parser.getArgs().get(0));
        assertEquals("bar", parser.getArgs().get(1));
        assertEquals("foo", parser.getArg(0));
        assertEquals("bar", parser.getArg(1));
    }


    @Test
    public void testPositionalArgsAsInts() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{"1", "11"});
        assertEquals(1, (int) parser.getArgsAsInts().get(0));
        assertEquals(11, (int) parser.getArgsAsInts().get(1));
    }


    @Test
    public void testPositionalArgsAsFloats() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{"1.1", "11.1"});
        assertEquals(1.1, parser.getArgsAsFloats().get(0), 0.01);
        assertEquals(11.1, parser.getArgsAsFloats().get(1), 0.01);
    }


    // ---------------------------------------------------------------------
    // Option-parsing switch.
    // ---------------------------------------------------------------------


    @Test
    public void testOptionParsingSwitch() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{"foo", "--", "--bar", "--baz"});
        assertEquals(3, parser.lenArgs());
    }


    // ---------------------------------------------------------------------
    // Commands.
    // ---------------------------------------------------------------------


    public static void callback(ArgParser parser) {}


    @Test
    public void testCommandAbsent() {
        ArgParser parser = new ArgParser();
        ArgParser cmdParser = parser.addCmd(
            "cmd", "helptext", ArgParserTest::callback
        );
        parser.parse(new String[]{});
        assertEquals(false, parser.hasCmd());
    }


    @Test
    public void testCommandPresent() {
        ArgParser parser = new ArgParser();
        ArgParser cmdParser = parser.addCmd(
            "cmd", "helptext", ArgParserTest::callback
        );
        parser.parse(new String[]{"cmd"});
        assertEquals(true, parser.hasCmd());
        assertEquals("cmd", parser.getCmdName());
        assertEquals(cmdParser, parser.getCmdParser());
    }


    @Test
    public void testCommandWithOptions() {
        ArgParser parser = new ArgParser();
        ArgParser cmdParser = parser.addCmd(
            "cmd", "helptext", ArgParserTest::callback
        );
        cmdParser.addFlag("bool");
        cmdParser.addStr("string", "default");
        cmdParser.addInt("int", 101);
        cmdParser.addFloat("float", 1.1);
        parser.parse(new String[]{
            "cmd",
            "foo", "bar",
            "--string", "value",
            "--int", "202",
            "--float", "2.2",
        });
        assertEquals(true, parser.hasCmd());
        assertEquals("cmd", parser.getCmdName());
        assertEquals(cmdParser, parser.getCmdParser());
        assertEquals(true, cmdParser.hasArgs());
        assertEquals(2, cmdParser.lenArgs());
        assertEquals("value", cmdParser.getStr("string"));
        assertEquals(202, cmdParser.getInt("int"));
        assertEquals(2.2, cmdParser.getFloat("float"), 0.01);
    }
}
