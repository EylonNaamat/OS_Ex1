#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT_NUM 10000

void do_Echo(char* splite_word)
{
    splite_word = strtok(NULL," ");
    while(splite_word!=NULL)
    {
        printf("%s ",splite_word);
        splite_word = strtok(NULL," ");
    }
}

void show_dir()
{
    DIR* thisdir = opendir(".");
    if(thisdir!=NULL)
    {
        struct dirent* dir_file = readdir(thisdir);
        while (dir_file!=NULL)
        {
            printf("%s\n",dir_file->d_name);
            dir_file = readdir(thisdir);
        }
        closedir(thisdir);
    }
}

void do_copy(char* splite_word)
{
    splite_word = strtok(NULL," ");
    FILE* copy_file = fopen(splite_word,"r");
    if(copy_file == NULL)
    {
        printf("not such file");
        return;
    }
    splite_word = strtok(NULL," ");
    FILE* past_file = fopen(splite_word,"w");  // fopen, fread and fwrite are all library functions (<stdio.h>)

    int buff_read_size = 30;
    char readen[buff_read_size];
    while(getc(copy_file)!= EOF)
    {
        fseek(copy_file,-1,SEEK_CUR);
        fread(readen,buff_read_size+1,1,copy_file);  // fopen, fread and fwrite are all library functions (<stdio.h>)
        fwrite(readen,buff_read_size+1,1,past_file);  // fopen, fread and fwrite are all library functions (<stdio.h>)
    }
    fclose(copy_file);
    fclose(past_file);
}

int main()
{
    int min_Size = 15;
    char* get_command;
    if((get_command = (char*)malloc((min_Size+1)*sizeof(char)))==NULL)
    {
        printf("fail malloc memory");
        return -1;
    }
    strcpy(get_command, "welcome");
    int flage = 1;
    int tcp_sock = 0;
    int old_out = dup(1);

    while(strcmp(get_command,"EXIT"))
    {
        free(get_command);
        if((get_command = (char*)malloc((min_Size+1)*sizeof(char)))==NULL)
        {
            printf("fail malloc memory");
            return -1;
        }
        char my_dir[1000];

        getcwd(my_dir, 1000);
        printf("\n %s",my_dir);
        printf(">>");
        char got_char;
        int place = 0;

        scanf("%c",&got_char);
        while((got_char!=EOF) && (got_char != '\n'))
        {
            if(place == min_Size)
            {
                min_Size = min_Size*2;
                if((get_command = (char*)realloc(get_command,(min_Size+1)*sizeof(char)))==NULL)
                {
                    printf("fail malloc memory");
                    return -1;
                }
            }
            *(get_command+place) = got_char;
            ++place;
            scanf("%c",&got_char);
        }

        *(get_command+place) = '\0';
        char* copy_command;
        if((copy_command = (char*)malloc(strlen(get_command)+1))==NULL)
        {
            printf("fail malloc memory");
            return -1;
        }
        strcpy(copy_command,get_command);
        char* splite_word = strtok(get_command," ");

        if(splite_word != NULL)
        {
            if(!(strcmp(splite_word,"ECHO")))
            {
                do_Echo(splite_word);
            }
            else if(!(strcmp(splite_word,"TCP")))
            {
                splite_word = strtok(NULL," ");
                if(!(strcmp(splite_word,"PORT")))
                {
                    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
                    if(tcp_sock == -1){
                        printf("%s", "socket creation failed");
                        return -1;
                    }

                    struct sockaddr_in server_ad;
                    memset(&server_ad, 0, sizeof(server_ad));
                    server_ad.sin_family = AF_INET;
                    server_ad.sin_port = htons(PORT_NUM);

                    if(connect(tcp_sock, (struct sockaddr *)(&server_ad), sizeof(server_ad)) == -1){
                        printf("%s", "connection failed");
                        return -1;
                    }
                    dup2(tcp_sock,1);
                }
            }
            else if(!(strcmp(splite_word,"LOCAL")))
            {
                dup2(old_out,1);
                close(tcp_sock);
                tcp_sock = 0;
            }
            else if(!(strcmp(splite_word,"DIR")))
            {
                show_dir();
            }
            else if(!(strcmp(splite_word,"CD")))
            {
                splite_word = strtok(NULL," ");
                chdir(splite_word); // chdir is a system call (function)
            }
            else if(!(strcmp(splite_word,"COPY")))
            {
                do_copy(splite_word);
            }
            else if(!(strcmp(splite_word,"DELETE")))
            {
                splite_word = strtok(NULL," ");
                printf("deletd %s",splite_word);
                unlink(splite_word);  // unlink is a system call (function)
            }
            else
            {
                //system(copy_command);  // system is a library function (<stdlib.h>)
                int pros_id = fork();
                if(pros_id < 0 )
                {
                    printf("fail allocate");
                    return -1;
                }
                if(pros_id > 0)
                {
                    wait(NULL);
                }
                else
                {
                    int count =0;
                    while(splite_word!=NULL)
                    {
                        ++count;
                        splite_word = strtok(NULL," ");
                    }

                    char** my_arg;

                    if((my_arg = (char**)malloc((count+1)*sizeof(char*)))==NULL)
                    {
                        printf("fail malloc memory");
                        return -1;
                    }
                    count = 0;
                    splite_word = strtok(copy_command," ");
                    while(splite_word!=NULL)
                    {
                        if((my_arg[count] = (char*)malloc((strlen(splite_word)+1)*sizeof(char)))==NULL)
                        {
                            printf("fail malloc memory");
                            return -1;
                        }
                        strcpy(my_arg[count],splite_word);
                        my_arg[count][strlen(splite_word)]=='\0';
                        count++;
                        splite_word = strtok(NULL," ");
                    }
                    my_arg[count]=NULL;
                    execvp(my_arg[0],my_arg);
                    for(int i = 0; i<=count;i++)
                    {
                        free(my_arg[i]);
                    }
                    free(my_arg);
                }
            }
        }
        free(copy_command);
        min_Size = 15;

    }
    free(get_command);
    return 0;
}