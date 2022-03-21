//
// Created by tyll on 2022-01-24.
//

#ifndef SGLOGGER_CLIPROGRAM_H
#define SGLOGGER_CLIPROGRAM_H

#include <stdint.h>
#include "etl/delegate.h"
#include "core/io/TextReader.h"
#include "core/io/TextWriter.h"
#include "core/io/FileSystemInfo.h"
#include "core/shared_ptr.h"

namespace core { namespace cli {

    class CliProgram {
    public:
        virtual uint8_t execute(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char** argv) = 0;
    };

    //typedef core::shared_ptr<core::cli::CliProgram> (*ProgramFactory)(void);
    typedef etl::delegate<core::shared_ptr<core::cli::CliProgram>(void)> ProgramFactory;
    }}
#endif //SGLOGGER_CLIPROGRAM_H
