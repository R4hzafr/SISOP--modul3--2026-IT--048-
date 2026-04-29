#include "protocol.h"
#include <time.h>
#include <sys/wait.h>

void log_event(char *tag, char *msg) {
    FILE *f = fopen("history.log", "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] [%s]\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec, tag, msg);
    fclose(f);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    log_event("System", "SERVER ONLINE");
    printf("[System] Wired is active...\n");

    while(1) {
        int new_sock = accept(server_fd, NULL, NULL);
        if (fork() == 0) { // Multi-client (Poin 1 & 3)
            char name[50], pass[50], cmd[BUFFER_SIZE];
            read(new_sock, name, 50);

            if (strcmp(name, "The Knights") == 0) {
                read(new_sock, pass, 50);
                if (strcmp(pass, "protocol7") == 0) {
                    send(new_sock, "AUTH_ADMIN", 10, 0);
                    log_event("System", "User 'The Knights' connected");
                    while(read(new_sock, cmd, BUFFER_SIZE) > 0) {
                        char log_msg[100]; sprintf(log_msg, "RPC_COMMAND_%s", cmd);
                        log_event("Admin", log_msg);
                    }
                }
            } else {
                send(new_sock, "AUTH_USER", 9, 0);
                char conn_msg[100]; sprintf(conn_msg, "User '%s' connected", name);
                log_event("System", conn_msg);
                while(read(new_sock, cmd, BUFFER_SIZE) > 0) {
                    char chat_msg[200]; sprintf(chat_msg, "[%s]: %s", name, cmd);
                    log_event("User", chat_msg);
                    // Sesuai poin 5, di sini harusnya ada logika broadcast ke user lain
                }
            }
            exit(0);
        }
        close(new_sock);
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
    return 0;
}