//
// Created by Xinda Wu on 2024/6/23.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>

// Inotify and Libnotify n/a on mac
#include <sys/inotify.h>
#include <libnotify/notify.h>

#define EXT_SUCCESS 0
#define EXT_ERR_REQUIRED_ARGS_MISS 1
#define EXT_ERR_INOTIFY_INIT 2
#define EXT_ERR_INOTIFY_ADD_WATCH 3
#define EXT_ERR_INOTIFY_RM_WATCH 4
#define EXT_ERR_BASENAME_NULL 5
#define EXT_ERR_READ_INOTIFY 6
#define EXT_ERR_LIBNOTIFY_INIT 7
#define EXT_ERR_NOTIFY_HANDLER_NULL 8


void err_shutdown_handler(bool err_condition, int ext_code, char* msg);
void sig_shutdown_handler(int signal);




int main(int argc, char** argv) {

    // Dependencies for inotify

    char buf[4096];
    int readlen = 0;
    int IeventQueue = -1 ;
    int IwatchDescriptor = -1 ;

    char* _basename = basename(argv[1]);
    const uint32_t watchMask = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE |
                                IN_MODIFY | IN_MOVE_SELF;

    const struct inotify_event* event;
    char* message = NULL;

    // Dependencies for libnotify

    bool notifyInitStatus;
    NotifyNotification* NNotification;


    // Check arguments
    err_shutdown_handler(argc<2, EXT_ERR_REQUIRED_ARGS_MISS, "Usage: watchman $PATH");
    printf("Basename: %s\n", _basename);
    err_shutdown_handler(_basename==NULL, EXT_ERR_BASENAME_NULL, "Err: file path basename null!");

    // Init libnotify notification handle
    notifyInitStatus = notify_init(argv[0]);
    err_shutdown_handler(!notifyInitStatus, EXT_ERR_LIBNOTIFY_INIT, "Err: fail initialising libnotify!");

    // Init inotify instance
    IeventQueue = inotify_init();
    err_shutdown_handler(IeventQueue==-1, EXT_ERR_INOTIFY_INIT, "Err: fail initialing event queue!");

    // Add event watch
    IwatchDescriptor = inotify_add_watch(IeventQueue, argv[1], watchMask);
    err_shutdown_handler(IwatchDescriptor==-1, EXT_ERR_INOTIFY_ADD_WATCH, "Err: fail adding file to watch!");

    // Main Daemon loop
    while (true) {

        printf("Listening for events ... \n");

        // read block until data available ?
        readlen = read(IeventQueue, buf, sizeof(buf));
        err_shutdown_handler(readlen==-1, EXT_ERR_READ_INOTIFY,
                             "Err: fail reading from inotify instance!");

        // loop over events
        for (char* ptr = buf; ptr < buf + readlen;
            ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event*) ptr;

            if (event->mask & IN_CREATE) {
                message = "File created. \n";
            }

            if (event->mask & IN_DELETE) {
                message = "File deleted. \n";
            }

            if (event->mask & IN_ACCESS) {
                message = "File accessed. \n";
            }

            if (event->mask & IN_CLOSE_WRITE) {
                message = "File written and closed. \n";
            }

            if (event->mask & IN_MODIFY) {
                message = "File modified. \n";
            }

            if (message==NULL) {
                continue;
            }

            printf("Event observed: %s\n", message);

            NNotification = notify_notification_new(_basename, message, "dialog-information");
            err_shutdown_handler(NNotification == NULL, EXT_ERR_NOTIFY_HANDLER_NULL,
                                 "Err: got null notification handler!");

        }
    }

// Daemon do not exit
//    exit(EXT_SUCCESS)

}

void err_shutdown_handler(bool err_condition, int ext_code, char* msg) {
    if (err_condition) {
        fprintf(stderr, "%s\n", msg);
        exit(ext_code);
    }
}
