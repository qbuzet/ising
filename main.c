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
#define NB_LIGNES 5
#define NB_COLONNES 5
#define SPIN_DEF 0 //0 = GRILLE ALEATOIRE, 1 = GRILLE UP, -1 = GRILLE DOWN
#define NB_ITERATIONS 1000

#define J 1.0
#define kB 1.0

#define T_MIN 1.0
#define T_MAX 5.0
#define NB_TEMPERATURES 100

#define FILENAME "./out.txt"

int randint(int a, int b);
double rand01();

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
        int tabE[NB_ITERATIONS+1];
        int tabM[NB_ITERATIONS+1];

        double EMoy = 0;
        double MMoy = 0;

        //Cases adjacentes
        int d[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};

        //Initialisation de l'énergie
        int E = 0;
        for(int i = 0; i < NB_LIGNES; i++){
            for(int j = 0; j < NB_COLONNES; j++){
                
                for(int k = 0; k < 4; k++){
                    int iv = (i + d[k][0]) % NB_LIGNES;
                    int jv = (j + d[k][1]) % NB_COLONNES;
                    E -= grille[i][j] * grille[iv][jv];
                }
            }
        }

        //Initialisation de l'aimantation
        int M = 0;
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
            int spin_ligne = randint(0, NB_LIGNES-1); //Exclu ici
            int spin_colonne = randint(0, NB_COLONNES-1); //Exclu ici
            //printf("Spin à renverser : %d %d\n", spin_ligne, spin_colonne);

            //Calcul de l'énergie de la nouvelle configuration
            int nE = E;
            for(int k = 0; k < 4; k++){
                int iv = (spin_ligne + d[k][0]) % NB_LIGNES;
                int jv = (spin_colonne + d[k][1]) % NB_COLONNES;
                nE -= (-2*grille[spin_ligne][spin_colonne]) * grille[iv][jv]; //2* car il faut ajouter également l'énergie déjà présente
            }

            //Calcul de l'aimantation de la nouvelle configuration
            int nM = M - 2*grille[spin_ligne][spin_colonne]; //il faut soustraire en plus l'état d'aimantation actuel de la case 

            int DE = nE - E;
            if(DE < 0){
                //Renversement du spin
                grille[spin_ligne][spin_colonne] = -grille[spin_ligne][spin_colonne];
                E = nE;
                M = nM;
            } else {
                int x = rand01();
                if(x < exp((-1.0) * (double) (DE / (kB * T)))){
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
            MMoy += (double )tabM[n];
        }
        EMoy /= (double)(NB_ITERATIONS + 1);
        MMoy /= (double)(NB_ITERATIONS + 1);

        tabEMoy[t] = EMoy;
        tabMMoy[t] = MMoy;

        //Ecriture de l'echantillonnage de l'énergie et de l'aimantation moyenne dans le fichier de sortie
        /*FILE* file = fopen(FILENAME, "w+");
        if (!file) {
            printf("Impossible d'ouvrir le fichier");
        }

        fprintf(file, "#n E M \n");
        for (int n = 0; n <= NB_ITERATIONS; n++) {
            fprintf(file, "%d %d %d \n", n, tabE[n], tabM[n]);
        }

        fclose(file);
        */
    }

    //Ecriture des résultats dans le fichier de sortie
    FILE* file = fopen(FILENAME, "w+");
    if (!file) {
        printf("Impossible d'ouvrir le fichier");
    }

    fprintf(file, "#T EMoy MMoy \n");
    for(int t=0; t < NB_TEMPERATURES; t++){
        double T = T_MIN + t * (T_MAX - T_MIN) / NB_TEMPERATURES;
        fprintf(file, "%f %f %f \n", T, tabEMoy[t], tabMMoy[t]);
    }

    fclose(file);

    return 0;
}

//Variable aléatoire uniforme entière entre a et b inclus
int randint(int a, int b){
    return (int) (a + (double) (b - a) * ((double) rand() / RAND_MAX)) + 1;
}

//Variable aléatoire uniforme réelle dans l'intervalle [0,1]
double rand01(){
    return (double) rand() / RAND_MAX;
}