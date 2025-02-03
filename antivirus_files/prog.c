/*********************************
* Name:Dorian                    *
* Credits: Dorian Salomon        *
**********************************/

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
#define TRUE 1
#define FALSE !TRUE
#define NEED_FILE_SIZE 3
#define SMART_CUT 5
#define MIDDAL_PRES 3
int checkSignature(char* buffer, int bufferSize, char* signature, int signatureSize);
void logResult(char* file_path, char* status, char* located);
void scanFile(char* filePath, char* virusSignature, int signatureSize, int quickScan);




int main(int argc, char* argv[]) 
{
    if (argc != NEED_FILE_SIZE) {
        printf("Not enough parameters");
        printf("Usage: %s <directory_path> <signature_file_path>\n", argv[0]);
        return 1;
    }
    FILE* sigFile;
    int signatureSize = 0, i = 0, quickScan = 0;
    DIR* dir;
    char* virusSignature;
    char* directoryPath;
    char* signatureFilePath;
    struct dirent* entry;
    char filePath[BUFFER_SIZE];
    char tempFilePath[BUFFER_SIZE];
    FILE* logFile = fopen(LOG_PATH, APPEND_MODE);
    if (!logFile) {
        printf("Failed to open AntiVirusLog file");
        return 1;
    }

    directoryPath = argv[1];
    signatureFilePath = argv[2];

    sigFile = fopen(signatureFilePath, READ_BINARY_MODE);
    if (!sigFile) {
        printf("Failed to open signature file");
        return 1;
    }

    fseek(sigFile, 0, SEEK_END);
    signatureSize = ftell(sigFile);
    fseek(sigFile, 0, SEEK_SET);

    virusSignature = (char*)malloc(signatureSize);
    if (!virusSignature) {
        printf("Failed to allocate memory for virus signature\n");
        fclose(sigFile);
        return 1;
    }

    for (i = 0; i < signatureSize; i++) {
        virusSignature[i] = fgetc(sigFile);
    }
    fclose(sigFile);

    dir = opendir(directoryPath);
    if (!dir) {
        printf("Failed to open directory");
        free(virusSignature);
        return 1;
    }
    strcpy(filePath, directoryPath);
    strcat(filePath, "/");
    printf("Press 0 for normal or press any other key for quick scan: ");
    scanf("%d", &quickScan);
    getchar();

    fprintf(logFile, "%s", WELCOME);
    
    fprintf(logFile, "%s\n",argv[1]);
    fprintf(logFile, "%s\n",TEMPLATE);
    fprintf(logFile, "%s\n",argv[2]);
    printf("%s %s\n%s %s\n", WELCOME, argv[1], TEMPLATE, argv[2]);


    if (quickScan == 0) {
        fprintf(logFile, "%s", MODE_NORMAL);
        printf("%s \n", MODE_NORMAL);
    }
    else {
        fprintf(logFile, "%s", MODE_QUICK);
        printf("%s \n", MODE_QUICK);
    }
    fclose(logFile);

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            strcpy(tempFilePath, filePath);
            strcat(tempFilePath, "/");
            strcat(tempFilePath, entry->d_name);
            scanFile(tempFilePath, virusSignature, signatureSize, quickScan);
        }
    }

    closedir(dir);
    free(virusSignature);

    getchar();
    return 0;
}


/*
func will write the result of the scan
input:char* file_path, char* status, char* located
output:none
*/
void logResult(char* file_path, char* status, char* located) {
    FILE* logFile = fopen(LOG_PATH, APPEND_MODE);
    if (logFile) {
        fprintf(logFile, "%s %s %s\n", file_path, status, located);
        printf("%s %s %s\n", file_path, status, located);
        fclose(logFile);
    }
    else
    {
        printf("Failed to open file");
        exit(1);
    }
}

/*
func will scan the file
input:char* filePath, char* virusSignature, int signatureSize, int quickScan
output:none
*/
void scanFile(char* filePath, char* virusSignature, int signatureSize, int quickScan) {
    int i = 0,portionSizeLast = 0,portionSize = 0, fileSize = 0, infected = 0;
    char* buffer;
    FILE* file = fopen(filePath, READ_BINARY_MODE);//for binary scan
    if (!file) {
        printf("Failed to open file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = (char*)malloc(fileSize);
    if (!buffer) {
        printf("Failed to allocate memory for buffer\n");
        exit(1);
    }

    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        printf("Error reading file\n");
        fclose(file);
        free(buffer);
        exit(1);
    }

    fclose(file);

    if (quickScan != 0) {
        portionSize = fileSize / SMART_CUT;
        portionSizeLast = MIDDAL_PRES * portionSize;
        if (checkSignature(buffer, portionSize + signatureSize - 1, virusSignature, signatureSize)) {
            infected = 1;
            logResult(filePath, INFECTED, FIRST);
        }
        else if (checkSignature(buffer + (fileSize - portionSize), portionSize, virusSignature, signatureSize)) {
            infected = 1;
            logResult(filePath, INFECTED, LAST);
        }
        else {
            if (checkSignature(buffer + portionSize, portionSizeLast + signatureSize - 1, virusSignature, signatureSize)) {
                infected = 1;
                logResult(filePath, INFECTED, EMPTY);
            }
        }
    }
    else {
        if (checkSignature(buffer, fileSize, virusSignature, signatureSize)) {
            infected = 1;
            logResult(filePath, INFECTED, EMPTY);
        }
    }

    if (!infected) {
        logResult(filePath, CLEAN, EMPTY);
    }

    free(buffer);
}

/*
func will check the signature of the virus
input:char* buffer, int bufferSize, char* signature, int signatureSize
output:int
*/
int checkSignature(char* buffer, int bufferSize, char* signature, int signatureSize) {
    int i = 0, j = 0;
    bool flag = false;
    for (i = 0; i <= bufferSize - signatureSize; i++) 
    {
        flag = false;
        for (j = 0; j < signatureSize&&!flag ; j++) 
        {
            if (buffer[i + j] != signature[j]) 
            {
                flag = true;
            }
        }
        if (j == signatureSize) {
            return TRUE;
        }
    }
    return FALSE;
}