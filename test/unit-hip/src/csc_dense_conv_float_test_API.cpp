#include "hip/hip_runtime.h"
#include "hipsparse.h"
#include <iostream>
#include <cmath>
#include "gtest/gtest.h"

TEST(csc_dense_conv_float_test, func_check)
{
    int num_row = 4;
    int num_col = 5;
    int num_nonzero = 9;

    float* valPtr = (float*)calloc(num_row*num_col, sizeof(float));
    int* colPtr = (int*)calloc(num_col+1, sizeof(int));
    int* rowInd = (int*)calloc(num_nonzero, sizeof(int));

    valPtr[0] = 1;
    valPtr[1] = 5;
    valPtr[2] = 4;
    valPtr[3] = 2;
    valPtr[4] = 3;
    valPtr[5] = 9;
    valPtr[6] = 7;
    valPtr[7] = 8;
    valPtr[8] = 6;

    colPtr[0] = 0;
    colPtr[1] = 2;
    colPtr[2] = 4;
    colPtr[3] = 6;
    colPtr[4] = 7;
    colPtr[5] = 9;

    rowInd[0] = 0;
    rowInd[1] = 2;
    rowInd[2] = 0;
    rowInd[3] = 1;
    rowInd[4] = 1;
    rowInd[5] = 3;
    rowInd[6] = 2;
    rowInd[7] = 2;
    rowInd[8] = 3;

    int *nnzPerCol_h = (int *)calloc(num_col, sizeof(int));
    nnzPerCol_h[0] = 2;
    nnzPerCol_h[1] = 2;
    nnzPerCol_h[2] = 2;
    nnzPerCol_h[3] = 1;
    nnzPerCol_h[4] = 2;

     /* Test New APIs */
    hipsparseHandle_t handle;
    hipsparseStatus_t status1;

    status1 = hipsparseCreate(&handle);
    if (status1 != HIPSPARSE_STATUS_SUCCESS) {
      std::cout << "Error Initializing the sparse library."<< status1 <<std::endl;
      exit(1);
    }

    hipsparseMatDescr_t descrA;

    status1 = hipsparseCreateMatDescr(&descrA);
    if (status1 != HIPSPARSE_STATUS_SUCCESS) {
      std::cout << "error creating mat descrptr"<<std::endl;
      exit(1);
    }

    float* cscValA = NULL;
    int* cscColPtrA = NULL;
    int *cscRowIndA = NULL;
    int *nnzPerCol = NULL;
    int *nnz;
    float *A = NULL;
    hipError_t err;
    err = hipMalloc(&cscValA, num_nonzero * sizeof(float));
    err = hipMalloc(&cscColPtrA, (num_col+1) * sizeof(int));
    err = hipMalloc(&cscRowIndA, num_nonzero * sizeof(int));
    err = hipMalloc(&A, num_row*num_col * sizeof(float));
    err = hipMalloc(&nnzPerCol, num_col * sizeof(int));
    err = hipMalloc(&nnz, 1 * sizeof(int));

    float *csc_res_val = (float*)calloc(num_nonzero, sizeof(float));
    int *csc_res_colPtr = (int*)calloc(num_col+1, sizeof(int));
    int *csc_res_rowInd = (int*)calloc(num_nonzero, sizeof(int));    
     
    hipMemcpy(cscValA, valPtr, num_nonzero * sizeof(float), hipMemcpyHostToDevice);
    hipMemcpy(cscColPtrA, colPtr, (num_col+1) * sizeof(int), hipMemcpyHostToDevice);
    hipMemcpy(cscRowIndA, rowInd, num_nonzero * sizeof(int), hipMemcpyHostToDevice);
//    hipMemcpy(nnzPerCol, nnzPerCol_h, num_col * sizeof(int), hipMemcpyHostToDevice);

    status1 = hipsparseScsc2dense(handle, num_row, num_col,
                                 descrA, cscValA, cscRowIndA, cscColPtrA, A, num_col);
    if (status1 != HIPSPARSE_STATUS_SUCCESS) {
      std::cout << "Error csc2dense conversion "<<std::endl;
      exit(1);
    }
    hipDeviceSynchronize();

    status1 = hipsparseSnnz(handle, HIPSPARSE_DIRECTION_COLUMN, num_row, num_col, descrA, A, num_col, nnzPerCol, nnz);

    status1 = hipsparseSdense2csc(handle, num_row, num_col,
                                 descrA, A, num_col, nnzPerCol, cscValA, cscRowIndA, cscColPtrA);
    if (status1 != HIPSPARSE_STATUS_SUCCESS) {
      std::cout << "Error dense2csc conversion "<<std::endl;
      exit(1);
    }
    hipDeviceSynchronize();

    hipMemcpy(csc_res_val, cscValA, num_nonzero * sizeof(float), hipMemcpyDeviceToHost);
    hipMemcpy(csc_res_colPtr, cscColPtrA, (num_col+1) * sizeof(int), hipMemcpyDeviceToHost);
    hipMemcpy(csc_res_rowInd, cscRowIndA, num_nonzero * sizeof(int), hipMemcpyDeviceToHost);

    bool ispassed = 1;

    for (int i = 0; i < num_nonzero; i++)
    {
        float diff = std::abs(valPtr[i] - csc_res_val[i]);
        EXPECT_LT(diff, 0.01);
    }

    for (int i = 0; i < num_nonzero; i++)
    {
        float diff = std::abs(rowInd[i] - csc_res_rowInd[i]);
        EXPECT_LT(diff, 0.01);
    }

    for (int i = 0; i < num_col+1; i++)
    {
        float diff = std::abs(colPtr[i] - csc_res_colPtr[i]);
        EXPECT_LT(diff, 0.01);
    }

    free(valPtr);
    free(colPtr);
    free(rowInd);
    free(csc_res_val);
    free(csc_res_colPtr);
    free(csc_res_rowInd);
    hipFree(cscValA);
    hipFree(cscColPtrA);
    hipFree(cscRowIndA);
    hipFree(nnzPerCol);
    hipFree(nnz);
    hipFree(A);

}
