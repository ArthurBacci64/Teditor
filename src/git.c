#include "ted.h"

// setup for the git bindings
void git_setup(void) {
    char buf[4000];
    char *slash = strrchr(filename, '/');

    if (slash) {
        int len = slash - filename;
        snprintf(buf, 4000,
                "git --version > /dev/null 2> /dev/null || exit 3;"
                "cd %.*s > /dev/null 2> /dev/null || exit 2;"
                "git rev-parse --git-dir > /dev/null 2> /dev/null || exit 2;"
                "git ls-files --error-unmatch %s > /dev/null 2> /dev/null; exit $?",
                len, filename, filename);
    } else {
        char cwd[1000];
        getcwd(cwd, 1000);
        snprintf(buf, 4000,
                "git --version > /dev/null 2> /dev/null || exit 3;"
                "cd %s > /dev/null 2> /dev/null || exit 2;"
                "git rev-parse --git-dir > /dev/null 2> /dev/null || exit 2;"
                "git ls-files --error-unmatch %s > /dev/null 2> /dev/null; exit $?",
                cwd, filename);
    }

    int ec = WEXITSTATUS(system(buf));
    if (ec == 3) {
        config.git_installed = 0;
        config.selected_buf.git_in_repo = 0;
        config.selected_buf.git_tracked = 0;
    } else if (ec == 2) {
        config.git_installed = 1;
        config.selected_buf.git_in_repo = 0;
        config.selected_buf.git_tracked = 0;
    } else if (ec == 1) {
        config.git_installed = 1;
        config.selected_buf.git_in_repo = 1;
        config.selected_buf.git_tracked = 0;
    } else {
        config.git_installed = 1;
        config.selected_buf.git_in_repo = 1;
        config.selected_buf.git_tracked = 1;
    }

    char *branch = malloc(1000);
    if (git_current_branch(branch, 1000)) {
        if (config.selected_buf.git_branch)
            free(config.selected_buf.git_branch);
        config.selected_buf.git_branch = branch;
    } else {
        free(branch);
        config.selected_buf.git_branch = NULL;
    }
}

// call when opening a new file
void git_update_info(char *fname) {
    if (!config.git_installed) return;

    char buf[4000];
    char *slash = strrchr(fname, '/');

    if (slash) {
        int len = slash - fname;
        snprintf(buf, 4000,
                "cd %.*s > /dev/null 2> /dev/null || exit 2;"
                "git rev-parse --git-dir > /dev/null 2> /dev/null || exit 2;"
                "git ls-files --error-unmatch %s > /dev/null 2> /dev/null; exit $?",
                len, fname, fname);
    } else {
        char cwd[1000];
        getcwd(cwd, 1000);
        snprintf(buf, 4000,
                "cd %s > /dev/null 2> /dev/null || exit 2;"
                "git rev-parse --git-dir > /dev/null 2> /dev/null || exit 2;"
                "git ls-files --error-unmatch %s > /dev/null 2> /dev/null; exit $?",
                cwd, fname);
    }

    int ec = WEXITSTATUS(system(buf));
    if (ec == 2) {
        config.git_installed = 1;
        config.selected_buf.git_in_repo = 0;
        config.selected_buf.git_tracked = 0;
    } else if (ec == 1) {
        config.git_installed = 1;
        config.selected_buf.git_in_repo = 1;
        config.selected_buf.git_tracked = 0;
    } else {
        config.git_installed = 1;
        config.selected_buf.git_in_repo = 1;
        config.selected_buf.git_tracked = 1;
    }

    char *branch = malloc(1000);
    if (git_current_branch(branch, 1000)) {
        if (config.selected_buf.git_branch)
            free(config.selected_buf.git_branch);
        config.selected_buf.git_branch = branch;
    } else {
        free(branch);
        config.selected_buf.git_branch = NULL;
    }
}

/*Utilities for future use*/
bool git_in_repo(char *path) {
    if (!config.git_installed) return 0;
    char buf[4000];
    char *temp;

    if (path == NULL) {
        char cwd[1000];
        if (getcwd(cwd, 1000) == NULL) return 0;
        snprintf(buf, 4000, "cd %s;", cwd);
    } else if ((temp = strrchr(path, '/'))) {
        int len = temp - path;
        snprintf(buf, 4000, "cd %.*s;", len, path);
    } else
        snprintf(buf, 4000, "cd %s;", path);

    strncat(buf, "git rev-parse --git-dir > /dev/null 2> /dev/null", 4000);
    return system(buf) == 0;
}

bool git_current_branch(char *buf, unsigned int buflen) {
    if (!config.git_installed || !config.selected_buf.git_in_repo) return 0;

    int stdout_pipe[2];
    if (pipe(stdout_pipe) < 0) return 0;

    pid_t pid = fork();
    if (pid < 0) {
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);
        return 0;
    }
    if (pid == 0) {//child process
        dup2(stdout_pipe[1], STDOUT_FILENO);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        execl("/bin/sh", "sh", "-c", "git rev-parse --abbrev-ref HEAD", NULL);
        exit(1);//unreachable
    }

    int size = -1;
    while ((size = read(stdout_pipe[0], buf, buflen)) == -1 && errno == EINTR);
    buf[size] = '\0';
    close(stdout_pipe[0]);
    close(stdout_pipe[1]);
    return 1;
}

bool git_tracked(char *fname) {
    if (!config.git_installed) return 0;
    char buf[4000];
    snprintf(buf, 4000, "git ls-files --error-unmatch %s", fname);
    return system(buf) == 0;
}
