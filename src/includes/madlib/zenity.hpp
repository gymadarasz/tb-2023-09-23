#pragma once

#include "str.hpp"
#include "sys.hpp"
#include "vectors.hpp"

using namespace std;

namespace madlib {

    string zenity_exec(const string& args, const string& err = "/dev/null") {
        return str_trim(exec("echo $(zenity " + args + " 2>" + err + ")"));
    }

    FILE* zenity_pipe(const string& args) {
        return pipe_exec("zenity " + args);
    }

    int zenity_result(const string& args) {
        return system(string("zenity " + args).c_str());
    }

    string zenity_esc(const string& inp, const string& sanitize = str_sanitizer_default_allowed_chars + ".'\",;:=?/()[]", const char replacement = '_') {
        string ret = inp;
        ret = str_sanitize(ret, sanitize, replacement);
        ret = str_esc(ret);
        return ret;
    }

    string zenity_quote(const string& inp, const char quote = '\'') {
        return quote + inp + quote;
    }

    string zenity_quote_esc(const string& inp, const string& sanitize = str_sanitizer_default_allowed_chars + ".'\",;:=?/()[]", const char replacement = '_', const char quote = '\'') {
        return zenity_quote(zenity_esc(inp, sanitize, replacement), quote);
    }

    string zenity_combo(
        const string& title, 
        const string& text,
        const string& label, 
        const vector<string>& items,
        const string& err = "/dev/null"
    ) {
        const string cmd = 
            str_replace(
                "--forms "
                "  --title={title}"
                "  --text={text}"
                "  --add-combo={label}"
                "  --combo-values={items}",
                {
                    { "{title}", zenity_quote_esc(title) },
                    { "{text}", zenity_quote_esc(text) },
                    { "{label}", zenity_quote_esc(label) },
                    { "{items}", zenity_quote(vector_concat(items, "|", zenity_esc)) },
                }
            );

        return zenity_exec(cmd, err);
    }

    string zenity_date(
        const string& title, 
        const string& text,
        const string& err = "/dev/null"
    ) {
        const string cmd = 
            str_replace(
                "--calendar"
                "  --title={title}"
                "  --text={text}"
                "  --date-format='%Y-%m-%d'",
                {
                    { "{title}", zenity_quote_esc(title) },
                    { "{text}", zenity_quote_esc(text) },
                }
            );

        return zenity_exec(cmd, err);
    }

    bool zenity_question(
        const string& title, 
        const string& text,
        const string& cancel = "No",
        const string& ok = "Yes"
    ) {
        const string cmd = 
            str_replace(
                "--question"
                "  --title={title}"
                "  --text={text}"
                "  --cancel-label={cancel}"
                "  --ok-label={ok}",
                {
                    { "{title}", zenity_quote_esc(title) },
                    { "{text}", zenity_quote_esc(text) },
                    { "{cancel}", zenity_quote_esc(cancel) },
                    { "{ok}", zenity_quote_esc(ok) },
                }
            );

        return !zenity_result(cmd);
    }

    string zenity_file_selection(
        const string& title = "", 
        const string& err = "/dev/null"
    ) {
        const string cmd = 
            str_replace(
                "--file-selection"
                "  --title={title}",
                {
                    { "{title}", zenity_quote_esc(title) },
                }
            );

        return zenity_exec(cmd, err);
    }

    FILE* zenity_progress(
        const string& title = "Loading...",
        bool noCancel = false,
        bool autoClose = true,
        bool timeRemaining = true
    ) {
        const string cmd = 
            str_replace(
                "--progress"
                "  --title={title}"
                "{noCancel}"
                "{autoClose}"
                "{timeRemaining}",
                {
                    { "{title}", zenity_quote_esc(title) },
                    { "{noCancel}", noCancel ? "  --no-cancel" : "" },
                    { "{autoClose}", autoClose ? "  --auto-close" : "" },
                    { "{timeRemaining}", timeRemaining ? "  --time-remaining" : "" },
                }
            );

        return zenity_pipe(cmd);
    }

    enum zenity_dialogue_type { INFO, WARNING, ERROR};
    void zenity_dialogue(
        const string& text, 
        const string* titleptr = nullptr,
        const zenity_dialogue_type type = ERROR,
        const string& err = "/dev/null"
    ) {
        string title = titleptr ? *titleptr : "Error";
        string typestr = "error";
        switch (type) {
            case INFO:
                typestr = "info";
                break;
            case WARNING:
                typestr = "warning";
                break;
            case ERROR:
                typestr = "error";
                break;
            default:
                throw ERROR("Invalid zenity dialogue type");
        }
        const string cmd = 
            str_replace(
                "--" + typestr +
                "  --text={title}"
                "  --text={text}",
                {
                    { "{title}", zenity_quote_esc(title) },
                    { "{text}", zenity_quote_esc(text) },
                }
            );

        zenity_exec(cmd, err);
    }

    bool zenity_progress_update(FILE* pipe, int percent) {
        return pipe_send(pipe, to_string(percent));
    }

    bool zenity_progress_update(FILE* pipe, const string& status) {
        return pipe_send(pipe, "# " + status);
    }

    int zenity_progress_close(FILE* pipe) {
        if (!zenity_progress_update(pipe, 100)) return 0;
        return pipe_close(pipe);
    }

}