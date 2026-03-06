/* posix-termios.c - Strict C89 Implementation */
/**
 * @file posix-termios.c
 * @brief Implementation of POSIX termios API for native MSVC.
 */
#include "posix-termios.h"

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif

/**
 * @brief Get input baud rate.
 * @param termios_p Pointer to termios structure.
 * @return Input speed.
 */
speed_t cfgetispeed(const struct termios *termios_p) {
    if (!termios_p) return 0;
    return termios_p->c_ispeed;
}

/**
 * @brief Get output baud rate.
 * @param termios_p Pointer to termios structure.
 * @return Output speed.
 */
speed_t cfgetospeed(const struct termios *termios_p) {
    if (!termios_p) return 0;
    return termios_p->c_ospeed;
}

/**
 * @brief Set input baud rate.
 * @param termios_p Pointer to termios structure.
 * @param speed Speed to set.
 * @return 0 on success, -1 on failure.
 */
int cfsetispeed(struct termios *termios_p, speed_t speed) {
    if (!termios_p) return -1;
    termios_p->c_ispeed = speed;
    return 0;
}

/**
 * @brief Set output baud rate.
 * @param termios_p Pointer to termios structure.
 * @param speed Speed to set.
 * @return 0 on success, -1 on failure.
 */
int cfsetospeed(struct termios *termios_p, speed_t speed) {
    if (!termios_p) return -1;
    termios_p->c_ospeed = speed;
    return 0;
}

#ifdef _WIN32
/**
 * @brief Internal helper to get HANDLE from file descriptor.
 * @param fd File descriptor.
 * @return HANDLE associated with the file descriptor.
 */
static HANDLE get_handle_from_fd(int fd) {
    HANDLE h = (HANDLE)_get_osfhandle(fd);
    if (h == INVALID_HANDLE_VALUE) {
        if (fd == 0) return GetStdHandle(STD_INPUT_HANDLE);
        if (fd == 1) return GetStdHandle(STD_OUTPUT_HANDLE);
        if (fd == 2) return GetStdHandle(STD_ERROR_HANDLE);
    }
    return h;
}
#endif

/**
 * @brief Wait for all output to be transmitted.
 * @param fd File descriptor.
 * @return 0 on success, -1 on failure.
 */
int tcdrain(int fd) {
#ifdef _WIN32
    HANDLE h = get_handle_from_fd(fd);
    if (h && h != INVALID_HANDLE_VALUE) {
        if (FlushFileBuffers(h)) {
            return 0;
        }
    }
    return -1;
#else
    (void)fd;
    return -1;
#endif
}

/**
 * @brief Suspend or restart transmission.
 * @param fd File descriptor.
 * @param action Action to perform.
 * @return 0 on success, -1 on failure.
 */
int tcflow(int fd, int action) {
    /* Mapped to polyfill, Windows doesn't easily support XON/XOFF directly without modifying DCB */
    (void)fd;
    (void)action;
    return 0;
}

/**
 * @brief Discard non-transmitted output data, non-read input data, or both.
 * @param fd File descriptor.
 * @param queue_selector Queue to flush.
 * @return 0 on success, -1 on failure.
 */
int tcflush(int fd, int queue_selector) {
#ifdef _WIN32
    HANDLE h = get_handle_from_fd(fd);
    if (h && h != INVALID_HANDLE_VALUE) {
        if (queue_selector == TCIFLUSH || queue_selector == TCIOFLUSH) {
            FlushConsoleInputBuffer(h);
        }
        /* No equivalent for output queue flushing on console */
        return 0;
    }
    return -1;
#else
    (void)fd;
    (void)queue_selector;
    return -1;
#endif
}

/**
 * @brief Get parameters associated with the terminal.
 * @param fd File descriptor.
 * @param termios_p Pointer to termios structure.
 * @return 0 on success, -1 on failure.
 */
int tcgetattr(int fd, struct termios *termios_p) {
#ifdef _WIN32
    HANDLE h;
    DWORD mode;

    if (!termios_p) {
        return -1;
    }

    h = get_handle_from_fd(fd);

    /* Initialize to default values */
    termios_p->c_iflag = 0;
    termios_p->c_oflag = 0;
    termios_p->c_cflag = 0;
    termios_p->c_lflag = 0;
    termios_p->c_ispeed = B9600;
    termios_p->c_ospeed = B9600;

    if (h && h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
        if (fd == 0) {
            /* Input console */
            if (mode & ENABLE_ECHO_INPUT) {
                termios_p->c_lflag |= ECHO;
            }
            if (mode & ENABLE_LINE_INPUT) {
                termios_p->c_lflag |= ICANON;
            }
            if (mode & ENABLE_PROCESSED_INPUT) {
                termios_p->c_lflag |= ISIG;
                termios_p->c_iflag |= IGNBRK;
            }
        } else {
            /* Output console */
            if (mode & ENABLE_PROCESSED_OUTPUT) {
                termios_p->c_oflag |= OPOST;
            }
        }
        return 0;
    }
    return -1; /* Not a console or failed */
#else
    (void)fd;
    if (termios_p) {
        termios_p->c_iflag = 0;
        termios_p->c_oflag = 0;
        termios_p->c_cflag = 0;
        termios_p->c_lflag = 0;
        termios_p->c_ispeed = B0;
        termios_p->c_ospeed = B0;
    }
    return -1;
#endif
}

/**
 * @brief Get process group ID of the session leader.
 * @param fd File descriptor.
 * @return Process group ID on success, -1 on failure.
 */
pid_t tcgetsid(int fd) {
    (void)fd;
    return -1; /* Not supported in this polyfill */
}

/**
 * @brief Send a break for a specific duration.
 * @param fd File descriptor.
 * @param duration Duration of the break.
 * @return 0 on success, -1 on failure.
 */
int tcsendbreak(int fd, int duration) {
    (void)fd;
    (void)duration;
    return 0; /* Stub */
}

/**
 * @brief Set parameters associated with the terminal.
 * @param fd File descriptor.
 * @param optional_actions When to apply the changes.
 * @param termios_p Pointer to termios structure.
 * @return 0 on success, -1 on failure.
 */
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p) {
#ifdef _WIN32
    HANDLE h;
    DWORD mode;

    (void)optional_actions;

    if (!termios_p) {
        return -1;
    }

    h = get_handle_from_fd(fd);
    if (h && h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode)) {
        if (fd == 0) {
            /* Input console */
            if (termios_p->c_lflag & ECHO) {
                mode |= ENABLE_ECHO_INPUT;
            } else {
                mode &= ~ENABLE_ECHO_INPUT;
            }
            if (termios_p->c_lflag & ICANON) {
                mode |= ENABLE_LINE_INPUT;
            } else {
                mode &= ~ENABLE_LINE_INPUT;
            }
            if (termios_p->c_lflag & ISIG) {
                mode |= ENABLE_PROCESSED_INPUT;
            } else {
                mode &= ~ENABLE_PROCESSED_INPUT;
            }
        } else {
            /* Output console */
            if (termios_p->c_oflag & OPOST) {
                mode |= ENABLE_PROCESSED_OUTPUT;
            } else {
                mode &= ~ENABLE_PROCESSED_OUTPUT;
            }
        }
        if (SetConsoleMode(h, mode)) {
            return 0;
        }
    }
    return -1;
#else
    (void)fd;
    (void)optional_actions;
    (void)termios_p;
    return -1;
#endif
}

