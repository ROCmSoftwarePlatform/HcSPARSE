#include <hcsparse.h>
#include <iostream>
#include <hc_am.hpp>
#include "gtest/gtest.h"

#define TOLERANCE 0.01

TEST(csr_dense_conv_double_test, func_check)
{
    hcsparseCsrMatrix gCsrMat;
    hcsparseCsrMatrix gCsrMat_res;
    hcdenseMatrix gMat;

    std::vector<accelerator>acc = accelerator::get_all();
    accelerator_view accl_view = (acc[1].create_view());

    hcsparseControl control(accl_view);

    hcsparseSetup();
    hcsparseInitCsrMatrix(&gCsrMat);
    hcsparseInitCsrMatrix(&gCsrMat_res);
    hcdenseInitMatrix(&gMat);

    gCsrMat.offValues = 0;
    gCsrMat.offColInd = 0;
    gCsrMat.offRowOff = 0;

    gCsrMat_res.offValues = 0;
    gCsrMat_res.offColInd = 0;
    gCsrMat_res.offRowOff = 0;

    gMat.offValues = 0;

    const char* filename = "./../../../../test/gtest/src/input.mtx";

    int num_nonzero, num_row, num_col;

    hcsparseStatus status;

    status = hcsparseHeaderfromFile(&num_nonzero, &num_row, &num_col, filename);

    if (status != hcsparseSuccess)
    {
        std::cout<<"The input file should be in mtx format"<<std::endl;
    }

    double *csr_values = (double*)calloc(num_nonzero, sizeof(double));
    int *csr_rowOff = (int*)calloc(num_row+1, sizeof(int));
    int *csr_colIndices = (int*)calloc(num_nonzero, sizeof(int));

    gCsrMat.values = (double*) am_alloc(num_nonzero * sizeof(double), acc[1], 0);
    gCsrMat.rowOffsets = (int*) am_alloc((num_row+1) * sizeof(int), acc[1], 0);
    gCsrMat.colIndices = (int*) am_alloc(num_nonzero * sizeof(int), acc[1], 0);

    double *csr_res_values = (double*)calloc(num_nonzero, sizeof(double));
    int *csr_res_rowOff = (int*)calloc(num_row+1, sizeof(int));
    int *csr_res_colIndices = (int*)calloc(num_nonzero, sizeof(int));

    gCsrMat_res.values = (double*) am_alloc(num_nonzero * sizeof(double), acc[1], 0);
    gCsrMat_res.rowOffsets = (int*) am_alloc((num_row+1) * sizeof(int), acc[1], 0);
    gCsrMat_res.colIndices = (int*) am_alloc(num_nonzero * sizeof(int), acc[1], 0);

    gMat.values = (double*) am_alloc(num_row*num_col * sizeof(double), acc[1], 0);
    gMat.num_rows = num_row;
    gMat.num_cols = num_col;

    // Memset the device memory to 0 manually as there is no API in hc
    control.accl_view.copy(csr_res_values, gCsrMat_res.values, num_nonzero * sizeof(double));
    control.accl_view.copy(csr_res_rowOff, gCsrMat_res.rowOffsets, (num_row+1) * sizeof(int));
    control.accl_view.copy(csr_res_colIndices, gCsrMat_res.colIndices, num_nonzero * sizeof(int));

    hcsparseDCsrMatrixfromFile(&gCsrMat, filename, &control, false);

    hcsparseDcsr2dense(&gCsrMat, &gMat, &control);

    control.accl_view.copy(gCsrMat.values, csr_values, num_nonzero * sizeof(double));
    control.accl_view.copy(gCsrMat.rowOffsets, csr_rowOff, (num_row+1) * sizeof(int));
    control.accl_view.copy(gCsrMat.colIndices, csr_colIndices, num_nonzero * sizeof(int));

    hcsparseDdense2csr(&gMat, &gCsrMat_res, &control);

    control.accl_view.copy(gCsrMat_res.values, csr_res_values, num_nonzero * sizeof(double));
    control.accl_view.copy(gCsrMat_res.rowOffsets, csr_res_rowOff, (num_row+1) * sizeof(int));
    control.accl_view.copy(gCsrMat_res.colIndices, csr_res_colIndices, num_nonzero * sizeof(int));

    bool ispassed = 1;

    for (int i = 0; i < num_nonzero; i++)
    {
        double diff = std::abs(csr_values[i] - csr_res_values[i]);
        EXPECT_LT(diff, TOLERANCE);
    }

    for (int i = 0; i < num_nonzero; i++)
    {
        EXPECT_EQ(csr_colIndices[i], csr_res_colIndices[i]);
    }

    for (int i = 0; i < num_row+1; i++)
    {
        EXPECT_EQ(csr_rowOff[i], csr_res_rowOff[i]);
    }


    hcsparseTeardown();

    free(csr_values);
    free(csr_rowOff);
    free(csr_colIndices);
    free(csr_res_values);
    free(csr_res_rowOff);
    free(csr_res_colIndices);
    am_free(gCsrMat.values);
    am_free(gCsrMat.rowOffsets);
    am_free(gCsrMat.colIndices);
    am_free(gCsrMat_res.values);
    am_free(gCsrMat_res.rowOffsets);
    am_free(gCsrMat_res.colIndices);
    am_free(gMat.values);
}

