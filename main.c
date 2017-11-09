#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*enum commands{
    "inserer nouvelle ligne avec texte (\"\")",
    "afficher page ",
    "mofifier caractere ligne  colonne  valeur ('')"
}*/

void rembobiner(FILE * file);
void getLine(char ** command, int * commandSize); // recupere une ligne tapée dans la commande de matiere optimisée
void deleteChar( char ** command, int * commandSize, int index;); // supprime un caractère d'une chaine de caractères

int main(int argc, char * argv[]){
    // On appelle notre editeur de texte sur un fichier depuis un shell.
    // On attend le nom de fichier à modifier en paremetre
    if(argc != 2){
        printf("Le nombre d'arguments passe ( %d ) est incorrect. Vous devez passer 1 nom de fichier\n", argc-1);
        exit( 1 );
    }
    printf("Vous editez le fichier %s\n", argv[1]);

    // TODO verifier que le nom du fichier est correct (chaque caractere est valide, donc [a-z]-_/...))

    FILE * file = fopen(argv[1], "r+"); // on crée une variable pointeur sur le fichier
    int fileSize = fseek(file, 0, SEEK_END);
    rembobiner(file);
    
    if(!file){
        printf("Fichier introuvable\nCréation du fichier\n");
        file = fopen(argv[1], "w+");
    }

    char * command = malloc(0);
    int commandSize;

    do{
        free(command); // on libere la mémoire
        int commandSize;
        char *command; // ce buffer va servir à stocker les commandes rentrées dans notre éditeur de fichier

        printf("Editeur@PC:$ "); // nom d'utilisateur op

        getLine(&command, &commandSize); // maintenant, on a la commande entrée par l'utilisateur et sa taille
        // pour utiliser la commande, on va retirer tous les caracteres parametres, des caracteres
        // qui ne sont pas strictement de la commande (texte à ajouter, numero de page...)

        unsigned int argsSize = 0; // nombre d'arguments
        char ** args = malloc(argsSize * sizeof(char*)); // liste des arguments
        unsigned int * argSize = malloc(argsSize * sizeof(unsigned int)); // liste des tailles individuelles des arguments

        int car, subCar;
        for( car = 0; car < commandSize; car++ ){
            if(command[car] >= '0' && command[car] <= '9'){ // on a trouvé un chiffre, caractère parametre
                argsSize++;
                args = realloc(args, argsSize * sizeof(char*)); // on a un nouvel argument, donc on agrandit notre liste d'arguments
                argSize = realloc(argSize, argsSize * sizeof(unsigned int)); // idem avec la taille des arguments

                argSize[argsSize-1] = 1; // on a un caractère dans l'argument pour l'instant
                args[argsSize-1] = malloc(argSize[argsSize-1] * sizeof(char));
                args[argsSize-1][argSize[argsSize-1] - 1] = command[car];

                deleteChar(&command, &commandSize, car); // on enleve le caractère de la chaine, maintenant car pointe sur le caractere suivant
                
                while(command[car] >= '0' && command[car] <= '9' && command[car] != '\0'){ // tant que le nombre continue, on ajoute le chiffre suivant à l'argument
                    argSize[argsSize-1]++;
                    args[argsSize-1] = realloc(args[argsSize-1], argSize[argsSize-1] * sizeof(char));
                    args[argsSize-1][argSize[argsSize-1] - 1] = command[car];
                    deleteChar(&command, &commandSize, car); // on enleve le caractère de la chaine
                }
            }
            // on fait exactement pareil pour une chaine de caractère
            else if(command[car] == '\"'){ // on a trouvé une nouvelle chaine de caractères, caractère paramètre
                car++; // le caractère " ne nous interesse pas

                argsSize++;
                args = realloc(args, argsSize * sizeof(char*)); // on a un nouvel argument, donc on agrandit notre liste d'arguments
                argSize = realloc(argSize, argsSize * sizeof(unsigned int)); // idem avec la taille des arguments

                argSize[argsSize-1] = 1; // on a un caractère dans l'argument pour l'instant
                args[argsSize-1] = malloc(argSize[argsSize-1] * sizeof(char));
                args[argsSize-1][argSize[argsSize-1] - 1] = command[car];

                deleteChar(&command, &commandSize, car); // on enleve le caractère de la chaine, maintenant car pointe sur le caractere suivant
                
                while(command[car] != '\"' && command[car] != '\0'){ // tant que le nombre continue, on ajoute le chiffre suivant à l'argument
                    argSize[argsSize-1]++;
                    args[argsSize-1] = realloc(args[argsSize-1], argSize[argsSize-1] * sizeof(char));
                    args[argsSize-1][argSize[argsSize-1] - 1] = command[car];
                    deleteChar(&command, &commandSize, car); // on enleve le caractère de la chaine
                }
            }
        }
        // command contient la basede la commande entrée, vidée de ses arguments
        // args contient tos les arguments entrés

        if( !strcmp(command, "ajouter nouvelle ligne texte \"\"") ){
            printf("ajout ligne, texte: %s\n", args[0]);
            fseek(file, 0, SEEK_END);
            fwrite("\n", sizeof(char), 1, file);
            fwrite(args[0], sizeof(char), argSize[0], file);
        }

    } while( strcmp(command, "quitter") );

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

void deleteChar( char ** command, int * commandSize, int index){
    int i;
    (*commandSize)--;
    for( i = index; i < *commandSize; i++ ){
        (*command)[i] = (*command)[i+1];
    }
    *command = realloc(*command, *commandSize);
}
