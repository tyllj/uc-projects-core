//
// Created by tyll on 2022-03-20.
//

#ifndef UC_CORE_CAT_H
#define UC_CORE_CAT_H
#include "core/cli/CliProgram.h"
#include "core/io/StreamReader.h"
#include "core/io/Path.h"
#include "core/StringBuilder.h"
namespace core { namespace cli {
        class Cat : public CliProgram {
            uint8_t execute(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) override {
                if (argc != 1)
                    return 0;

                core::StringBuilder sb;
                core::io::path::combine(sb, workingDirectory.getFullName(), argv[0]);
                const char* path = sb.toString();
                core::io::FileInfo file(workingDirectory, path);
                if(file.exists()) {
                    core::shared_ptr<core::io::Stream> fileStream = file.open(io::READ);
                    core::io::StreamReader reader(*fileStream);
                    int32_t c;
                    while ((c = reader.read()) > -1) {
                        output.write(c);
                    }
                    output.writeLine();
                } else {
                    output.write("cat: ");
                    output.write(path);
                    output.writeLine(": No such file.");
                }


                return 0;
            }
        };
    }}
#endif //UC_CORE_CAT_H
