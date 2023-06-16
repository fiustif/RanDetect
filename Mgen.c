#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

//generator settings

#define LEN 4000                            //string size
#define N_STRINGS 2                         //N of strings to split from the original string
#define L_CONF_X 5                          //X config size (bigger means more precision but also more noise) (must be >= L_MOD_Y)
#define L_CONF_Y 30                         //Y config size (bigger means more precision but also more noise) (must be >= L_MOD_Y)
#define L_MOD_X 5                           //X model size (bigger means more precision but also more noise)
#define L_MOD_Y 30                          //Y model size (bigger means more precision but also more noise)
#define L_TRIG 5                            //N of 0 in the model to stop calculating the configuration
#define CONF_MULT 0.025                     //configuration multiplier used to calculate the multipliers for the configuration (should be the same in the validation)
#define MAX_MULT 1                          //max multiplier

/**


This is the model and configuration generator program for RanDetect.
Developed by fiustif and Nick806 in 2023.

Suggested settings for 8000 char dataset and 2000 char test sample:

LEN:        8000
N_STRINGS:  4
L_CONF_X:   5
L_CONF_Y:   30
L_TRIG:     5
CONF_MULT:  2


**/



char input[LEN];                                                            //input dataset
double data [N_STRINGS][LEN/N_STRINGS][LEN/N_STRINGS] = {0};                //raw model [N of rep][rep dist to next rep N]
double config [L_CONF_X][L_CONF_Y] = {0};                                   //contains configuration multipliers for the verification

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

void generate_config()                      //generate multipliers for the verification
{
    int i, x, k;
    for(i=0;i<L_CONF_X;i++)                 //calculate average absolute delta between reps
    {
        for(x=0;x<L_CONF_Y;x++)
        {
            for(k=0;k<N_STRINGS;k++)
            {
                config[i][x] += fabs(data[0][i][x]-data[k][i][x]);
            }
            if(CONF_MULT*(config[i][x]/N_STRINGS)>MAX_MULT)
            config[i][x] = MAX_MULT;
            else
            config[i][x] = CONF_MULT*(config[i][x]/N_STRINGS);
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

void save_to_file(int type)
{
    int i, j, k;
    switch(type)
    {
    case 1: ;
        FILE *Model = fopen("Model.r4nd", "w");
        if (Model)
        {
            for (j = 0; j < L_MOD_X; j++)
            {
                for (k = 0; k < L_MOD_Y; k++)
                    fprintf(Model, "%lf, ", data[0][j][k]);
                }
                fprintf(Model, "\n");
            }
        fprintf(Model, "\n");
        break;
    case 2: ;
        FILE *Conf = fopen("Config.r4nd", "w");
        if (Conf)
        {
            for (j = 0; j < L_CONF_X; j++)
            {
                for (k = 0; k < L_CONF_Y; k++)
                {
                    fprintf(Conf, "%lf, ", config[j][k]);
                }
                fprintf(Conf, "\n");
            }
            fprintf(Conf, "\n");
        }
    }
    return;
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
        "/_/|_|\\_,_/_//_/____/\\__/\\__/\\__/\\__/\\__/ mGen 1.2\n\n";

    printf("%s", banner);

    validateconfig();

    printf("[*] Enter string (%d chars): ",LEN);
    fgets(input, LEN, stdin);
    printf("\n");

    printf("[*] Generating statistic model...");
    generate_model();
    printf("    [DONE]\n");

    printf("[*] Generating configuration...  ");
    generate_config();
    printf("    [DONE]\n");

    printf("[*] Normalizing model...         ");
    normalize_model();
    printf("    [DONE]\n");

    printf("[*] Saving statistic model...    ");
    save_to_file(1);
    printf("    [DONE]\n");

    printf("[*] Saving Config...             ");
    save_to_file(2);
    printf("    [DONE]\n");

    return 0;
}
