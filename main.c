#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

enum commands{
    "inserer nouvelle ligne avec texte (\"\")",
    "afficher page ",
    "mofifier caractere ligne  colonne  valeur ('')"
}

void rembobiner(FILE * file);
void getLine(char ** command, int * commandSize); // recupere une ligne tapée dans la commande de matiere optimisée

int main(int argc, char * argv[]){
    // On appelle notre editeur de texte sur un fichier depuis un shell.
    // On attend le nom de fichier à modifier en paremetre
    if(argc != 2){
        printf("Le nombre d'arguments passe ( %d ) est incorrect. Vous devez passer 1 nom de fichier\n", argc-1);
        exit( 1 );
    }
    printf("Vous editez le fichier %s\n", argv[1]);

    // TODO verifier que le nom du fichier est correct (chaque caractere est valide, donc [a-z]-_/...))

    FILE * file = fopen(argv[1], "rw"); // on crée une variable pointeur sur le fichier
    int fileSize = fseek(file, 0, SEEK_END);
    rembobiner(file);
    
    if(!file){
        printf("Fichier introuvable\n");
        exit( 1 );
    }

    char * command;

    do{
        int commandSize;
        char *command; // ce buffer va servir à stocker les commandes rentrées dans notre éditeur de fichier

        printf("Editeur@PC:$ "); // nom d'utilisateur op

        getLine(&command, &commandSize); // maintenant, on a la commande entrée par l'utilisateur et sa taille
        // pour utiliser la commande, on va retirer tous les caracteres parametres, des caraccteres
        // qui ne sont pas strictement de la commande (texte à ajouter, numero de page...)
        char **

        free(command); // on libere la mémoire
    } while(1);

    fclose(file);

    return 0;
}

void rembobiner(FILE * file){
    fseek(file, 0, SEEK_SET);
}

void getLine(char ** command, int * commandSize){
    *commandSize = 0;
    *command = (char*) malloc(*commandSize * sizeof(char)); // ce buffer va servir à stocker les commandes passées à notre éditeur de fichier
    int c; // variable char pour lire l'entrée standard. int car on doit gerer le caractère EOF qui est -1, et char est non signé
    // on lit l'entrée standard sous forme de fichier (un peu comme si on avait ouvert un fichier avec fopen)

    for(;;){ // boucle infinie
        c = fgetc(stdin); // on prend le caractere suivant sur l'entrée standard
        if(c == EOF || c == '\n'){ // à la fin de l'entrée standard on a un caractere EOF (end of file) car on lit avec fopen
            *command = realloc(*command, ++(*commandSize) * sizeof(char)); // On rajoute la caractère de fin de chaine
            (*command)[*commandSize-1] = '\0';
            break; // on sort de la boucle infinie car on a fini de lire
        }
        *command = realloc(*command, ++(*commandSize) * sizeof(char)); // on agrandit le buffer d'un caractere
        (*command)[*commandSize-1] = c; // on ajoute le nouveau caractere
    }
}
