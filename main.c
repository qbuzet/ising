#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>

/*
 * Attention, on se place dans les conditions BVK dans la suite du programme
 */

//Constante de la simulation
#define SEED 0
#define NB_LIGNES 10
#define NB_COLONNES 10
#define SPIN_DEF 1 //0 = GRILLE ALEATOIRE, 1 = GRILLE UP, -1 = GRILLE DOWN
#define NB_ITERATIONS 100000

#define J 1.0
#define kB 1.0

#define T_MIN 0.1
#define T_MAX 5 
#define NB_TEMPERATURES 100

#define FILENAME "out.txt"

int randint(int a, int b);
double rand01();

int test_alea(int argc, char* argv[]){
    if(SEED == 0){
        srand(time(NULL));
    } else {
        srand(SEED);
    }

    FILE* file = fopen(FILENAME, "w+");
    if (!file) {
        printf("Impossible d'ouvrir le fichier");
    }

    fprintf(file, "#t double int \n");
    for(int t=0; t < NB_TEMPERATURES; t++){
        double d = rand01();
        int i = randint(1,5);
        fprintf(file, "%d %f %d \n", t, d, i);
    }

    fclose(file);

    return 0;
}

int test_bord(int argc, char* argv[]){
    int d[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            printf("%d", -2%3);
            
            for(int k = 0; k < 4; k++){
                int iv = (i + d[k][0]) % 3;
                int jv = (j + d[k][1]) % 3;

                if(iv < 0) iv += 3;
                if(jv < 0) jv += 3;

                printf("(%d %d) ", iv,jv);
            }
            printf("\n");
        }
    }

    return 0;
}

int main(int argc, char* argv[]){
    //Initialisation du générateur de nombre aléatoire
    if(SEED == 0){
        srand(time(NULL));
    } else {
        srand(SEED);
    }

    //Initialisation des tableaux contenant les valeurs successives de l'énergie moyenne et l'aimantation moyenne à T fixé
    double tabEMoy[NB_TEMPERATURES];
    double tabMMoy[NB_TEMPERATURES];
    double tabCvMoy[NB_TEMPERATURES];
    double tabChiMoy[NB_TEMPERATURES];

    for(int t=0; t < NB_TEMPERATURES; t++){
        double T = T_MIN + t * (T_MAX - T_MIN) / NB_TEMPERATURES;

        //Initialisation de la grille
        int grille[NB_LIGNES][NB_COLONNES];

        for(int i = 0; i < NB_LIGNES; i++){
            for(int j = 0; j < NB_COLONNES; j++){
                if(SPIN_DEF == 0){
                    int spin = (rand() % 2 == 0) ? -1 : 1;
                    grille[i][j] = spin;
                } else {
                    grille[i][j] = SPIN_DEF;
                }
            }
        }

        //Affichage de la grille
        /*for(int i = 0; i < NB_LIGNES; i++){
            for(int j = 0; j < NB_COLONNES; j++){
                printf("%d ", grille[i][j]);
            }
            printf("\n");
        }*/

        //Initialisation des tableaux contenant les valeurs successives de l'énergie et l'aimantation
        double tabE[NB_ITERATIONS+1];
        double tabM[NB_ITERATIONS+1];

        double EMoy = 0;
        double E2Moy = 0;
        double MMoy = 0;
        double M2Moy = 0;

        //Cases adjacentes
        int d[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};

        //Initialisation de l'énergie
        double E = 0;
        for(int i = 0; i < NB_LIGNES; i++){
            for(int j = 0; j < NB_COLONNES; j++){
                
                for(int k = 0; k < 4; k++){
                    int iv = (i + d[k][0]) % NB_LIGNES;
                    int jv = (j + d[k][1]) % NB_COLONNES;

                    if(iv < 0) iv += NB_LIGNES;
                    if(jv < 0) jv += NB_COLONNES;

                    E -= J * grille[i][j] * grille[iv][jv];
                }
            }
        }

        //Initialisation de l'aimantation
        double M = 0;
        for (int i = 0; i < NB_LIGNES; i++) {
            for (int j = 0; j < NB_COLONNES; j++) {
                M += grille[i][j];
            }
        }

        //On stocke les valeurs initiales
        tabE[0] = E;
        tabM[0] = M;

        for(int n = 1; n <= NB_ITERATIONS; n++){

            //Choix d'un spin à renverser
            int spin_ligne = randint(0, NB_LIGNES);
            int spin_colonne = randint(0, NB_COLONNES);
            //printf("Spin à renverser : %d %d\n", spin_ligne, spin_colonne);

            //Calcul de l'énergie de la nouvelle configuration
            double nE = E;
            for(int k = 0; k < 4; k++){
                int iv = (spin_ligne + d[k][0]) % NB_LIGNES;
                int jv = (spin_colonne + d[k][1]) % NB_COLONNES;

                if(iv < 0) iv += NB_LIGNES;
                if(jv < 0) jv += NB_COLONNES;

                //printf("%d %d ", grille[spin_ligne][spin_colonne], grille[iv][jv]);
                nE -= (double) -2 * J * grille[spin_ligne][spin_colonne] * grille[iv][jv]; //2* car il faut ajouter également l'énergie déjà présente
            }

            //Calcul de l'aimantation de la nouvelle configuration
            int nM = M - 2 * grille[spin_ligne][spin_colonne]; //il faut soustraire en plus l'état d'aimantation actuel de la case 

            double DE = nE - E;
            if(DE < 0){
                //Renversement du spin
                grille[spin_ligne][spin_colonne] = -grille[spin_ligne][spin_colonne];
                E = nE;
                M = nM;
            } else {
                double x = rand01();
                double seuil = exp(- DE / (kB * T));
                if(x < seuil){
                    //On accepte la nouvelle configuration
                    grille[spin_ligne][spin_colonne] = -grille[spin_ligne][spin_colonne];
                    E = nE;
                    M = nM;
                } else {
                    //On rejette la nouvelle configuration (rien à faire)
                }
            }

            //On stocke les nouvelles valeurs de l'énergie et de l'aimantation
            tabE[n] = E;
            tabM[n] = M;
        }

        //Calcul de l'énergie et de l'aimantation moyenne
        for (int n = 0; n < NB_ITERATIONS + 1; n++) {
            EMoy += (double) tabE[n];
            E2Moy += (double) tabE[n] * tabE[n];
            MMoy += (double) tabM[n];
            M2Moy += (double) tabM[n] * tabM[n];

        }
        EMoy /= (double) (NB_ITERATIONS + 1);
        E2Moy /= (double) (NB_ITERATIONS + 1);
        MMoy /= (double) (NB_ITERATIONS + 1);
        M2Moy /= (double) (NB_ITERATIONS + 1);

        //Normalisation
        EMoy /= 4 * NB_LIGNES * NB_COLONNES;
        E2Moy /= (4 * NB_LIGNES * NB_COLONNES) * (4 * NB_LIGNES * NB_COLONNES);
        MMoy /= NB_LIGNES * NB_COLONNES;
        M2Moy /= (NB_LIGNES * NB_COLONNES) * (NB_LIGNES * NB_COLONNES);

        tabEMoy[t] = EMoy;
        tabMMoy[t] = MMoy;
        tabCvMoy[t] = (E2Moy - EMoy * EMoy) / ((kB * T) * (kB * T));
        tabChiMoy[t] = (M2Moy - MMoy * MMoy) / (kB * T);


        //Ecriture de l'echantillonnage de l'énergie et de l'aimantation moyenne dans le fichier de sortie
        char name_E[10];
        char name_M[10];
        sprintf(name_E, "E_%0.2f", T);
        sprintf(name_M, "M_%0.2f", T);
        
        FILE* file_E = fopen(name_E, "w+");
        FILE* file_M = fopen(name_M, "w+");
        if (!file_E) {
            printf("Impossible d'ouvrir le fichier");
        }
        if (!file_M) {
            printf("Impossible d'ouvrir le fichier");
        }

        fprintf(file_E, "#n E\n");
        fprintf(file_M, "#n M\n");
        for (int n = 0; n <= NB_ITERATIONS; n++) {
            fprintf(file_E, "%d %f\n", n, tabE[n]);
            fprintf(file_M, "%d %f\n", n, tabM[n]);
        }

        fclose(file_E);
        fclose(file_M);
    }

    //Ecriture des résultats dans le fichier de sortie
    FILE* file = fopen(FILENAME, "w+");
    if (!file) {
        printf("Impossible d'ouvrir le fichier");
    }

    fprintf(file, "#T EMoy MMoy CvMoy ChiMoy\n");
    for(int t=0; t < NB_TEMPERATURES; t++){
        double T = T_MIN + t * (T_MAX - T_MIN) / NB_TEMPERATURES;
        fprintf(file, "%f %f %f %f %f\n", T, tabEMoy[t], tabMMoy[t], tabCvMoy[t], tabChiMoy[t]);
    }

    fclose(file);

    return 0;
}

//Variable aléatoire uniforme entière entre a et b exclus
int randint(int a, int b){
    return (int) (a + (double) (b - a) * ((double) rand() / RAND_MAX));
}

//Variable aléatoire uniforme réelle dans l'intervalle [0,1]
double rand01(){
    return (double) rand() / RAND_MAX;
}