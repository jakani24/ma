#ifndef PERMISSIONS_CPP
#define PERMISSIONS_CPP
#include "permissions.h"
/*
1 create file (as admin)
2 set file as read only (also as admin)
file cannot be deleted or modified by anyone. admin can delete

*/
/*
int main() {
    FILE* fp;
    fp = fopen("c:\\programdata\\jakach\\aa.txt", "w");
    fprintf(fp, "secure text");
    fclose(fp);
    chmod("c:\\programdata\\jakach\\aa.txt", _S_IREAD);
}

*/
int protect_file(char* path) {
    return _chmod(path, _S_IREAD);
}
int unprotect_file(char* path) {
	return _chmod(path, _S_IWRITE | _S_IREAD);
}

#endif