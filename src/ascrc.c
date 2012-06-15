#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include "crc.h"

const int BUF_SIZE = 1024;
const char APPENDIX[] = ":crc\0";
const char USAGE[] = "Usage: %s [check|write] [FILENAME|FOLDER] [-r]\n";

void HandleFolder(char *name, char recursive, void (*func)(char *)){
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError;
    hFind = FindFirstFile(name, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE){
    	printf("Invalid file handle. Error is %u\n", GetLastError());
        exit (-1);
    }
    if (! (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
        func(name);
        return;
    }
    strcat(name, "\\*");
    hFind = FindFirstFile(name, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE){
        if (hFind == INVALID_HANDLE_VALUE){
            printf("Invalid file handle. Error is %u\n", GetLastError());
            exit (-1);
        }
    }
    do {
        if (!strcmp(FindFileData.cFileName, ".") || !strcmp(FindFileData.cFileName, ".."))
            continue;
        char nextName[MAX_PATH];
        strncpy(nextName, name, strlen(name)-1);
        nextName[strlen(name)-1] = '\0';
        strcat(nextName, FindFileData.cFileName);
        if (recursive && FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
            HandleFolder(nextName, 1, func);
        } else {
            func(nextName);
        }
    } while (FindNextFile(hFind, &FindFileData) != 0);
    dwError = GetLastError();
    FindClose(hFind);
    if (dwError != ERROR_NO_MORE_FILES){
        printf("FindNextFile error. Error is %u\n", dwError);
        exit(-1);
    }
}

unsigned long FileCRC(char *filename){
    FILE *F;
    F = fopen(filename, "rb");
    if (F != NULL){
		unsigned char buf[BUF_SIZE];
		unsigned long crc = 0;
		int cnt;
		do{
			cnt = fread(buf, 1, BUF_SIZE, F);
			crc = crc_cycle(crc, buf, cnt);
		} while(cnt == BUF_SIZE);
		fclose(F);
		return crc;
	} else {
		printf("Unable to open %s\n", filename);
        exit(-1);
    }
}

void WriteCRC(char *filename){
    char streamFilename[MAX_PATH];
    FILE *F;
    strcpy(streamFilename, filename);
    strcat(streamFilename, APPENDIX);
    F = fopen(streamFilename, "wb");
    if (F != NULL){
        unsigned long crc = FileCRC(filename);
        int cnt;
        cnt = fwrite(&crc, sizeof(crc), 1, F);
        fclose(F);
        if (!cnt){
            printf("Unable to write to %s\n", streamFilename);
            exit(-1);
        }
    } else {
        printf("Unable to open %s\n", streamFilename);
        exit(-1);
    }
}

void CheckCRC(char *filename){
    char streamFilename[MAX_PATH];
    FILE *F;
    strcpy(streamFilename, filename);
    strcat(streamFilename, APPENDIX);
    F = fopen(streamFilename, "rb");
    if (F != NULL){
        unsigned long realCrc = FileCRC(filename);
        unsigned long crc;
        int cnt;
        cnt = fread(&crc, sizeof(crc), 1, F);
        fclose(F);
        if (!cnt){
            printf("%s has no CRC in stream\n", filename);
        } else {
            if (crc != realCrc){
                printf("%s changed\n", filename);
            }
        }        
    } else {
        printf("%s changed\n", filename);
    }
}

int main(int argc, char* argv[]){
    setlocale(LC_ALL, "Russian_Russia");
    crc_init();
    if (argc > 1){
        void (*f)(char *);
        char recursive = 0; 
        if (!strcmp(argv[1], "check"))
            f = CheckCRC;
        else if (!strcmp(argv[1], "write"))
            f = WriteCRC;
        else {
            printf(USAGE, argv[0]);
            return(0);
        }
        if (argc > 3 && !strcmp(argv[3], "-r"))
            recursive = 1;
        HandleFolder(argv[2], recursive, f);
    } else {
        printf(USAGE, argv[0]);
    }
    return (0);
}