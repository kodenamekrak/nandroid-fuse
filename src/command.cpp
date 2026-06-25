#include "command.hpp"
#include "logger.hpp"
#include "unistd.h"

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sys/wait.h>

namespace nandroid::command
{
    // there's some wierd memory issues going on in here, not really sure where
    int run_with_output(const std::string& executable_path, std::vector<std::string> parameters, std::function<void(const std::string& line)> line_read_callback)
    {
        int child_stdout_pipe[2];
        if(pipe(child_stdout_pipe) != 0)
        {
            throw std::runtime_error("Failed to create pipe for child process");
        }
        auto [child_stdout_read, child_stdout_write] = child_stdout_pipe;

        size_t parameter_count = parameters.size();
        // allocate extra slot for program name + null terminator 
        std::vector<const char*> argv;
        argv.reserve(parameter_count + 2);
        argv.emplace_back(executable_path.c_str());
        for(const std::string& s : parameters)
        {
            argv.emplace_back(s.c_str());
        }
        argv.emplace_back(nullptr);

        pid_t pid = fork();
        if(pid == 0)
        {
            // point process stdout into the newly created pipe
            dup2(child_stdout_pipe[1], STDOUT_FILENO);
            dup2(child_stdout_pipe[1], STDERR_FILENO);
            // pipes no longer needed in child
            close(child_stdout_read);
            close(child_stdout_write);
            execv(executable_path.c_str(), (char* const*)argv.data());
            _Exit(1);
        }
        else
        {
            close(child_stdout_write);
            size_t BUF_SIZE = 256;
            char* line_buf = new char[BUF_SIZE];
            FILE* stream = fdopen(child_stdout_read, "r");
            for(int len; (len = getline((char**)&line_buf, &BUF_SIZE, stream)) != EOF;)
            {
                // dont copy newline character
                line_read_callback(std::string(line_buf, len - 1));
            }
            delete[] line_buf;
        }

        int status;
        if(waitpid(pid, &status, 0) == -1)
        {
            return -1;
        }

        int exit_code = WEXITSTATUS(status);
        Logger::verbose("Recieved exit code {} from command {} {}", exit_code, executable_path, parameters);
        return exit_code;
    }

    int run(const std::string& executable_path, std::vector<std::string> parameters)
    {
        return run_with_output(executable_path, parameters, [](auto _){});
    }
}