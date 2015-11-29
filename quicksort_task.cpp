#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM 32767                                             // Elementanzahl

// ---------------------------------------------------------------------------
// Vertausche zwei Zahlen im Feld v an der Position i und j

void swap(float *v, int i, int j)
{
    float t = v[i]; 
    v[i] = v[j];
    v[j] = t;
}

// ---------------------------------------------------------------------------
// Serielle Version von Quicksort (Wirth) 

void quicksort(float *v, int start, int end) 
{
    int i = start, j = end;
    float pivot;

    pivot = v[(start + end) / 2];                         // mittleres Element
    do {
        while (v[i] < pivot)
            i++;
        while (pivot < v[j])
            j--;
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j);
            i++;
            j--;
        }
   } while (i <= j);
   if (start < j)                                        // Teile und herrsche
       quicksort(v, start, j);                      // Linkes Segment zerlegen
   if (i < end)
       quicksort(v, i, end);                       // Rechtes Segment zerlegen
}

// ---------------------------------------------------------------------------
// Parallele Version von Quicksort (Wirth) 

void quicksort_parallel(float *v, int start, int end) 
{
    int i = start, j = end;
    float pivot;

    pivot = v[(start + end) / 2];                         // mittleres Element
    do {
        while (v[i] < pivot)
            i++;
        while (pivot < v[j])
            j--;
		
        if (i <= j) {               // wenn sich beide Indizes nicht beruehren
            swap(v, i, j);
            i++;
            j--;
        } 
   } while (i <= j);
   #pragma omp task 
   {
   if (start < j)                                        // Teile und herrsche
       quicksort(v, start, j); }                      // Linkes Segment zerlegen
   #pragma omp task 
   {
	   if (i < end)
       quicksort(v, i, end);       }                // Rechtes Segment zerlegen
}

// ---------------------------------------------------------------------------
// Hauptprogramm

int main(int argc, char *argv[])
{
    float *v, *w;                                                    // Felder
    int iter;                                                // Wiederholungen
    int errors = 0;            // speichert Anzahl aller falschen Feldelemente
    if (argc != 2) {                                      // Benutzungshinweis
        printf ("Vector sorting\nUsage: %s <NumIter>\n", argv[0]); 
        return 0;
    }
    iter = atoi(argv[1]);                               
    v = (float *) calloc(NUM, sizeof(float));          // Speicher reservieren
    w = (float *) calloc(NUM, sizeof(float));      // Speicher für Kopie von v

    double startTime, endTime, parallelTime=0.0, serialTime=0.0;
    printf("Perform vector sorting %d times...\n", iter);
    for (int i = 0; i < iter; i++) {               // Wiederhole das Sortieren
        for (int j = 0; j < NUM; j++) {    // Mit Zufallszahlen initialisieren
            v[j] = (float)rand();
            w[j] = v[j];    // damit beide Algotihmen gleiche Mengen sortieren
        }

        startTime = omp_get_wtime();
        quicksort(v, 0, NUM-1);                         // serielle Sortierung
        endTime = omp_get_wtime();
        serialTime += endTime - startTime;

        startTime = omp_get_wtime();
        quicksort_parallel(w, 0, NUM-1);               // parallele Sortierung
        endTime = omp_get_wtime();
        parallelTime += endTime - startTime;

        for(int j = 0; j < NUM; j++) {          // falsche Elemente aufrechnen
            if(v[j] != w[j])
                errors++;
        }
    }

    printf("\n%d errors in total\n",errors);
    printf("\nSerial algorithm took %f seconds\n"
        "Parallel algorithm took %f seconds\n",
        serialTime, parallelTime);
    printf("\nDone.\n");
    return 0;
}