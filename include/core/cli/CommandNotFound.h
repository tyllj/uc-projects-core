//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_COMMANDNOTFOUND_H
#define SGLOGGER_COMMANDNOTFOUND_H
#include "core/cli/CliProgram.h"

namespace core { namespace cli {
        class CommandNotFound : public CliProgram {
            uint8_t execute(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) override {
                output.writeLine("Command not found.");
                return 0;
            }
        };
}}
#endif //SGLOGGER_COMMANDNOTFOUND_H
