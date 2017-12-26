#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define HAUTEUR_PAGE 60
#define LARGEUR_PAGE 80

int min(int x, int y){
    return (x < y)? x: y;
}

void rembobiner(FILE * file);
void getLine(char ** command, int * commandSize); // recupere une ligne tapée dans la commande de matiere optimisée
void deleteChar( char ** command, int * commandSize, int index;); // supprime un caractère d'une chaine de caractères
int positionFromLinCol( FILE * file, int ligne, int colonne ); // convertit une position en 2 dimensions en une position en 1 dimmension

int main(int argc, char * argv[]){
    // On appelle notre editeur de texte sur un fichier depuis un shell.
    // On attend le nom de fichier à modifier en paremetre
    if(argc != 2){
        printf("Le nombre d'arguments passe ( %d ) est incorrect. Vous devez passer 1 nom de fichier\n", argc-1);
        exit( 1 );
    }
    printf("\nVous editez le fichier %s\n\n", argv[1]);

    // TODO verifier que le nom du fichier est correct (chaque caractere est valide, donc [a-z]-_/...))

    FILE * file = fopen(argv[1], "r+"); // on crée une variable pointeur sur le fichier
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    printf("File size: %d caracters\n\n", fileSize);
    rembobiner(file);
    
    if(!file){
        printf("Fichier introuvable\nCreation du fichier\n");
        file = fopen(argv[1], "w+");
    }

    char * command = malloc(0); // ce buffer va servir à stocker les commandes rentrées dans notre éditeur de fichier
    int commandSize;

    do{
        free(command); // on libere la mémoire

        printf("Editeur@PC:$ "); // nom d'utilisateur op

        getLine(&command, &commandSize); // maintenant, on a la commande entrée par l'utilisateur et sa taille
        // pour utiliser la commande, on va retirer tous les caracteres parametres, des caracteres
        // qui ne sont pas strictement de la commande (texte à ajouter, numero de page...)

        int argsSize = 0; // nombre d'arguments
        char ** args = malloc(argsSize * sizeof(char*)); // liste des arguments
        int * argSize = malloc(argsSize * sizeof(int)); // liste des tailles individuelles des arguments

        int car, subCar;
        for( car = 0; car < commandSize; car++ ){
            if(command[car] >= '0' && command[car] <= '9'){ // on a trouvé un chiffre, caractère parametre
                argsSize++;
                args = realloc(args, argsSize * sizeof(char*)); // on a un nouvel argument, donc on agrandit notre liste d'arguments
                argSize = realloc(argSize, argsSize * sizeof(int)); // idem avec la taille des arguments

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
                argSize = realloc(argSize, argsSize * sizeof(int)); // idem avec la taille des arguments

                argSize[argsSize-1] = 1; // on a un caractère dans l'argument pour l'instant
                args[argsSize-1] = malloc(argSize[argsSize-1] * sizeof(char));
                args[argsSize-1][argSize[argsSize-1] - 1] = command[car];

                deleteChar(&command, &commandSize, car); // on enleve le caractère de la chaine, maintenant car pointe sur le caractere suivant
                
                do{ // tant que la chaine continue, on ajoute le caractere suivant à l'argument
                    argSize[argsSize-1]++;
                    args[argsSize-1] = realloc(args[argsSize-1], argSize[argsSize-1] * sizeof(char));
                    args[argsSize-1][argSize[argsSize-1] - 1] = command[car];
                    deleteChar(&command, &commandSize, car); // on enleve le caractère de la chaine
                }while(command[car] != '\"' && command[car] != '\0');
            }
        }
        // command contient la base de la commande entrée, vidée de ses arguments
        // args contient tous les arguments entrés

        if( strcmp(command, "ajouter nouvelle ligne texte \"\"") == 0 ){
            printf("ajout ligne\n");
            fseek(file, 0, SEEK_END);
            int currentLine = 0;
            do{
                fwrite("\n", sizeof(char), 1, file);
                fwrite(args[0] + currentLine * LARGEUR_PAGE, sizeof(char), min( LARGEUR_PAGE, argSize[0] - (currentLine * LARGEUR_PAGE)), file);
                currentLine++;
            }while( (argSize[0] - (currentLine * LARGEUR_PAGE)) > 0 );
            // on ecrit notre texte sur des lignes de taille LARGEUR_PAGE au max (+ \n si necessaire)
        }
        else if( strcmp(command, "afficher page ") == 0 ){
            printf("\n");

            int pages = atoi(args[0]) - 1;
            int ligneDepart = HAUTEUR_PAGE * pages;
            int ligneFin = HAUTEUR_PAGE * (pages + 1);
            int positionDepart = positionFromLinCol(file, ligneDepart, 0);
            int positionFin = positionFromLinCol(file, ligneFin, 0) - 1;

            int carToPrint = positionFin - positionDepart;

            fseek(file, positionDepart, SEEK_SET);

            int i;
            for(i = 0; i < carToPrint; i++){
                printf("%c", fgetc(file));
            }
            printf("\n\n");
        }
        else if( strcmp(command, "ajouter caracteres ligne  colonne  texte \"\"") == 0 ){
            int positionDepart = positionFromLinCol(file, atoi(args[0]) - 1, atoi(args[1]) - 1);
            int i, car, nombre = argSize[2]; // nombre est la longeur de la chaine à ajouter
            int caracteresCopies = fileSize - positionDepart;
            fileSize += nombre;
            fseek(file, positionDepart, SEEK_SET); // on revient où on veut ajouter du texte
            char buffer[caracteresCopies];
            fread(buffer, sizeof(char), caracteresCopies + 1, file);
            fseek(file, positionDepart, SEEK_SET);
            
            for( i = 0; i < nombre; i += LARGEUR_PAGE){ // on ajoute le texte, avec des sauts de ligne si necessaire
                fwrite(args[2] + i * sizeof(args[2][0]), sizeof(char), min(LARGEUR_PAGE, nombre - i), file);
                if(i + LARGEUR_PAGE <= nombre)
                    fputc('\n', file);
            }

            fwrite(buffer, sizeof(char), caracteresCopies + 1, file); // on recopie le texte qu'il y avait à la fin de l'insertion
        }
        else if( strcmp(command, "supprimer caracteres ligne  colonne  nombre ") == 0 ){
            int positionDepart = positionFromLinCol(file, atoi(args[0]) - 1, atoi(args[1]) - 1);
            int car, nombre = min(atoi(args[2]), fileSize - positionDepart); // on ne veut pas supprimer de caractères en dehors du fichier
            int caracteresCopies = fileSize - positionDepart - nombre;
            fileSize -= nombre;
            char * buffer[caracteresCopies];
            fseek(file, positionDepart + nombre, SEEK_SET); // on place le pointeur sur le premier des caracteres qu'on va copier
            fread(buffer, sizeof(char), caracteresCopies, file); // on copie les caracteres à deplacer de la fin de la suppresion à la fin du fichier
            fseek(file, positionDepart, SEEK_SET); // on decale le pointeur au debut de la suppresion
            fwrite(buffer, sizeof(char), caracteresCopies, file); // on a decalé le texte à la fin du fichier du nombre de caraceres à supprimer

            fflush(file); // on actualise le fichier sur le disque, necessaire pour la fonction ftruncate
            ftruncate(fileno(file), fileSize); // on raccourcit la taille du fichier
        }
        else if( strcmp(command, "ajouter ligne position  texte \"\"") == 0 ){
            int positionDepart = positionFromLinCol(file, atoi(args[0]) - 1, 0);
            int i, car, nombre = argSize[1]; // nombre est la longeur de la chaine à ajouter
            int caracteresCopies = fileSize - positionDepart - 1;
            fileSize += nombre-1;
            fseek(file, positionDepart, SEEK_SET); // on revient où on veut ajouter du texte
            char buffer[caracteresCopies];
            fread(buffer, sizeof(char), caracteresCopies, file);
            fseek(file, positionDepart, SEEK_SET);
            
            for( i = 0; i < nombre; i += LARGEUR_PAGE){ // on ajoute le texte, avec des sauts de ligne si necessaire
                fwrite(args[1] + i * sizeof(args[1][0]), sizeof(char), min(LARGEUR_PAGE, nombre - i), file);
                if(i + LARGEUR_PAGE <= nombre){
                    fputc('\n', file);
                    fileSize++;
                }
            }

            fwrite(buffer, sizeof(char), caracteresCopies, file); // on recopie le texte qu'il y avait à la fin de l'insertion
        }

        printf("\nFile size: %d caracters\n\n", fileSize);
        fflush(file); // on actualise le fichier sur le disque

    }while( strcmp(command, "quitter") != 0 );

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

int positionFromLinCol( FILE * file, int ligne, int colonne ){
    rembobiner(file);
    int i, car, position = 0;
    for( i = 0; i < ligne; i++){
        while((car = fgetc(file)) != '\n'){ // pour chaque ligne, on parcourt le fichier jusqu'au prochain saut de ligne
            position++;
            if(car == EOF){ // si on atteint la fin du fichier, la valeur retournée pointe sur fin du fichier
                position -= colonne - 1;
                i = ligne;
                break;
            }
        }
        position++;
        position++; // on n'a pas incrémenté position pour le caractère \n, on le fait donc ici
    }
    position += colonne;
    rembobiner(file);
    return position;
}
