#include "file/shop.h"

void storeUserData(const Data* userData)
{
    FILE* file = fopen("userdata.txt", "w");
    if (file == NULL) {
        printf("Failed to open userdata file.\n");
        return;
    }

    fprintf(file, "%s;%f;%d;%d\n", mpz_get_str(NULL, 10, userData->score), userData->multiplier,
            userData->level, userData->isNewbie);

    fclose(file);
}