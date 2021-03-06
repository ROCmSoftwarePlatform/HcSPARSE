#include <hcsparse.h>
#include <iostream>
#include "hc_am.hpp"
#include "gtest/gtest.h"

TEST(transform_double_test, func_check)
{
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

    srand (time(NULL));
    for (int i = 0; i < num_elements; i++)
    {
        host_R[i] = rand()%100;
        host_X[i] = rand()%100;
        host_Y[i] = rand()%100;
    }
    
    hcsparseSetup();
    hcsparseInitVector(&gR);
    hcsparseInitVector(&gX);
    hcsparseInitVector(&gY);

    gX.values = am_alloc(sizeof(double) * num_elements, acc[1], 0);
    gY.values = am_alloc(sizeof(double) * num_elements, acc[1], 0);
    gR.values = am_alloc(sizeof(double) * num_elements, acc[1], 0);

    control.accl_view.copy(host_X, gX.values, sizeof(double) * num_elements);
    control.accl_view.copy(host_Y, gY.values, sizeof(double) * num_elements);
    control.accl_view.copy(host_R, gR.values, sizeof(double) * num_elements);

    gR.offValues = 0;
    gX.offValues = 0;
    gY.offValues = 0;

    gR.num_values = num_elements;
    gX.num_values = num_elements;
    gY.num_values = num_elements;

    hcsparseStatus status;

    bool ispassed = 1;

    for (int j = 0; j < 4; j++)
    {
        switch(j)
        {
            case 0:
                status = hcdenseDadd(&gR, &gX, &gY, &control);

                for (int i = 0; i < num_elements; i++)
                {
                    host_res[i] = host_X[i] + host_Y[i];
                }
                break;
            case 1:
                status = hcdenseDsub(&gR, &gX, &gY, &control);

                for (int i = 0; i < num_elements; i++)
                {
                    host_res[i] = host_X[i] - host_Y[i];
                }
                break;
            case 2:
                status = hcdenseDmul(&gR, &gX, &gY, &control);

                for (int i = 0; i < num_elements; i++)
                {
                    host_res[i] = host_X[i] * host_Y[i];
                }
                break;
            case 3:
                status = hcdenseDdiv(&gR, &gX, &gY, &control);

                for (int i = 0; i < num_elements; i++)
                {
                    host_res[i] = host_Y[i] == 0 ? 0 : (host_X[i] / host_Y[i]);
                }
                break;
        }

        control.accl_view.copy(gR.values, host_R, sizeof(double) * num_elements);

        for (int i = 0; i < num_elements; i++)
        {
            double diff = std::abs(host_res[i] - host_R[i]);
            EXPECT_LT (diff, 0.01);
        }
    }

    hcsparseTeardown();

    free(host_R);
    free(host_res);
    free(host_X);
    free(host_Y);
    am_free(gR.values);
    am_free(gX.values);
    am_free(gY.values);
}
