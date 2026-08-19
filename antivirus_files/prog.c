#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "dirent.h"

#define TEMPLATE "Virus signature: "
#define BUFFER_SIZE 1024
#define MODE_NORMAL "Scanning option: \nNormal Scan \nResults:\n"
#define MODE_QUICK "Scanning option: \nQuick Scan \nResults:\n"
#define CLEAN "clean"
#define INFECTED "Infected!"
#define LAST "(last 20%)"
#define FIRST "(first 20%)"
#define EMPTY ""
#define WELCOME "Anti-virus began! Welcome!\n \nFolder to scan:"
#define LOG_PATH "AntiVirusLog.txt"
#define APPEND_MODE "a"
#define READ_BINARY_MODE "rb"
#define NEED_FILE_SIZE 3
#define FILE_PATH_PLACE 1
#define SIGNATURE_PLACE 2

int checkSignature(const char* buffer, size_t bufferSize, const char* signature, size_t signatureSize);
void logResult(const char* filePath, const char* status, const char* located);
int scanFile(const char* filePath, const char* virusSignature, size_t signatureSize, int quickScan);
int scanDirectory(const char* directoryPath, const char* virusSignature, size_t signatureSize, int quickScan);

static int joinPath(char* destination, size_t destinationSize, const char* directory, const char* name)
{
    int written = snprintf(destination, destinationSize, "%s/%s", directory, name);
    return written >= 0 && (size_t)written < destinationSize;
}

int main(int argc, char* argv[])
{
    FILE* sigFile = NULL;
    FILE* logFile = NULL;
    char* virusSignature = NULL;
    long signatureSizeLong = 0;
    size_t signatureSize = 0;
    int quickScan = 0;
    int result = 1;

    if (argc != NEED_FILE_SIZE) {
        fprintf(stderr, "Usage: %s <directory_path> <signature_file_path>\n", argv[0]);
        return 1;
    }

    sigFile = fopen(argv[SIGNATURE_PLACE], READ_BINARY_MODE);
    if (!sigFile) {
        fprintf(stderr, "Failed to open signature file\n");
        goto cleanup;
    }

    if (fseek(sigFile, 0, SEEK_END) != 0) {
        fprintf(stderr, "Failed to inspect signature file\n");
        goto cleanup;
    }

    signatureSizeLong = ftell(sigFile);
    if (signatureSizeLong <= 0) {
        fprintf(stderr, "Signature file must not be empty\n");
        goto cleanup;
    }
    signatureSize = (size_t)signatureSizeLong;
    rewind(sigFile);

    virusSignature = (char*)malloc(signatureSize);
    if (!virusSignature) {
        fprintf(stderr, "Failed to allocate signature buffer\n");
        goto cleanup;
    }

    if (fread(virusSignature, 1, signatureSize, sigFile) != signatureSize) {
        fprintf(stderr, "Failed to read signature file\n");
        goto cleanup;
    }
    fclose(sigFile);
    sigFile = NULL;

    printf("Press 0 for normal or any other integer for quick scan: ");
    if (scanf("%d", &quickScan) != 1) {
        fprintf(stderr, "Invalid scan mode\n");
        goto cleanup;
    }

    logFile = fopen(LOG_PATH, "w");
    if (!logFile) {
        fprintf(stderr, "Failed to open %s\n", LOG_PATH);
        goto cleanup;
    }

    fprintf(logFile, "%s%s\n%s%s\n", WELCOME, argv[FILE_PATH_PLACE], TEMPLATE, argv[SIGNATURE_PLACE]);
    fprintf(logFile, "%s", quickScan == 0 ? MODE_NORMAL : MODE_QUICK);
    fclose(logFile);
    logFile = NULL;

    printf("%s%s\n%s%s\n", WELCOME, argv[FILE_PATH_PLACE], TEMPLATE, argv[SIGNATURE_PLACE]);
    printf("%s", quickScan == 0 ? MODE_NORMAL : MODE_QUICK);

    if (!scanDirectory(argv[FILE_PATH_PLACE], virusSignature, signatureSize, quickScan)) {
        goto cleanup;
    }

    result = 0;

cleanup:
    if (sigFile) {
        fclose(sigFile);
    }
    if (logFile) {
        fclose(logFile);
    }
    free(virusSignature);
    return result;
}

void logResult(const char* filePath, const char* status, const char* located)
{
    FILE* logFile = fopen(LOG_PATH, APPEND_MODE);
    if (!logFile) {
        fprintf(stderr, "Failed to open %s\n", LOG_PATH);
        return;
    }

    fprintf(logFile, "%s %s %s\n", filePath, status, located);
    printf("%s %s %s\n", filePath, status, located);
    fclose(logFile);
}

int scanDirectory(const char* directoryPath, const char* virusSignature, size_t signatureSize, int quickScan)
{
    DIR* dir = opendir(directoryPath);
    struct dirent* entry;

    if (!dir) {
        fprintf(stderr, "Failed to open directory: %s\n", directoryPath);
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[BUFFER_SIZE];

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (!joinPath(path, sizeof(path), directoryPath, entry->d_name)) {
            fprintf(stderr, "Path too long; skipping: %s/%s\n", directoryPath, entry->d_name);
            continue;
        }

        if (entry->d_type == DT_DIR) {
            if (!scanDirectory(path, virusSignature, signatureSize, quickScan)) {
                closedir(dir);
                return 0;
            }
        } else {
            if (!scanFile(path, virusSignature, signatureSize, quickScan)) {
                closedir(dir);
                return 0;
            }
        }
    }

    closedir(dir);
    return 1;
}

int scanFile(const char* filePath, const char* virusSignature, size_t signatureSize, int quickScan)
{
    FILE* file = fopen(filePath, READ_BINARY_MODE);
    char* buffer = NULL;
    long fileSizeLong;
    size_t fileSize;
    int infected = 0;

    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filePath);
        return 0;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return 0;
    }

    fileSizeLong = ftell(file);
    if (fileSizeLong < 0) {
        fclose(file);
        return 0;
    }
    fileSize = (size_t)fileSizeLong;
    rewind(file);

    if (fileSize == 0) {
        fclose(file);
        logResult(filePath, CLEAN, EMPTY);
        return 1;
    }

    buffer = (char*)malloc(fileSize);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Failed to allocate file buffer\n");
        return 0;
    }

    if (fread(buffer, 1, fileSize, file) != fileSize) {
        fclose(file);
        free(buffer);
        fprintf(stderr, "Failed to read file: %s\n", filePath);
        return 0;
    }
    fclose(file);

    if (quickScan == 0) {
        infected = checkSignature(buffer, fileSize, virusSignature, signatureSize);
        if (infected) {
            logResult(filePath, INFECTED, EMPTY);
        }
    } else {
        size_t edgeSize = fileSize / 5;
        if (edgeSize < signatureSize) {
            edgeSize = fileSize;
        }

        if (checkSignature(buffer, edgeSize, virusSignature, signatureSize)) {
            infected = 1;
            logResult(filePath, INFECTED, FIRST);
        } else if (edgeSize < fileSize &&
                   checkSignature(buffer + (fileSize - edgeSize), edgeSize, virusSignature, signatureSize)) {
            infected = 1;
            logResult(filePath, INFECTED, LAST);
        } else {
            size_t middleStart = edgeSize;
            size_t middleSize = fileSize > 2 * edgeSize ? fileSize - 2 * edgeSize : 0;
            if (checkSignature(buffer + middleStart, middleSize, virusSignature, signatureSize)) {
                infected = 1;
                logResult(filePath, INFECTED, EMPTY);
            }
        }
    }

    if (!infected) {
        logResult(filePath, CLEAN, EMPTY);
    }

    free(buffer);
    return 1;
}

int checkSignature(const char* buffer, size_t bufferSize, const char* signature, size_t signatureSize)
{
    size_t i;

    if (!buffer || !signature || signatureSize == 0 || bufferSize < signatureSize) {
        return 0;
    }

    for (i = 0; i <= bufferSize - signatureSize; ++i) {
        if (memcmp(buffer + i, signature, signatureSize) == 0) {
            return 1;
        }
    }

    return 0;
}
