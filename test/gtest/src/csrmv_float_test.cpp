#include <hcsparse.h>
#include <iostream>
#include <hc_am.hpp>
#include "gtest/gtest.h"

#define TOLERANCE 0.1

TEST(csrmv_float_test, func_check)
{
    hcsparseCsrMatrix gCsrMat;
    hcdenseVector gX;
    hcdenseVector gY;
    hcsparseScalar gAlpha;
    hcsparseScalar gBeta;

    std::vector<accelerator>acc = accelerator::get_all();
    accelerator_view accl_view = (acc[1].create_view()); 

    hcsparseControl control(accl_view);

    const char* filename = "./../../../../test/gtest/src/input.mtx";

    int num_nonzero, num_row, num_col;

    hcsparseStatus status;

    status = hcsparseHeaderfromFile(&num_nonzero, &num_row, &num_col, filename);

    if (status != hcsparseSuccess)
    {
        std::cout<<"The input file should be in mtx format"<<std::endl;
        exit (1);
    } 

    float *host_res = (float*) calloc(num_row, sizeof(float));
    float *host_X = (float*) calloc(num_col, sizeof(float));
    float *host_Y = (float*) calloc(num_row, sizeof(float));
    float *host_alpha = (float*) calloc(1, sizeof(float));
    float *host_beta = (float*) calloc(1, sizeof(float));

    srand (time(NULL));
    for (int i = 0; i < num_col; i++)
    {
       host_X[i] = rand()%100;
    } 

    for (int i = 0; i < num_row; i++)
    {
        host_res[i] = host_Y[i] = rand()%100;
    }

    host_alpha[0] = rand()%100;
    host_beta[0] = rand()%100;

    hcsparseSetup();
    hcsparseInitCsrMatrix(&gCsrMat);
    hcsparseInitScalar(&gAlpha);
    hcsparseInitScalar(&gBeta);
    hcsparseInitVector(&gX);
    hcsparseInitVector(&gY);

    gAlpha.offValue = 0;
    gBeta.offValue = 0;
    gX.offValues = 0;
    gY.offValues = 0;

    gX.num_values = num_col;
    gY.num_values = num_row;

    gCsrMat.offValues = 0;
    gCsrMat.offColInd = 0;
    gCsrMat.offRowOff = 0;

    float *values = (float*)calloc(num_nonzero, sizeof(float));
    int *rowOffsets = (int*)calloc(num_row+1, sizeof(int));
    int *colIndices = (int*)calloc(num_nonzero, sizeof(int));

    gX.values = am_alloc(sizeof(float) * num_col, acc[1], 0);
    gY.values = am_alloc(sizeof(float) * num_row, acc[1], 0);
    gAlpha.value = am_alloc(sizeof(float) * 1, acc[1], 0);
    gBeta.value = am_alloc(sizeof(float) * 1, acc[1], 0);

    control.accl_view.copy(gX.values, host_X, sizeof(float) * num_col);
    control.accl_view.copy(gY.values, host_Y, sizeof(float) * num_row);
    control.accl_view.copy(gAlpha.value, host_alpha, sizeof(float) * 1);
    control.accl_view.copy(gBeta.value, host_beta, sizeof(float) * 1);

    gCsrMat.values = am_alloc(sizeof(float) * num_nonzero, acc[1], 0);
    gCsrMat.rowOffsets = am_alloc(sizeof(int) * (num_row+1), acc[1], 0);
    gCsrMat.colIndices = am_alloc(sizeof(int) * num_nonzero, acc[1], 0);

    status = hcsparseSCsrMatrixfromFile(&gCsrMat, filename, &control, false);
   
    if (status != hcsparseSuccess)
    {
        std::cout<<"The input file should be in mtx format"<<std::endl;
        exit (1);
    }
 
    control.accl_view.copy(values, gCsrMat.values, sizeof(float) * num_nonzero);
    control.accl_view.copy(rowOffsets, gCsrMat.rowOffsets, sizeof(int) * (num_row+1));
    control.accl_view.copy(colIndices, gCsrMat.colIndices, sizeof(int) * num_nonzero);

    hcsparseScsrmv(&gAlpha, &gCsrMat, &gX, &gBeta, &gY, &control); 

    int col = 0;
    for (int row = 0; row < num_row; row++)
    {
        host_res[row] *= host_beta[0];
        for (; col < rowOffsets[row+1]; col++)
        {
            host_res[row] = host_alpha[0] * host_X[colIndices[col]] * values[col] + host_res[row];
        }
    }
    control.accl_view.copy(host_Y, gY.values, sizeof(float) * num_row);

    for (int i = 0; i < num_row; i++)
    {
        float diff = std::abs(host_res[i] - host_Y[i]);
        EXPECT_LT(diff, TOLERANCE);
    }

    hcsparseTeardown();

    free(host_res);
    free(host_X);
    free(host_Y);
    free(host_alpha);
    free(host_beta);
    free(values);
    free(rowOffsets);
    free(colIndices);
    am_free(gX.values);
    am_free(gY.values);
    am_free(gAlpha.value);
    am_free(gBeta.value);
    am_free(gCsrMat.values);
    am_free(gCsrMat.rowOffsets);
    am_free(gCsrMat.colIndices);
}
