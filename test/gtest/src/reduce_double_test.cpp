#include <hcsparse.h>
#include <iostream>
#include "gtest/gtest.h"

#define TOLERANCE 0.01

TEST(reduce_double_test, func_check)
{
    hcsparseScalar gR;
    hcdenseVector gX;

    std::vector<accelerator>acc = accelerator::get_all();
    accelerator_view accl_view = (acc[1].create_view()); 

    hcsparseControl control(accl_view);

    int num_elements = 1000;
    double *host_res = (double*) calloc(1, sizeof(double));
    double *host_X = (double*) calloc(num_elements, sizeof(double));
    double *host_R = (double*) calloc(1, sizeof(double));

    srand (time(NULL));
    for (int i = 0; i < num_elements; i++)
    {
        host_X[i] = rand()%100;
    }
    
    array_view<double> dev_X(num_elements, host_X);
    array_view<double> dev_R(1, host_R);

    hcsparseSetup();
    hcsparseInitScalar(&gR);
    hcsparseInitVector(&gX);

    gR.value = &dev_R;
    gX.values = &dev_X;

    gR.offValue = 0;
    gX.offValues = 0;

    gX.num_values = num_elements;

    hcsparseStatus status;

    status = hcdenseDreduce(&gR, &gX, &control);

    for (int i = 0; i < num_elements; i++)
    {
        host_res[0] += host_X[i];
    }

    array_view<double> *av_res = static_cast<array_view<double> *>(gR.value);
 
    double diff = std::abs(host_res[0] - (*av_res)[0]);
    EXPECT_LT(diff, TOLERANCE);

    dev_X.synchronize();
    dev_R.synchronize();

    hcsparseTeardown();

    free(host_res);
    free(host_X);
    free(host_R);
}