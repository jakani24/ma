#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define db_files 485
#define BUFFER_SIZE 1024

typedef struct
{
    char* prefix;
    FILE* file;
} OutputFile;
int download_files()
{
	char buf[500];
	for (int i=0;i<db_files;i++)
	{
		sprintf(buf,"curl https://virusshare.com/hashfiles/VirusShare_%.5d.md5 -o /var/www/html/server/db/%d.md5 --insecure",i,i);
		printf("downloading file %d = %.1f%% \n",i,(float)100/db_files*i);
		system(buf);
	}
	system("freshclam");
	system("cp /var/lib/clamav/main.cvd /var/www/html/server/db");
	system("cp /var/lib/clamav/daily.cvd /var/www/html/server/db");
	system("cd /var/www/html/server/db/ && sigtool -u /var/www/html/server/db/main.cvd");
	system("cd /var/www/html/server/db/ && sigtool -u /var/www/html/server/db/daily.cvd");
	return 0;
}
void sort_hashes_into_files(char* input_filename, char* output_filename_suffix, int hash_length)
{
    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "Error: Could not open input file \"%s\"\n", input_filename);
        return;
    }

    OutputFile output_files[BUFFER_SIZE];
    int output_file_count = 0;

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, input_file) != NULL)
    {
        buffer[strcspn(buffer, "\r\n")] = '\0'; // Remove newline characters

        char hash[BUFFER_SIZE];
        strncpy(hash, buffer, hash_length);
        hash[hash_length] = '\0';

        char output_filename_prefix[BUFFER_SIZE];
        strcpy(output_filename_prefix, hash);

        char output_filename[BUFFER_SIZE];
        strcpy(output_filename,"/var/www/html/server/");
        strcat(output_filename, output_filename_prefix);
        strcat(output_filename, output_filename_suffix);

        FILE* output_file = NULL;
        bool output_file_found = false;
        for (int i = 0; i < output_file_count; ++i)
        {
            if (strcmp(output_files[i].prefix, output_filename_prefix) == 0)
            {
                output_file = output_files[i].file;
                output_file_found = true;
                break;
            }
        }

        if (!output_file_found)
        {
            output_file = fopen(output_filename, "a");
            if (output_file == NULL)
            {
                fprintf(stderr, "Error: Could not open output file \"%s\"\n", output_filename);
                return;
            }

            OutputFile new_output_file;
            new_output_file.prefix = strdup(output_filename_prefix);
            new_output_file.file = output_file;
            output_files[output_file_count++] = new_output_file;
        }

        fprintf(output_file, "%s\n", buffer);
    }

    for (int i = 0; i < output_file_count; ++i)
    {
        fclose(output_files[i].file);
        free(output_files[i].prefix);
    }

    fclose(input_file);
}
int main(int argc, char *argv[])
{
	char path[500];
	if(argc==2)
	{
 	   if(strcmp(argv[1],"d")==0)
 	   {
 	           printf("Downloading files\n");
 	           return download_files();
 	           //printf("Please add the main.hsb and the daily.hsb files into the folder /var/www/html/server and then run the updater with \"u\"\n");
 	   }
 	   else if(strcmp(argv[1],"u")==0){
 	   		printf("generating own.jdbf\n");
			sort_hashes_into_files("/var/www/html/server/own.jdbf", ".jdbf", 2);
			printf("generating daily.hsb\n");
			sort_hashes_into_files("/var/www/html/server/db/daily.hsb", ".jdbf", 2);
			printf("generating main.hsb\n");
			sort_hashes_into_files("/var/www/html/server/db/main.hsb", ".jdbf", 2);
			printf("generating md5 files\n");
			
			for(int i=0;i<db_files;i++)
			{
				sprintf(path,"/var/www/html/server/db/%d.md5",i);
				printf("creating database file(%.1f%%) \n",(float)100/db_files*i);
				sort_hashes_into_files(path, ".jdbf", 2);
			}
			
			
		}
	}
	else
		printf("no parameter supplied\n");

	return 0;
}