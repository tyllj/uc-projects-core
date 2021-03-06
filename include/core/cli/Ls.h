//
// Created by tyll on 2022-03-19.
//

#ifndef UC_CORE_LS_H
#define UC_CORE_LS_H
#include "core/cli/CliProgram.h"

namespace core { namespace cli {
        class Ls : public CliProgram {
            uint8_t execute(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) override {
                workingDirectory.forEach([&](core::io::FileSystemInfo& fsInfo) {
                    output.writeLine(fsInfo.getName());
                });
                return 0;
            }
        };
    }}
#endif //UC_CORE_LS_H
