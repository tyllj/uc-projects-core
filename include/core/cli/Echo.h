//
// Created by tyll on 2022-01-26.
//

#ifndef UC_CORE_ECHO_H
#define UC_CORE_ECHO_H
#include "core/cli/CliProgram.h"

namespace core { namespace cli {
        class Echo : public CliProgram {
            uint8_t execute(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) override {
                for (uint8_t i = 0; i < argc; i++) {
                    output.write(argv[i]);
                    output.write(' ');
                }
                output.writeLine("");

                return 0;
            }
        };
    }}
#endif //UC_CORE_ECHO_H
