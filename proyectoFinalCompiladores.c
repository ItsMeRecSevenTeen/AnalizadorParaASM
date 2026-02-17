#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct tex {
    int id;
    char lex[30];
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
char lexemas[50][10] = {0};
struct tex *ini = NULL, *fin = NULL, *aux = NULL;//Punteros para la tabla de símbolos
unsigned int id = 0;
unsigned int k = 0; // Counter for number of words separated by spaces
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
    char palabra[15] = "";
    int i, j = 0, fila = 0;
    k = 0; // Reseteo a 0 
    while (strlen(entrada[fila]) > 0) {
        for (i = 0; i <= strlen(entrada[fila]); i++) {
            if (entrada[fila][i] != ' ' && entrada[fila][i] != '\0' && entrada[fila][i] != '\n' && entrada[fila][i] != ',' && entrada[fila][i] != ';') {
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
        fila++;
    }
}

void agregaTablaSimbolos(char L[30], char T[3])
{
    if (ini == NULL)
    {
        ini = malloc(sizeof(struct tex));
        ini->id = id++;
        memset(ini->lex, 0, sizeof(ini->lex)); //Limpiar basura de memoria
        memset(ini->tok, 0, sizeof(ini->tok)); //Limpiar basura de memoria
        strcpy(ini->lex, L);
        strcpy(ini->tok, T);
        ini->sig = NULL;
        fin = ini;
    }
    else
    {
        aux = malloc(sizeof(struct tex));
        aux->id = id++;
        memset(aux->lex, 0, sizeof(aux->lex)); //Limpiar basura de memoria
        memset(aux->tok, 0, sizeof(aux->tok)); //Limpiar basura de memoria
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
    char AG[] = "(){}[]";
    char OR[] = "<>=!";
    char OL[] = "&|!";
    char RESERVADAS[10][10] = {
        "if", "else", "for", "while", "switch", "case", "default", "return", "int", "char"};
    
     int i = 0, k = 0;

        while (1)
        {
            if (cadEnt[i] != ' ' && cadEnt[i] != '\n' && cadEnt[i] != '\0')
            {
                cadEnt[k++] = cadEnt[i]; // Verifica que la línea no esté vacía
            }
            else
            {
                if (k > 0)
                {
                    cadEnt[k] = '\0';
                    int caso = 0, cont = 0, tipo = -1;
                    int lenCad = strlen(cadEnt);
                    char let = cadEnt[cont];

                    while (cont < lenCad)
                    {
                        switch (caso)
                        {
                        case -1: // inválido
                        cont = lenCad;
                        break;
                        case 0: // estado inicial
                            if (strchr(L, let))
                            {
                                caso = 1; // palabra o identificador
                                tipo = 0;
                            }
                            else if (strchr(D, let))
                            {
                                caso = 4; // número entero
                                tipo = 4;
                            }
                            else if ((let == '+' || let == '-') && lenCad > 1 && strchr(D, cadEnt[cont + 1]))
                            {
                                caso = 4;
                                tipo = 4;
                            }
                            else if (strchr(O, let) && lenCad == 1)
                            {
                                // Caso: operador aritmético puro (+, -, *, /, %)
                                caso = 2;
                                tipo = 1;
                            }

                            else if (strchr(OR, let))
                            {
                                caso = 5; // relacional
                                tipo = 2;
                            }
                            else if (strchr(OL, let) && lenCad == 1)
                            {
                                caso = 6; // lógico
                                tipo = 3;
                            }
                            else if (strchr(AG, let) && lenCad == 1)
                            {
                                caso = 3; // agrupación
                                tipo = 7; 
                            }
                            
                            else
                            {
                                caso = -1; // inválido
                            }
                            break;

                        case 1: // Identificador
                            if (strchr(L, let))
                            {
                                // sigue como Identificador
                            }
                            else if (strchr(D, let))
                            {
                                tipo = 2; // Tipo para identificador
                            }
                            else
                            {
                                caso = -1; // No es valido
                            }
                            break;
                        case 2: // operador aritmético validado
                            if (strchr(D, let))
                            {
                                caso = 4; // Digitos despues de + o -
                            }
                            break;
                        case 3:
                        if (strchr(AG, let))
                        {
                            tipo = 7; // agrupación
                            caso = -1; // no puede tener más caracteres
                        }
                            break;
                        case 4: // número entero
                            if (strchr(D, let))
                            {
                                // sigue siendo número entero
                            }
                            else if (let == '.')
                            {
                                caso = 10; // posible decimal
                            }
                            else if (let == 'E')
                            {
                                caso = 7; // exponencial
                            }else if(strchr(L, let)){
                                caso = 1; // palabra o identificador
                                tipo = 0;
                            }
                            break;

                        case 5: // relacional
                            if ((let == '=' && (cadEnt[cont - 1] == '!' || cadEnt[cont - 1] == '<' || cadEnt[cont - 1] == '>' || cadEnt[cont - 1] == '=')))
                            {

                                tipo = 2;
                                caso = 5; // sigue siendo relacional
                            }
                            else
                            {
                                // si viene otro caracter, invalida
                                caso = -1;
                            }
                            break;
                        case 6:            // lógico validado
                            cont = lenCad; // salir
                            break;
                        case 7: // después de E
                            if (let == '+' || let == '-')
                            {
                                caso = 8; // Digitos despues de + o -
                            }
                            else if (strchr(D, let))
                            {
                                tipo = 5; // Número Exponencial válido
                                caso = 9; // estado de "leyendo exponente"
                            }
                            else
                            {
                                caso = -1; // inválido
                            }
                            break;

                        case 8: // después de E+ o E-
                            if (strchr(D, let))
                            {
                                tipo = 5;
                                caso = 9;
                            }
                            else
                            {
                                caso = -1; // inválido
                            }
                            break;

                        case 9: // leyendo los dígitos del exponente
                            if (strchr(D, let))
                            {
                                tipo = 5; // exponente
                            }
                            else
                            {
                                caso = -1; // no válido
                            }
                            break;
                        case 10: // después del punto decimal
                            if (strchr(D, let))
                            {
                                tipo = 6;  // decimal
                                caso = 11; // estamos en parte fraccionaria
                            }
                            else
                            {
                                caso = -1; // inválido si no hay dígito tras el punto
                            }
                            break;

                        case 11: // leyendo parte fraccionaria
                            if (strchr(D, let))
                            {
                                tipo = 6; // decimal
                            }
                            else if (let == 'E')
                            {
                                caso = 7; // decimal con exponente
                            }
                            else
                            {
                                caso = -1; // no válido
                            }
                            break;
                        case 12:
                        if (strchr(AG, let))
                        {
                            tipo = 7; // agrupación
                            caso = -1; // no puede tener más caracteres
                        }
                            break;
                        }
                        cont++;
                        let = cadEnt[cont];
                    }

                    int esReservada = 0;
                    if (caso == 1 && tipo == 0)
                    {
                        for (int r = 0; r < 10; r++)
                        {
                            if (strcmp(cadEnt, RESERVADAS[r]) == 0)
                            {
                                esReservada = 1;
                                break;
                            }
                        }
                        if (esReservada)
                        {
                            printf("%-10s = PALABRA RESERVADA\n", cadEnt);
                            agregaTablaSimbolos(cadEnt, "RES");
                        }
                        else
                        {
                            printf("%-10s = IDENTIFICADOR\n", cadEnt);
                            agregaTablaSimbolos(cadEnt, "ID");
                        }
                    }
                    else if (caso == 1 && tipo == 2)
                    {
                        printf("%-10s = IDENTIFICADOR\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "ID");
                    }
                    else if (caso == 2 && tipo == 1)
                    {
                        printf("%-10s = OPERADOR ARITMETICO\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "OA");
                    }
                     else if(caso == 3 && tipo == 7)
                    {
                        printf("%-10s = AGRUPACION\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "AG");
                    }  
                    else if (caso == 4 && tipo == 4)
                    {
                        printf("%-10s = NUMERO ENTERO\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "NE");
                    }
                    else if (caso == 4 && tipo == 4)
                    {
                        printf("%-10s = NUMERO ENTERO\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "NE");
                    }
                    else if (caso == 5 && tipo == 2)
                    {
                        printf("%-10s = OPERADOR RELACIONAL\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "OR");
                    }
                    else if (caso == 9 && tipo == 5)
                    {
                        printf("%-10s = NUMERO EXPONENCIAL\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "NE");
                    }
                    else if (caso == 6 && tipo == 3)
                    {
                        printf("%-10s = OPERADOR LOGICO\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "OL");
                    }                    
                    else if (caso == 11 && tipo == 6)
                    {
                        printf("%-10s = NUMERO DECIMAL\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "ND");
                    }
                    
                    else
                    {
                        printf("%-10s = NO VALIDO\n", cadEnt);
                    }

                    k = 0;
                }
                if (cadEnt[i] == '\0')
                    break;
            }
            i++;
        }
}

void mostrarTablaDeSimbolos()
{
    aux = ini;
    printf("\nID \tLexema \t\t Token\n");
    while (aux != NULL)
    {
        printf("%-8d %-20s %-10s\n", aux->id, aux->lex, aux->tok);
        aux = aux->sig;
    }
}
void tokenizarPalabras(){
     int terminado = 0, indice = 0;
    while(!terminado){
        tokenizador(palabras[indice]);
        if (strlen(palabras[indice]) == 0) {
            terminado = 1;
        }
        indice++;
    }
}
void derivacion(){
    char produccion1[3][10] = {"SO", "SU", "num"};
    char produccion2[2][10] = {"*S", "/S"};
    char produccion3[2][10] = {"+S", "-S"};
    //---------------Clasificacion de casos------------------------
    for (int i = 0; i < k; i++)
    {
        char *end;
        int has_dot = 0;
        int isExp = 0;
        int start = 0;

        // Clasifica el signo del número en caso de que exista
        if (palabras[i][0] == '+' || palabras[i][0] == '-')
        {
            start = 1;
        }

        // Clasifica si es número decimal o no
        int is_number = 1;
        for (int j = start; j < strlen(palabras[i]); j++)
        {
            if (!isdigit(palabras[i][j]))
            {
                is_number = 0;
                isExp = 0;
                has_dot = 0;
                break;
            }
            else if (palabras[i][j] == '.')
            {
                has_dot = 1;
                is_number = 1;
            }
            else if (palabras[i][j] == 'e' || palabras[i][j] == 'E' && (palabras[i][j + 1] == '+' || palabras[i][j + 1] == '-'))
            {
                isExp = 1;
                is_number = 1;
                j++;
            }
        }
        if ((strcmp(palabras[i], "Var") == 0 || strcmp(palabras[i], "var") == 0) && is_number == 0 && has_dot == 0 && isExp == 0){
            strcpy(lexemas[i], "var");
        }
        else if(is_number && strlen(palabras[i]) > start)
        {
            strcpy(lexemas[i], "num");
        }
        else if (palabras[i][0] == '+' || palabras[i][0] == '-')
        {
            strcpy(lexemas[i], "O");
        }
        else if (palabras[i][0] == '*' || palabras[i][0] == '/')
        {
            strcpy(lexemas[i], "U");
        }
        else if(is_number == 0 && isExp == 0 && has_dot == 0){
            strcpy(lexemas[i], "id");
        }
        else if (is_number && has_dot && isExp)
        {
            strcpy(lexemas[i], "num");
        }
    }
}
int main(){
    obtenerRutaArchivo();
    obtencionContenidoArchivo();
    separarPalabrasPorEspacios();
    tokenizarPalabras();
    mostrarTablaDeSimbolos();
    derivacion();
    return 0;
}