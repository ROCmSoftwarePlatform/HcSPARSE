#include "hipsparse.h"
#include "hcsparse.h"

#ifdef __cplusplus
extern "C" {
#endif

hipsparseStatus_t hipHCSPARSEStatusToHIPStatus(hcsparseStatus_t hcStatus) 
{
   switch(hcStatus)
   {
      case HCSPARSE_STATUS_SUCCESS:
         return HIPSPARSE_STATUS_SUCCESS;
      case HCSPARSE_STATUS_NOT_INITIALIZED:
         return HIPSPARSE_STATUS_NOT_INITIALIZED;
      case HCSPARSE_STATUS_ALLOC_FAILED:
         return HIPSPARSE_STATUS_ALLOC_FAILED;
      case HCSPARSE_STATUS_INVALID_VALUE:
         return HIPSPARSE_STATUS_INVALID_VALUE;
      case HCSPARSE_STATUS_ARCH_MISMATCH:
         return HIPSPARSE_STATUS_ARCH_MISMATCH;
      case HCSPARSE_STATUS_MAPPING_ERROR:
         return HIPSPARSE_STATUS_MAPPING_ERROR;
      case HCSPARSE_STATUS_EXECUTION_FAILED:
         return HIPSPARSE_STATUS_EXECUTION_FAILED;
      case HCSPARSE_STATUS_INTERNAL_ERROR:
         return HIPSPARSE_STATUS_INTERNAL_ERROR;
      default:
         throw "Unimplemented status";
   }
}

hcsparseOperation_t hipHIPOperationToHCSPARSEOperation(hipsparseOperation_t op) 
{
   switch(op)
   {
      case HIPSPARSE_OPERATION_NON_TRANSPOSE:
         return HCSPARSE_OPERATION_NON_TRANSPOSE;
      case HIPSPARSE_OPERATION_TRANSPOSE:
         return HCSPARSE_OPERATION_TRANSPOSE;
      case HIPSPARSE_OPERATION_CONJUGATE_TRANSPOSE:
         return HCSPARSE_OPERATION_CONJUGATE_TRANSPOSE;
      default:
         throw "Invalid Operation Specified";
   }
}

hcsparseIndexBase_t hipHIPIndexBaseToHCSPARSEIndexBase(hipsparseIndexBase_t idBase) 
{
   switch(idBase)
   {
      case HIPSPARSE_INDEX_BASE_ZERO:
         return HCSPARSE_INDEX_BASE_ZERO;
      case HIPSPARSE_INDEX_BASE_ONE:
         return HCSPARSE_INDEX_BASE_ONE;
      default:
         throw "Invalid Index Base Specified";
   }
}

hcsparseMatrixType_t hipHIPMatrixTypeToHCSPARSEMatrixType(hipsparseMatrixType_t matType) 
{
   switch(matType)
   {
      case HIPSPARSE_MATRIX_TYPE_GENERAL:
         return HCSPARSE_MATRIX_TYPE_GENERAL;
      case HIPSPARSE_MATRIX_TYPE_SYMMETRIC:
         return HCSPARSE_MATRIX_TYPE_SYMMETRIC;
      case HIPSPARSE_MATRIX_TYPE_HERMITIAN:
         return HCSPARSE_MATRIX_TYPE_HERMITIAN;
      case HIPSPARSE_MATRIX_TYPE_TRIANGULAR:
         return HCSPARSE_MATRIX_TYPE_TRIANGULAR;
      default:
         throw "Invalid Matrix Type Specified";
   }
}

hcsparseDirection_t hipHIPDirectionToHCSPARSEDirection(hipsparseDirection_t dir) 
{
   switch(dir)
   {
      case HIPSPARSE_DIRECTION_ROW:
         return HCSPARSE_DIRECTION_ROW;
      case HIPSPARSE_DIRECTION_COLUMN:
         return HCSPARSE_DIRECTION_COLUMN;
      default:
         throw "Invalid Index Base Specified";
   }
}

hcsparseAction_t hipHIPActionToHCSPARSEAction(hipsparseAction_t act)
{
   switch(act)
   { 
     case HIPSPARSE_ACTION_SYMBOLIC:
        return HCSPARSE_ACTION_SYMBOLIC;
     case HIPSPARSE_ACTION_NUMERIC:
        return HCSPARSE_ACTION_NUMERIC;
     default:
        throw "Invalid Action Specified";
   }
}

hipsparseStatus_t hipsparseCreate(hipsparseHandle_t *handle)
{
   int deviceId;
   hipError_t err;
   hipsparseStatus_t retval = HIPSPARSE_STATUS_SUCCESS;

   err = hipGetDevice(&deviceId);
   if (err == hipSuccess) {
     hc::accelerator_view *av;
     err = hipHccGetAcceleratorView(hipStreamDefault, &av);
     if (err == hipSuccess) {
       retval = hipHCSPARSEStatusToHIPStatus(hcsparseCreate(&*handle, av));
     } else {
       retval = HIPSPARSE_STATUS_EXECUTION_FAILED;
     }
   }
  return retval;
}

hipsparseStatus_t hipsparseDestroy(hipsparseHandle_t handle)
{
   return hipHCSPARSEStatusToHIPStatus(hcsparseDestroy(&handle));
}

hipsparseStatus_t hipsparseCreateMatDescr(hipsparseMatDescr_t *descrA)
{
   return hipHCSPARSEStatusToHIPStatus(hcsparseCreateMatDescr(descrA));
}

hipsparseStatus_t hipsparseDestroyMatDescr(hipsparseMatDescr_t descrA)
{
   return hipHCSPARSEStatusToHIPStatus(hcsparseDestroyMatDescr(descrA));
}


hipsparseStatus_t hipsparseSetMatType(hipsparseMatDescr_t descrA,
                                      hipsparseMatrixType_t type)
{
  return hipHCSPARSEStatusToHIPStatus(hcsparseSetMatType(descrA, 
                                      hipHIPMatrixTypeToHCSPARSEMatrixType(type)));
}

hipsparseStatus_t hipsparseSetMatIndexBase(hipsparseMatDescr_t descrA, 
                                           hipsparseIndexBase_t base)
{
  return hipHCSPARSEStatusToHIPStatus(hcsparseSetMatIndexBase(descrA,
                                      hipHIPIndexBaseToHCSPARSEIndexBase(base)));
}

//Sparse L1 BLAS operations

hipsparseStatus_t hipsparseSdoti(hipsparseHandle_t handle, int nnz, 
                              const float           *xVal, 
                              const int *xInd, const float           *y, 
                              float           *resultDevHostPtr, 
                              hipsparseIndexBase_t idxBase){

  return hipHCSPARSEStatusToHIPStatus(hcsparseSdoti(handle, nnz, xVal, xInd, y, 
                                                    resultDevHostPtr,
                                                    hipHIPIndexBaseToHCSPARSEIndexBase(idxBase)));
}

hipsparseStatus_t hipsparseDdoti(hipsparseHandle_t handle, int nnz, 
                                 const double    *xVal, 
                                 const int *xInd, const double *y, 
                                 double *resultDevHostPtr, 
                                 hipsparseIndexBase_t idxBase){

  return hipHCSPARSEStatusToHIPStatus(hcsparseDdoti(handle, nnz, xVal, xInd, y, 
                                                    resultDevHostPtr,
                                                    hipHIPIndexBaseToHCSPARSEIndexBase(idxBase)));
}

//Sparse L2 BLAS operations

hipsparseStatus_t hipsparseScsrmv(hipsparseHandle_t handle, hipsparseOperation_t transA, 
                                  int m, int n, int nnz, const float           *alpha, 
                                  const hipsparseMatDescr_t descrA, 
                                  const float           *csrValA, 
                                  const int *csrRowPtrA, const int *csrColIndA,
                                  const float           *x, const float           *beta, 
                                  float           *y) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseScsrmv(handle, hipHIPOperationToHCSPARSEOperation(transA), m, n,
                                                      nnz, alpha, descrA,
                                                      csrValA, csrRowPtrA,
                                                      csrColIndA, x, beta, y));
}


hipsparseStatus_t hipsparseDcsrmv(hipsparseHandle_t handle, hipsparseOperation_t transA, 
                                  int m, int n, int nnz, const double           *alpha, 
                                  const hipsparseMatDescr_t descrA, 
                                  const double           *csrValA, 
                                  const int *csrRowPtrA, const int *csrColIndA,
                                  const double           *x, const double           *beta, 
                                  double           *y) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseDcsrmv(handle, hipHIPOperationToHCSPARSEOperation(transA), m, n,
                                                      nnz, alpha, descrA,
                                                      csrValA, csrRowPtrA,
                                                      csrColIndA, x, beta, y));

}

//Sparse L3 BLAS operations

hipsparseStatus_t hipsparseScsrmm(hipsparseHandle_t handle, 
                                                hipsparseOperation_t transA, 
                                                int m, int n, int k, int nnz, 
                                                const float           *alpha, 
                                                const hipsparseMatDescr_t descrA, 
                                                const float             *csrValA, 
                                                const int             *csrRowPtrA, 
                                                const int             *csrColIndA,
                                                const float *B,             int ldb,
                                                const float *beta, float *C, int ldc) {
   float *alpha_d = NULL;
   float *beta_d = NULL;
   hipMalloc(&alpha_d, 1 * sizeof(float));
   hipMalloc(&beta_d, 1 * sizeof(float));
   hipMemcpy(alpha_d, alpha, 1 * sizeof(float), hipMemcpyHostToDevice);
   hipMemcpy(beta_d, beta, 1 * sizeof(float), hipMemcpyHostToDevice);

   hipsparseStatus_t status;

   if(transA == HIPSPARSE_OPERATION_TRANSPOSE)
   {
	float* cscVal = NULL;
	int *cscRowInd = NULL;
	int *cscColPtr = NULL;
	hipMalloc(&cscVal, nnz * sizeof(float));
	hipMalloc(&cscRowInd, nnz * sizeof(int));
	hipMalloc(&cscColPtr, (k+1) * sizeof(int));

	status = hipsparseScsr2csc(handle, m, k, nnz, csrValA, csrRowPtrA, csrColIndA, cscVal, cscRowInd, cscColPtr, HIPSPARSE_ACTION_NUMERIC, HIPSPARSE_INDEX_BASE_ZERO);

	if(status == HIPSPARSE_STATUS_SUCCESS)
	{
		transA = HIPSPARSE_OPERATION_NON_TRANSPOSE;
		status = hipHCSPARSEStatusToHIPStatus(hcsparseScsrmm(handle,hipHIPOperationToHCSPARSEOperation(transA), k, n, m, nnz,
                                                      alpha_d, descrA, cscVal, cscColPtr, cscRowInd,
						      B, ldb, beta_d, C, ldc));
	}

        hipFree(&cscVal);
        hipFree(&cscRowInd);
        hipFree(&cscColPtr);
   }
   else
   {
   	status = hipHCSPARSEStatusToHIPStatus(hcsparseScsrmm(handle,hipHIPOperationToHCSPARSEOperation(transA), m, n, k, nnz,
                                                      alpha_d, descrA, csrValA, csrRowPtrA,
                                                      csrColIndA, B, ldb, beta_d, C, ldc));
   }

   hipFree(&alpha_d);
   hipFree(&beta_d);

   return status;
}

hipsparseStatus_t hipsparseDcsrmm(hipsparseHandle_t handle, 
                                                hipsparseOperation_t transA, 
                                                int m, int n, int k, int nnz, 
                                                const double           *alpha, 
                                                const hipsparseMatDescr_t descrA, 
                                                const double             *csrValA, 
                                                const int             *csrRowPtrA, 
                                                const int             *csrColIndA,
                                                const double *B,             int ldb,
                                                const double *beta, double *C, int ldc) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseDcsrmm(handle, hipHIPOperationToHCSPARSEOperation(transA), m, n, k, nnz,
                                                      alpha, descrA, csrValA, csrRowPtrA,
                                                      csrColIndA, B, ldb, beta, C, ldc));
}

hipsparseStatus_t hipsparseScsrmm2(hipsparseHandle_t handle, 
                                                hipsparseOperation_t transA, 
                                                hipsparseOperation_t transB, 
                                                int m, int n, int k, int nnz, 
                                                const float           *alpha, 
                                                const hipsparseMatDescr_t descrA, 
                                                const float             *csrValA, 
                                                const int             *csrRowPtrA, 
                                                const int             *csrColIndA,
                                                const float *B,             int ldb,
                                                const float *beta, float *C, int ldc) {


   return hipHCSPARSEStatusToHIPStatus(hcsparseScsrmm2(handle, hipHIPOperationToHCSPARSEOperation(transA),
                                                       hipHIPOperationToHCSPARSEOperation(transB), m, n, k, nnz,
                                                       alpha, descrA, csrValA, csrRowPtrA,
                                                       csrColIndA, B, ldb, beta, C, ldc));
}

hipsparseStatus_t hipsparseDcsrmm2(hipsparseHandle_t handle, 
                                                hipsparseOperation_t transA, 
                                                hipsparseOperation_t transB, 
                                                int m, int n, int k, int nnz, 
                                                const double           *alpha, 
                                                const hipsparseMatDescr_t descrA, 
                                                const double             *csrValA, 
                                                const int             *csrRowPtrA, 
                                                const int             *csrColIndA,
                                                const double *B,             int ldb,
                                                const double *beta, double *C, int ldc) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseDcsrmm2(handle, hipHIPOperationToHCSPARSEOperation(transA),
                                                       hipHIPOperationToHCSPARSEOperation(transB), m, n, k, nnz,
                                                       alpha, descrA, csrValA, csrRowPtrA,
                                                       csrColIndA, B, ldb, beta, C, ldc));
}

hipsparseStatus_t hipsparseXcsrgemmNnz(hipsparseHandle_t handle,
                                       hipsparseOperation_t transA,
                                       hipsparseOperation_t transB,
                                       int m, int n, int k,
                                       const hipsparseMatDescr_t descrA,
                                       const int nnzA,
                                       const int *csrRowPtrA,
                                       const int *csrColIndA,
                                       const hipsparseMatDescr_t descrB,
                                       const int nnzB,
                                       const int *csrRowPtrB,
                                       const int *csrColIndB,
                                       const hipsparseMatDescr_t descrC,
                                       int *csrRowPtrC,
                                       int *nnzTotalDevHostPtr) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseXcsrgemmNnz(handle,
                                                           hipHIPOperationToHCSPARSEOperation(transA),
                                                           hipHIPOperationToHCSPARSEOperation(transB),
                                                           m, n, k, descrA,
                                                           nnzA, csrRowPtrA,
                                                           csrColIndA, descrB,
                                                           nnzB, csrRowPtrB,
                                                           csrColIndB, descrC,
                                                           csrRowPtrC,
                                                           nnzTotalDevHostPtr));
}

hipsparseStatus_t hipsparseScsrgemm(hipsparseHandle_t handle,
                                    hipsparseOperation_t transA, 
                                    hipsparseOperation_t transB,
                                    int m, int n, int k,
                                    const hipsparseMatDescr_t descrA, 
                                    const int nnzA, const float *csrValA,
                                    const int *csrRowPtrA, 
                                    const int *csrColIndA,
                                    const hipsparseMatDescr_t descrB, 
                                    const int nnzB, const float *csrValB, 
                                    const int *csrRowPtrB, 
                                    const int *csrColIndB,
                                    const hipsparseMatDescr_t descrC,
                                    float *csrValC, const int *csrRowPtrC, 
                                    int *csrColIndC ) { 

   return hipHCSPARSEStatusToHIPStatus(hcsparseScsrgemm(handle, 
                                                        hipHIPOperationToHCSPARSEOperation(transA),
                                                        hipHIPOperationToHCSPARSEOperation(transB),
                                                        m, n, k, descrA,
                                                        nnzA, csrValA, csrRowPtrA,
                                                        csrColIndA, descrB, nnzB,
                                                        csrValB, csrRowPtrB,
                                                        csrColIndB, descrC,
                                                        csrValC, csrRowPtrC,
                                                        csrColIndC));
}

hipsparseStatus_t hipsparseDcsrgemm(hipsparseHandle_t handle,
                                    hipsparseOperation_t transA, 
                                    hipsparseOperation_t transB,
                                    int m, int n, int k,
                                    const hipsparseMatDescr_t descrA, 
                                    const int nnzA, const double *csrValA,
                                    const int *csrRowPtrA, 
                                    const int *csrColIndA,
                                    const hipsparseMatDescr_t descrB, 
                                    const int nnzB, const double *csrValB, 
                                    const int *csrRowPtrB, 
                                    const int *csrColIndB,
                                    const hipsparseMatDescr_t descrC,
                                    double *csrValC, const int *csrRowPtrC, 
                                    int *csrColIndC ) { 

   return hipHCSPARSEStatusToHIPStatus(hcsparseDcsrgemm(handle, 
                                                        hipHIPOperationToHCSPARSEOperation(transA),
                                                        hipHIPOperationToHCSPARSEOperation(transB),
                                                        m, n, k, descrA,
                                                        nnzA, csrValA, csrRowPtrA,
                                                        csrColIndA, descrB, nnzB,
                                                        csrValB, csrRowPtrB,
                                                        csrColIndB, descrC,
                                                        csrValC, csrRowPtrC,
                                                        csrColIndC));
}

hipsparseStatus_t hipsparseDcsrgemm(hipsparseHandle_t handle,
                                    hipsparseOperation_t transA, 
                                    hipsparseOperation_t transB,
                                    int m, int n, int k,
                                    const hipsparseMatDescr_t descrA, 
                                    const int nnzA, const double *csrValA,
                                    const int *csrRowPtrA, 
                                    const int *csrColIndA,
                                    const hipsparseMatDescr_t descrB, 
                                    const int nnzB, const double *csrValB, 
                                    const int *csrRowPtrB, 
                                    const int *csrColIndB,
                                    const hipsparseMatDescr_t descrC,
                                    double *csrValC, const int *csrRowPtrC, 
                                    int *csrColIndC ) { 

   return hipHCSPARSEStatusToHIPStatus(hcsparseDcsrgemm(handle,
                                                        hipHIPOperationToHCSPARSEOperation(transA),
                                                        hipHIPOperationToHCSPARSEOperation(transB),
                                                        m, n, k, descrA,
                                                        nnzA, csrValA, csrRowPtrA,
                                                        csrColIndA, descrB, nnzB,
                                                        csrValB, csrRowPtrB,
                                                        csrColIndB, descrC,
                                                        csrValC, csrRowPtrC,
                                                        csrColIndC));
}

hipsparseStatus_t hipsparseXcsrgeamNnz(hipsparseHandle_t handle, 
                                       int m, int n,
                                       const hipsparseMatDescr_t descrA, 
                                       int nnzA,
                                       const int *csrRowPtrA, 
                                       const int *csrColIndA,
                                       const hipsparseMatDescr_t descrB, 
                                       int nnzB,
                                       const int *csrRowPtrB, 
                                       const int *csrColIndB,
                                       const hipsparseMatDescr_t descrC,
                                       int *csrRowPtrC, 
                                       int *nnzTotalDevHostPtr) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseXcsrgeamNnz(handle, m, n, descrA,
                                                           nnzA, csrRowPtrA,
                                                           csrColIndA, descrB, nnzB,
                                                           csrRowPtrB, csrColIndB,
                                                           descrC, csrRowPtrC,
                                                           nnzTotalDevHostPtr));

}

hipsparseStatus_t hipsparseScsrgeam(hipsparseHandle_t handle, 
                                    int m, int n, const float *alpha,
                                    const hipsparseMatDescr_t descrA, 
                                    int nnzA,
                                    const float *csrValA, 
                                    const int *csrRowPtrA, 
                                    const int *csrColIndA,
                                    const float *beta,
                                    const hipsparseMatDescr_t descrB, 
                                    int nnzB,
                                    const float *csrValB, 
                                    const int *csrRowPtrB, 
                                    const int *csrColIndB,
                                    const hipsparseMatDescr_t descrC,
                                    float *csrValC, 
                                    int *csrRowPtrC, 
                                    int *csrColIndC) {

   float *alpha_d = NULL;
   float *beta_d = NULL;
   hipMalloc(&alpha_d, 1 * sizeof(float));
   hipMalloc(&beta_d, 1 * sizeof(float));
   hipMemcpy(alpha_d, alpha, 1 * sizeof(float), hipMemcpyHostToDevice);
   hipMemcpy(beta_d, beta, 1 * sizeof(float), hipMemcpyHostToDevice);
   
   hipsparseStatus_t status;
   status = hipHCSPARSEStatusToHIPStatus(hcsparseScsrgeam(handle, m, n, alpha_d, descrA,
                                                        nnzA, csrValA, csrRowPtrA,
                                                        csrColIndA, beta_d, descrB, nnzB,
                                                        csrValB, csrRowPtrB, csrColIndB,
                                                        descrC, csrValC, csrRowPtrC,
                                                        csrColIndC));
   hipFree(&alpha_d);
   hipFree(&beta_d);

   return status;

}

hipsparseStatus_t hipsparseDcsrgeam(hipsparseHandle_t handle, 
                                    int m, int n, const double *alpha,
                                    const hipsparseMatDescr_t descrA, 
                                    int nnzA,
                                    const double *csrValA, 
                                    const int *csrRowPtrA, 
                                    const int *csrColIndA,
                                    const double *beta,
                                    const hipsparseMatDescr_t descrB, 
                                    int nnzB,
                                    const double *csrValB, 
                                    const int *csrRowPtrB, 
                                    const int *csrColIndB,
                                    const hipsparseMatDescr_t descrC,
                                    double *csrValC, 
                                    int *csrRowPtrC, 
                                    int *csrColIndC) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseDcsrgeam(handle, m, n, alpha, descrA,
                                                        nnzA, csrValA, csrRowPtrA,
                                                        csrColIndA, beta, descrB, nnzB,
                                                        csrValB, csrRowPtrB, csrColIndB,
                                                        descrC, csrValC, csrRowPtrC,
                                                        csrColIndC));

}

//Matrix conversion routines

hipsparseStatus_t hipsparseSdense2csr(hipsparseHandle_t handle, int m, int n, 
                                                      const hipsparseMatDescr_t descrA, 
                                                      const float           *A, 
                                                      int lda, const int *nnzPerRow, 
                                                      float           *csrValA, 
                                                      int *csrRowPtrA, int *csrColIndA) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseSdense2csr( handle, m, n, descrA, 
                                                           A, lda, nnzPerRow, csrValA,
                                                           csrRowPtrA, csrColIndA)); 
}

hipsparseStatus_t hipsparseDdense2csr(hipsparseHandle_t handle, int m, int n, 
                                                      const hipsparseMatDescr_t descrA, 
                                                      const double           *A, 
                                                      int lda, const int *nnzPerRow, 
                                                      double           *csrValA, 
                                                      int *csrRowPtrA, int *csrColIndA) {

   return hipHCSPARSEStatusToHIPStatus(hcsparseDdense2csr( handle, m, n, descrA, 
                                                           A, lda, nnzPerRow, csrValA,
                                                           csrRowPtrA, csrColIndA)); 
}

hipsparseStatus_t hipsparseScsr2dense(hipsparseHandle_t handle, int m, int n, 
                                                    const hipsparseMatDescr_t descrA, 
                                                    const float             *csrValA, 
                                                    const int *csrRowPtrA, const int *csrColIndA,
                                                    float *A, int lda){

   return hipHCSPARSEStatusToHIPStatus(hcsparseScsr2dense( handle, m, n,
                                                          descrA, csrValA, csrRowPtrA,
                                                          csrColIndA, A, lda));
}


hipsparseStatus_t hipsparseDcsr2dense(hipsparseHandle_t handle, int m, int n, 
                                                    const hipsparseMatDescr_t descrA, 
                                                    const double             *csrValA, 
                                                    const int *csrRowPtrA, const int *csrColIndA,
                                                    double *A, int lda){

   return hipHCSPARSEStatusToHIPStatus(hcsparseDcsr2dense( handle, m, n,
                                                          descrA, csrValA, csrRowPtrA,
                                                          csrColIndA, A, lda));
}


hipsparseStatus_t hipsparseXcoo2csr(hipsparseHandle_t handle, const int *cooRowIndA, 
                                                  int nnz, int m, int *csrRowPtrA, 
                                                  hipsparseIndexBase_t idxBase){

   return hipHCSPARSEStatusToHIPStatus(hcsparseXcoo2csr(handle, cooRowIndA, nnz,
                                                        m, csrRowPtrA,
                                                        hipHIPIndexBaseToHCSPARSEIndexBase(idxBase)));
}

hipsparseStatus_t hipsparseXcsr2coo(hipsparseHandle_t handle, const int *csrRowPtrA,
                                                  int nnz, int m, int *cooRowIndA, 
                                                  hipsparseIndexBase_t idxBase){

  return hipHCSPARSEStatusToHIPStatus(hcsparseXcsr2coo(handle, csrRowPtrA, nnz,
                                                       m, cooRowIndA,
                                                       hipHIPIndexBaseToHCSPARSEIndexBase(idxBase)));
}

hipsparseStatus_t hipsparseSnnz(hipsparseHandle_t handle, hipsparseDirection_t dirA, int m, 
                              int n, const hipsparseMatDescr_t descrA, 
                              const float           *A, int lda, 
                              int *nnzPerRowColumn, int *nnzTotalDevHostPtr){

  return hipHCSPARSEStatusToHIPStatus(hcsparseSnnz(handle, hipHIPDirectionToHCSPARSEDirection(dirA),
                                      m, n, descrA, A, lda,
                                      nnzPerRowColumn, nnzTotalDevHostPtr));
}

hipsparseStatus_t hipsparseDnnz(hipsparseHandle_t handle, hipsparseDirection_t dirA, int m, 
                              int n, const hipsparseMatDescr_t descrA, 
                              const double *A, int lda, 
                              int *nnzPerRowColumn, int *nnzTotalDevHostPtr){

  return hipHCSPARSEStatusToHIPStatus(hcsparseDnnz(handle, hipHIPDirectionToHCSPARSEDirection(dirA),
                                                   m, n, descrA, A, lda,
                                                   nnzPerRowColumn, nnzTotalDevHostPtr));
}

hipsparseStatus_t hipsparseScsc2dense(hipsparseHandle_t handle, int m, int n, 
                              const hipsparseMatDescr_t descrA, 
                              const float           *cscValA, 
                              const int *cscRowIndA, const int *cscColPtrA,
                              float           *A, int lda){

  return hipHCSPARSEStatusToHIPStatus(hcsparseScsc2dense(handle, m, n, descrA, 
                                                         cscValA, cscRowIndA, 
                                                         cscColPtrA, A, lda));
}

hipsparseStatus_t hipsparseDcsc2dense(hipsparseHandle_t handle, int m, int n, 
                              const hipsparseMatDescr_t descrA, 
                              const double *cscValA, 
                              const int *cscRowIndA, const int *cscColPtrA,
                              double    *A, int lda){

  return hipHCSPARSEStatusToHIPStatus(hcsparseDcsc2dense(handle, m, n, descrA, 
                                                         cscValA, cscRowIndA, 
                                                         cscColPtrA, A, lda));
}


hipsparseStatus_t hipsparseSdense2csc(hipsparseHandle_t handle, int m, int n, 
                              const hipsparseMatDescr_t descrA, 
                              const float           *A, 
                              int lda, const int *nnzPerCol, 
                              float           *cscValA, 
                              int *cscRowIndA, int *cscColPtrA){

  return hipHCSPARSEStatusToHIPStatus(hcsparseSdense2csc(handle, m, n, descrA, A, 
                                                         lda, nnzPerCol, cscValA, 
                                                         cscRowIndA, cscColPtrA));
}

hipsparseStatus_t hipsparseDdense2csc(hipsparseHandle_t handle, int m, int n, 
                              const hipsparseMatDescr_t descrA, 
                              const double *A, 
                              int lda, const int *nnzPerCol, 
                              double *cscValA, 
                              int *cscRowIndA, int *cscColPtrA){

  return hipHCSPARSEStatusToHIPStatus(hcsparseDdense2csc(handle, m, n, descrA, A, 
                                                         lda, nnzPerCol, cscValA, 
                                                         cscRowIndA, cscColPtrA));
}

hipsparseStatus_t 
hipsparseScsr2csc(hipsparseHandle_t handle, int m, int n, int nnz,
                 const float *csrVal, const int *csrRowPtr, 
                 const int *csrColInd, float           *cscVal,
                 int *cscRowInd, int *cscColPtr, 
                 hipsparseAction_t copyValues, 
                 hipsparseIndexBase_t idxBase)

{
  hipsparseStatus_t status = HIPSPARSE_STATUS_EXECUTION_FAILED;

  if(copyValues == HIPSPARSE_ACTION_NUMERIC && idxBase == HIPSPARSE_INDEX_BASE_ZERO)
  {
	hipsparseMatDescr_t descrA;
    	status = hipsparseCreateMatDescr(&descrA);
    	if (status != HIPSPARSE_STATUS_SUCCESS) {
      		std::cout << "error creating mat descrptr"<<std::endl;
      		exit(1);
    	}

	float *denseValdevice = NULL;
	hipMalloc(&denseValdevice, m * n * sizeof(float));
	hipsparseStatus_t status;

        status = hipsparseScsr2dense(handle, m, n, descrA, csrVal, csrRowPtr, csrColInd, denseValdevice, n);
	if(status != HIPSPARSE_STATUS_SUCCESS )
  	{
        	std::cout<< " csr 2 dense conversion";
        	return status;
  	}

	int *nnz_per_column = NULL;
  	int *nnz_ptr = NULL;

  	hipMalloc(&nnz_per_column, n * sizeof(int));
  	hipMalloc(&nnz_ptr, 1 * sizeof(int));

        status = hipsparseSnnz(handle, HIPSPARSE_DIRECTION_COLUMN, m, n, descrA, denseValdevice, n, nnz_per_column, nnz_ptr);
  	if(status != HIPSPARSE_STATUS_SUCCESS )
  	{
        	std::cout<< " nnz calculation error";
        	return status;
  	}

        status = hipsparseSdense2csc(handle, m, n, descrA, denseValdevice, n, nnz_per_column,
                                cscVal, cscRowInd, cscColPtr);

  	if(status != HIPSPARSE_STATUS_SUCCESS )
  	{
                std::cout<< " dense 2 csc conversion error";
		std::cout<< status;
        	return status;
  	}

	hipFree(&denseValdevice);
	hipFree(&nnz_per_column);
	hipFree(&nnz_ptr);
 }
 	return status;
}

hipsparseStatus_t 
hipsparseDcsr2csc(hipsparseHandle_t handle, int m, int n, int nnz,
                 const double *csrVal, const int *csrRowPtr, 
                 const int *csrColInd, double           *cscVal,
                 int *cscRowInd, int *cscColPtr, 
                 hipsparseAction_t copyValues, 
                 hipsparseIndexBase_t idxBase)

{

  return hipHCSPARSEStatusToHIPStatus(hcsparseDcsr2csc(handle, m, n, nnz, csrVal,
                                                       csrRowPtr, csrColInd, cscVal,
                                                       cscRowInd, cscColPtr, 
                                                       hipHIPActionToHCSPARSEAction(copyValues),
                                                       hipHIPIndexBaseToHCSPARSEIndexBase(idxBase)));

}

hipsparseStatus_t
hipsparseSetStream(hipsparseHandle_t handle, hipStream_t streamId)
{
  if (handle == nullptr) {
    return HIPSPARSE_STATUS_NOT_INITIALIZED;
  }

  hc::accelerator_view *pAcclView;
  hipError_t err = hipHccGetAcceleratorView(streamId, &pAcclView);

  if (err != hipSuccess) {
    return HIPSPARSE_STATUS_NOT_INITIALIZED;
  }

  return hipHCSPARSEStatusToHIPStatus(
      hcsparseSetAcclView(handle, *pAcclView, static_cast<void *>(streamId)));
}
#ifdef __cplusplus
}
#endif

