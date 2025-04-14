#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define FILE_NAME      "games.txt"
#define MAX_GAMES      100
#define MAX_MEMORY_GB  1024  

					// Colors

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RED     "\x1b[31m"

					// Structures

typedef struct {
    char name[100];
    int size;  
} Game;

					// Functions

void clearScreen() {
    system("cls || clear");
}

void pause() {
    printf("Press ENTER to continue...");
    getchar();
    getchar(); 
    clearScreen();
}

int parseSize(char *input) {
    int size = 0;
    char unit[5];

    sscanf(input, "%d%2s", &size, unit);
    for (int i = 0; unit[i]; i++) unit[i] = tolower(unit[i]);

    if (strcmp(unit, "gb") == 0) return size;
    if (strcmp(unit, "mb") == 0) return size / 1024;
    if (strcmp(unit, "tb") == 0) return size * 1024;

    return -1; 
}

					// File Handling

int loadGames(Game games[]) {
    FILE *file = fopen(FILE_NAME, "r");
    int count = 0;

    if (!file) return 0;

    while (fscanf(file, "%99[^0-9] %dGB\n", games[count].name, &games[count].size) == 2) {
        count++;
    }

    fclose(file);
    return count;
}

void saveGames(Game games[], int count) {
    FILE *file = fopen(FILE_NAME, "w");

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s %dGB\n", games[i].name, games[i].size);
    }

    fclose(file);
}

					// Memory 

int totalUsedMemory(Game games[], int count) {
    int total = 0;

    for (int i = 0; i < count; i++) {
        total += games[i].size;
    }

    return total;
}

					// Viewing

void viewGames(Game games[], int count) {
    printf(COLOR_YELLOW "\nInstalled Games:\n" COLOR_RESET);
    printf("--------------------------\n");

    if (count == 0) {
        printf(COLOR_RED "No games installed.\n" COLOR_RESET);
    } else {
        for (int i = 0; i < count; i++) {
            printf("%d. " COLOR_GREEN "%s" COLOR_RESET " - " COLOR_CYAN "%dGB\n" COLOR_RESET, 
                   i + 1, games[i].name, games[i].size);
        }
    }

    printf("--------------------------\n");
    printf("Total used space: %dGB / %dGB\n", totalUsedMemory(games, count), MAX_MEMORY_GB);

    pause();
}

					// Game Management

void installGame(Game games[], int *count) {
    char name[100], sizeInput[20];
    int size;
    int used = totalUsedMemory(games, *count);

    clearScreen();
    printf("\n--- Install Game ---\n");
    printf("Available space: " COLOR_CYAN "%dGB / %dGB\n" COLOR_RESET, MAX_MEMORY_GB - used, MAX_MEMORY_GB);

    printf("Enter " COLOR_GREEN "game name" COLOR_RESET ": ");
    getchar(); 
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0'; 

    printf("Enter " COLOR_CYAN "game size (e.g., 50GB, 200MB, 1TB)" COLOR_RESET ": ");
    fgets(sizeInput, sizeof(sizeInput), stdin);
    sizeInput[strcspn(sizeInput, "\n")] = '\0';

    size = parseSize(sizeInput);
    if (size <= 0) {
        printf(COLOR_RED "Invalid size format!\n" COLOR_RESET);
        pause();
        return;
    }

    if (used + size > MAX_MEMORY_GB) {
        printf(COLOR_RED "Not enough memory to install this game.\n" COLOR_RESET);
        pause();
        return;
    }

    strcpy(games[*count].name, name);
    games[*count].size = size;
    (*count)++;

    saveGames(games, *count);

    printf(COLOR_GREEN "Game installed successfully!\n" COLOR_RESET);
    printf("Remaining space: " COLOR_CYAN "%dGB\n" COLOR_RESET, MAX_MEMORY_GB - totalUsedMemory(games, *count));
    pause();
}

void uninstallGame(Game games[], int *count) {
    viewGames(games, *count);
    if (*count == 0) return;

    int index;
    printf("Enter the number of the game to uninstall: ");
    scanf("%d%*c", &index);  

    if (index < 1 || index > *count) {
        printf(COLOR_RED "Invalid game number.\n" COLOR_RESET);
        pause();
        return;
    }

    for (int i = index - 1; i < *count - 1; i++) {
        games[i] = games[i + 1];
    }

    (*count)--;
    saveGames(games, *count);

    printf(COLOR_GREEN "Game uninstalled successfully!\n" COLOR_RESET);
    printf("Remaining space: " COLOR_CYAN "%dGB\n" COLOR_RESET, MAX_MEMORY_GB - totalUsedMemory(games, *count));
    pause();
}

					// Sorting

void sortGamesByName(Game games[], int count) {
    Game temp;

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(games[i].name, games[j].name) > 0) {
                temp = games[i];
                games[i] = games[j];
                games[j] = temp;
            }
        }
    }
}

void sortGamesBySize(Game games[], int count) {
    Game temp;

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (games[i].size > games[j].size) {
                temp = games[i];
                games[i] = games[j];
                games[j] = temp;
            }
        }
    }
}

					// Menus

void manageGames(Game games[], int *count) {
    int choice;

    do {
        printf("\n--- Manage Games ---\n");
        printf("1. Install Game\n");
        printf("2. Uninstall Game\n");
        printf("3. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: installGame(games, count); break;
            case 2: uninstallGame(games, count); break;
            case 3: break;
            default:
                printf(COLOR_RED "Invalid option.\n" COLOR_RESET);
                pause();
                break;
        }
    } while (choice != 3);
}

void sortMenu(Game games[], int count) {
    int choice;

    do {
        printf("\n--- Sort Games ---\n");
        printf("1. By Name\n");
        printf("2. By Size\n");
        printf("3. Back to Main Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                sortGamesByName(games, count);
                printf(COLOR_GREEN "Games sorted by name.\n" COLOR_RESET);
                pause();
                break;

            case 2:
                sortGamesBySize(games, count);
                printf(COLOR_GREEN "Games sorted by size.\n" COLOR_RESET);
                pause();
                break;

            case 3: break;

            default:
                printf(COLOR_RED "Invalid option.\n" COLOR_RESET);
                pause();
                break;
        }
    } while (choice != 3);
}

void mainMenu() {
    Game games[MAX_GAMES];
    int count = loadGames(games);
    int choice;

    do {
        printf("\n=== Game Manager ===\n");
        printf("1. Manage Games\n");
        printf("2. Sort Games\n");
        printf("3. View Installed Games\n");
        printf("0. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: manageGames(games, &count); break;
            case 2: sortMenu(games, count); break;
            case 3: viewGames(games, count); break;
            case 0: printf("Goodbye!\n"); break;
            default:
                printf(COLOR_RED "Invalid choice.\n" COLOR_RESET);
                pause();
                break;
        }
    } while (choice != 0);
}

					// Main

int main() {
    mainMenu();
    return 0;
}