#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFFER_SIZE 2048

char buf[BUFFER_SIZE] = {0};
char result[BUFFER_SIZE] = {0};
char polecenieorg[BUFFER_SIZE];
char cwd[BUFFER_SIZE];
char oldcwd[BUFFER_SIZE];
char localcwd[BUFFER_SIZE];

char* microshell(char polecenie[BUFFER_SIZE])
{
    int gdzienull;
    char *user=getenv("USER");
    char *home=getenv("HOME");
    char *arg[BUFFER_SIZE]= {};
    char cdarg[BUFFER_SIZE]= {};

    polecenie[strlen(polecenie)]='\n';
    strcpy(polecenieorg, polecenie);

    for(int i=0; i<BUFFER_SIZE; i++)
    {
        if(polecenie[i]=='\n')
        {
            polecenie[i]='\0';
            gdzienull=i;
            break;
        }
    }

    for(int i=0; i<gdzienull; i++)
    {
        if(polecenie[i]==' ')
        {
            i++;
            int j=0;
            while(i<gdzienull)
            {
                cdarg[j]=polecenie[i];
                i++;
                j++;
            }
            cdarg[j]='\0';
            break;
        }
    }

    for(int i=0; i<gdzienull; i++)            //Pętla kodu for kodu wzorowana na kodzie ze strony: https://cpp0x.pl/dokumentacja/standard-C/strtok/442
    {
        char korektor[] = " ";
        char * schowek;
        schowek = strtok(polecenie, korektor);
        for(int i=0; schowek!=NULL; i++)
        {
            arg[i]=schowek;
            schowek = strtok(NULL, korektor);
        }
    }

    if(strcmp(arg[0], "help")=='\0')
    {
        return "\n*** Remote MicroShell SIECI ***\nAutor: Bartosz Jackowiak\n\nMy implementation:\n'cd ~' || 'cd' - Home directory\n'cd -' - Previous directory\n'cd ..' - Parent directory\n'cd o' - Origin directory\n'cd /etc' - Absolute path\n'cd Downloads' - Relative path\n\n'Fork & exec':\n'ls -l -a' - 1 and more arguments\n'mkdir name' - Create a folder\n'rm -r name' - Remove a folder\n'date', 'echo'\n\n'exit' to shutdown server and client.\n";
    }
    else if(strcmp(arg[0], "path")=='\0')
    {
        return getcwd(cwd, sizeof(cwd));
    }
    else if(strcmp(arg[0], "cd")=='\0')
    {
        if(strcmp(cdarg, "-")=='\0')
        {
            chdir(oldcwd);
            return "";
        }
        else if(strcmp(cdarg, "~")=='\0')
        {
            getcwd(oldcwd,sizeof(oldcwd));
            chdir(home);
            return "";
        }
        else if(strcmp(cdarg, "o")=='\0')
        {
            getcwd(oldcwd,sizeof(oldcwd));
            chdir(localcwd);
            return "";
        }
        else
        {
            getcwd(oldcwd,sizeof(oldcwd));
            if(chdir(cdarg)!=0)
                fprintf(stderr, "cd: %s: %s\n",cdarg, strerror(errno));
            return "";
        }
    }
    else if(strcmp(arg[0], "ls")=='\0' || strcmp(arg[0], "rm")=='\0' || strcmp(arg[0], "mkdir")=='\0' || strcmp(arg[0], "date")=='\0' || strcmp(arg[0], "echo")=='\0')
    {
        pid_t pid;
        int fd[2];
        char buf[BUFFER_SIZE] = {0};
        pipe(fd);

        int signal=0;
        if(fork()==0)
        {
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            execvp(arg[0], arg);
            fprintf(stderr, "%s: command not found: Error code (%d)\n",arg[0], errno);
            return 0;
        }
        else
        {
            wait(&signal);
            close(fd[1]);

            read(fd[0], buf, BUFFER_SIZE);
            if(strcmp(buf, " "))
            {
                strcpy(polecenie, buf);
                memset(buf, 0, sizeof buf);
                return polecenie;
            }
        return "";
        }
    }
    else
        return "Command not found. Type 'help'.";
}

//argv[1] - numer portu
int main(int argc, char **argv)
{
    struct sockaddr_in myaddr, endpoint;
    int sdsocket, sdconnection, addrlen, received;
    char cwd[BUFFER_SIZE];

    if (argc < 2)
    {
        printf("Podaj numer portu jako parametr\n");
        return 1;
    }

    getcwd(localcwd,sizeof(localcwd));

    sdsocket = socket(AF_INET, SOCK_STREAM, 0);
    addrlen = sizeof(struct sockaddr_in);

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[1]));
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sdsocket,(struct sockaddr*) &myaddr,addrlen) < 0)
    {
        printf("bind() nie powiodl sie\n");
        return 1;
    }

    if (listen(sdsocket, 10) < 0)
    {
        printf("listen() nie powiodl sie\n");
        return 1;
    }

    char hostname[1024] = {0};
    gethostname(hostname, 1023);

    printf("Serwer: '%s' oczekuje na porcie: '%d'\n", hostname, atoi(argv[1]));

    while ((sdconnection = accept(sdsocket, (struct sockaddr*) &endpoint, &addrlen)) >= 0)
    {
        while(1)
        {
            recv(sdconnection, buf, BUFFER_SIZE, 0);
            printf("Wiadomosc od %s: %s\n", inet_ntoa(endpoint.sin_addr), buf);
            if ((strcmp(buf, "exit"))==0 || strlen(buf) == 0)
            {
                close(sdconnection);
                close(sdsocket);
                printf("Wyłączanie serwera\n");
                return 0;
            }
            else
            {
                strcpy(result, microshell(buf));
                send(sdconnection, result, BUFFER_SIZE, 0);
            }
            memset(buf, 0, sizeof buf);
            memset(result, 0, sizeof result);
        }
    }
}
