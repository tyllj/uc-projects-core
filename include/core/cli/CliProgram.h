//
// Created by tyll on 2022-01-24.
//

#ifndef SGLOGGER_CLIPROGRAM_H
#define SGLOGGER_CLIPROGRAM_H

#include <stdint.h>

#include "core/io/TextReader.h"
#include "core/io/TextWriter.h"
#include "core/io/Directory.h"

namespace core { namespace cli {
    class CliProgram {
    public:
        virtual uint8_t execute(io::TextReader& input, io::TextWriter& output, io::Directory& workingDirectory, uint8_t argc, const char** argv) = 0;
    };
}}
#endif //SGLOGGER_CLIPROGRAM_H
