/*
    Unit tests for the ArgParser class.
*/

import static org.junit.Assert.*;
import org.junit.Test;


public class ArgParserTest {


    // --------------------------------------------------------
    // Boolean options.
    // --------------------------------------------------------


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
        parser.addFlag("bool", 'b');
        parser.parse(new String[]{"-b"});
        assertEquals(true, parser.getFlag("bool"));
    }


    // --------------------------------------------------------
    // String options.
    // --------------------------------------------------------


    @Test
    public void testStringOptionEmpty() {
        ArgParser parser = new ArgParser();
        parser.addStrOpt("string", "default");
        parser.parse(new String[]{});
        assertEquals("default", parser.getStrOpt("string"));
    }


    @Test
    public void testStringOptionMissing() {
        ArgParser parser = new ArgParser();
        parser.addStrOpt("string", "default");
        parser.parse(new String[]{"foo", "bar"});
        assertEquals("default", parser.getStrOpt("string"));
    }


    @Test
    public void testStringOptionLongform() {
        ArgParser parser = new ArgParser();
        parser.addStrOpt("string", "default");
        parser.parse(new String[]{"--string", "value"});
        assertEquals("value", parser.getStrOpt("string"));
    }


    @Test
    public void testStringOptionShortform() {
        ArgParser parser = new ArgParser();
        parser.addStrOpt("string", "default", 's');
        parser.parse(new String[]{"-s", "value"});
        assertEquals("value", parser.getStrOpt("string"));
    }


    // --------------------------------------------------------
    // Integer options.
    // --------------------------------------------------------


    @Test
    public void testIntOptionEmpty() {
        ArgParser parser = new ArgParser();
        parser.addIntOpt("int", 101);
        parser.parse(new String[]{});
        assertEquals(101, parser.getIntOpt("int"));
    }


    @Test
    public void testIntOptionMissing() {
        ArgParser parser = new ArgParser();
        parser.addIntOpt("int", 101);
        parser.parse(new String[]{"foo", "bar"});
        assertEquals(101, parser.getIntOpt("int"));
    }


    @Test
    public void testIntOptionLongform() {
        ArgParser parser = new ArgParser();
        parser.addIntOpt("int", 101);
        parser.parse(new String[]{"--int", "202"});
        assertEquals(202, parser.getIntOpt("int"));
    }


    @Test
    public void testIntOptionShortform() {
        ArgParser parser = new ArgParser();
        parser.addIntOpt("int", 101, 'i');
        parser.parse(new String[]{"-i", "202"});
        assertEquals(202, parser.getIntOpt("int"));
    }


    @Test
    public void testIntOptionNegative() {
        ArgParser parser = new ArgParser();
        parser.addIntOpt("int", 101);
        parser.parse(new String[]{"--int", "-202"});
        assertEquals(-202, parser.getIntOpt("int"));
    }


    // --------------------------------------------------------
    // Float options.
    // --------------------------------------------------------


    @Test
    public void testFloatOptionEmpty() {
        ArgParser parser = new ArgParser();
        parser.addFloatOpt("float", 1.1);
        parser.parse(new String[]{});
        assertEquals(1.1, (double)parser.getFloatOpt("float"), 0.001);
    }


    @Test
    public void testFloatOptionMissing() {
        ArgParser parser = new ArgParser();
        parser.addFloatOpt("float", 1.1);
        parser.parse(new String[]{"foo", "bar"});
        assertEquals(1.1, (double)parser.getFloatOpt("float"), 0.01);
    }


    @Test
    public void testFloatOptionLongform() {
        ArgParser parser = new ArgParser();
        parser.addFloatOpt("float", 1.1);
        parser.parse(new String[]{"--float", "2.2"});
        assertEquals(2.2, (double)parser.getFloatOpt("float"), 0.01);
    }


    @Test
    public void testFloatOptionShortform() {
        ArgParser parser = new ArgParser();
        parser.addFloatOpt("float", 1.1, 'f');
        parser.parse(new String[]{"-f", "2.2"});
        assertEquals(2.2, (double)parser.getFloatOpt("float"), 0.01);
    }


    @Test
    public void testFloatOptionNegative() {
        ArgParser parser = new ArgParser();
        parser.addFloatOpt("float", 1.1);
        parser.parse(new String[]{"--float", "-2.2"});
        assertEquals(-2.2, (double)parser.getFloatOpt("float"), 0.01);
    }


    // --------------------------------------------------------
    // Multiple options.
    // --------------------------------------------------------


    @Test
    public void testMultiOptionsEmpty() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool1");
        parser.addFlag("bool2", 'b');
        parser.addStrOpt("string1", "default1");
        parser.addStrOpt("string2", "default2", 's');
        parser.addIntOpt("int1", 101);
        parser.addIntOpt("int2", 202, 'i');
        parser.addFloatOpt("float1", 1.1);
        parser.addFloatOpt("float2", 2.2, 'f');
        parser.parse(new String[]{});
        assertEquals(false, parser.getFlag("bool1"));
        assertEquals(false, parser.getFlag("bool2"));
        assertEquals("default1", parser.getStrOpt("string1"));
        assertEquals("default2", parser.getStrOpt("string2"));
        assertEquals(101, parser.getIntOpt("int1"));
        assertEquals(202, parser.getIntOpt("int2"));
        assertEquals(1.1, (double)parser.getFloatOpt("float1"), 0.01);
        assertEquals(2.2, (double)parser.getFloatOpt("float2"), 0.01);
    }


    @Test
    public void testMultiOptionsLongform() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool1");
        parser.addFlag("bool2", 'b');
        parser.addStrOpt("string1", "default1");
        parser.addStrOpt("string2", "default2", 's');
        parser.addIntOpt("int1", 101);
        parser.addIntOpt("int2", 202, 'i');
        parser.addFloatOpt("float1", 1.1);
        parser.addFloatOpt("float2", 2.2, 'f');
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
        assertEquals("value1", parser.getStrOpt("string1"));
        assertEquals("value2", parser.getStrOpt("string2"));
        assertEquals(303, parser.getIntOpt("int1"));
        assertEquals(404, parser.getIntOpt("int2"));
        assertEquals(3.3, (double)parser.getFloatOpt("float1"), 0.01);
        assertEquals(4.4, (double)parser.getFloatOpt("float2"), 0.01);
    }


    @Test
    public void testMultiOptionsShortform() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool1");
        parser.addFlag("bool2", 'b');
        parser.addStrOpt("string1", "default1");
        parser.addStrOpt("string2", "default2", 's');
        parser.addIntOpt("int1", 101);
        parser.addIntOpt("int2", 202, 'i');
        parser.addFloatOpt("float1", 1.1);
        parser.addFloatOpt("float2", 2.2, 'f');
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
        assertEquals("value1", parser.getStrOpt("string1"));
        assertEquals("value2", parser.getStrOpt("string2"));
        assertEquals(303, parser.getIntOpt("int1"));
        assertEquals(404, parser.getIntOpt("int2"));
        assertEquals(3.3, (double)parser.getFloatOpt("float1"), 0.01);
        assertEquals(4.4, (double)parser.getFloatOpt("float2"), 0.01);
    }


    // --------------------------------------------------------
    // Condensed short-form options.
    // --------------------------------------------------------


    @Test
    public void testCondensedOptions() {
        ArgParser parser = new ArgParser();
        parser.addFlag("bool", 'b');
        parser.addStrOpt("string", "default", 's');
        parser.addIntOpt("int", 101, 'i');
        parser.addFloatOpt("float", 1.1, 'f');
        parser.parse(new String[]{"-bsif", "value", "202", "2.2"});
        assertEquals(true, parser.getFlag("bool"));
        assertEquals("value", parser.getStrOpt("string"));
        assertEquals(202, parser.getIntOpt("int"));
        assertEquals(2.2, (double)parser.getFloatOpt("float"), 0.01);
    }


    // --------------------------------------------------------
    // Positional arguments.
    // --------------------------------------------------------


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
        assertEquals(2, parser.numArgs(), 2);
        assertEquals("foo", parser.getArgs().get(0));
        assertEquals("bar", parser.getArgs().get(1));
        assertEquals("foo", parser.getArg(0));
        assertEquals("bar", parser.getArg(1));
    }


    @Test
    public void testPositionalArgsAsInts() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{"1", "11"});
        assertEquals(1, (int)parser.getArgsAsInts().get(0));
        assertEquals(11, (int)parser.getArgsAsInts().get(1));
    }


    @Test
    public void testPositionalArgsAsFloats() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{"1.1", "11.1"});
        assertEquals(1.1, (double)parser.getArgsAsFloats().get(0), 0.01);
        assertEquals(11.1, (double)parser.getArgsAsFloats().get(1), 0.01);
    }


    // --------------------------------------------------------
    // Option-parsing switch.
    // --------------------------------------------------------


    @Test
    public void testOptionParsingSwitch() {
        ArgParser parser = new ArgParser();
        parser.parse(new String[]{"foo", "--", "--bar", "--baz"});
        assertEquals(3, parser.numArgs());
    }


    // --------------------------------------------------------
    // Commands.
    // --------------------------------------------------------


    public static void callback(ArgParser parser) {}


    @Test
    public void testCommandAbsent() {
        ArgParser parser = new ArgParser();
        ArgParser cmdParser = parser.addCmd("cmd", ArgParserTest::callback, "helptext");
        parser.parse(new String[]{});
        assertEquals(false, parser.hasCmd());
    }


    @Test
    public void testCommandPresent() {
        ArgParser parser = new ArgParser();
        ArgParser cmdParser = parser.addCmd("cmd", ArgParserTest::callback, "helptext");
        parser.parse(new String[]{"cmd"});
        assertEquals(true, parser.hasCmd());
        assertEquals("cmd", parser.getCmd());
        assertEquals(cmdParser, parser.getCmdParser());
    }


    @Test
    public void testCommandWithOptions() {
        ArgParser parser = new ArgParser();
        ArgParser cmdParser = parser.addCmd("cmd", ArgParserTest::callback, "helptext");
        cmdParser.addFlag("bool");
        cmdParser.addStrOpt("string", "default");
        cmdParser.addIntOpt("int", 101);
        cmdParser.addFloatOpt("float", 1.1);
        parser.parse(new String[]{
            "cmd",
            "foo", "bar",
            "--string", "value",
            "--int", "202",
            "--float", "2.2",
        });
        assertEquals(true, parser.hasCmd());
        assertEquals("cmd", parser.getCmd());
        assertEquals(cmdParser, parser.getCmdParser());
        assertEquals(true, cmdParser.hasArgs());
        assertEquals(2, cmdParser.numArgs());
        assertEquals("value", cmdParser.getStrOpt("string"));
        assertEquals(202, cmdParser.getIntOpt("int"));
        assertEquals(2.2, (double)cmdParser.getFloatOpt("float"), 0.01);
    }
}
