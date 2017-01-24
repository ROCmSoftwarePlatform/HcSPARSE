#include <hcsparse.h>
#include <iostream>
#include "hc_am.hpp"
int main()
{
    hcsparseScalar gAlpha;
    hcdenseVector gR;
    hcdenseVector gX;
    hcdenseVector gY;

    std::vector<accelerator>acc = accelerator::get_all();
    accelerator_view accl_view = (acc[1].create_view()); 

    hcsparseControl control(accl_view);

    int num_elements = 100;
    float *host_R = (float*) calloc(num_elements, sizeof(float));
    float *host_res = (float*) calloc(num_elements, sizeof(float));
    float *host_X = (float*) calloc(num_elements, sizeof(float));
    float *host_Y = (float*) calloc(num_elements, sizeof(float));
    float *host_alpha = (float*) calloc(1, sizeof(float));

    hcsparseSetup();
    hcsparseInitScalar(&gAlpha);
    hcsparseInitVector(&gR);
    hcsparseInitVector(&gX);
    hcsparseInitVector(&gY);

    gR.values = am_alloc(sizeof(float) * num_elements, acc[1], 0);
    gX.values = am_alloc(sizeof(float) * num_elements, acc[1], 0);
    gY.values = am_alloc(sizeof(float) * num_elements, acc[1], 0);
    gAlpha.value = am_alloc(sizeof(float) * 1, acc[1], 0);

    srand (time(NULL));
    for (int i = 0; i < num_elements; i++)
    {
        host_R[i] = rand()%100;
        host_X[i] = rand()%100;
        host_Y[i] = rand()%100;
    }
    
    host_alpha[0] = rand()%100;

    control.accl_view.copy(host_R, gR.values, sizeof(float) * num_elements);
    control.accl_view.copy(host_X, gX.values, sizeof(float) * num_elements);
    control.accl_view.copy(host_Y, gY.values, sizeof(float) * num_elements);
    control.accl_view.copy(host_alpha, gAlpha.value, sizeof(float) * 1);

    gAlpha.offValue = 0;
    gR.offValues = 0;
    gX.offValues = 0;
    gY.offValues = 0;

    gR.num_values = num_elements;
    gX.num_values = num_elements;
    gY.num_values = num_elements;

    hcsparseStatus status;

    status = hcdenseSaxpy(&gR, &gAlpha, &gX, &gY, &control);

    for (int i = 0; i < num_elements; i++)
    {
        host_res[i] = host_alpha[0] * host_X[i] + host_Y[i];
    }

    bool ispassed = 1;

    control.accl_view.copy(gR.values, host_R, sizeof(float) * num_elements);

    for (int i = 0; i < num_elements; i++)
    {
        if (host_res[i] != host_R[i])
        {
            ispassed = 0;
            break;
        }
    }

    std::cout << (ispassed?"TEST PASSED":"TEST FAILED") << std::endl;

    hcsparseTeardown();

    free(host_R);
    free(host_res);
    free(host_X);
    free(host_Y);
    free(host_alpha);
    am_free(gR.values);
    am_free(gX.values);
    am_free(gY.values);
    am_free(gAlpha.value);

    return 0; 
}
