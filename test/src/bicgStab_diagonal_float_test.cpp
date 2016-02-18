#include <hcsparse.h>
#include <iostream>
int main(int argc, char *argv[])
{
    hcsparseCsrMatrix gA;
    hcdenseVector gX;
    hcdenseVector gB;

    std::vector<Concurrency::accelerator>acc = Concurrency::accelerator::get_all();
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

    float *host_X = (float*) calloc(num_col, sizeof(float));
    float *host_B = (float*) calloc(num_row, sizeof(float));

    srand (time(NULL));
    for (int i = 0; i < num_col; i++)
    {
       host_X[i] = rand()%100;
    } 

    for (int i = 0; i < num_row; i++)
    {
        host_B[i] = rand()%100;
    }

    Concurrency::array_view<float> dev_X(num_col, host_X);
    Concurrency::array_view<float> dev_B(num_row, host_B);

    hcsparseSetup();
    hcsparseInitCsrMatrix(&gA);
    hcsparseInitVector(&gX);
    hcsparseInitVector(&gB);

    gX.values = &dev_X;
    gB.values = &dev_B;

    gX.offValues = 0;
    gB.offValues = 0;

    gX.num_values = num_col;
    gB.num_values = num_row;

    gA.offValues = 0;
    gA.offColInd = 0;
    gA.offRowOff = 0;

    float *values = (float*)calloc(num_nonzero, sizeof(float));
    int *rowIndices = (int*)calloc(num_row+1, sizeof(int));
    int *colIndices = (int*)calloc(num_nonzero, sizeof(int));

    Concurrency::array_view<float> av_values(num_nonzero, values);
    Concurrency::array_view<int> av_rowOff(num_row+1, rowIndices);
    Concurrency::array_view<int> av_colIndices(num_nonzero, colIndices);

    gA.values = &av_values;
    gA.rowOffsets = &av_rowOff;
    gA.colIndices = &av_colIndices;

    status = hcsparseSCsrMatrixfromFile(&gA, filename, &control, false);
   
    if (status != hcsparseSuccess)
    {
        std::cout<<"The input file should be in mtx format"<<std::endl;
        return 0;
    }

    int maxIter = 100;
    double relTol = 0.0001;
    double absTol = 0.0001;

    hcsparseSolverControl *solver_control;

    solver_control = hcsparseCreateSolverControl(DIAGONAL, maxIter, relTol, absTol); 

    hcsparseScsrbicgStab(&gX, &gA, &gB, solver_control, &control); 

    hcsparseTeardown();

    return 0; 
}