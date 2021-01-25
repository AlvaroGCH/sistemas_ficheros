#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps){ // Imprime el mapeo de Bytemaps
	int i;
	printf("Inodos :");
	for(i=0;i<MAX_INODOS;i++){
		printf("%d ", ext_bytemaps->bmap_inodos[i]);
	}
	printf("\nBloques :");
        for(i=0;i<MAX_BLOQUES_PARTICION;i++){
                printf("%d ", ext_bytemaps->bmap_inodos[i]);
        }
	printf("\n");
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2){ // Comprueba el comando y sus argumentos
	char *buscar_espacio, *aux;
	int tam;
	if(strstr(strcomando,"info")!= NULL){
		strncpy(orden,"info",4);
		orden[4] = '\0';
	}
	else if(strstr(strcomando,"bytemaps")!= NULL){
                strncpy(orden,"bytemaps",8);
		orden[8] = '\0';
        }
	else if(strstr(strcomando,"dir")!= NULL){
                strncpy(orden,"dir",3);
		orden[3] = '\0';
        }
	else if(strstr(strcomando,"rename")!= NULL){
                strncpy(orden,"rename",6);
		orden[6] = '\0';
        }
	else if(strstr(strcomando,"imprimir")!= NULL){
                strncpy(orden,"imprimir",8);
		orden[8] = '\0';
        }
	else if(strstr(strcomando,"remove")!= NULL){
                strncpy(orden,"remove",6);
		orden[6] = '\0';
        }
	else if(strstr(strcomando,"copy")!= NULL){
                strncpy(orden,"copy",4);
		orden[4] = '\0';
        }
	else if(strstr(strcomando,"salir")!= NULL){
                strncpy(orden,"salir",5);
		orden[5] = '\0';
        }
	else{
		printf("ERROR: Comando ilegal [bytemaps,copy,dir,info,imprimir,rename,remove,salir]\n");
		return 1;
	}
	buscar_espacio = strrchr(strcomando, ' ');
	if(buscar_espacio != NULL){
		aux = buscar_espacio;
		buscar_espacio = strchr(buscar_espacio+1,' ');
		if(buscar_espacio == NULL)
			buscar_espacio = strchr(strcomando, '\n');
		if(buscar_espacio-aux != 0){
			tam = buscar_espacio -(aux+1);
			strncpy(argumento1, aux+1,tam);
			argumento1[tam] = '\0';
			aux = buscar_espacio;
			buscar_espacio = strchr(buscar_espacio+1,'\n');
		}
		if(buscar_espacio-aux != 0 && buscar_espacio != NULL){
			tam = buscar_espacio -(aux+1);
			strncpy(argumento2, aux+1, tam);
			argumento2[tam] = '\0';
		}
	}
	return 0;
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup){ // imprime el contenido del superbloque
	printf("Bloque %d bytes\ninodos particion = %d\ninodos libre = %d\nBloques particion = %d\nBloques libres = %d\nPrimer bloque de datos = %d \n",psup->s_block_size, psup->s_inodes_count, psup->s_free_inodes_count, psup->s_blocks_count, psup->s_free_blocks_count, psup->s_first_data_block);
}

int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre){ // Busqueda de un fichero
	for(int i = 0; i < MAX_FICHEROS; i++){
		if(strcmp(directorio[i].dir_nfich, nombre)){
			return inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque[0];
		}
	}
	return 0;
}

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos){ // Muestra los ficheros existentes junto con sus detalles
	for(int i = 0; i<MAX_FICHEROS; i++){
		if(directorio[i].dir_inodo != NULL_INODO){
			unsigned short int *i_nbloque = inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque;
			printf("%s", directorio[i].dir_nfich);
			printf("       tamanio: %d", inodos->blq_inodos[directorio[i].dir_inodo].size_fichero);
			printf("       inodos:%d ", directorio[i].dir_inodo);
			printf("bloques: ");
			for(int j = 0; j < MAX_NUMS_BLOQUE_INODO;j++){
				if(i_nbloque[j] != NULL_BLOQUE){
					printf("%d ", i_nbloque[j]);
				}
			}
			printf("\n");
		}
	}
}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo){ // cambia el nombre de un fichero
	int i;
	for(i = 0;i < MAX_FICHEROS;i++){
		if(strcmp(directorio[i].dir_nfich, nombrenuevo)==0){
			return 1;
		}
	}
	for(i = 0;i < MAX_FICHEROS;i++){
                if(strcmp(directorio[i].dir_nfich, nombreantiguo)==0){
                        strcpy(directorio[i].dir_nfich,nombrenuevo);
			return 0;
                }
        }
	return -1;
}

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre){ // imprime el contenido de un fichero
	unsigned short int *i_nbloque;
	for(int i = 0;i < MAX_FICHEROS;i++){
                if(strcmp(directorio[i].dir_nfich, nombre)==0){
			i_nbloque = inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque;
                        for(int j = 0;j < MAX_NUMS_BLOQUE_INODO ;j++){
				if(i_nbloque[j] != NULL_BLOQUE){
					for(int k = 0;k < SIZE_BLOQUE;k++){
						printf("%c", memdatos->dato[SIZE_BLOQUE*(i_nbloque[j] - 4) + 1]);
					}
				}
			}
			printf("\n");
			return 0;
                }
        }
	return 1;
}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich){ // borra un fichero
		for(int i = 0; i < MAX_INODOS; i++){
		if(strcmp(directorio[i].dir_nfich, nombre) == 0){
			int *i_nbloque = inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque;
			for(int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++){
				if(i_nbloque[j] != NULL_BLOQUE){
					ext_bytemaps->bmap_bloques[i_nbloque[j]] = 0;
					i_nbloque[j] = NULL_BLOQUE;
					ext_superblock->s_free_blocks_count++;
				}
			}
			inodos->blq_inodos[directorio[i].dir_inodo].size_fichero = 0;
			ext_superblock->s_free_inodes_count++;
			directorio[i].dir_inodo = NULL_INODO;
			return 0;
		}
	}
	return 1;
}

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich){ // Copia el contenido de un fichero a otro
	EXT_ENTRADA_DIR *directorio_aux;
	EXT_SIMPLE_INODE *inodo;
	EXT_SIMPLE_INODE *inodo_aux;
	for(int i = 0; i < MAX_FICHEROS; i++){
		if(strcmp(directorio[i].dir_nfich, nombreorigen) == 0){
			for(int j = 0; j < MAX_FICHEROS; j++){
                		if(strcmp(directorio[j].dir_nfich, nombredestino) == 0){
					inodo_aux = &(inodos->blq_inodos[directorio[i].dir_inodo]);
                       			for(int k = 0; k < MAX_FICHEROS; k++){
						if(directorio[k].dir_inodo == NULL_INODO){
							directorio_aux = &directorio[k];
							break;
						}
					}
					strcpy(directorio_aux->dir_nfich, nombredestino);
					for(int k = 0; k < MAX_INODOS; k++){
						if(ext_bytemaps->bmap_inodos[k] == 0){
							inodo = &(inodos->blq_inodos[k]);
							directorio_aux->dir_inodo = k;
							ext_bytemaps->bmap_inodos[k] = 1;
							break;
						}
					}
					inodo->size_fichero = inodo_aux->size_fichero;
					ext_superblock->s_free_inodes_count--;
					for(int k = 0; k < MAX_NUMS_BLOQUE_INODO; k++){
						if(inodo_aux->i_nbloque[k] != NULL_BLOQUE){
							for(int h = ext_superblock->s_first_data_block; h < MAX_BLOQUES_DATOS; h++){
								if(ext_bytemaps->bmap_bloques[h] == 0){
									inodo->i_nbloque[k] = h;
									memcpy(&(memdatos->dato[(h-4)*SIZE_BLOQUE]),&(memdatos->dato[-4 + inodo_aux->i_nbloque[k]+SIZE_BLOQUE]),SIZE_BLOQUE);
									ext_superblock->s_free_blocks_count--;
									ext_bytemaps->bmap_bloques[h] = 1;
									return 0;
								}
							}
						}
					}
                		}
        		}
			return 1;
		}
	}
	return -1;
}

void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich){ //Graba en el fichero los inodos y directorios
	EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
        rewind(fich);
	fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
	memcpy(&datosfich[2], inodos, SIZE_BLOQUE);
	memcpy(&datosfich[3], directorio, SIZE_BLOQUE);
	rewind(fich);
	fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
}

void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich){	// Graba los bytesmaps
	EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
        rewind(fich);
        fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
        memcpy(&datosfich[1], ext_bytemaps, SIZE_BLOQUE);
        rewind(fich);
        fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
}

void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich){ // graba los contenidos del superbloque
	EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
        rewind(fich);
        fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
        memcpy(&datosfich[0], ext_superblock, SIZE_BLOQUE);
        rewind(fich);
        fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);

}
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich){	//  Graba todos los datos de todos los ficheros
	EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
        rewind(fich);
        fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
        memcpy(&datosfich[4], memdatos, SIZE_BLOQUE);
        rewind(fich);
        fwrite(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
}

int main(){
	char *comando[LONGITUD_COMANDO];
	char *orden[LONGITUD_COMANDO];
	char *argumento1[LONGITUD_COMANDO];
	char *argumento2[LONGITUD_COMANDO];

	int i,j;
	unsigned long int m;
    	EXT_SIMPLE_SUPERBLOCK ext_superblock;
     	EXT_BYTE_MAPS ext_bytemaps;
     	EXT_BLQ_INODOS ext_blq_inodos;
     	EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
     	EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
     	EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
     	int entradadir;
     	int grabardatos;
     	FILE *fent;

     // Lectura del fichero completo de una sola vez

     	fent = fopen("particion.bin","r+b");
     	fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);
     	memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     	memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     	memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     	memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     	memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);

     // Bucle de tratamiento de comandos
     	for (;;){
		do {
			printf (">> ");
			fflush(stdin);
			fgets(comando, LONGITUD_COMANDO, stdin);
		} while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
	if (strcmp(orden,"dir")==0) {
        	Directorio(&directorio,&ext_blq_inodos);
            	continue;
       	}
	else if(strcmp(orden,"info")==0){
		LeeSuperBloque(&ext_superblock);
	}
	else if(strcmp(orden,"bytemaps")==0){
		Printbytemaps(&ext_bytemaps);
        }
	else if(strcmp(orden,"rename")==0){
        	i = Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
		if(i == 1){
			printf("ERROR: El fichero %s ya existe\n", argumento2);
		}
		else if(i == -1){
			printf("ERROR: Fichero %s n encontrado\n", argumento1);
		}
        }
	else if(strcmp(orden,"imprimir")==0){
        	if(Imprimir(&directorio, &ext_blq_inodos, &memdatos, argumento1)==1){
			printf("ERROR: Fichero %s no encontrado\n", argumento1);
		}
        }
	else if(strcmp(orden,"remove")==0){
                if(Borrar(&directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1,  fent) == 1){
			printf("ERROR: Fichero %s no encontrado\n", argumento1);
		}
        }
	else if(strcmp(orden,"copy")==0){
		i = Copiar(&directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, &memdatos, argumento1, argumento2, fent);
                if(i == 1){
                        printf("ERROR: El fichero destino %s no existe\n", argumento2);
                }
                else if(i == -1){
                        printf("ERROR: El fichero origen %s no existe\n", argumento1);
                }
        }
         // Escritura de metadatos en comandos rename, remove, copy
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         /*if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;*/
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==0){
            GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
     }
}
