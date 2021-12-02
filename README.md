# Remote-Microshell
Remote Microshell napisany na sieci

```shell
Remote MiCrOShElL SIECI
My implementation:
'cd ~' - Home directory
'cd -' - Previous directory
'cd ..' - Parent directory
'cd o' - Origin directory
'cd /etc' - Absolute path
'cd /etc' - Relative path

'Fork & exec': (1 and more arguments)
'ls -l -a' - List all the files in the current working directory
'mkdir name' - Create a folder
'rm -r name' - Remove a folder
'cat plik' - Read a file
'wc -c plik' - How many chars
'date', 'echo', 'ifconfig', 'iwconfig', 'hostname'
You can use all avaible arguments in this programs like 'wc --help'
'exit' to shutdown server and client\n";
```

Kompilacja
```shell
/serwer
gcc -o main main.c
./main port

/klient/src/main/java
javac klient.java
java klient hostname port
```
