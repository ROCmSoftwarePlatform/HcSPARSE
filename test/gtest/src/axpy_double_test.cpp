#include <hcsparse.h>
#include <iostream>
#include <hc_am.hpp>
#include "gtest/gtest.h"

#define TOLERANCE 0.01

TEST(axpy_double_test, func_check)
{
    hcsparseScalar gAlpha;
    hcdenseVector gR;
    hcdenseVector gX;
    hcdenseVector gY;

    std::vector<accelerator>acc = accelerator::get_all();
    accelerator_view accl_view = (acc[1].create_view()); 

    hcsparseControl control(accl_view);

    int num_elements = 100;
    double *host_R = (double*) calloc(num_elements, sizeof(double));
    double *host_res = (double*) calloc(num_elements, sizeof(double));
    double *host_X = (double*) calloc(num_elements, sizeof(double));
    double *host_Y = (double*) calloc(num_elements, sizeof(double));
    double *host_alpha = (double*) calloc(1, sizeof(double));

    srand (time(NULL));
    for (int i = 0; i < num_elements; i++)
    {
        host_R[i] = rand()%100;
        host_X[i] = rand()%100;
        host_Y[i] = rand()%100;
    }
    
    host_alpha[0] = rand()%100;

    hcsparseSetup();
    hcsparseInitScalar(&gAlpha);
    hcsparseInitVector(&gR);
    hcsparseInitVector(&gX);
    hcsparseInitVector(&gY);

    gR.values = am_alloc(sizeof(double) * num_elements, acc[1], 0);
    gX.values = am_alloc(sizeof(double) * num_elements, acc[1], 0);
    gY.values = am_alloc(sizeof(double) * num_elements, acc[1], 0);
    gAlpha.value = am_alloc(sizeof(double) * 1, acc[1], 0);

    gAlpha.offValue = 0;
    gR.offValues = 0;
    gX.offValues = 0;
    gY.offValues = 0;

    gR.num_values = num_elements;
    gX.num_values = num_elements;
    gY.num_values = num_elements;

    control.accl_view.copy(host_R, gR.values, sizeof(double) * num_elements);
    control.accl_view.copy(host_X, gX.values, sizeof(double) * num_elements);
    control.accl_view.copy(host_Y, gY.values, sizeof(double) * num_elements);
    control.accl_view.copy(host_alpha, gAlpha.value, sizeof(double) * 1);
    hcsparseStatus status;

    status = hcdenseDaxpy(&gR, &gAlpha, &gX, &gY, &control);

    for (int i = 0; i < num_elements; i++)
    {
        host_res[i] = host_alpha[0] * host_X[i] + host_Y[i];
    }

    control.accl_view.copy(gR.values, host_R, sizeof(double) * num_elements);

    for (int i = 0; i < num_elements; i++)
    {
        double diff = std::abs(host_res[i] - host_R[i]);
        EXPECT_LT(diff, TOLERANCE);
    }

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
}
