#pragma once

#include <iostream>
#include <array>
#include <signal.h>
#include <poll.h>

#include "Log.hpp"

using namespace std;

namespace madlib {

    int exec_last_exit_status = 0;
    int exec_last_exit_code = 0;
    string exec(const string& command, bool captureOutput = true, bool showOutput = false) {        
        exec_last_exit_status = 0;
        exec_last_exit_code = 0;

        array<char, 128> buffer;
        string result;

        // Open the command for reading, redirecting stderr to stdout
        FILE* pipe = popen((command + " 2>&1").c_str(), "r");
        if (!pipe) throw ERROR("Failed to execute command.");

        // Read the output (if requested)
        if (captureOutput || showOutput) {
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
                result += buffer.data();
            if (showOutput) cout << result;
        }

        // Close the pipe and get the exit status
        exec_last_exit_status = pclose(pipe);

        // Check if the command exited normally
        if (WIFEXITED(exec_last_exit_status)) 
            exec_last_exit_code = WEXITSTATUS(exec_last_exit_status);
        else if (
            WIFSIGNALED(exec_last_exit_status) && 
            WTERMSIG(exec_last_exit_status) == SIGPIPE
        )
            // Handle the case where the process was terminated by a broken pipe
            LOG("Warning: The process was terminated by a broken pipe.");
        else
            throw ERROR(
                "Error occurred while closing the pipe. Exit signal: " 
                + to_string(WTERMSIG(exec_last_exit_status))
            );
        
        return result;
    }

    FILE* pipe_exec(const string& command) {
        // Execute the command in the background
        FILE* pipe = popen(command.c_str(), "w");
        if (!pipe) throw ERROR("Failed to open pipe for command execution.");
        return pipe;
    }

    bool pipe_is_closed(FILE* pipe) {
        // Get the file descriptor associated with the FILE*
        pollfd pfd = { fileno(pipe), POLLOUT, 0 };
        if (poll(&pfd, 1, 1) < 0) return false;
        return pfd.revents & POLLERR;
    }

    bool pipe_send(FILE* pipe, const string& updates) {
        // Check writing to the pipe failed (possibly closed by user), return false
        if (pipe_is_closed(pipe)) return false;
        fprintf(pipe, "%s\n", updates.c_str());
        if (pipe_is_closed(pipe)) return false;
        fflush(pipe);
        return true;

    }

    int pipe_close(FILE* pipe) {
        // Close the pipe
        return pclose(pipe);
    }

}