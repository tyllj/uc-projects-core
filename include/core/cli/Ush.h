//
// Created by tyll on 2022-03-23.
//

#ifndef UC_CORE_USH_H
#define UC_CORE_USH_H
#include "core/shared_ptr.h"
#include "core/cli/LineEditor.h"
#include "core/cli/CliProgram.h"
#include "core/cli/StringMap.h"
#include "core/cli/CommandNotFound.h"
#include "core/Sleep.h"
#include "core/StringBuilder.h"
#include "core/io/Path.h"

namespace core { namespace cli {
    class Ush {
        static constexpr uint8_t MAX_WORDS = 10;
    public:
        Ush(StringMap<ProgramFactory, 128> &commands, io::TextReader &input, io::TextWriter &output,
            io::DirectoryInfo workingDirectory)
                : _input(input),
                _output(output),
                _workingDirectory(workingDirectory),
                _lineEditor(input, output),
                _commands(commands),
                _needPrintPrompt(true)  {
        }

        bool accept() {
            printPromptIfNeeded();
            if (_lineEditor.accept()) {
                char* command = nullptr;
                char* words[MAX_WORDS] = {nullptr};
                uint8_t wordsCount = 0;
                command = _lineEditor.getBuffer();

                if (!cstrings::isNullOrEmpty(command)) {
                    wordsCount = cstrings::split(command, ' ', words, MAX_WORDS);
                    const char **cmdArgv = (const char **) &words[1];
                    const uint8_t cmdArgc = wordsCount - 1;

                    bool handled = handleBuildIn(command, _input, _output, _workingDirectory, cmdArgc, cmdArgv);
                    if (!handled) {
                        shared_ptr<core::cli::CliProgram> program = _commands.getOrDefault(command, _commandNotFound)();
                        program->execute(_input, _output, _workingDirectory, cmdArgc, cmdArgv);
                    }
                }
                setNeedPrintPrompt();
                _lineEditor.clear();
                return true;
            }
            return false;
        }

        bool isExit() {
            return _isExit;
        }

    private:
        void setNeedPrintPrompt() {
            _needPrintPrompt = true;
        }

        void printPromptIfNeeded() {
            if (_needPrintPrompt) {
                _needPrintPrompt = false;
                _output.write("\r");
                _output.write(_workingDirectory.getFullName());
                _output.write(" > ");
            }
        }

        bool handleBuildIn(const char* command, io::TextReader& input, io::TextWriter& output, io::DirectoryInfo& workingDirectory, uint8_t argc, const char **argv) {
            if (cstrings::equals(command, "exit")) {
                //exit(input, output, workingDirectory, argc, argv);
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
            _isExit = true;
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

    private:
        io::TextReader& _input;
        io::TextWriter& _output;
        io::DirectoryInfo _workingDirectory;
        LineEditor<80> _lineEditor; //(_input, _output)
        ProgramFactory _commandNotFound = []() { return shared_ptr<core::cli::CliProgram>(new CommandNotFound()); };
        core::cli::StringMap<ProgramFactory, 128>& _commands;
        bool _needPrintPrompt;
        bool _isExit;
    };
}}
#endif //UC_CORE_USH_H
