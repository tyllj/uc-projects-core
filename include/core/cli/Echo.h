//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_ECHO_H
#define SGLOGGER_ECHO_H
#include "core/cli/CliProgram.h"

namespace core { namespace cli {
        class Echo : public CliProgram {
            uint8_t execute(io::TextReader& input, io::TextWriter& output, io::Directory& workingDirectory, uint8_t argc, const char **argv) override {
                for (uint8_t i = 0; i < argc; i++) {
                    output.write(argv[i]);
                    output.write(' ');
                }
                output.write("\r\n");
            }
        };
    }}
#endif //SGLOGGER_ECHO_H
