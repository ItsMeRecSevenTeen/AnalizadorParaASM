#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

struct tex {
    int id;
    char lex[30]; // Inicializar con ceros para evitar basura
    char tok[3];
    char tipoDato[20];
    struct tex* sig;
};

//---------DECLARACIÓN DE VARIABLES GLOBALES----------------
FILE *archivo;
char entrada[50][70] = {0};//Matriz para almacenar líneas del archivo
//Tamaño máximo de 50 líneas y 70 caracteres por línea
char palabras[50][10] = {0};//Matriz para almacenar palabras separadas por espacios
//con un máximo de 50 palabras y 10 caracteres por palabra

//-------------------------------------------------------
void obtenerRutaArchivo(){
    int rutaValida = 0;
    while(rutaValida == 0){
        char ruta_archivo[100] = {0};//En ceros para eliminar memoria basura
        printf("Ingrese la ruta del archivo: ");
        fgets(ruta_archivo, sizeof(ruta_archivo), stdin);//Obtención de línea
        ruta_archivo[strcspn(ruta_archivo, "\n")] = 0; //Supresión de salto de línea
        printf("La ruta especificada es: %s\n", ruta_archivo);
        archivo = fopen(ruta_archivo, "r");
        if (archivo == NULL) {
            printf("NO SE PUDO OBTENER EL ARCHIVO, INTÉNTELO DE NUEVO\n");
        } else {
            printf("Archivo abierto exitosamente.\n");
            
            rutaValida = 1; // Salir del bucle si la ruta es válida
        }
    }
}

void obtencionContenidoArchivo(){
    char linea[100];
    printf("\n---------CONTENIDO DEL ARCHIVO---------\n");
    int a = 0;
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        strcpy(entrada[a], linea); // Copia línea a la matriz entrada
        printf("%s", linea);//Obtiene línea por línea el contenido de nuestro
        //Archivo y lo imprime hasta llegar al final
        a++;
    }

    printf("\n--------------------------------------\n");
}

void separarPalabrasPorEspacios(){
    char palabra[10] = "";
    int i, j = 0, k = 0, fila = 0;

    for (fila = 0; fila < 50; fila++) {
        if (strlen(entrada[fila]) == 0) break; // Salir si la línea está vacía
        
        for (i = 0; i <= strlen(entrada[fila]); i++) {
            if (entrada[fila][i] != ' ' && entrada[fila][i] != '\0' && entrada[fila][i] != '\n') {
                palabra[j++] = entrada[fila][i];
            }
            else {
                if (j > 0) {
                    palabra[j] = '\0';
                    strcpy(palabras[k++], palabra);
                    j = 0;
                    memset(palabra, 0, sizeof(palabra));
                }
            }
        }
    }
}
struct tex *ini = NULL, *fin = NULL, *aux = NULL;
unsigned int id = 0;
void agregaTablaSimbolos(char L[30], char T[3])
{
    if (ini == NULL)
    {
        ini = malloc(sizeof(struct tex));
        ini->id++;
        strcpy(ini->lex, L);
        strcpy(ini->tok, T);
        ini->sig = NULL;
        fin = ini;
    }
    else
    {
        aux = malloc(sizeof(struct tex));
        strcpy(aux->lex, L);
        strcpy(aux->tok, T);
        aux->sig = NULL;
        fin->sig = aux;
        fin = aux;
    }
}

void tokenizador(char cadEnt[100]) {
    char L[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char D[] = "0123456789";
    char O[] = "+-*/%";
    char arr[10][10] = {"if", "for","else","char","while","case", "switch","default","return","int"};
    int caso = 0; // estado actual
    int cont = 0; // posición en la cadena
    int lenCad;
    int tipo = -1; // 0 = palabra, 1 = operador, 2 = identificador
    lenCad = strlen(cadEnt);

    char let = cadEnt[cont];

    while (cont < lenCad)
            {
                switch (caso)
                {
                case 0: // q0: estado inicial
                    if (strchr(L, let))
                    {
                        caso = 1; // va a palabra o identificador
                        tipo = 0;
                    }
                    else if (strchr(O, let) && lenCad == 1)
                    {
                        caso = 2; // Caso cuando es un operador
                        tipo = 1;
                    }
                    else
                    {
                        caso = 3; // Caso cuando es invalido
                    }
                    break;

                case 1: // q1: letras → palabra o identificador
                    if (strchr(L, let))
                    {
                        // sigue siendo válido: palabra o identificador
                    }
                    else if (strchr(D, let))
                    {
                        tipo = 2; // ya no es palabra, ahora es un identificador
                    }
                    else
                    {
                        caso = 3; // invalido
                    }
                    break;

                case 2: // Operador Validado
                    break;

                case 3:            // inválido
                    cont = lenCad; // forzar salida
                    break;
                }

                cont++;
                let = cadEnt[cont]; // siguiente caracter
            }

            if (caso == 1 && tipo == 0)
            {
                printf("Es una PALABRA\n");
                agregaTablaSimbolos(cadEnt, "Palabra");
            }
            else if (caso == 2 && tipo == 1)
            {
                printf("Es un OPERADOR ARITMÉTICO\n");
                agregaTablaSimbolos(cadEnt, "Operador");
            }
            else if (caso == 1 && tipo == 2)
            {
                printf("Es un IDENTIFICADOR\n");
                agregaTablaSimbolos(cadEnt, "Identificador");
            }
            else
            {
                printf("No es válido\n");
            }
            
}

int main(){
    obtenerRutaArchivo();
    obtencionContenidoArchivo();
    separarPalabrasPorEspacios();
    printf("\n---------PALABRAS SEPARADAS POR ESPACIOS---------\n");
    for (int i = 0; i < 50; i++) {
        if (strlen(palabras[i]) > 0) {
            printf("%s\n", palabras[i]);
        }
    }
    tokenizador(palabras[0]);
    
    return 0;
}