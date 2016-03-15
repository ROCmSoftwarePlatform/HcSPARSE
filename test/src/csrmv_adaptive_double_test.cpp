#include <hcsparse.h>
#include <iostream>
#include "hc_am.hpp"
int main(int argc, char *argv[])
{
    hcsparseCsrMatrix gCsrMat;
    hcdenseVector gX;
    hcdenseVector gY;
    hcsparseScalar gAlpha;
    hcsparseScalar gBeta;

    std::vector<accelerator>acc = accelerator::get_all();
    accelerator_view accl_view = (acc[1].create_view()); 

    hcsparseControl control(accl_view);

    if (argc != 2)
    {
        std::cout<<"Required mtx input file"<<std::endl;
        return 0;
    }

    const char* filename = argv[1];

    int num_nonzero, num_row, num_col;

    hcsparseStatus status;

    status = hcsparseHeaderfromFile(&num_nonzero, &num_row, &num_col, filename);

    if (status != hcsparseSuccess)
    {
        std::cout<<"The input file should be in mtx format"<<std::endl;
        return 0;
    } 

    double *host_res = (double*) calloc(num_row, sizeof(double));
    double *host_X = (double*) calloc(num_col, sizeof(double));
    double *host_Y = (double*) calloc(num_row, sizeof(double));
    double *host_alpha = (double*) calloc(1, sizeof(double));
    double *host_beta = (double*) calloc(1, sizeof(double));

    hcsparseSetup();
    hcsparseInitCsrMatrix(&gCsrMat);
    hcsparseInitScalar(&gAlpha);
    hcsparseInitScalar(&gBeta);
    hcsparseInitVector(&gX);
    hcsparseInitVector(&gY);

    gX.values = am_alloc(sizeof(double) * num_col, acc[1], 0);
    gY.values = am_alloc(sizeof(double) * num_row, acc[1], 0);
    gAlpha.value = am_alloc(sizeof(double) * 1, acc[1], 0);
    gBeta.value = am_alloc(sizeof(double) * 1, acc[1], 0);

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

    am_copy(gX.values, host_X, sizeof(double) * num_col);
    am_copy(gY.values, host_Y, sizeof(double) * num_row);
    am_copy(gAlpha.value, host_alpha, sizeof(double) * 1);
    am_copy(gBeta.value, host_beta, sizeof(double) * 1);

    gAlpha.offValue = 0;
    gBeta.offValue = 0;
    gX.offValues = 0;
    gY.offValues = 0;

    gX.num_values = num_col;
    gY.num_values = num_row;

    gCsrMat.offValues = 0;
    gCsrMat.offColInd = 0;
    gCsrMat.offRowOff = 0;

    double *values = (double*)calloc(num_nonzero, sizeof(double));
    int *rowOffsets = (int*)calloc(num_row+1, sizeof(int));
    int *colIndices = (int*)calloc(num_nonzero, sizeof(int));
    ulong *rowBlocks = (ulong*)calloc(num_nonzero, sizeof(ulong));

    gCsrMat.values = am_alloc(sizeof(double) * num_nonzero, acc[1], 0);
    gCsrMat.rowOffsets = am_alloc(sizeof(int) * (num_row+1), acc[1], 0);
    gCsrMat.colIndices = am_alloc(sizeof(int) * num_nonzero, acc[1], 0);
    gCsrMat.rowBlocks = am_alloc(sizeof(ulong) * num_nonzero, acc[1], 0);

    status = hcsparseDCsrMatrixfromFile(&gCsrMat, filename, &control, false);
   
    am_copy(values, gCsrMat.values, sizeof(double) * num_nonzero);
    am_copy(rowOffsets, gCsrMat.rowOffsets, sizeof(int) * (num_row+1));
    am_copy(colIndices, gCsrMat.colIndices, sizeof(int) * num_nonzero);
    am_copy(rowBlocks, gCsrMat.rowBlocks, sizeof(ulong) * num_nonzero);

    if (status != hcsparseSuccess)
    {
        std::cout<<"The input file should be in mtx format"<<std::endl;
        return 0;
    }

    hcsparseCsrMetaSize(&gCsrMat, &control);

    hcsparseCsrMetaCompute(&gCsrMat, &control);
 
    hcsparseDcsrmv(&gAlpha, &gCsrMat, &gX, &gBeta, &gY, &control); 

    int col = 0;
    for (int row = 0; row < num_row; row++)
    {
        host_res[row] *= host_beta[0];
        for (; col < rowOffsets[row+1]; col++)
        {
            host_res[row] = host_alpha[0] * host_X[colIndices[col]] * values[col] + host_res[row];
        }
    }

    am_copy(host_Y, gY.values, sizeof(double) * num_row);

    bool isPassed = 1;  
 
    for (int i = 0; i < num_row; i++)
    {
        double diff = std::abs(host_res[i] - host_Y[i]);
        if (diff > 0.1)
        {
            std::cout<<i<<" "<<host_res[i]<<" "<<host_Y[i]<<" "<<diff<<std::endl;
            isPassed = 0;
            break;
        }
    }

    std::cout << (isPassed?"TEST PASSED":"TEST FAILED") << std::endl;

    hcsparseTeardown();

    free(host_res);
    free(host_X);
    free(host_Y);
    free(host_alpha);
    free(host_beta);
    free(values);
    free(rowOffsets);
    free(colIndices);
    free(rowBlocks);
    am_free(gX.values);
    am_free(gY.values);
    am_free(gAlpha.value);
    am_free(gBeta.value);
    am_free(gCsrMat.values);
    am_free(gCsrMat.rowOffsets);
    am_free(gCsrMat.colIndices);
    am_free(gCsrMat.rowBlocks);

    return 0; 
}
