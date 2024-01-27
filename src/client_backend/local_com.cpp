#ifndef LOCAL_COM_CPP
#define LOCAL_COM_CPP
#include "local_com.h"

int check_for_com_tasks(const char* com_name, const char* com_path) {
	//check for new tasks in com file and add them into the queue
	FILE* fp=nullptr;
	char* command=new char [300];
	if ((fopen_s(&fp,com_path, "r")) != 0) {
		//no com file found = no communication needed
		delete[] command;
		return 1;
	}
	else {
		while (!feof(fp)) {
			fscanf_s(fp, "%s", command, 295); // get the command
			if (!feof(fp)) {								//last line =\n we dont want to process that
				//we scanned the command, now we will scan the path. If the process does not need a path the path will be nopath. (set from client)
				//get a full path. => " = start, second "= end
				char* path = new char[300];
				path[0] = '\0';
				//search for starting ", then loop until ending "
				int cnt = 0;
				int chr = 0;
				fgetc(fp);
				fgetc(fp);
				while (cnt < 295 && chr != '\"') {
					chr = fgetc(fp); //get a char
					if(chr!='\"')
						path[cnt] = chr;
					path[cnt+1] = '\0';
					cnt++;
				}
				//printf("%s\n", path);
				//we now have the command and the path. Now we will add it to the queue
				char* queue_entry = new char[300*2+5];						//to enshure we have enough space
				queue_entry[0] = '\0';
				strcpy_s(queue_entry,600, command);							//copy the command
				strcat_s(queue_entry, 600, ";");							//add a ; to seperate command and path
				strcat_s(queue_entry, 600, path);							//add the path

				queue_push(queue_entry);
				//(queue_entry);
				delete[] queue_entry;
				delete[] path;
			}
		}
	}

	remove(com_path);
	fclose(fp);
	delete[] command;
	return 0;
}
#endif // !LOCAL_COM_CPP