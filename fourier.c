#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define NMAX 200      /* nombre maximum d'harmoniques */
#define POINTS 20000  /* nombre de points pour calculer les integrales */

/* Ramene t dans [-PI, PI[ (les signaux sont de periode 2*PI) */
double ramener(double t) {
    double x = fmod(t + PI, 2 * PI);
    if (x < 0) x += 2 * PI;
    return x - PI;
}

/* Signal choisi : 1 = carre, 2 = dent de scie, 3 = triangle */
double signal(double t, int choix) {
    double x = ramener(t);
    if (choix == 1) return (x >= 0) ? 1.0 : -1.0;
    if (choix == 2) return x;
    return fabs(x);
}

/* Calcul de a0, an et bn avec la methode du point milieu sur [-PI, PI] */
void calculer_coefficients(int choix, int N, double *a0, double a[], double b[]) {
    double h = 2 * PI / POINTS;

    for (int n = 0; n <= N; n++) {
        double somme_a = 0, somme_b = 0;

        for (int i = 0; i < POINTS; i++) {
            double t = -PI + (i + 0.5) * h;
            double f = signal(t, choix);

            somme_a += f * cos(n * t);
            somme_b += f * sin(n * t);
        }

        if (n == 0) {
            *a0 = somme_a * h / PI;
        } else {
            a[n] = somme_a * h / PI;
            b[n] = somme_b * h / PI;
        }
    }
}

/* Reconstruit le signal avec les coefficients calcules */
double serie(double t, int N, double a0, double a[], double b[]) {
    double s = a0 / 2.0;
    for (int n = 1; n <= N; n++) {
        s += a[n] * cos(n * t) + b[n] * sin(n * t);
    }
    return s;
}

int main() {
    int choix, N;
    double a0;
    double a[NMAX + 1], b[NMAX + 1];

    printf("Choisis le signal :\n");
    printf("1 - Signal carre\n");
    printf("2 - Dent de scie\n");
    printf("3 - Signal triangle\n");
    printf("Ton choix : ");
    scanf("%d", &choix);

    printf("Nombre d'harmoniques N (max %d) : ", NMAX);
    scanf("%d", &N);

    if (choix < 1 || choix > 3 || N < 1 || N > NMAX) {
        printf("Valeurs incorrectes.\n");
        return 1;
    }

    calculer_coefficients(choix, N, &a0, a, b);

    /* Affichage des coefficients */
    printf("\na0 = %f\n", a0);
    printf("  n        an          bn\n");
    for (int n = 1; n <= N; n++) {
        printf("%3d  %10.6f  %10.6f\n", n, a[n], b[n]);
    }

    /* Fichier des courbes */
    FILE *f = fopen("fourier.csv", "w");
    if (f == NULL) {
        printf("Erreur : impossible de creer le fichier.\n");
        return 1;
    }

    fprintf(f, "t,exact,fourier\n");
    for (double t = -2 * PI; t <= 2 * PI; t += 0.01) {
        fprintf(f, "%f,%f,%f\n", t, signal(t, choix), serie(t, N, a0, a, b));
    }
    fclose(f);

    printf("\nFichier fourier.csv cree avec succes.\n");
    return 0;
}