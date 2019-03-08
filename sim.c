#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define BUFFER_SIZE 100000
#define TRACE_LENGTH 16
#define SIMULATION_TYPE_LENGTH 30
#define BIMODAL "bimodal"
#define GSHARE "gshare"
#define HYBRID "hybrid"


#define LOWEST_TWO_BIT 2
#define TAKEN "t"
#define NOT_TAKEN "n"

FILE *srcFP = 0;

char trace_file_name[SIMULATION_TYPE_LENGTH];
char simulator_type[SIMULATION_TYPE_LENGTH];

int K = 0;
int N = 0;
int M1 = 0;
int M2 = 0;

char trace_str[TRACE_LENGTH];
long long num_prediction;
int *predictor_table;
int *branch_history_register;
int *gshare_predictor_table;

char file_branch_prediction;
unsigned int file_branch_instruction_address;

int *chooser_table;

long long findNumLines(FILE *fp) {
    long long numLines = 0;
    char str[100];
    while (!feof(fp)) {
        fgets(str, 100, fp);
        numLines++;
    }
    numLines--;
    if (fseek(fp, 0L, SEEK_SET) == EOF) {
        perror("Error while seeking to beginning of file");
        exit(0);
    }
    return numLines;
}

int get_bit_index(int byte, int start, int end) {
    return ((byte >> start) & ~(~0 << (end - start + 1)));
}


int min(int a, int b) {
    return (((a) < (b)) ? (a) : (b));
}

int max(int a, int b) {
    return (((a) > (b)) ? (a) : (b));
}

int main(int argc, char *argv[]) {

    strcpy(simulator_type, argv[1]);
    if ((argc != 4 || strcmp(simulator_type, "bimodal") != 0) &&
        (argc != 5 || strcmp(simulator_type, "gshare") != 0) &&
        (argc != 7 || strcmp(simulator_type, "hybrid") != 0)) {
        printf("Invalid number of argument\n");

        printf("\n-----------------------------------------------------------------------\n");
        printf("To simulate a bimodal predictor\n");
        printf("$./sim bimodal <M2> <tracefile>\n");
        printf("M2 : Number of PC bits used to index the bimodal table.\n");
        printf("\n-----------------------------------------------------------------------\n");


        printf("\n-----------------------------------------------------------------------\n");
        printf("To simulate a gshare predictor\n");
        printf("$./sim gshare <M1> <N> <tracefile>\n");
        printf("M1 : Number of PC bits used to index the gshare table\n");
        printf("N : Number of global branch history register bits used to index the gshare table.\n");
        printf("\n-----------------------------------------------------------------------\n");


        printf("\n-----------------------------------------------------------------------\n");
        printf("To simulate a hybrid predictor\n");
        printf("$./sim hybrid <K> <M1> <N> <M2> <tracefile>\n");
        printf("K : Number of PC bits used to index the chooser table\n");
        printf("M1 : Number of PC bits used to index the gshare table\n");
        printf("N : Number of global branch history register bits used to index the gshare table.\n");
        printf("M2 : Number of PC bits used to index the bimodal table");
        printf("\n-----------------------------------------------------------------------\n");
        exit(1);

    }

//    char str[BUFFER_SIZE];

    if (strcmp(simulator_type, BIMODAL) == 0 && argc == 4) {
        strcpy(simulator_type, argv[1]);
        M2 = atoi(argv[2]);
        strcpy(trace_file_name, argv[3]);
    } else if (strcmp(simulator_type, GSHARE) == 0 && argc == 5) {
        strcpy(simulator_type, argv[1]);
        M1 = atoi(argv[2]);
        N = atoi(argv[3]);
        strcpy(trace_file_name, argv[4]);
    } else if (strcmp(simulator_type, HYBRID) == 0 && argc == 7) {
        strcpy(simulator_type, argv[1]);
        K = atoi(argv[2]);
        M1 = atoi(argv[3]);
        N = atoi(argv[4]);
        M2 = atoi(argv[5]);
        strcpy(trace_file_name, argv[6]);
    }
    if ((srcFP = fopen(trace_file_name, "r")) == NULL) {
        perror("Error while opening input file.");
        exit(1);
    }

    num_prediction = findNumLines(srcFP);

    long mispredictions_counter = 0;
    double misprediction_rate;

    if (strcmp(simulator_type, BIMODAL) == 0) {
        int num_entries = pow(2, M2);
        predictor_table = (int *) malloc(num_entries * sizeof(int));


        for (int i = 0; i < num_entries; i++) {
            predictor_table[i] = 4;     // weakly taken = 4
        }


        for (int i = 0; i < num_prediction; i++) {
            fgets(trace_str, TRACE_LENGTH, srcFP);
            sscanf(trace_str, "%x %s\n", &file_branch_instruction_address, &file_branch_prediction);
//          printf(" %x   %c  --> ", file_branch_instruction_address, file_branch_prediction);
            int m2_bits_in_decimal = get_bit_index(file_branch_instruction_address, LOWEST_TWO_BIT, M2 + 1);
//          printf("m2_bits_in_decimal : %d", m2_bits_in_decimal);
//          printf(" Predictor Table before : %d ", predictor_table[m2_bits_in_decimal]);
            if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
                if (predictor_table[m2_bits_in_decimal] > 3) {
                    mispredictions_counter++;
                }
                predictor_table[m2_bits_in_decimal] = max(predictor_table[m2_bits_in_decimal] - 1, 0);
            } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
                if (predictor_table[m2_bits_in_decimal] < 4) {
                    mispredictions_counter++;
                }
                predictor_table[m2_bits_in_decimal] = min(predictor_table[m2_bits_in_decimal] + 1, 7);
            }
        }
        misprediction_rate = ((double) mispredictions_counter / (double) num_prediction) * 100;
        printf("COMMAND\n");
        printf("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
        printf("OUTPUT\n");
        printf("number of predictions: \t\t%lld \n", num_prediction);
        printf("number of mispredictions: \t%ld \n", mispredictions_counter);
        printf("misprediction rate: \t\t%2.2f%%\n", misprediction_rate);
        printf("FINAL BIMODAL CONTENTS\n");

        for (int i = 0; i < num_entries; i++) {
            printf("%d\t%d\n", i, predictor_table[i]);
        }
    } else if (strcmp(simulator_type, GSHARE) == 0) {

        int num_entries = pow(2, M1);
        predictor_table = (int *) malloc(num_entries * sizeof(int));

        for (int i = 0; i < num_entries; i++) {
            predictor_table[i] = 4;     // weakly taken = 4
        }

        branch_history_register = (int *) malloc(N * sizeof(int));

        for (int i = 0; i < N; i++) {
            branch_history_register[i] = 0;     // Initialize to Zero
        }

        for (int i = 0; i < num_prediction; i++) {

            fgets(trace_str, TRACE_LENGTH, srcFP);
            sscanf(trace_str, "%x %s\n", &file_branch_instruction_address, &file_branch_prediction);

            int m_bits_in_decimal = get_bit_index(file_branch_instruction_address, LOWEST_TWO_BIT, M1 + 1);
            int index_from_bhr = 0;
            for (int j = 0; j < N; j++) {
                index_from_bhr = index_from_bhr + branch_history_register[j] * pow(2, N - 1 - j);
            }
            m_bits_in_decimal = m_bits_in_decimal ^ index_from_bhr;
            for (int j = N - 1; j > 0; j--) {
                branch_history_register[j] = branch_history_register[j - 1];
            }

//            if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
//                branch_history_register[0] = 0;
//            } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
//                branch_history_register[0] = 1;
//            }

            if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
                branch_history_register[0] = 0;
                if (predictor_table[m_bits_in_decimal] > 3) {
                    mispredictions_counter++;
                }
                predictor_table[m_bits_in_decimal] = max(predictor_table[m_bits_in_decimal] - 1, 0);
            } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
                branch_history_register[0] = 1;
                if (predictor_table[m_bits_in_decimal] < 4) {
                    mispredictions_counter++;
                }
                predictor_table[m_bits_in_decimal] = min(predictor_table[m_bits_in_decimal] + 1, 7);
            }
        }
        misprediction_rate = ((double) mispredictions_counter / (double) num_prediction) * 100;
        printf("COMMAND\n");
        printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
        printf("OUTPUT\n");
        printf("number of predictions: \t\t%lld \n", num_prediction);
        printf("number of mispredictions: \t%ld \n", mispredictions_counter);
        printf("misprediction rate: \t\t%2.2f%%\n", misprediction_rate);
        printf("FINAL GSHARE CONTENTS\n");
        for (int i = 0; i < num_entries; i++) {
            printf("%d\t%d\n", i, predictor_table[i]);
        }

    } else if (strcmp(simulator_type, HYBRID) == 0) {

//        printf("INSIDE HYBRID");
        int chooser_table_num_entries = pow(2, K);
//        printf(" chooser_table_num_entries : %d ", chooser_table_num_entries);
        chooser_table = (int *) malloc(chooser_table_num_entries * sizeof(int));

        for (int i = 0; i < chooser_table_num_entries; i++) {
            chooser_table[i] = 1;     // initialize to 1
        }
        bool is_bimodal = false;
        bool is_gshare = false;

//      BIMODAL
        int bimodal_num_entries = pow(2, M2);
//        printf(" bimodal_num_entries : %d", bimodal_num_entries);
        int *bimodal_predictor_table = (int *) malloc(bimodal_num_entries * sizeof(int));
        for (int i = 0; i < bimodal_num_entries; i++) {
            bimodal_predictor_table[i] = 4;     // weakly taken = 4
        }

//      GSHARE
        int gshare_num_entries = pow(2, M1);
//        printf(" gshare_num_entries : %d", gshare_num_entries);
        gshare_predictor_table = (int *) malloc(gshare_num_entries * sizeof(int));
        for (int i = 0; i < gshare_num_entries; i++) {
            gshare_predictor_table[i] = 4;     // weakly taken = 4
        }
        branch_history_register = (int *) malloc(N * sizeof(int));
        for (int i = 0; i < N; i++) {
            branch_history_register[i] = 0;     // Initialize to Zero
        }


        for (int i = 0; i < num_prediction; i++) {
            fgets(trace_str, TRACE_LENGTH, srcFP);
            sscanf(trace_str, "%x %s\n", &file_branch_instruction_address, &file_branch_prediction);

            //Chooser Table indexing
            int chooser_table_bits_in_decimal = get_bit_index(file_branch_instruction_address, LOWEST_TWO_BIT, K + 1);
#ifdef DEBUG_OP
            printf("CT index : \t%d\n", chooser_table_bits_in_decimal);
            printf("CT value : \t%d\n", chooser_table[chooser_table_bits_in_decimal]);

#endif
            //from bimodal
            int m2_bits_in_decimal = get_bit_index(file_branch_instruction_address, LOWEST_TWO_BIT, M2 + 1);
            if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
                if (bimodal_predictor_table[m2_bits_in_decimal] > 3) {
                    is_bimodal = false;
                } else {
                    is_bimodal = true;
                }
            } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
                if (bimodal_predictor_table[m2_bits_in_decimal] < 4) {
                    is_bimodal = false;
                } else {
                    is_bimodal = true;
                }
            }

            //gshare indexing
            int m1_bits_in_decimal = get_bit_index(file_branch_instruction_address, LOWEST_TWO_BIT, M1 + 1);
            int index_from_bhr = 0;
            for (int j = 0; j < N; j++) {
                index_from_bhr = index_from_bhr + branch_history_register[j] * pow(2, N - 1 - j);
            }
            m1_bits_in_decimal = m1_bits_in_decimal ^ index_from_bhr;

            if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
                if (gshare_predictor_table[m1_bits_in_decimal] > 3) {
                    is_gshare = false;

                } else {
                    is_gshare = true;
                }
            } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
                if (gshare_predictor_table[m1_bits_in_decimal] < 4) {
                    is_gshare = false;
                } else {
                    is_gshare = true;
                }
            }

            if (chooser_table[chooser_table_bits_in_decimal] < 2) {
                //from bimodal
                int m2_bits_in_decimal = get_bit_index(file_branch_instruction_address, LOWEST_TWO_BIT, M2 + 1);
#ifdef DEBUG_OP
                printf("bimodal-PT index: \t%d\n", m2_bits_in_decimal);
                printf("bimodal-PT value: \t%d\n", bimodal_predictor_table[m2_bits_in_decimal]);
#endif
                if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
                    if (bimodal_predictor_table[m2_bits_in_decimal] > 3) {
                        mispredictions_counter++;
                        is_bimodal = false;
                    } else {
                        is_bimodal = true;
                    }
                    bimodal_predictor_table[m2_bits_in_decimal] = max(bimodal_predictor_table[m2_bits_in_decimal] - 1,
                                                                      0);
                } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
                    if (bimodal_predictor_table[m2_bits_in_decimal] < 4) {
                        mispredictions_counter++;
                        is_bimodal = false;
                    } else {
                        is_bimodal = true;
                    }
                    bimodal_predictor_table[m2_bits_in_decimal] = min(bimodal_predictor_table[m2_bits_in_decimal] + 1,
                                                                      7);
                }
#ifdef DEBUG_OP
                if(is_bimodal){
                    printf("Prediction: true\n");
                }else{
                    printf("Prediction: false\n");
                }
                printf("New bimodial-PT value:\t%d\n",bimodal_predictor_table[m2_bits_in_decimal]);
#endif
            } else if (chooser_table[chooser_table_bits_in_decimal] >= 2) {


                //gshare indexing
                int m1_bits_in_decimal = get_bit_index(file_branch_instruction_address, LOWEST_TWO_BIT, M1 + 1);
                int index_from_bhr = 0;
                for (int j = 0; j < N; j++) {
                    index_from_bhr = index_from_bhr + branch_history_register[j] * pow(2, N - 1 - j);
                }
                m1_bits_in_decimal = m1_bits_in_decimal ^ index_from_bhr;
//                printf("GSHARE : m1_bits_in_decimal : %d ", m1_bits_in_decimal);
#ifdef DEBUG_OP
                printf("gshare-PT index: \t%d\n", m1_bits_in_decimal);
                printf("gshare-PT value: \t%d\n", gshare_predictor_table[m1_bits_in_decimal]);
#endif
                if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
                    if (gshare_predictor_table[m1_bits_in_decimal] > 3) {
                        mispredictions_counter++;
                        is_gshare = false;

                    } else {
                        is_gshare = true;
                    }
                    gshare_predictor_table[m1_bits_in_decimal] = max(gshare_predictor_table[m1_bits_in_decimal] - 1, 0);
                } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
                    if (gshare_predictor_table[m1_bits_in_decimal] < 4) {
                        mispredictions_counter++;
                        is_gshare = false;
                    } else {
                        is_gshare = true;
                    }
                    gshare_predictor_table[m1_bits_in_decimal] = min(gshare_predictor_table[m1_bits_in_decimal] + 1, 7);
                }
#ifdef DEBUG_OP
                if(is_gshare){
                    printf("Prediction: true\n");
                }else{
                    printf("Prediction: false\n");
                }
                printf("New gshare-PT value:\t%d\n",gshare_predictor_table[m1_bits_in_decimal]);
#endif
            }
            for (int j = N - 1; j > 0; j--) {
                branch_history_register[j] = branch_history_register[j - 1];
            }

            if (strcmp(&file_branch_prediction, NOT_TAKEN) == 0) {
                branch_history_register[0] = 0;
            } else if (strcmp(&file_branch_prediction, TAKEN) == 0) {
                branch_history_register[0] = 1;
            }
            if (is_gshare && !is_bimodal) {
                chooser_table[chooser_table_bits_in_decimal] = min(chooser_table[chooser_table_bits_in_decimal] + 1, 3);
            } else if (!is_gshare && is_bimodal) {
                chooser_table[chooser_table_bits_in_decimal] = max(chooser_table[chooser_table_bits_in_decimal] - 1, 0);
            }
#ifdef DEBUG_OP
            printf("BHR now set to:\t");
            for (int j = 0; j < N; j++) {
                printf("[%d]", branch_history_register[j]);
            }
            printf("\n");
            printf("New CT value:\t%d\n", chooser_table[chooser_table_bits_in_decimal]);
#endif
        }

        misprediction_rate = ((double) mispredictions_counter / (double) num_prediction) * 100;
        printf("COMMAND\n");
        printf("%s %s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
        printf("OUTPUT\n");
        printf("number of predictions: \t\t%lld \n", num_prediction);
        printf("number of mispredictions: \t%ld \n", mispredictions_counter);
        printf("misprediction rate: \t\t%2.2f%%\n", misprediction_rate);

        printf("FINAL CHOOSER CONTENTS\n");
        for (int i = 0; i < chooser_table_num_entries; i++) {
            printf("%d\t%d\n", i, chooser_table[i]);
        }

        printf("FINAL GSHARE CONTENTS\n");
        for (int i = 0; i < gshare_num_entries; i++) {
            printf("%d\t%d\n", i, gshare_predictor_table[i]);
        }


        printf("FINAL BIMODAL CONTENTS\n");
        for (int i = 0; i < bimodal_num_entries; i++) {
            printf("%d\t%d\n", i, bimodal_predictor_table[i]);
        }
    }

}
