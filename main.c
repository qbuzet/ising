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

#define J 1.0
#define kB 1.0

/**
 * 0 Echantillonnage à T constant
 * 1 Etude en température
 * 2 Etude en taille
 **/
#define MODE 1
#define FILENAME_OUT_T "out.txt"
#define FILENAME_TEST "test.txt"

typedef struct EM
{
    double EMoy;
    double MMoy;
    double CvMoy;
    double ChiMoy;
    int** grille;
} EM;

typedef struct
{
    double TcriCv;
    double TcriChi;
} Tcri;

int randint(int a, int b);
double rand01();

/**
 * SPIN_DEF
 * 0 Aléatoire
 * 1 Spin Up
 * -1 Spin Down
 **/
int** init_grille(int NB_LIGNES, int NB_COLONNES, int SPIN_DEF);
void detruire_grille(int NB_LIGNES, int NB_COLONNES, int** grille);

/**
 * OUT_ECH
 * 0 Pas de fichier de sortie avec l'échantillonnage
 * 1 Création d'un fichier de sortie contenant l'échantillonnage
 **/
EM echantillonner(int NB_LIGNES, int NB_COLONNES, int** grille, int NB_ITERATIONS, double T, int OUT_ECH);

//Par défaut OUT_ECH = 0
Tcri simuler_T(int NB_LIGNES, int NB_COLONNES, int NB_ITERATIONS, double T_MIN, double T_MAX, int NB_TEMPERATURES, int SPIN_DEF);

int test_alea(int argc, char* argv[]){
    if(SEED == 0){
        srand(time(NULL));
    } else {
        srand(SEED);
    }

    FILE* file = fopen(FILENAME_TEST, "w+");
    if (!file) {
        printf("Impossible d'ouvrir le fichier");
    }

    fprintf(file, "#t double int \n");

    int NB_TEMPERATURES = 100;
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


    if(MODE == 0){

        int NB_LIGNES = 10;
        int NB_COLONNES = 10;
        int SPIN_DEF = 1;
        int NB_ITERATIONS = 100000;

        double T = 1.0;

        int OUT_ECH = 1;

        int** grille = init_grille(NB_LIGNES,NB_COLONNES, SPIN_DEF);
        echantillonner(NB_LIGNES,NB_COLONNES,grille,NB_ITERATIONS,T,OUT_ECH);

    } else if(MODE == 1){

        int NB_LIGNES = 10;
        int NB_COLONNES = 10;
        int SPIN_DEF = 1;
        int NB_ITERATIONS = 100000;

        double T_MIN = 0.1;
        double T_MAX = 5.0;
        int NB_TEMPERATURES = 100;

        simuler_T(NB_LIGNES, NB_COLONNES, NB_ITERATIONS, T_MIN, T_MAX, NB_TEMPERATURES, SPIN_DEF);

    } else if(MODE == 2) {

        int SPIN_DEF = 1;
        double T_MIN = 0.1;
        double T_MAX = 5.0;
        int NB_TEMPERATURES = 100;

        int lignes[9] = {3,5,10,20,30,50,65,80,100};
        Tcri* tabTcri = (Tcri*) malloc(9 * sizeof(Tcri));

        for (int i = 0; i < 9; i++){
            tabTcri[i] = simuler_T(lignes[i], lignes[i], lignes[i]*lignes[i]*1000, T_MIN, T_MAX, NB_TEMPERATURES, SPIN_DEF);
        }

        FILE* file = fopen("Tcri.txt", "w+");
        if (!file) {
            printf("Impossible d'ouvrir le fichier");
        }
        fprintf(file, "#nbLignes nbColonnes nbIterations TcriCv TcriChi\n");
        for(int i = 0; i < 9; i++){
            fprintf(file, "%d %d %d %f %f\n", lignes[i], lignes[i], lignes[i]*lignes[i]*1000, tabTcri[i].TcriCv, tabTcri[i].TcriCv);
        }
        fclose(file);

    } else {
        printf("Mode inconnue");
    }

    return 0;
}

int** init_grille(int NB_LIGNES, int NB_COLONNES, int SPIN_DEF){
    
    int **grille = malloc(sizeof(int*[NB_LIGNES]));

    if (grille == NULL)
    {
        printf("Échec de l'allocation\n");
        abort();
    }

    for (unsigned i = 0; i < NB_LIGNES; i++)
    {
        grille[i] = malloc(sizeof(int[NB_COLONNES]));

        if (grille[i] == NULL)
        {
            printf("Échec de l'allocation\n");
            abort();
        }
    }

    //Initialisation de la grille
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

    return grille;
}

void detruire_grille(int NB_LIGNES, int NB_COLONNES, int** grille){
    for (unsigned i = 0; i < NB_LIGNES; ++i)
        free(grille[i]);

    free(grille);
}

EM echantillonner(int NB_LIGNES, int NB_COLONNES, int** grille, int NB_ITERATIONS, double T, int OUT_ECH){
     //Initialisation des tableaux contenant les valeurs successives de l'énergie et l'aimantation
    double* tabE = (double*) malloc((NB_ITERATIONS + 1) * sizeof(double));
    double* tabM = (double*) malloc((NB_ITERATIONS + 1) * sizeof(double));

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

    //Ecriture de l'echantillonnage de l'énergie et de l'aimantation moyenne dans le fichier de sortie
    if(OUT_ECH){
        char name_E[32];
        char name_M[32];
        sprintf(name_E, "E/E_%0.2f.ech", T);
        sprintf(name_M, "M/M_%0.2f.ech", T);
        
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

    //Calcul de l'énergie et de l'aimantation moyenne
    double EMoy = 0;
    double E2Moy = 0;
    double MMoy = 0;
    double M2Moy = 0;
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

    double CvMoy = (E2Moy - EMoy * EMoy) / ((kB * T) * (kB * T));
    double ChiMoy = (M2Moy - MMoy * MMoy) / (kB * T);

    EM em = {EMoy, MMoy, CvMoy, ChiMoy, grille};
    em.EMoy = EMoy;
    em.MMoy = MMoy;
    em.CvMoy = CvMoy;
    em.ChiMoy = ChiMoy;
    em.grille = grille;

    free(tabE);
    free(tabM);
        
    return em;
}

Tcri simuler_T(int NB_LIGNES, int NB_COLONNES, int NB_ITERATIONS, double T_MIN, double T_MAX, int NB_TEMPERATURES, int SPIN_DEF){
    int OUT_ECH = 0;

    //Initialisation des tableaux contenant les valeurs successives de l'énergie moyenne et l'aimantation moyenne à T fixé
    double* tabEMoy = (double*) malloc(NB_TEMPERATURES * sizeof(double)) ;
    double* tabMMoy = (double*) malloc(NB_TEMPERATURES * sizeof(double)) ;
    double* tabCvMoy = (double*) malloc(NB_TEMPERATURES * sizeof(double));
    double* tabChiMoy = (double*) malloc(NB_TEMPERATURES * sizeof(double));

    for(int t=0; t < NB_TEMPERATURES; t++){
        double T = T_MIN + t * (T_MAX - T_MIN) / NB_TEMPERATURES;
        printf("Itération : T = %f\n", T);

        //Initialisation de la grille
        int** grille = init_grille(NB_LIGNES,NB_COLONNES,SPIN_DEF);

        EM em = echantillonner(NB_LIGNES, NB_COLONNES, grille, NB_ITERATIONS, T, OUT_ECH);
        tabEMoy[t] = em.EMoy;
        tabMMoy[t] = em.MMoy;
        tabCvMoy[t] = em.CvMoy;
        tabChiMoy[t] = em.ChiMoy;

        detruire_grille(NB_LIGNES, NB_COLONNES, grille);
    }

    //Identification de la température critique
    double TcriCv = 0;
    double Cvmax = 0;
    double TcriChi = 0;
    double Chimax = 0;
    for(int t=0; t < NB_TEMPERATURES; t++){
        double T = T_MIN + t * (T_MAX - T_MIN) / NB_TEMPERATURES;

        if(tabCvMoy[t] > Cvmax){
            Cvmax = tabCvMoy[t];
            TcriCv = T;
        }
        
        if(tabChiMoy[t] > Chimax){
            Chimax = tabChiMoy[t];
            TcriChi = T;
        }
    }

    //Ecriture des résultats dans le fichier de sortie
    FILE* file = fopen(FILENAME_OUT_T, "w+");
    if (!file) {
        printf("Impossible d'ouvrir le fichier");
    }

    fprintf(file, "#Température critique Cv: %f, Chi: %f\n", TcriCv, TcriChi);

    fprintf(file, "#T EMoy MMoy CvMoy ChiMoy\n");
    for(int t=0; t < NB_TEMPERATURES; t++){
        double T = T_MIN + t * (T_MAX - T_MIN) / NB_TEMPERATURES;
        fprintf(file, "%f %f %f %f %f\n", T, tabEMoy[t], tabMMoy[t], tabCvMoy[t], tabChiMoy[t]);
    }
    fclose(file);

    free(tabEMoy);
    free(tabMMoy);
    free(tabCvMoy);
    free(tabChiMoy);

    Tcri tcri = {TcriCv, TcriChi};
    return tcri;
}

//Variable aléatoire uniforme entière entre a et b exclus
int randint(int a, int b){
    return (int) (a + (double) (b - a) * ((double) rand() / RAND_MAX));
}

//Variable aléatoire uniforme réelle dans l'intervalle [0,1]
double rand01(){
    return (double) rand() / RAND_MAX;
}