#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <time.h>

//generator settings

#define LEN 4000                            //string size
#define N_STRINGS 2                         //N of strings to split from the original string
#define L_CONF_X 5                          //X config size (bigger means more precision but also more noise) (must be >= L_MOD_Y)
#define L_CONF_Y 30                         //Y config size (bigger means more precision but also more noise) (must be >= L_MOD_Y)
#define L_MOD_X 5                           //X model size (bigger means more precision but also more noise)
#define L_MOD_Y 30                          //Y model size (bigger means more precision but also more noise)
#define CONF_MULT 0.025                     //configuration multiplier used to calculate the multipliers for the configuration (should be the same in the validation)
#define MAX_MULT 1                          //max multiplier

/**


This is the model and configuration generator program for RanDetect.
Casually developed by fiustif and Nick806 in 2023.

Suggested settings for 4000 char dataset and 2000 char test sample:

LEN:        4000
N_STRINGS:  2
L_CONF_X:   5
L_CONF_Y:   30
L_MOD_X:    L_CONF_X
L_MOD_Y:    L_CONF_Y
CONF_MULT:  2


**/

char input[LEN];                                                            //input dataset
double data [N_STRINGS][LEN/N_STRINGS][LEN/N_STRINGS] = {0};                //raw model [N of rep][rep dist to next rep N]
double mult [L_CONF_X][L_CONF_Y] = {0};                                   //contains configuration multipliers for the verification
float Dist;

void generate_model()                       //model generation
{
    int i, x, k;                            //for cycles variables
    int c1 = 0;                             //rep len counter
    int c2 = 0;                             //distance counter
    char s1 = input[0];                     //last char
    bool B;                                 //is repeated (used in the first cycle)
    for(k=0;k<N_STRINGS;k++)
    for(i=1;i<LEN/N_STRINGS;i++)            //find i rep
    {
        c1 = 0;
        c2 = 0;
        B = 0;
        for(x=1+(k*LEN/N_STRINGS);x<(k+1)*LEN/N_STRINGS;x++)
        {
            c2++;
            if(input[x]==s1)
            {
                c1++;
            }
            else
            {
                if(c1==i)
                {
                    if (B == 1)
                    {
                        data[k][i-1][c2-i]++;
                        c2=0;
                    }
                    else
                    {
                        B = 1;
                    }
                }
                c1=0;
                s1=input[x];
            }
        }
    }
    return;
}

void generate_multipliers()                      //generate multipliers for the verification
{
    int i, x, k;
    for(i=0;i<L_CONF_X;i++)                 //calculate average absolute delta between reps
    {
        for(x=0;x<L_CONF_Y;x++)
        {
            for(k=0;k<N_STRINGS;k++)
            {
                mult[i][x] += fabs(data[0][i][x]-data[k][i][x]);
            }
            if(CONF_MULT*(mult[i][x]/N_STRINGS)>MAX_MULT)
            mult[i][x] = MAX_MULT;
            else
            mult[i][x] = CONF_MULT*(mult[i][x]/N_STRINGS);
        }
    }
    return;
}

void normalize_model()
{
    int i, x, k;
    for(i=0;i<LEN/N_STRINGS;i++)                 //calculate average absolute delta between reps
    {
        for(x=0;x<LEN/N_STRINGS;x++)
        {
            for(k=0;k<N_STRINGS;k++)
            {
                data[0][i][x]+=data[k][i][x];
            }
            data[0][i][x]=data[0][i][x]/N_STRINGS;
        }
    }
}

void save_to_file()
{
    int i, j, k;
    time_t now1 = time(NULL);
    struct tm *t1 = localtime(&now1);
    char folder_name[100];
    strftime(folder_name, sizeof(folder_name), "Model-%Y%m%d-%H%M%S", t1);
    struct stat st1 = {0};
    if (stat(folder_name, &st1) == -1)
    {
        #ifdef _WIN32
        mkdir(folder_name);
        #else
        mkdir(folder_name, 0700);
        #endif
    }

    char file_path[100];

    snprintf(file_path, sizeof(file_path), "%s/Model.r4nd", folder_name);
    FILE *Model = fopen(file_path, "w");
        if (Model) {
            for (j = 0; j < L_MOD_X; j++) {
                for (k = 0; k < L_MOD_Y; k++){
                    fprintf(Model, "%lf, ", data[0][j][k]);
                }
                fprintf(Model, "\n");
            }
            fprintf(Model, "\n");
            fclose(Model);
        }

    snprintf(file_path, sizeof(file_path), "%s/Multipliers.r4nd", folder_name);
    FILE *Conf = fopen(file_path, "w");
    if (Conf) {
        for (j = 0; j < L_CONF_X; j++) {
            for (k = 0; k < L_CONF_Y; k++) {
                fprintf(Conf, "%lf, ", mult[j][k]);
            }
            fprintf(Conf, "\n");
        }
        fprintf(Conf, "\n");
        fclose(Conf);
    }

    snprintf(file_path, sizeof(file_path), "%s/Alignment.r4nd", folder_name);
    FILE *Allign = fopen(file_path, "w");
    if (Allign) {
        fprintf(Allign, "%lf ", Dist);
    }

    return;
}

void align_model ()    //needed for non optimized configurations
{
    int i, x, k;
    for(k=1;k<N_STRINGS;k++)
    for(i=0;i<L_MOD_X;i++)
    {
        for(x=0;x<L_MOD_Y;x++)
        {
            Dist += fabs(data[0][i][x] - data[k][i][x])*mult[i][x]; //calculate distance between the strings and the generated dataset
        }
    }
    Dist = Dist/N_STRINGS;
}

void validateconfig()
{
    if  (LEN%N_STRINGS!=0)
    {
        printf("[*] [ERROR] Invalid configuration detected, the sample length must be divisible by the number of strings.");
        abort();
    }
    if  (L_CONF_X < L_MOD_X || L_CONF_Y < L_MOD_Y)
    {
        printf("[*] [ERROR] Invalid configuration detected, the size of the config array must be bigger or equal to the model size.");
        abort();
    }
}

int main()
{
    char c;
    char* banner =
        "   ___            ___      __          __ \n"
        "  / _ \\___ ____  / _ \\___ / /____ ____/ /_\n"
        " / , _/ _ `/ _ \\/ // / -_) __/ -_) __/ __/\n"
        "/_/|_|\\_,_/_//_/____/\\__/\\__/\\__/\\__/\\__/ mGen 1.3.1\n\n";

    printf("%s", banner);

    validateconfig();

    printf("[*] Enter string (%d chars): ",LEN);
    fgets(input, LEN, stdin);
    printf("\n");

    printf("[*] Generating statistic model...");
    generate_model();
    printf("    [DONE]\n");

    printf("[*] Generating multipliers...    ");
    generate_multipliers();
    printf("    [DONE]\n");

    printf("[*] Normalizing model...         ");
    normalize_model();
    printf("    [DONE]\n");

    printf("[*] Aligning model...            ");
    align_model();
    printf("    [DONE]\n");

    printf("\n[*] [INFO] alignment factor: [%lf]\n\n", Dist);

    printf("[*] Saving statistic model...    ");
    save_to_file();
    printf("    [DONE]\n");

    return 0;
}
