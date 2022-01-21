#include <iostream>
#include "io/ConsoleWriter.h"
#include "io/ConsoleReader.h"
int main() {
    core::io::ConsoleReader in;
    core::io::ConsoleWriter out;
    char* buf = (char*) malloc(128);
    in.readLine(buf, 128);
    out.printf("Hello, %s!\n", buf);
    return 0;
}
