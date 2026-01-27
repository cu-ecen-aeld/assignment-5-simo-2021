/*
 * Serveur Socket TCP Simplifié
 * Objectif : Expliquer la logique de base (réception → écriture → renvoi)
 * Auteur : Exemple pour Arnaud
 * Port : 9000
 * Fichier de stockage : /tmp/simple_socket_data.txt
 */

// Headers essentiels (pas de daemon/signaux compliqués)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Définitions simples
#define PORT 9000
#define BUFFER_SIZE 1024
#define FILE_PATH "/tmp/simple_socket_data.txt"

int main() {
    // 1. Initialisation des variables
    int server_fd;          // Descripteur du socket serveur
    int client_fd;          // Descripteur du socket client
    struct sockaddr_in server_addr; // Adresse du serveur
    struct sockaddr_in client_addr; // Adresse du client
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = {0}; // Buffer pour recevoir les données
    ssize_t bytes_read;             // Nombre d'octets lus

    // 2. Création du socket (TCP/IPv4)
    // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = protocole par défaut
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erreur création socket");
        return EXIT_FAILURE;
    }

    // 3. Configuration de l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr)); // Initialiser à zéro
    server_addr.sin_family = AF_INET;             // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;     // Écouter sur toutes les interfaces
    server_addr.sin_port = htons(PORT);           // Port 9000 (format réseau)

    // 4. Liaison du socket au port (bind)
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur liaison socket (bind)");
        close(server_fd);
        return EXIT_FAILURE;
    }

    // 5. Mise en écoute du socket (listen)
    // 1 = nombre max de connexions en attente (suffisant pour le test)
    if (listen(server_fd, 1) == -1) {
        perror("Erreur écoute socket (listen)");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("✅ Serveur démarré sur le port %d\n", PORT);
    printf("🔍 Attente d'une connexion client...\n");

    // 6. Accepter une connexion client (bloquant jusqu'à connexion)
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == -1) {
        perror("Erreur acceptation connexion");
        close(server_fd);
        return EXIT_FAILURE;
    }

    // Afficher l'adresse IP du client
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("🔌 Connexion acceptée depuis %s\n", client_ip);

    // 7. Lire les données du client (jusqu'à la fin de ligne \n)
    printf("📥 Attente de données du client...\n");
    bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1); // Lire dans le buffer
    if (bytes_read == -1) {
        perror("Erreur lecture données client");
        close(client_fd);
        close(server_fd);
        return EXIT_FAILURE;
    }

    // Ajouter un caractère de fin de chaîne (sécurité)
    buffer[bytes_read] = '\0';
    printf("📩 Données reçues : %s", buffer);

    // 8. Écrire les données dans le fichier
    FILE *file = fopen(FILE_PATH, "a"); // "a" = mode ajout (append)
    if (file == NULL) {
        perror("Erreur ouverture fichier");
        close(client_fd);
        close(server_fd);
        return EXIT_FAILURE;
    }
    fprintf(file, "%s", buffer); // Écrire les données dans le fichier
    fclose(file);
    printf("💾 Données écrites dans %s\n", FILE_PATH);

    // 9. Renvoyer le contenu du fichier au client
    printf("📤 Envoi du fichier au client...\n");
    file = fopen(FILE_PATH, "r"); // "r" = mode lecture
    if (file == NULL) {
        perror("Erreur lecture fichier");
        close(client_fd);
        close(server_fd);
        return EXIT_FAILURE;
    }

    // Lire le fichier par blocs et envoyer au client
    char file_buffer[BUFFER_SIZE];
    ssize_t file_bytes;
    while ((file_bytes = fread(file_buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_fd, file_buffer, file_bytes, 0); // Envoyer les données
    }
    fclose(file);
    printf("✅ Contenu du fichier envoyé au client\n");

    // 10. Nettoyage et fermeture
    close(client_fd);  // Fermer la connexion client
    close(server_fd);  // Fermer le socket serveur
    printf("🔌 Connexion fermée\n");

    // Supprimer le fichier (optionnel, comme dans ton code)
    if (remove(FILE_PATH) == 0) {
        printf("🗑️ Fichier %s supprimé\n", FILE_PATH);
    } else {
        perror("Erreur suppression fichier");
    }

    return EXIT_SUCCESS;
}
