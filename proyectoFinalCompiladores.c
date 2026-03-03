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
                            else if(strchr(OR, let) && lenCad == 1)
                            {
                                caso = 5; // relacional
                                tipo = 8; // asignacion
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
                    else if (caso == 5 && tipo == 8)
                    {
                        printf("%-10s = ASIGNACION\n", cadEnt);
                        agregaTablaSimbolos(cadEnt, "AS");
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
void clasificadorDeCasos(){
    //---------------Clasificacion de casos------------------------
    for (int i = 0; i < k; i++)
    {
        char *end;
        int has_dot = 0;
        int isExp = 0;
        int is_grouping = 0;
        int is_relational = 0;
        int is_asig = 0;
        int start = 0;

        // Clasifica el signo del número en caso de que exista
        if (palabras[i][0] == '+' || palabras[i][0] == '-')
        {
            start = 1;
        }
        else if (palabras[i][0] == '=') //Clasifica un posible operador relacional '==' o asignación '='
        {
            is_asig = 1;//Se asume que es una asignación hasta que se demuestre lo contrario
            start = 1;
        }
        

        // Clasifica si es número decimal o no
        int is_number = 1;
        for (int j = start; j < strlen(palabras[i]); j++)
        {
            if(palabras[i][j] == '<' || palabras[i][j] == '>' || palabras[i][j] == '!' || palabras[i][j] == '&' || palabras[i][j] == '|' || palabras[i][j] == '='){
                is_relational = 1;
                is_grouping = 0;
                is_number = 0;
                has_dot = 0;
                isExp = 0;
                break;

            }
            else if(palabras[i][j] == '(' || palabras[i][j] == ')' || palabras[i][j] == '{' || palabras[i][j] == '}' || palabras[i][j] == '[' || palabras[i][j] == ']'){
                is_grouping = 1;
                is_number = 0;
                has_dot = 0;
                isExp = 0;
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
            else if (!isdigit(palabras[i][j]))
            {
                is_number = 0;
                isExp = 0;
                has_dot = 0;
                break;
            }
            
        }
        if ((strcmp(palabras[i], "Var") == 0 || strcmp(palabras[i], "var") == 0) && is_number == 0 && has_dot == 0 && isExp == 0){
            strcpy(lexemas[i], "var");
        }
        else if(strcmp(palabras[i],"=") == 0 && is_asig == 1){
            strcpy(lexemas[i], "=");
        }
        else if(strcmp(palabras[i],"int") == 0){
            strcpy(lexemas[i], "int");
        }
        else if(strcmp(palabras[i],"float") == 0){
            strcpy(lexemas[i], "float");
        }
        else if(strcmp(palabras[i],"char") == 0){
            strcpy(lexemas[i], "char");
        }
        else if(strcmp(palabras[i],"if") == 0){
            strcpy(lexemas[i], "if");
        }
        else if(strcmp(palabras[i],"else") == 0){
            strcpy(lexemas[i], "else");
        }
        else if(strcmp(palabras[i],"for") == 0){
            strcpy(lexemas[i], "for");
        }
        else if(strcmp(palabras[i],"while") == 0){
            strcpy(lexemas[i], "while");
        }
        else if(strcmp(palabras[i],"switch") == 0){
            strcpy(lexemas[i], "switch");
        }
        else if(strcmp(palabras[i],"case") == 0){
            strcpy(lexemas[i], "case");
        }
        else if(strcmp(palabras[i],"default") == 0){
            strcpy(lexemas[i], "default");
        }
        else if(strcmp(palabras[i],"return") == 0){
            strcpy(lexemas[i], "return");
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
        else if(is_relational){
            strcpy(lexemas[i], "OR");
        }
        else if(is_grouping){
            strcpy(lexemas[i], "AG");
        }
        else if((is_number == 0 && isExp == 0 && has_dot == 0)!= 0 && is_grouping == 0 && is_relational == 0){
            strcpy(lexemas[i], "id");
        }
        else if (is_number && has_dot && isExp)
        {
            strcpy(lexemas[i], "num");
        }
    }
}
//Producciones
//PARA OPERACIONES ARITMÉTICAS:
// S -> SO | SU | num
// O -> +S | -S
// U -> *S | /S

//PARA LA ESTRUCTURA FOR:
//for( I ; C ; A ){sentencias}
// A -> id++ | id-- | ++id | --id
// I -> id = num | id = id
// C -> id OR num | id OR id
// OR -> < | > | <= | >= | == | !=
void derivador() {
    printf("\n------------Derivacion por LL(1)--------------:\n");
    
    int index = 0;
    int linea = 1;

    // Procesando todos los lexemas
    while (index < k) {
        char derivacion[200] = "S";
        printf("\nEstructura %d:\n", linea++);
        printf("%s\n", derivacion);
        
        int terminado = 0;

        while (!terminado && index < k) {
            //Descarte de AG
            while (index < k && strcmp(lexemas[index], "AG") == 0) {
                index++;
            }
            if (index >= k) break;

            // Búsqueda del No-Terminal más a la izquierda
            // S=Statement, T=Tipo, I=Init, C=Condicion, A=Aumento, R=Relacional, E=Expresion, O/U=Operadores
            char *pos = strpbrk(derivacion, "STICAEROU");
            
            if (pos == NULL) {
                terminado = 1;
                continue; // Saltarse la línea actual
            }

            char nueva[200] = "";
            strncat(nueva, derivacion, pos - derivacion); // Copia lo que hay antes del No-Terminal

            // S: Sentencia inicial
            if (*pos == 'S') {
                if (strcmp(lexemas[index], "var") == 0) {
                    strcat(nueva, "var T id = E");
                    index++; // var consumido
                }
                else if (strcmp(lexemas[index], "for") == 0) {
                    strcat(nueva, "for( I ; C ; A ){ S }");
                    index++; // for consumido
                }
                else if (strcmp(lexemas[index], "if") == 0) {
                    strcat(nueva, "if( C ){ S }");
                    index++; // if consumido
                }
                //else if (strcmp(lexemas[index], "int") == 0 || strcmp(lexemas[index], "float") == 0 || strcmp(lexemas[index], "char") == 0) {
                //    strcat(nueva, "T id = E"); // Declaración
                //}
                else if (strcmp(lexemas[index], "id") == 0 && index + 1 < k && strcmp(lexemas[index+1], "=") == 0) {
                    strcat(nueva, "id = E"); // Asignación
                    index += 2; // id y = consumido
                }
                else if (strcmp(lexemas[index], "return") == 0) {
                    strcat(nueva, "return E");
                    index++;
                }
                else {
                    strcat(nueva, "E"); // Expresión si no evaluó condiciones
                }
            }
            //T: Tipo dato
            else if (*pos == 'T') {
                strcat(nueva, palabras[index]); // int, float, char...
                index++;
            }
            // 3. for ( I , C , A )
            else if (*pos == 'I') {
                strcat(nueva, "id = E");
                index += 2; // id = E
            }
            else if (*pos == 'C') {
                strcat(nueva, "E R E"); // Expresión OperadorRelacional Expresión
            }
            else if (*pos == 'A') {
                strcat(nueva, "id");
                index++; // id consumido
                // Caso para ++ y --, se toman como caracter unitario (+,-)
                while (index < k && (strcmp(palabras[index], "+") == 0 || strcmp(palabras[index], "-") == 0)) {
                    strcat(nueva, palabras[index]);
                    index++;
                }
            }
            // 4. R: Operadores
            else if (*pos == 'R') {
                strcat(nueva, palabras[index]); // <, >, ==.
                index++;
            }
            // E: Expresiones
            else if (*pos == 'E') {

                int sig_O = 0, sig_U = 0;
                // Si entra a E, Significa que es una expresión, por lo tanto contiene SO o SU
                if (index + 1 < k) {
                    if (strcmp(lexemas[index + 1], "=") == 0){
                        index += 2; // Consume el '=' y el siguiente token
                        if (strcmp(lexemas[index], "O") == 0) sig_O = 1;
                        if (strcmp(lexemas[index], "U") == 0) sig_U = 1;
                    }
                    else if (strcmp(lexemas[index + 1], "O") == 0) sig_O = 1;
                    else if (strcmp(lexemas[index + 1], "U") == 0) sig_U = 1;
                }

                if (strncmp(pos, "EO", 2) == 0 || strncmp(pos, "EU", 2) == 0) {
                    strcat(nueva, lexemas[index]); // Transforma en id o num
                    index++; //Consume id o num
                } 
                else {
                    if (sig_O) strcat(nueva, "EO");
                    else if (sig_U) strcat(nueva, "EU");
                    else {
                        strcat(nueva, lexemas[index]); // Fin de expresión
                        index++;
                    }
                }
            }
            // 6. O y Y: Operadores aritmeticos
            else if (*pos == 'O') {
                if (strcmp(palabras[index], "+") == 0) strcat(nueva, "+E");
                else strcat(nueva, "-E");
                index++;
            }
            else if (*pos == 'U') {
                if (strcmp(palabras[index], "*") == 0) strcat(nueva, "*E");
                else strcat(nueva, "/E");
                index++;
            }

            // Copiamos el resto de la cadena
            strcat(nueva, pos + 1);
            strcpy(derivacion, nueva);
            printf("%s\n", derivacion);
        }
    }
}

// Variables globales para TAC
int cont_temp = 1;
int cont_etiqueta = 1;

void evaluarExpresionCompleja(int inicio, int fin, char* destino) {
    char exp[50][30];
    int len = 0;

    // PASO 1: Copiar la expresión y pedir variables desconocidas
    for (int i = inicio; i <= fin; i++) {
        if (strcmp(lexemas[i], "id") == 0) {
            char valorIngresado[30];
            // Aquí puedes buscar en tu tabla de símbolos el tipo de dato real
            // Por ahora, simulamos la petición al usuario
            printf(">> Ingrese el valor para la variable '%s': ", palabras[i]);
            scanf("%s", valorIngresado);
            
            // Reemplazamos el 'id' directamente por el número ingresado
            strcpy(exp[len], valorIngresado); 
        } else {
            strcpy(exp[len], palabras[i]);
        }
        len++;
    }

    // PASO 2: Resolver Multiplicaciones y Divisiones (Prioridad Alta)
    for (int i = 0; i < len; i++) {
        if (strcmp(exp[i], "*") == 0 || strcmp(exp[i], "/") == 0) {
            // Generamos TAC
            printf("t%d = %s %s %s\n", cont_temp, exp[i-1], exp[i], exp[i+1]);
            
            // El resultado ocupa el lugar del primer operando
            sprintf(exp[i-1], "t%d", cont_temp++);
            
            // Recorremos el arreglo 2 posiciones a la izquierda para borrar el operador y el segundo operando
            for (int j = i; j < len - 2; j++) {
                strcpy(exp[j], exp[j+2]);
            }
            len -= 2;
            i--; // Retrocedemos el índice para no saltarnos operadores adyacentes
        }
    }

    // PASO 3: Resolver Sumas y Restas (Prioridad Baja)
    for (int i = 0; i < len; i++) {
        if (strcmp(exp[i], "+") == 0 || strcmp(exp[i], "-") == 0) {
            // Generamos TAC
            printf("t%d = %s %s %s\n", cont_temp, exp[i-1], exp[i], exp[i+1]);
            
            sprintf(exp[i-1], "t%d", cont_temp++);
            
            for (int j = i; j < len - 2; j++) {
                strcpy(exp[j], exp[j+2]);
            }
            len -= 2;
            i--; 
        }
    }

    // PASO 4: Asignación final
    printf("%s = %s\n", destino, exp[0]);
}

void generarCodigoIntermedio() {
    printf("\n------------ Codigo de 3 Direcciones (TAC) --------------\n");
    int i = 0;
    
    while (i < k) {
        // Ignoramos tipos de datos al inicio de la línea (int, float, etc.)
        if (strcmp(lexemas[i], "int") == 0 || strcmp(lexemas[i], "float") == 0) {
            i++;
        }

        // Detectar asignación: id = Expresion
        if (strcmp(lexemas[i], "id") == 0 && i + 1 < k && strcmp(lexemas[i+1], "=") == 0) {
            char destino[30];
            strcpy(destino, palabras[i]);
            
            int inicio_expr = i + 2;
            int fin_expr = inicio_expr;
            
            // Buscar dónde termina la expresión (hasta encontrar un punto y coma, otra estructura o el fin del arreglo)
            while (fin_expr < k && strcmp(palabras[fin_expr], ";") != 0 && strcmp(lexemas[fin_expr], "if") != 0) {
                fin_expr++;
            }
            fin_expr--; // Retrocedemos uno para no incluir el ';' o la siguiente palabra
            
            // Llamamos a nuestra función destructora de expresiones
            evaluarExpresionCompleja(inicio_expr, fin_expr, destino);
            
            i = fin_expr + 1; // Avanzamos el cursor principal
            if (i < k && strcmp(palabras[i], ";") == 0) i++; // Saltamos el punto y coma si existe
        }
        else {
            i++; // Si no es asignación, avanzamos (aquí meteremos el IF y el FOR luego)
        }
    }
    printf("\n---------------------------------------------------------\n");
}

int main(){
    obtenerRutaArchivo();
    obtencionContenidoArchivo();
    separarPalabrasPorEspacios();
    tokenizarPalabras();
    mostrarTablaDeSimbolos();
    clasificadorDeCasos();
    derivador();
    generarCodigoIntermedio();
    return 0;
}