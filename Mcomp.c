#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

//comparator settings

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

Suggested settings for 8000 char dataset and 2000 char test sample:

LEN:        8000
N_STRINGS:  4
L_CONF_X:   5
L_CONF_Y:   30
L_MOD_X:    L_CONF_X
L_MOD_Y:    L_CONF_Y
L_TRIG:     5
CONF_MULT:  2


**/
FILE* Model;
FILE* Conf;
float Avg_D = 0;
float Max_D = 0;
int Xm = 0;
int Ym = 0;
char input[LEN];                                                            //input dataset
double data [N_STRINGS][LEN/N_STRINGS][LEN/N_STRINGS] = {0};                //raw model [N of rep][rep dist to next rep N]
double config [L_CONF_X][L_CONF_Y] = {0};                                   //contains configuration multipliers for the verification
double C_data [LEN/N_STRINGS][LEN/N_STRINGS] = {0};                         //raw model [N of rep][rep dist to next rep N]
double alig;

void generate_C_model()
{
    int i, x, k;                            //for cycles variables
    int c1 = 0;                             //rep len counter
    int c2 = 0;                             //distance counter
    char s1 = input[0];                     //last char
    bool B;                                 //is repeated (used in the first cycle)

    for(i=1;i<LEN/N_STRINGS;i++)
    {
        c1 = 0;
        c2 = 0;
        B = 0;
        for(x=1;x<LEN/N_STRINGS;x++)
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
                        C_data[i-1][c2-i]++;
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
}

void import_from_file(const char *folderName) {
    int j, k, i;
    char currentDir[100];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
        printf("[*] [ERROR] Unable to find specified model\n");
        return;
    }

    char folderPath[200];
    snprintf(folderPath, sizeof(folderPath), "%s/%s", currentDir, folderName);

    for (int i = 0; i < strlen(folderPath); i++) {
    if (folderPath[i] == '\n') {
        folderPath[i] = '\0';
        break;
    }
}
    char modelFilePath[100];
    snprintf(modelFilePath, sizeof(modelFilePath), "%s/Model.r4nd", folderPath);

    FILE *Model = fopen(modelFilePath, "r");
    if (Model) {
        for (j = 0; j < L_MOD_X; j++) {
            for (k = 0; k < L_MOD_Y; k++) {
                fscanf(Model, "%lf, ", &data[0][j][k]);
                }
            fscanf(Model, "\n");
            }
        fclose(Model);
        printf("[*] Imported model\n");
    }
    else
    {
        printf("\n[*] [ERROR] Could not read model file\n");
        abort();
    }

    char configFilePath[100];
    snprintf(configFilePath, sizeof(configFilePath), "%s/Multipliers.r4nd", folderPath);

        printf("%s",configFilePath);

    FILE *Config = fopen(configFilePath, "r");
    if (Config) {
        for (j = 0; j < L_CONF_X; j++) {
            for (k = 0; k < L_CONF_Y; k++) {
                fscanf(Config, "%lf, ", &config[j][k]);
                }
            fscanf(Config, "\n");
            }
        fclose(Config);
        printf("[*] Imported multipliers\n\n");
    }
    else
    {
        printf("\n[*] [ERROR] Could not read config file\n");
        abort();
    }

    char aligFilePath[100];
    snprintf(aligFilePath, sizeof(aligFilePath), "%s/Alignment.r4nd", folderPath);

    FILE *Align = fopen(aligFilePath, "r");
    if (Align) {
        fscanf(Align, "%lf, ", &alig);
        fclose(Align);
        printf("[*] Imported alignment factor\n\n");
    }
    else
    {
        printf("\n[*] [ERROR] Could not read alignment file\n");
        abort();
    }
}
float distance ()
{
    int i, x;
    float Dist;
    for(i=0;i<L_MOD_X;i++)
    {
        for(x=0;x<L_MOD_Y;x++)
        {
            Dist += fabs(data[0][i][x] - C_data[i][x])*config[i][x]; //calculate distance between the two datasets
        }
    }
    return Dist;
}

void Delt ()
{
    int i, x;
    for(i=0;i<L_MOD_X;i++)
    {
        for(x=0;x<L_MOD_Y;x++)
        {
            Avg_D += fabs(data[0][i][x] - C_data[i][x])*config[i][x]; //calculate distance between the two datasets
            if(fabs(data[0][i][x] - C_data[i][x])>Max_D)
            {
                Max_D = data[0][i][x] - C_data[i][x];
                Xm = i;
                Ym = x;
            }
        }
    }
    Avg_D=Avg_D/(L_MOD_X*L_MOD_Y);
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
    char folderName[100];

    char* banner =
        "   ___            ___      __          __ \n"
        "  / _ \\___ ____  / _ \\___ / /____ ____/ /_\n"
        " / , _/ _ `/ _ \\/ // / -_) __/ -_) __/ __/\n"
        "/_/|_|\\_,_/_//_/____/\\__/\\__/\\__/\\__/\\__/ mComp 1.1\n\n";

    printf("%s", banner);

    validateconfig();

    float Dist = 0;

    printf("[*] Enter model ID: ");
    fgets(folderName, 1000, stdin);
    printf("\n");

    import_from_file(folderName);

    printf("[*] Enter string to compare (%d chars): ", LEN/N_STRINGS);
    fgets(input, LEN/N_STRINGS, stdin);
    printf("\n");

    printf("[*] Generating statistic model...");
    generate_C_model();
    printf("    [DONE]\n");

    printf("[*] Calculating distance...      ");
    Dist = distance();
    printf("    [DONE]\n");

    printf("[*] Calculating stats...         ");
    Delt();
    printf("    [DONE]\n");

    printf("\n[*] [RESULTS] Stats:    Avg abs delta: [%lf] Max abs Delta: [%lf]  @ X: [%d] Y: [%d]", Avg_D, Max_D, Xm, Ym);
    printf("\n[*] [RESULTS] Distance: Abs distance:  [%lf] Aligned distance: [%lf]  Rel distance:  [%lf]\n\n", Dist, fabs(Dist-alig), Max_D/Avg_D);

    return 0;
}
