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

namespace core { namespace cli {
        typedef core::shared_ptr<core::cli::CliProgram> (*ProgramFactory)(void);


        class Sgsh : public CliProgram {
            static constexpr uint8_t MAX_WORDS = 10;
        public:
            explicit Sgsh(core::cli::StringMap<ProgramFactory, 128>& commands) : _commands(commands) {

            }

            uint8_t execute(io::TextReader& input, io::TextWriter& output, io::Directory& workingDirectory, uint8_t argc, const char **argv) override {
                LineEditor<80> _lineEditor(input, output);
                char* command = "";
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

                    if (handleBuildIn(command))
                        continue;

                    shared_ptr<core::cli::CliProgram> program = _commands.getOrDefault(command, _commandNotFound)();
                    program->execute(input, output, workingDirectory, cmdArgc, cmdArgv);
                }
            }

        private:
            ProgramFactory _commandNotFound = []() { return shared_ptr<core::cli::CliProgram>(new CommandNotFound()); };
            core::cli::StringMap<ProgramFactory, 128>& _commands;

            bool _isShutdownRequested = false;

            static void printPrompt(io::TextWriter& output) {
                output.write("\r > ");
            }

            bool handleBuildIn(const char* command) {
                if (strcasecmp(command, "exit") == 0) {
                    _isShutdownRequested = true;
                }
            }
        };
    }}
#endif //SGLOGGER_SGSH_H
