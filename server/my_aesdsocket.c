/*
 * Serveur Socket ULTRA SIMPLE
 * Objectif : Comprendre le cycle socket() → bind() → listen() → accept() → read()
 * Test : nc localhost 9000 → tape une chaîne → appuie sur Entrée
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main() {
    // 1. Variables ESSENTIELLES (pas plus)
    int server_fd;          // Descripteur du socket serveur
    int client_fd;          // Descripteur du client connecté
    struct sockaddr_in addr;// Adresse du serveur
    char buffer[BUFFER_SIZE] = {0}; // Buffer pour recevoir les données

    // 2. Créer le socket (TCP/IPv4)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erreur création socket");
        return 1;
    }

    // 3. Configurer l'adresse (IPv4, port 9000, toutes interfaces)
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    // 4. Lier le socket au port 9000
    int lier = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));  
    printf("Résultat de bind() : %d\n", lier); // Affiche -1 si échec, 0 si succès      
    if (lier == -1) {
        perror("Erreur bind");
        close(server_fd);
        return 1;
    }

    // 5. Mettre le socket en écoute (attendre clients)
    if (listen(server_fd, 1) == -1) {
        perror("Erreur listen");
        close(server_fd);
        return 1;
    }

    printf("✅ Serveur prêt sur le port %d\n", PORT);
    printf("🔍 Attente d'un client (lancez : nc localhost 9000)\n");

    // 6. Accepter la connexion client (bloquant jusqu'à connexion)
    client_fd = accept(server_fd, NULL, NULL); // On ignore l'adresse du client (simplicité)
    if (client_fd == -1) {
        perror("Erreur accept");
        close(server_fd);
        return 1;
    }

    printf("🔌 Client connecté !\n");

    // 7. Lire les données envoyées par le client
    read(client_fd, buffer, BUFFER_SIZE - 1); // Lire dans le buffer
    printf("📩 Données reçues du client : %s\n", buffer);

    // 8. Nettoyage (fermer les connexions)
    close(client_fd);
    close(server_fd);
    printf("🔌 Connexion fermée\n");

    return 0;
}
