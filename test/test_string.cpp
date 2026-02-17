#include "Arduino.h"
#include <cassert>
#include <cstdio>

int main() {
    // Construction
    String s1("hello");
    assert(s1.length() == 5);
    assert(s1 == "hello");

    String s2(42);
    assert(s2 == "42");

    String s3(3.14f, 2);
    assert(s3.startsWith("3.14"));

    // Concatenation
    String s4 = s1 + " world";
    assert(s4 == "hello world");

    s4 += "!";
    assert(s4 == "hello world!");

    // indexOf / substring
    assert(s4.indexOf(' ') == 5);
    assert(s4.indexOf("world") == 6);
    assert(s4.substring(6) == "world!");
    assert(s4.substring(6, 11) == "world");

    // startsWith / endsWith
    assert(s4.startsWith("hello"));
    assert(s4.endsWith("!"));

    // toUpperCase / toLowerCase
    String s5("AbCd");
    s5.toUpperCase();
    assert(s5 == "ABCD");
    s5.toLowerCase();
    assert(s5 == "abcd");

    // trim
    String s6("  hi  ");
    s6.trim();
    assert(s6 == "hi");

    // replace
    String s7("foo bar foo");
    s7.replace("foo", "baz");
    assert(s7 == "baz bar baz");

    // toInt / toFloat
    String s8("123");
    assert(s8.toInt() == 123);
    String s9("1.5");
    assert(s9.toFloat() > 1.4f && s9.toFloat() < 1.6f);

    // equalsIgnoreCase
    String s10("Hello");
    assert(s10.equalsIgnoreCase("hELLO"));

    // isEmpty
    String s11;
    assert(s11.isEmpty());
    assert(!s1.isEmpty());

    printf("test_string: all assertions passed\n");
    return 0;
}
