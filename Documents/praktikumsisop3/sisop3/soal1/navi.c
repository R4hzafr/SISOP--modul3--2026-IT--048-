#include "protocol.h"
#include <pthread.h>

// Fungsi untuk dengerin broadcast/pesan dari server terus-menerus
void *listen_server(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE];
    while (read(sock, buffer, BUFFER_SIZE) > 0) {
        printf("\n%s\n> ", buffer);
        fflush(stdout);
        memset(buffer, 0, BUFFER_SIZE);
    }
    return NULL;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    char name[50], password[50], cmd[BUFFER_SIZE];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) return -1;

    printf("Enter your name: ");
    scanf(" %[^\n]s", name);
    send(sock, name, strlen(name), 0);

    // Sesuai poin 6: The Knights butuh password
    if (strcmp(name, "The Knights") == 0) {
        printf("Enter Password: ");
        scanf(" %[^\n]s", password);
        send(sock, password, strlen(password), 0);
    }

    char auth_res[BUFFER_SIZE] = {0};
    read(sock, auth_res, BUFFER_SIZE);

    if (strstr(auth_res, "AUTH_ADMIN")) {
        printf("\n[System] Authentication Successful. Granted Admin privileges.\n");
        printf("\n=== THE KNIGHTS CONSOLE ===\n1. Check Entities\n2. Uptime\n3. Shutdown\n4. Disconnect\n");
        
        while(1) {
            printf("Command >> ");
            scanf(" %[^\n]s", cmd);
            send(sock, cmd, strlen(cmd), 0);
            if (strcmp(cmd, "4") == 0) break;
        }
    } else {
        printf("\n--- Welcome to The Wired, %s ---\n", name);
        
        // Buat thread buat dengerin broadcast (Poin 2 & 5)
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, listen_server, &sock);

        while(1) {
            printf("> ");
            scanf(" %[^\n]s", cmd);
            if (strcmp(cmd, "/exit") == 0) break;
            send(sock, cmd, strlen(cmd), 0);
        }
    }

    close(sock);
    return 0;
}