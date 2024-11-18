#include <stdio.h>


typedef struct {
    char character;
    double probability;
    int count;
} CharInfo;

int main() {
    FILE *inFile = fopen("input.txt", "rb");
    if (inFile == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    int charCounts[256] = {0};
    int totalChars = 0;
    int cha;
    while ((cha = fgetc(inFile)) != EOF) {
        charCounts[cha]++;
        totalChars++;
    }
    fclose(inFile);

    CharInfo charInfos[256];
    int uniChars = 0;
    for (int i = 0; i < 256; i++) {
        if (charCounts[i] > 0) {
            charInfos[uniChars].character = (char)i;
            charInfos[uniChars].count = charCounts[i];
            charInfos[uniChars].probability = (double)charCounts[i] / totalChars;
            uniChars++;
        }
    }

    FILE *outFile = fopen("output.csv", "w");
    if (outFile == NULL) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    int Max = 0;
    for (int k = 0; k < uniChars; k++) {
        for (int i = 0; i < uniChars; i++) {
            if (charInfos[i].count > Max) {
                Max = i;
            }
        }

        fprintf(outFile, "%c,%d,%.15f\n", charInfos[Max].character, charInfos[Max].count, charInfos[Max].probability);
        charInfos[Max].count = 0;
        Max = 0;
    }

    fclose(outFile);
    return EXIT_SUCCESS;
}