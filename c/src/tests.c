// -------------------------------------------------------------------------
// Unit tests for the Clio library.
// -------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "clio.h"


// -------------------------------------------------------------------------
// Boolean options.
// -------------------------------------------------------------------------


void test_bool_option_empty() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool", NULL);
    clio_parse(parser, 1, (char *[]){""});
    assert(clio_get_flag(parser, "bool") == false);
    clio_free(parser);
    printf(".");
}


void test_bool_option_missing() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool", NULL);
    clio_parse(parser, 3, (char *[]){"", "foo", "bar"});
    assert(clio_get_flag(parser, "bool") == false);
    clio_free(parser);
    printf(".");
}


void test_bool_option_longform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool", NULL);
    clio_parse(parser, 2, (char *[]){"", "--bool"});
    assert(clio_get_flag(parser, "bool") == true);
    clio_free(parser);
    printf(".");
}


void test_bool_option_shortform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool", "b");
    clio_parse(parser, 2, (char *[]){"", "-b"});
    assert(clio_get_flag(parser, "bool") == true);
    clio_free(parser);
    printf(".");
}


// -------------------------------------------------------------------------
// String options.
// -------------------------------------------------------------------------


void test_string_option_empty() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_str(parser, "string", "default", NULL);
    clio_parse(parser, 1, (char *[]){""});
    assert(strcmp(clio_get_str(parser, "string"), "default") == 0);
    printf(".");
}


void test_string_option_missing() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_str(parser, "string", "default", NULL);
    clio_parse(parser, 3, (char *[]){"", "foo", "bar"});
    assert(strcmp(clio_get_str(parser, "string"), "default") == 0);
    printf(".");
}


void test_string_option_longform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_str(parser, "string", "default", NULL);
    clio_parse(parser, 3, (char *[]){"", "--string", "value"});
    assert(strcmp(clio_get_str(parser, "string"), "value") == 0);
    printf(".");
}


void test_string_option_shortform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_str(parser, "string", "default", "s");
    clio_parse(parser, 3, (char *[]){"", "-s", "value"});
    assert(strcmp(clio_get_str(parser, "string"), "value") == 0);
    printf(".");
}


// -------------------------------------------------------------------------
// Integer options.
// -------------------------------------------------------------------------


void test_int_option_empty() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_int(parser, "int", 101, NULL);
    clio_parse(parser, 1, (char *[]){""});
    assert(clio_get_int(parser, "int") == 101);
    printf(".");
}


void test_int_option_missing() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_int(parser, "int", 101, NULL);
    clio_parse(parser, 3, (char *[]){"", "foo", "bar"});
    assert(clio_get_int(parser, "int") == 101);
    printf(".");
}


void test_int_option_longform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_int(parser, "int", 101, NULL);
    clio_parse(parser, 3, (char *[]){"", "--int", "202"});
    assert(clio_get_int(parser, "int") == 202);
    printf(".");
}


void test_int_option_shortform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_int(parser, "int", 101, "i");
    clio_parse(parser, 3, (char *[]){"", "-i", "202"});
    assert(clio_get_int(parser, "int") == 202);
    printf(".");
}


// -------------------------------------------------------------------------
// Float options.
// -------------------------------------------------------------------------


void test_float_option_empty() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_float(parser, "float", 1.1, NULL);
    clio_parse(parser, 1, (char *[]){""});
    assert(clio_get_float(parser, "float") == 1.1);
    printf(".");
}


void test_float_option_missing() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_float(parser, "float", 1.1, NULL);
    clio_parse(parser, 3, (char *[]){"", "foo", "bar"});
    assert(clio_get_float(parser, "float") == 1.1);
    printf(".");
}


void test_float_option_longform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_float(parser, "float", 1.1, NULL);
    clio_parse(parser, 3, (char *[]){"", "--float", "2.2"});
    assert(clio_get_float(parser, "float") == 2.2);
    printf(".");
}


void test_float_option_shortform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_float(parser, "float", 1.1, "f");
    clio_parse(parser, 3, (char *[]){"", "-f", "2.2"});
    assert(clio_get_float(parser, "float") == 2.2);
    printf(".");
}


// -------------------------------------------------------------------------
// Multiple options.
// -------------------------------------------------------------------------


void test_multi_options_empty() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool1", NULL);
    clio_add_flag(parser, "bool2", "b");
    clio_add_str(parser, "string1", "default1", NULL);
    clio_add_str(parser, "string2", "default2", "s");
    clio_add_int(parser, "int1", 101, NULL);
    clio_add_int(parser, "int2", 202, "i");
    clio_add_float(parser, "float1", 1.1, NULL);
    clio_add_float(parser, "float2", 2.2, "f");
    clio_parse(parser, 1, (char *[]){""});
    assert(clio_get_flag(parser, "bool1") == false);
    assert(clio_get_flag(parser, "bool2") == false);
    assert(strcmp(clio_get_str(parser, "string1"), "default1") == 0);
    assert(strcmp(clio_get_str(parser, "string2"), "default2") == 0);
    assert(clio_get_int(parser, "int1") == 101);
    assert(clio_get_int(parser, "int2") == 202);
    assert(clio_get_float(parser, "float1") == 1.1);
    assert(clio_get_float(parser, "float2") == 2.2);
    printf(".");
}


void test_multi_options_longform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool1", NULL);
    clio_add_flag(parser, "bool2", "b");
    clio_add_str(parser, "string1", "default1", NULL);
    clio_add_str(parser, "string2", "default2", "s");
    clio_add_int(parser, "int1", 101, NULL);
    clio_add_int(parser, "int2", 202, "i");
    clio_add_float(parser, "float1", 1.1, NULL);
    clio_add_float(parser, "float2", 2.2, "f");
    clio_parse(parser, 15, (char *[]){
        "",
        "--bool1",
        "--bool2",
        "--string1", "value1",
        "--string2", "value2",
        "--int1", "303",
        "--int2", "404",
        "--float1", "3.3",
        "--float2", "4.4",
    });
    assert(clio_get_flag(parser, "bool1") == true);
    assert(clio_get_flag(parser, "bool2") == true);
    assert(strcmp(clio_get_str(parser, "string1"), "value1") == 0);
    assert(strcmp(clio_get_str(parser, "string2"), "value2") == 0);
    assert(clio_get_int(parser, "int1") == 303);
    assert(clio_get_int(parser, "int2") == 404);
    assert(clio_get_float(parser, "float1") == 3.3);
    assert(clio_get_float(parser, "float2") == 4.4);
    printf(".");
}


void test_multi_options_shortform() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool1", NULL);
    clio_add_flag(parser, "bool2", "b");
    clio_add_str(parser, "string1", "default1", NULL);
    clio_add_str(parser, "string2", "default2", "s");
    clio_add_int(parser, "int1", 101, NULL);
    clio_add_int(parser, "int2", 202, "i");
    clio_add_float(parser, "float1", 1.1, NULL);
    clio_add_float(parser, "float2", 2.2, "f");
    clio_parse(parser, 15, (char *[]){
        "",
        "--bool1",
        "-b",
        "--string1", "value1",
        "-s", "value2",
        "--int1", "303",
        "-i", "404",
        "--float1", "3.3",
        "-f", "4.4",
    });
    assert(clio_get_flag(parser, "bool1") == true);
    assert(clio_get_flag(parser, "bool2") == true);
    assert(strcmp(clio_get_str(parser, "string1"), "value1") == 0);
    assert(strcmp(clio_get_str(parser, "string2"), "value2") == 0);
    assert(clio_get_int(parser, "int1") == 303);
    assert(clio_get_int(parser, "int2") == 404);
    assert(clio_get_float(parser, "float1") == 3.3);
    assert(clio_get_float(parser, "float2") == 4.4);
    printf(".");
}


// -------------------------------------------------------------------------
// Condensed short-form options.
// -------------------------------------------------------------------------


void test_condensed_options() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_flag(parser, "bool", "b");
    clio_add_str(parser, "string", "default", "s");
    clio_add_int(parser, "int", 101, "i");
    clio_add_float(parser, "float", 1.1, "f");
    clio_parse(parser, 5, (char *[]){"", "-bsif", "value", "202", "2.2"});
    assert(clio_get_flag(parser, "bool") == true);
    assert(strcmp(clio_get_str(parser, "string"), "value") == 0);
    assert(clio_get_int(parser, "int") == 202);
    assert(clio_get_float(parser, "float") == 2.2);
    printf(".");
}


// -------------------------------------------------------------------------
// Positional arguments.
// -------------------------------------------------------------------------


void test_positional_args_empty() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_parse(parser, 1, (char *[]){""});
    assert(clio_has_args(parser) == false);
    printf(".");
}


void test_positional_args() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_parse(parser, 3, (char *[]){"", "foo", "bar"});
    assert(clio_has_args(parser) == true);
    assert(clio_num_args(parser) == 2);
    assert(strcmp(clio_get_args(parser)[0], "foo") == 0);
    assert(strcmp(clio_get_args(parser)[1], "bar") == 0);
    printf(".");
}


void test_positional_args_as_ints() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_parse(parser, 3, (char *[]){"", "1", "11"});
    assert(clio_get_args_as_ints(parser)[0] == 1);
    assert(clio_get_args_as_ints(parser)[1] == 11);
    printf(".");
}


void test_positional_args_as_floats() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_parse(parser, 3, (char *[]){"", "1.1", "11.1"});
    assert(clio_get_args_as_floats(parser)[0] == 1.1);
    assert(clio_get_args_as_floats(parser)[1] == 11.1);
    printf(".");
}


// -------------------------------------------------------------------------
// Option parsing switch.
// -------------------------------------------------------------------------


void test_option_parsing_switch() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_parse(parser, 5, (char *[]){"", "foo", "--", "--bar", "--baz"});
    assert(clio_num_args(parser) == 3);
    printf(".");
}


// -------------------------------------------------------------------------
// Commands.
// -------------------------------------------------------------------------


void callback(ArgParser *parser) {}


void test_command_absent() {
    ArgParser *parser = clio_new(NULL, NULL);
    clio_add_cmd(parser, "cmd", callback, "helptext");
    clio_parse(parser, 1, (char *[]){""});
    assert(clio_has_cmd(parser) == false);
    printf(".");
}


void test_command_present() {
    ArgParser *parser = clio_new(NULL, NULL);
    ArgParser *cmd_parser = clio_add_cmd(parser, "cmd", callback, "helptext");
    clio_parse(parser, 2, (char *[]){"", "cmd"});
    assert(clio_has_cmd(parser) == true);
    assert(strcmp(clio_get_cmd(parser), "cmd") == 0);
    assert(clio_get_cmd_parser(parser) == cmd_parser);
    printf(".");
}


void test_command_with_options() {
    ArgParser *parser = clio_new(NULL, NULL);
    ArgParser *cmd_parser = clio_add_cmd(parser, "cmd", callback, "helptext");
    clio_add_flag(cmd_parser, "bool", NULL);
    clio_add_str(cmd_parser, "string", "default", NULL);
    clio_add_int(cmd_parser, "int", 101, NULL);
    clio_add_float(cmd_parser, "float", 1.1, NULL);
    clio_parse(parser, 10, (char *[]){
        "",
        "cmd",
        "foo", "bar",
        "--string", "value",
        "--int", "202",
        "--float", "2.2",
    });
    assert(clio_has_cmd(parser) == true);
    assert(strcmp(clio_get_cmd(parser), "cmd") == 0);
    assert(clio_get_cmd_parser(parser) == cmd_parser);
    assert(clio_has_args(cmd_parser) == true);
    assert(clio_num_args(cmd_parser) == 2);
    assert(strcmp(clio_get_str(cmd_parser, "string"), "value") == 0);
    assert(clio_get_int(cmd_parser, "int") == 202);
    assert(clio_get_float(cmd_parser, "float") == 2.2);
    printf(".");
}


// -------------------------------------------------------------------------
// Test runner.
// -------------------------------------------------------------------------


int main(int argc, char **argv) {
    setbuf(stdout, NULL);

    test_bool_option_empty();
    test_bool_option_missing();
    test_bool_option_longform();
    test_bool_option_shortform();

    test_string_option_empty();
    test_string_option_missing();
    test_string_option_longform();
    test_string_option_shortform();

    test_int_option_empty();
    test_int_option_missing();
    test_int_option_longform();
    test_int_option_shortform();

    test_float_option_empty();
    test_float_option_missing();
    test_float_option_longform();
    test_float_option_shortform();

    test_multi_options_empty();
    test_multi_options_longform();
    test_multi_options_shortform();

    test_condensed_options();

    test_positional_args_empty();
    test_positional_args();
    test_positional_args_as_ints();
    test_positional_args_as_floats();

    test_option_parsing_switch();

    test_command_absent();
    test_command_present();
    test_command_with_options();

    printf("\n");
}
