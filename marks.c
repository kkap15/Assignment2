#include <stdio.h>
#include <math.h>

struct marks {
    int assignments[4];
    int me;
    int fe;
};

/* calculate a 2310 final mark */
double calc_marks(struct marks values); 

int main(int argc, char** argv) {
    struct marks inputs;
    inputs.assignments[0] = 41;
    inputs.assignments[1] = 49;
    inputs.assignments[2] = 25;
    inputs.assignments[3] = 20;
    inputs.me = 13;
    inputs.fe = 40;
    printf("%f\n", calc_marks(inputs));
    return 0;
}

/* calculate a 2310 final mark */
double calc_marks(struct marks values) {
    double totalA = 0;
    for (int i = 0 ; i < 4; ++i) {
        totalA += values.assignments[i];	// equiv to totalA = totalA + values.assignments[i]
    }
    totalA = totalA / 2.0;
    double ex1 = values.me + 0.85 * values.fe;
    double ex2 = 2 * values.me + 0.7 * values.fe;
    double totalEx = ex1;
    if (ex2 > ex1) {
        totalEx = ex2;
    }
    double result = sqrt(totalEx * totalA);
    return result;    
}
