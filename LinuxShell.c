/**
@author
Name: Metin Mert AKÇAY
E-Mail: metinmertakcayhpalytu@gmail.com
Operating System: Ubuntu
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 1024
//Argümanları tutacak olan degisken
char *argv[32];

//Kelimede verilmis olan bosluklar temizlendi ve geriye donduruldu.
char *deleteExtraGaps(char *command,char *cmd)
{
	int i = 0;
	int j = 0;
	int tmp;
    int size = strlen(command);

	//Silme islemi icin bellekten yer ayrildi.
	cmd = (char *) calloc(sizeof(char),(size + 1));
	//Yerin acilip acilmadigi kontrol ediliyor.
	if(cmd == NULL)
	{
		printf("Yer acilamadi");
		exit(0);
	}

	//Null karakteri gorene kadar dongu icerisinde kalinacaktir.
	while(command[i])
	{
		//Ardarda bosluk karakterinin olup olmadigi kontrol ediliyor.
		if(command[i] == ' ')
		{
			tmp = i + 1;
			if(command[tmp] != '\0')
			{
				while((command[tmp] == ' ') && (command[tmp] != '\0'))
				{
					tmp++;
				}
                /*Bosluk sayisi fazla bile olsa sadece 1 tane bosluk yeni acilan degere yerlestirilecek*/
                i = tmp;
                cmd[j] = ' ';
                j++;
			}
		}
		cmd[j] = command[i];
		i++;
		j++;
	}
	//String duzeltildikten sonra bittigini belirtecek NULL karakteri yerlestirilimistir.
	cmd[j] = command[i];
	return cmd;
}

//Komutun kullanilabilmesi icin komut strtok fonksiyonu kullanilarak parcalara bolunmustur.
void divideCommentArguments(char *command)
{
	char *tmp;
	int i = 0;

    //Bolme islemine baslaniliyor.
	tmp = strtok(command," ");
	while(tmp != NULL)
	{
        //Elde edilen degerler arguman olarak yerlestiriliyor.
		argv[i] = tmp;
		i++;
		tmp = strtok(NULL," ");
	}
	/*En sona NULL degeri konulmustur. Execvp parametre aktaracagimiz icin bunun yapilmasi gerekir.*/
	argv[i] = NULL;
}

//Girilen komutun sonunda new line karakteri bulunmaktadir. New line karakteri silme islemi gerceklestiriliyor.*/
void *organizeCommand(char *command)
{
	char *cmd;			//Changing command
	int i;

	while(command[i] != '\n')
	{
		i += 1;
	}
	command[i] = '\0';
	//Bosluklar ile girilmis komut varsa bosluklar silinerek degerlendirme yapiliyor.
	cmd = deleteExtraGaps(command,cmd);
	//Komut kullanilmak uzere parcalara bolunuyor.
	divideCommentArguments(cmd);
}

/*Process olusturarak, olusturulan child processte istenilen islemin gerceklenmesi saglanmistir.*/
void processCreate()
{
	int status;
	//Yeni bir process olusturuluyor.
	pid_t pid = fork();
	//Child process olup olmadigi kontrol ediliyor.
	if(pid == 0)
	{
		//execvp ile -1 degeri donuyorsa calistirilmak istenen komut yoktur.
		if (execvp( argv[0], argv) == -1)
		{
			printf("%s : command not found\n",argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	//Parent process islemini bitirdikten sonra child processi bekler.
	wait(&status);
}

//Olusturulmus olan linux shell ismi ve bulunulan dizin bilgisi ekrana basiliyor.
void printShellWorkingDirectory()
{
	char cwd[128];			//Current_working_directory
    char shell[256];		//Shell name + current_working_directory
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strcpy(shell, "\nMetinshell:");
        strcat(shell, cwd);
        strcat(shell, "# ");
        printf("%s", shell);
    }
    else
    {
       perror("getcwd() error");
    }
}

int main()
{
	void *organizeCommand(char *);
	char *deleteExtraGaps(char *, char *);
	void divideCommentArguments(char *);
	void processCreate();
    void printShellWorkingDirectory();

    char *command;
    char *home = "/home";

	//Dinamik olarak bellekten yer aciliyor.
	command = (char *)malloc(sizeof(char)*SIZE);
	//Yerin acilip acilmadigi kontrol diliyor.
	if(command == NULL)
	{
		printf("Yer acilamamistir.");
		exit(0);
	}

	while (1)
	{
		//Kullanilan shell ismi yazilmis ve buffer temizlenmistir.
		printShellWorkingDirectory();
		//Enter tusuna basildigi zaman isleme girecektir
		fgets(command, SIZE, stdin);
		if(strcmp(command,"\n"))
        {
            //Girilen komutta bazi duzenlemeler yapilmasi icin fonksiyona gonderiliyor.
            organizeCommand(command);
            //Exit tusana basilip basilmadigi kontrol ediliyor.
            if(!strcmp(argv[0],"exit"))
            {
                printf("\n");
                //Yer bellege iade edilir.
                free(command);
                exit(EXIT_SUCCESS);
            }
            //Dizin degistirme islemi yapilmak istenip istenmedigi kontrol ediliyor.
            else if(!strcmp(argv[0],"cd"))
            {
                //Eger parametre alinmamissa gidilecek olan dizindir.
                if(argv[1] == NULL)
                {
                    chdir(home);
                }
                //Parametre alinmis ise verilen parametreye gore islem yapilacaktir.
                else
                {
                    if(chdir(argv[1]) == -1)
                    {
                        printf("%s No such directory\n",argv[1]);
                    }
                }
            }
            //Help tusuna basilmasi sonucu help kismi acilir.
            else if(!strcmp(argv[0],"help"))
            {
                system("bash -c help");
            }
            else
            {
                //Process olusturumasını ve verilen kodun calistirilmasi icin olusturulmustur
                processCreate();
            }
        }
	}
	return 0;
}
