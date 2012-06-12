#include <windows.h>
#include <string.h>
#include <stdio.h>

void HandleFolder(char* folder, char recursive){
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError;
    strncat(folder, "\\*", 3);
    hFind = FindFirstFile(folder, &FindFileData);
    //printf("%s\n", folder);
    if (hFind == INVALID_HANDLE_VALUE){
        printf("Invalid file handle. Error is %u\n", GetLastError());
        exit (-1);
    } else {
        do {
            if (!strcmp(FindFileData.cFileName, ".") || !strcmp(FindFileData.cFileName, ".."))
                continue;
            if (recursive && FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                char nextFolder[MAX_PATH];
                strncpy(nextFolder, folder, strlen(folder)-1);
                nextFolder[strlen(folder)-1] = '\0';
                strcat(nextFolder, FindFileData.cFileName);
                HandleFolder(nextFolder, 1);
            }
            printf("File name is %s\n", FindFileData.cFileName);
        } while (FindNextFile(hFind, &FindFileData) != 0);
        dwError = GetLastError();
        FindClose(hFind);
        if (dwError != ERROR_NO_MORE_FILES){
            printf("FindNextFile error. Error is %u\n", dwError);
            exit(-1);
        }
    }
}

int main(int argc, char *argv[]){
    HandleFolder(argv[1], 1);
    return (0);
}