//
// Created by pclaoa on 2016/10/10.
//

#include "log.h"
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/system_properties.h>

#define MAX_LEN 2048

volatile int running;

char *string_cat(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char *out = (char *) malloc(len1 + len2 + 1);
    if (out == NULL) {
        return NULL;
    }
    strcpy(out, s1);
    strcat(out, s2);
    out[len1 + len2] = 0;
    return out;
}

int find_pids_by_name(const char *pname, int *pid_list, int *len) {
    int i = 0;
    DIR *dir;
    struct dirent *pentry;
    dir = opendir("/proc");
    if (dir == NULL) {
        return -1;
    }
    char proc_file_name[MAX_LEN];
    char buffer[MAX_LEN];
    char process_name[MAX_LEN];

    while (i < *len && (pentry = readdir(dir)) != NULL) {
        FILE *fp;
        if (!strcmp(pentry->d_name, "..")) {
            continue;
        }
        if (!isdigit(*pentry->d_name)) {
            continue;
        }
        sprintf(proc_file_name, "/proc/%s/cmdline", pentry->d_name);
        if ((fp = fopen(proc_file_name, "r")) == NULL) {
            continue;
        }
        if ((fgets(buffer, MAX_LEN - 1, fp)) == NULL) {
            fclose(fp);
            continue;
        }
        fclose(fp);
        sscanf(buffer, "%[^-]", process_name);
        if (!strcmp(process_name, pname)) {
            pid_list[i++] = atoi(pentry->d_name);
        }
    }

    *len = i;
    closedir(dir);
    return 0;
}

int get_version() {
    char value[8];
    __system_property_get("ro.build.version.sdk", value);
    return atoi(value);
}

/**
 * 睡眠mills毫秒
 */
void msleep(long mills){
    struct timeval v;
    long sec = mills / 1000; //秒
    v.tv_sec = sec;
    v.tv_usec = mills % 1000 * 1000; //微妙
    select(0, NULL, NULL, NULL, &v);
}

void start_service(const char *package_name, const char *service_name) {
    int version = get_version();

    LOGD("LEE", "version-->>%d", version);
    pid_t pid;
    if (package_name == NULL || service_name == NULL) {
        LOGE("LEE", "package or service name is null");
        return;
    }
    if ((pid = fork()) == 0) {
        char *pname = string_cat(package_name, "/");
        char *sname = string_cat(pname, service_name);
        LOGD("LEE", "service -->>%s", sname);

        if (version >= 17 || version == 0) {
            execlp("am", "am", "startservice", "--user", "0", "-n", sname, NULL);
        } else{
            execlp("am", "am", "startservice", "-n", sname, NULL);
        }

        exit(EXIT_SUCCESS);
    } else if(pid > 0){
        waitpid(pid, NULL, 0);
    }
}



void sig_term(int sig_no) {
    LOGE("LEE", "sig_chld");
    running = 0;
}

int main(int argc, char *argv[]) {
    pid_t pid;
    char *package_name;
    char *service_name;
    int interval;
    if(argc < 4) {
        LOGE("LEE", "usage: daemon <package_name> <service> <interval>");
        return -1;
    }
    package_name = argv[1];
    service_name = argv[2];
    interval = atoi(argv[3]);

    if((pid = fork()) == 0) {
        signal(SIGTERM, sig_term);
        setsid();
        chdir("/");
        int pid_list[200];
        int len = 200;
        if(find_pids_by_name(argv[0], pid_list, &len) == 0){
            int i;
            int ret;
            LOGD("LEE", "total number: %d", len);
            for(i = 0; i < len; i++){
                int daemon_pid = pid_list[i];
                if(daemon_pid > 1 && daemon_pid != getpid()){
                    ret = kill(daemon_pid, SIGTERM);
                    if(ret < 0) {
                        LOGE("LEE", "kill daemon process %d failed: %s", daemon_pid,
                             strerror(errno));
                    } else {
                        LOGE("LEE", "kill daemon process success: %d", daemon_pid);
                    }
                }
            }
        }

        running = 1;
        while(running) {
            msleep(interval);
            start_service(package_name, service_name);
        }
    } else if(pid > 0){
        exit(0);
    }

    return 0;
}
