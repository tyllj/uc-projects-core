//
// Created by tyll on 2022-01-26.
//

#ifndef SGLOGGER_SGSH_H
#define SGLOGGER_SGSH_H

#include "core/shared_ptr.h"
#include "core/cli/LineEditor.h"
#include "core/cli/CliProgram.h"
#include "core/cli/StringMap.h"
#include "core/cli/CommandNotFound.h"
#include "core/Sleep.h"
#include "core/StringBuilder.h"
#include "core/io/Path.h"

namespace core { namespace cli {
        class Sgsh : public CliProgram {
            static constexpr uint8_t MAX_WORDS = 10;
        public:
            explicit Sgsh(core::cli::StringMap<ProgramFactory, 128>& commands) : _commands(commands), _workingDirectory(io::DirectoryInfo(io::NullFileSystem, "/")) {}

            uint8_t execute(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) override {
                _workingDirectory = workingDirectory;
                LineEditor<80> _lineEditor(input, output);
                char* command = nullptr;
                char* words[MAX_WORDS] = {nullptr};
                uint8_t wordsCount = 0;
                while (!_isShutdownRequested) {
                    printPrompt(output);
                    command = _lineEditor.readLine();

                    if (cstrings::isNullOrEmpty(command))
                        continue;

                    wordsCount = cstrings::split(command, ' ', words, MAX_WORDS);

                    const char** cmdArgv = (const char**) &words[1];
                    const uint8_t cmdArgc = wordsCount - 1;

                    if (handleBuildIn(command, input, output, _workingDirectory, cmdArgc, cmdArgv))
                        continue;

                    shared_ptr<core::cli::CliProgram> program = _commands.getOrDefault(command, _commandNotFound)();
                    program->execute(input, output, _workingDirectory, cmdArgc, cmdArgv);
                }

                return 0;
            }

        private:
            ProgramFactory _commandNotFound = []() { return shared_ptr<core::cli::CliProgram>(new CommandNotFound()); };
            core::cli::StringMap<ProgramFactory, 128>& _commands;
            io::DirectoryInfo _workingDirectory;

            bool _isShutdownRequested = false;

            void printPrompt(io::TextWriter& output) {
                output.write("\r");
                output.write(_workingDirectory.getFullName());
                output.write(" > ");
            }

            bool handleBuildIn(const char* command, io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) {
                if (cstrings::equals(command, "exit")) {
                    exit(input, output, workingDirectory, argc, argv);
                    return true;
                }
                if (cstrings::equals(command, "pwd")) {
                    pwd(input, output, workingDirectory, argc, argv);
                    return true;
                }
                if (cstrings::equals(command, "cd")) {
                    // change directory
                    cd(input, output, workingDirectory, argc, argv);
                    return true;
                }
                return false;
            }

            void exit(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) {
                _isShutdownRequested = true;
            }

            void pwd(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) {
                output.writeLine(workingDirectory.getFullName());
            }

            void cd(io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) {
                if (argc != 1 || core::cstrings::isNullOrEmpty(argv[0]))
                    return;
                const char* path = argv[0];
                if (core::cstrings::equals(".", argv[0])){
                    return;
                }
                if (core::cstrings::equals("..", argv[0])) {
                    workingDirectory = workingDirectory.getDirectory();
                    return;
                }

                core::StringBuilder sb;
                core::io::path::combine(sb, workingDirectory.getFullName(), path);

                io::DirectoryInfo wd(workingDirectory,  sb.toString());
                if (wd.exists()) {
                    _workingDirectory = wd;
                } else {
                    output.writeLine("Directory not found.");
                }
            }
        };
    }}
#endif //SGLOGGER_SGSH_H
