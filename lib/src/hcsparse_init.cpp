#include "hcsparse.h"
#include "blas2/csrmv.h"
#include "blas3/csrmm.h"
#include "blas3/hcsparse-spm-spm.h"
#include "blas1/hcdense-scale.h"
#include "blas1/hcdense-axpby.h"
#include "blas1/hcdense-axpy.h"
#include "blas1/hcdense-reduce.h"
#include "blas1/reduce-operators.h"
#include "blas1/hcdense-nrm1.h"
#include "blas1/hcdense-nrm2.h"
#include "blas1/hcdense-dot.h"
#include "blas1/elementwise-transform.h"
#include "io/mm_reader.h"
#include "blas2/csr_meta.h"
#include "solvers/preconditioners/preconditioner.h"
#include "solvers/preconditioners/diagonal.h"
#include "solvers/preconditioners/void.h"
#include "solvers/solver-control.h"
#include "solvers/biconjugate-gradients-stabilized.h"
#include "solvers/conjugate-gradients.h"
#include "transform/scan.h"
#include "transform/reduce-by-key.h"
#include "transform/conversion-utils.h"
#include "transform/hcsparse-coo2csr.h"
#include "transform/hcsparse-csr2coo.h"
#include "transform/hcsparse-csr2dense.h"
#include "transform/hcsparse-dense2csr.h"

int hcsparseInitialized = 0;

hcsparseStatus
hcsparseSetup(void)
{
    if(hcsparseInitialized)
    {
        return hcsparseSuccess;
    }

    hcsparseInitialized = 1;
    return hcsparseSuccess;
}

hcsparseStatus
hcsparseTeardown(void)
{
    if(!hcsparseInitialized)
    {
        return hcsparseSuccess;
    }

    hcsparseInitialized = 0;
    return hcsparseSuccess;
}

// Convenience sparse matrix construction functions
hcsparseStatus
hcsparseInitScalar( hcsparseScalar* scalar )
{
    scalar->clear( );

    return hcsparseSuccess;
};

hcsparseStatus
hcsparseInitVector( hcdenseVector* vec )
{
    vec->clear( );

    return hcsparseSuccess;
};

hcsparseStatus
hcsparseInitCooMatrix( hcsparseCooMatrix* cooMatx )
{
    cooMatx->clear( );

    return hcsparseSuccess;
};

hcsparseStatus
hcsparseInitCsrMatrix( hcsparseCsrMatrix* csrMatx )
{
    csrMatx->clear( );

    return hcsparseSuccess;
};

hcsparseStatus
hcdenseInitMatrix( hcdenseMatrix* denseMatx )
{
    denseMatx->clear();

    return hcsparseSuccess;
}

hcsparseStatus
    hcsparseScsrmv( const hcsparseScalar* alpha,
                        const hcsparseCsrMatrix* matx,
                        const hcdenseVector* x,
                        const hcsparseScalar* beta,
                        hcdenseVector* y,
                        const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    //check opencl elements
    if (x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csrmv<float>(alpha, matx, x, beta, y, control);
}

hcsparseStatus
    hcsparseDcsrmv( const hcsparseScalar* alpha,
                        const hcsparseCsrMatrix* matx,
                        const hcdenseVector* x,
                        const hcsparseScalar* beta,
                        hcdenseVector* y,
                        const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    //check opencl elements
    if (x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csrmv<double>(alpha, matx, x, beta, y, control);
}

hcsparseStatus
hcsparseScsrmm( const hcsparseScalar* alpha,
                const hcsparseCsrMatrix* sparseCsrA,
                const hcdenseMatrix* denseB,
                const hcsparseScalar* beta,
                hcdenseMatrix* denseC,
                const hcsparseControl *control )
{
    if( !hcsparseInitialized )
    {
        return hcsparseInvalid;
    }

    if( denseB->values == nullptr || denseC->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csrmm<float> ( alpha, sparseCsrA, denseB, beta, denseC, control );
}

hcsparseStatus
hcsparseDcsrmm( const hcsparseScalar* alpha,
                const hcsparseCsrMatrix* sparseCsrA,
                const hcdenseMatrix* denseB,
                const hcsparseScalar* beta,
                hcdenseMatrix* denseC,
                const hcsparseControl *control )
{
    if( !hcsparseInitialized )
    {
        return hcsparseInvalid;
    }

    if( denseB->values == nullptr || denseC->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csrmm<double> ( alpha, sparseCsrA, denseB, beta, denseC, control );
}

hcsparseStatus
hcdenseSscale ( hcdenseVector* r,
                const hcsparseScalar* alpha,
                const hcdenseVector* y,
                const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return scale<float> (r, alpha, y, control);
}


hcsparseStatus
hcdenseDscale( hcdenseVector* r,
               const hcsparseScalar* alpha,
               const hcdenseVector* y,
               const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return scale<double> (r, alpha, y, control);
}

hcsparseStatus
hcdenseSaxpy ( hcdenseVector* r,
                const hcsparseScalar* alpha,
                const hcdenseVector* x,
                const hcdenseVector* y,
                const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || y->values == nullptr || x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return axpy<float> (r, alpha, x, y,  control);
}

hcsparseStatus
hcdenseDaxpy ( hcdenseVector* r,
                const hcsparseScalar* alpha,
                const hcdenseVector* x,
                const hcdenseVector* y,
                const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || y->values == nullptr || x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return axpy<double> (r, alpha, x, y,  control);
}

hcsparseStatus
hcdenseSaxpby ( hcdenseVector* r,
                const hcsparseScalar* alpha,
                const hcdenseVector* x,
                const hcsparseScalar* beta,
                const hcdenseVector* y,
                const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || y->values == nullptr || x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return axpby<float> (r, alpha, x, beta, y,  control);
}

hcsparseStatus
hcdenseDaxpby ( hcdenseVector* r,
                const hcsparseScalar* alpha,
                const hcdenseVector* x,
                const hcsparseScalar* beta,
                const hcdenseVector* y,
                const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || y->values == nullptr || x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return axpby<double> (r, alpha, x, beta, y,  control);
}

hcsparseStatus
hcdenseIreduce(hcsparseScalar* s,
               const hcdenseVector* x,
               const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return reduce<int, RO_PLUS>(s, x, control);
}

hcsparseStatus
hcdenseSreduce(hcsparseScalar* s,
               const hcdenseVector* x,
               const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return reduce<float, RO_PLUS>(s, x, control);
}

hcsparseStatus
hcdenseDreduce(hcsparseScalar* s,
               const hcdenseVector* x,
               const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return reduce<double, RO_PLUS>(s, x, control);
}

hcsparseStatus
hcdenseSadd( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<float, EW_PLUS> (r, x, y, control);
}

hcsparseStatus
hcdenseDadd( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<double, EW_PLUS> (r, x, y, control);
}

hcsparseStatus
hcdenseSsub( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<float, EW_MINUS> (r, x, y, control);
}

hcsparseStatus
hcdenseDsub( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<double, EW_MINUS> (r, x, y, control);
}

hcsparseStatus
hcdenseSmul( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<float, EW_MULTIPLY> (r, x, y, control);
}

hcsparseStatus
hcdenseDmul( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<double, EW_MULTIPLY> (r, x, y, control);
}

hcsparseStatus
hcdenseSdiv( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<float, EW_DIV> (r, x, y, control);
}

hcsparseStatus
hcdenseDdiv( hcdenseVector* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (r->values == nullptr || x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return elementwise_transform<double, EW_DIV> (r, x, y, control);
}

hcsparseStatus
hcdenseSnrm1(hcsparseScalar* s,
             const hcdenseVector* x,
             const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return Norm1<float>(s, x, control);
}

hcsparseStatus
hcdenseDnrm1(hcsparseScalar* s,
             const hcdenseVector* x,
             const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return Norm1<double>(s, x, control);
}

hcsparseStatus
hcdenseSnrm2(hcsparseScalar* s,
             const hcdenseVector* x,
             const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return Norm2<float>(s, x, control);
}

hcsparseStatus
hcdenseDnrm2(hcsparseScalar* s,
             const hcdenseVector* x,
             const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return Norm2<double>(s, x, control);
}

hcsparseStatus
hcdenseSdot (hcsparseScalar* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    //check opencl elements
    if (x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return dot<float>(r, x, y, control);
}

hcsparseStatus
hcdenseDdot (hcsparseScalar* r,
             const hcdenseVector* x,
             const hcdenseVector* y,
             const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    //check opencl elements
    if (x->values == nullptr || y->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return dot<double>(r, x, y, control);
}

// This function reads the file header at the given filepath, and returns the size
// of the sparse matrix in the hcsparseCooMatrix parameter.
// Post-condition: clears hcsparseCooMatrix, then sets pCooMatx->m, pCooMatx->n
// pCooMatx->nnz
hcsparseStatus
hcsparseHeaderfromFile( int* nnz, int* row, int* col, const char* filePath )
{

    // Check that the file format is matrix market; the only format we can read right now
    // This is not a complete solution, and fails for directories with file names etc...
    // TODO: Should we use boost filesystem?
    std::string strPath( filePath );
    if( strPath.find_last_of( '.' ) != std::string::npos )
    {
        std::string ext = strPath.substr( strPath.find_last_of( '.' ) + 1 );
        if( ext != "mtx" )
            return hcsparseInvalid;
    }
    else
        return hcsparseInvalid;

   MatrixMarketReader< float > mm_reader;

    if( mm_reader.MMReadHeader( filePath ) )
        return hcsparseInvalid;

    *row = mm_reader.GetNumRows( );
    *col = mm_reader.GetNumCols( );
    *nnz = mm_reader.GetNumNonZeroes( );

    return hcsparseSuccess;
}

template<typename T>
bool CoordinateCompare( const Coordinate<T> &c1, const Coordinate<T> &c2 )
{
    if( c1.x != c2.x )
        return ( c1.x < c2.x );
    else
        return ( c1.y < c2.y );
}

// This function reads the file at the given filepath, and returns the sparse
// matrix in the COO struct.  All matrix data is written to device memory
// Pre-condition: This function assumes that the device memory buffers have been
// pre-allocated by the caller
hcsparseStatus
hcsparseSCooMatrixfromFile( hcsparseCooMatrix* cooMatx, const char* filePath, hcsparseControl *control, bool read_explicit_zeroes )
{
    // Check that the file format is matrix market; the only format we can read right now
    // This is not a complete solution, and fails for directories with file names etc...
    // TODO: Should we use boost filesystem?
    std::string strPath( filePath );
    if( strPath.find_last_of( '.' ) != std::string::npos )
    {
        std::string ext = strPath.substr( strPath.find_last_of( '.' ) + 1 );
        if( ext != "mtx" )
            return hcsparseInvalid;
    }
    else
        return hcsparseInvalid;

    MatrixMarketReader< float > mm_reader;
    if( mm_reader.MMReadFormat( filePath, read_explicit_zeroes ) )
        return hcsparseInvalid;

    cooMatx->num_rows = mm_reader.GetNumRows( );
    cooMatx->num_cols = mm_reader.GetNumCols( );
    cooMatx->num_nonzeros = mm_reader.GetNumNonZeroes( );

    Coordinate<float>* coords = mm_reader.GetUnsymCoordinates( );

    std::sort( coords, coords + cooMatx->num_nonzeros, CoordinateCompare< float > );

    hc::array_view<float> *values = static_cast<hc::array_view<float> *>(cooMatx->values);
    hc::array_view<int> *rowIndices = static_cast<hc::array_view<int> *>(cooMatx->rowIndices);
    hc::array_view<int> *colIndices = static_cast<hc::array_view<int> *>(cooMatx->colIndices);

    for( int c = 0; c < cooMatx->num_nonzeros; ++c )
    {
        (*(rowIndices))[ c ] = coords[c].x;
        (*(colIndices))[ c ] = coords[c].y;
        (*(values))[ c ] = coords[c].val;
    }

    return hcsparseSuccess;
}

hcsparseStatus
hcsparseDCooMatrixfromFile( hcsparseCooMatrix* cooMatx, const char* filePath, hcsparseControl *control, bool read_explicit_zeroes )
{
    // Check that the file format is matrix market; the only format we can read right now
    // This is not a complete solution, and fails for directories with file names etc...
    // TODO: Should we use boost filesystem?
    std::string strPath( filePath );
    if( strPath.find_last_of( '.' ) != std::string::npos )
    {
        std::string ext = strPath.substr( strPath.find_last_of( '.' ) + 1 );
        if( ext != "mtx" )
            return hcsparseInvalid;
    }
    else
        return hcsparseInvalid;

    MatrixMarketReader< double > mm_reader;
    if( mm_reader.MMReadFormat( filePath, read_explicit_zeroes ) )
        return hcsparseInvalid;

    cooMatx->num_rows = mm_reader.GetNumRows( );
    cooMatx->num_cols = mm_reader.GetNumCols( );
    cooMatx->num_nonzeros = mm_reader.GetNumNonZeroes( );

    Coordinate<double>* coords = mm_reader.GetUnsymCoordinates( );

    std::sort( coords, coords + cooMatx->num_nonzeros, CoordinateCompare<double> );

    hc::array_view<double> *values = static_cast<hc::array_view<double> *>(cooMatx->values);
    hc::array_view<int> *rowIndices = static_cast<hc::array_view<int> *>(cooMatx->rowIndices);
    hc::array_view<int> *colIndices = static_cast<hc::array_view<int> *>(cooMatx->colIndices);

    for( int c = 0; c < cooMatx->num_nonzeros; ++c )
    {
        (*(rowIndices))[ c ] = coords[c].x;
        (*(colIndices))[ c ] = coords[c].y;
        (*(values))[ c ] = coords[c].val;
    }

    return hcsparseSuccess;
}

hcsparseStatus
hcsparseSCsrMatrixfromFile(hcsparseCsrMatrix* csrMatx, const char* filePath, hcsparseControl *control, bool read_explicit_zeroes )
{

    // Check that the file format is matrix market; the only format we can read right now
    // This is not a complete solution, and fails for directories with file names etc...
    // TODO: Should we use boost filesystem?
    std::string strPath( filePath );
    if( strPath.find_last_of( '.' ) != std::string::npos )
    {
        std::string ext = strPath.substr( strPath.find_last_of( '.' ) + 1 );
        if( ext != "mtx" )
            return hcsparseInvalid;
    }
    else
        return hcsparseInvalid;

    // Read data from a file on disk into CPU buffers
    // Data is read natively as COO format with the reader
    MatrixMarketReader< float > mm_reader;
    if( mm_reader.MMReadFormat( filePath, read_explicit_zeroes ) )
        return hcsparseInvalid;

    // JPA: Shouldn't that just be an assertion check? It seems to me that
    // the user have to call hcsparseHeaderfromFile before calling this function,
    // otherwise the whole pCsrMatrix will be broken;

    csrMatx->num_rows = mm_reader.GetNumRows( );
    csrMatx->num_cols = mm_reader.GetNumCols( );
    csrMatx->num_nonzeros = mm_reader.GetNumNonZeroes( );

    Coordinate<float>* coords = mm_reader.GetUnsymCoordinates( );

    std::sort( coords, coords + csrMatx->num_nonzeros, CoordinateCompare< float > );

    hc::array_view<float> *values = static_cast<hc::array_view<float> *>(csrMatx->values);
    hc::array_view<int> *rowOffsets = static_cast<hc::array_view<int> *>(csrMatx->rowOffsets);
    hc::array_view<int> *colIndices = static_cast<hc::array_view<int> *>(csrMatx->colIndices);

    int current_row = 0;
    (*(rowOffsets))[ 0 ] = 0;
    for( int i = 0; i < csrMatx->num_nonzeros; i++ )
    {
        (*(colIndices))[ i ] = coords[i].y;
        (*(values))[ i ] = coords[i].val;

        while( coords[i].x >= current_row )
            (*(rowOffsets))[ current_row++ ] = i;
    }
    (*(rowOffsets))[ current_row ] = csrMatx->num_nonzeros;
    while( current_row <= csrMatx->num_rows )
        (*(rowOffsets))[ current_row++ ] = csrMatx->num_nonzeros;

    return hcsparseSuccess;
}

hcsparseStatus
hcsparseDCsrMatrixfromFile( hcsparseCsrMatrix* csrMatx, const char* filePath, hcsparseControl *control, bool read_explicit_zeroes )
{
    // Check that the file format is matrix market; the only format we can read right now
    // This is not a complete solution, and fails for directories with file names etc...
    // TODO: Should we use boost filesystem?
    std::string strPath( filePath );
    if( strPath.find_last_of( '.' ) != std::string::npos )
    {
        std::string ext = strPath.substr( strPath.find_last_of( '.' ) + 1 );
        if( ext != "mtx" )
            return hcsparseInvalid;
    }
    else
        return hcsparseInvalid;

    // Read data from a file on disk into CPU buffers
    // Data is read natively as COO format with the reader
    MatrixMarketReader< double > mm_reader;
    if( mm_reader.MMReadFormat( filePath, read_explicit_zeroes ) )
        return hcsparseInvalid;

    csrMatx->num_rows = mm_reader.GetNumRows( );
    csrMatx->num_cols = mm_reader.GetNumCols( );
    csrMatx->num_nonzeros = mm_reader.GetNumNonZeroes( );

    Coordinate<double>* coords = mm_reader.GetUnsymCoordinates( );

    std::sort( coords, coords + csrMatx->num_nonzeros, CoordinateCompare<double> );

    hc::array_view<double> *values = static_cast<hc::array_view<double> *>(csrMatx->values);
    hc::array_view<int> *rowOffsets = static_cast<hc::array_view<int> *>(csrMatx->rowOffsets);
    hc::array_view<int> *colIndices = static_cast<hc::array_view<int> *>(csrMatx->colIndices);

    int current_row = 1;
    (*(rowOffsets))[ 0 ] = 0;
    for( int i = 0; i < csrMatx->num_nonzeros; i++ )
    {
        (*(colIndices))[ i ] = coords[i].y;
        (*(values))[ i ] = coords[i].val;

        while( coords[i].x >= current_row )
            (*(rowOffsets))[ current_row++ ] = i;
    }
    (*(rowOffsets))[ current_row ] = csrMatx->num_nonzeros;
    while( current_row <= csrMatx->num_rows )
        (*(rowOffsets))[ current_row++ ] = csrMatx->num_nonzeros;

    return hcsparseSuccess;
}

hcsparseStatus
hcsparseCsrMetaSize( hcsparseCsrMatrix* csrMatx, hcsparseControl *control )
{
    hc::array_view<int> *rCsrRowOffsets = static_cast<hc::array_view<int> *>(csrMatx->rowOffsets);
    int * dataRO = rCsrRowOffsets->data();

    csrMatx->rowBlockSize = ComputeRowBlocksSize( dataRO, csrMatx->num_rows, BLOCKSIZE, BLOCK_MULTIPLIER, ROWS_FOR_VECTOR );

    return hcsparseSuccess;
}

hcsparseStatus
hcsparseCsrMetaCompute( hcsparseCsrMatrix* csrMatx, hcsparseControl *control )
{
    // Check to ensure nRows can fit in 32 bits
    if( static_cast<ulong>( csrMatx->num_rows ) > static_cast<ulong>( std::pow( 2, ( 64 - ROWBITS ) ) ) )
    {
        printf( "Number of Rows in the Sparse Matrix is greater than what is supported at present ((64-WG_BITS) bits) !" );
        return hcsparseInvalid;
    }

    hc::array_view<int> *rCsrRowOffsets = static_cast<hc::array_view<int> *>(csrMatx->rowOffsets);
    int *dataRO = rCsrRowOffsets->data();
    hc::array_view<ulong> *rRowBlocks = static_cast<hc::array_view<ulong> *>(csrMatx->rowBlocks);
    ulong *dataRB = rRowBlocks->data();

    ComputeRowBlocks(dataRB, csrMatx->rowBlockSize, dataRO, csrMatx->num_rows, BLOCKSIZE, BLOCK_MULTIPLIER, ROWS_FOR_VECTOR, true );

    return hcsparseSuccess;
}

hcsparseStatus
hcsparseScsrbicgStab(hcdenseVector* x, const hcsparseCsrMatrix *A, const hcdenseVector *b,
               hcsparseSolverControl *solverControl, hcsparseControl *control)
{
    using T = float;

    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr || b->values == nullptr)
    {
        return hcsparseInvalid;
    }

    if (solverControl == nullptr)
    {
        return hcsparseInvalid;
    }

    std::shared_ptr<PreconditionerHandler<T>> preconditioner;

    if (solverControl->preconditioner == DIAGONAL)
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new DiagonalHandler<T>());
        // call constructor of preconditioner class
        preconditioner->notify(A, control);
    }
    else
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new VoidHandler<T>());
        preconditioner->notify(A, control);
    }

    hcsparseStatus status = bicgStab<T>(x, A, b, *preconditioner, solverControl, control);

    solverControl->printSummary(status);

    return status;
}

hcsparseStatus
hcsparseDcsrbicgStab(hcdenseVector* x, const hcsparseCsrMatrix *A, const hcdenseVector *b,
               hcsparseSolverControl *solverControl, hcsparseControl *control)
{
    using T = double;

    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr || b->values == nullptr)
    {
        return hcsparseInvalid;
    }

    if (solverControl == nullptr)
    {
        return hcsparseInvalid;
    }

    std::shared_ptr<PreconditionerHandler<T>> preconditioner;

    if (solverControl->preconditioner == DIAGONAL)
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new DiagonalHandler<T>());
        // call constructor of preconditioner class
        preconditioner->notify(A, control);
    }
    else
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new VoidHandler<T>());
        preconditioner->notify(A, control);
    }

    hcsparseStatus status = bicgStab<T>(x, A, b, *preconditioner, solverControl, control);

    solverControl->printSummary(status);

    return status;
}

hcsparseStatus
hcsparseScsrcg(hcdenseVector *x,
               const hcsparseCsrMatrix *A,
               const hcdenseVector *b,
               hcsparseSolverControl *solverControl,
               hcsparseControl *control)
{
    using T = float;

    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr || b->values == nullptr)
    {
        return hcsparseInvalid;
    }

    if (solverControl == nullptr)
    {
        return hcsparseInvalid;
    }

    std::shared_ptr<PreconditionerHandler<T>> preconditioner;

    if (solverControl->preconditioner == DIAGONAL)
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new DiagonalHandler<T>());
        // call constructor of preconditioner class
        preconditioner->notify(A, control);
    }
    else
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new VoidHandler<T>());
        preconditioner->notify(A, control);
    }

    hcsparseStatus status = cg<T>(x, A, b, *preconditioner, solverControl, control);

    solverControl->printSummary(status);

    return status;
}

hcsparseStatus
hcsparseDcsrcg(hcdenseVector *x,
               const hcsparseCsrMatrix *A,
               const hcdenseVector *b,
               hcsparseSolverControl *solverControl,
               hcsparseControl *control)
{
    using T = double;

    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (x->values == nullptr || b->values == nullptr)
    {
        return hcsparseInvalid;
    }

    if (solverControl == nullptr)
    {
        return hcsparseInvalid;
    }

    std::shared_ptr<PreconditionerHandler<T>> preconditioner;

    if (solverControl->preconditioner == DIAGONAL)
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new DiagonalHandler<T>());
        // call constructor of preconditioner class
        preconditioner->notify(A, control);
    }
    else
    {
        preconditioner = std::shared_ptr<PreconditionerHandler<T>>(new VoidHandler<T>());
        preconditioner->notify(A, control);
    }

    hcsparseStatus status = cg<T>(x, A, b, *preconditioner, solverControl, control);

    solverControl->printSummary(status);

    return status;
}

hcsparseStatus
hcsparseScoo2csr (const hcsparseCooMatrix* coo,
                  hcsparseCsrMatrix* csr,
                  const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (coo->values == nullptr || csr->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return coo2csr<float> (coo, csr, control);
}

hcsparseStatus
hcsparseDcoo2csr (const hcsparseCooMatrix* coo,
                  hcsparseCsrMatrix* csr,
                  const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (coo->values == nullptr || csr->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return coo2csr<double> (coo, csr, control);
}

hcsparseStatus
hcsparseScsr2coo(const hcsparseCsrMatrix* csr,
                 hcsparseCooMatrix* coo,
                 const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (csr->values == nullptr || coo->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csr2coo<float> (csr, coo, control);
}

hcsparseStatus
hcsparseDcsr2coo(const hcsparseCsrMatrix* csr,
                 hcsparseCooMatrix* coo,
                 const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (csr->values == nullptr || coo->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csr2coo<double> (csr, coo, control);
}

hcsparseStatus
hcsparseScsr2dense(const hcsparseCsrMatrix* csr,
                   hcdenseMatrix* A,
                   const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (csr->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csr2dense<float> (csr, A, control);
}

hcsparseStatus
hcsparseDcsr2dense(const hcsparseCsrMatrix* csr,
                   hcdenseMatrix* A,
                   const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (csr->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return csr2dense<double> (csr, A, control);
}

hcsparseStatus
hcsparseSdense2csr(const hcdenseMatrix* A,
                   hcsparseCsrMatrix* csr,
                   const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (csr->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return dense2csr<float> (A, csr, control);
}

hcsparseStatus
hcsparseDdense2csr(const hcdenseMatrix* A,
                   hcsparseCsrMatrix* csr,
                   const hcsparseControl* control)
{
    if (!hcsparseInitialized)
    {
        return hcsparseInvalid;
    }

    if (csr->values == nullptr)
    {
        return hcsparseInvalid;
    }

    return dense2csr<double> (A, csr, control);
}

hcsparseStatus
hcsparseScsrSpGemm(const hcsparseCsrMatrix* sparseMatA,
                   const hcsparseCsrMatrix* sparseMatB,
                   hcsparseCsrMatrix* sparseMatC,
                   const hcsparseControl* control )
{
    if (!hcsparseInitialized)
    {
       return hcsparseInvalid;
    }

    if (sparseMatA->values == nullptr || sparseMatB->values == nullptr || sparseMatC->values == nullptr)
    {
       return hcsparseInvalid;
    }

    return csrSpGemm<float> (sparseMatA, sparseMatB, sparseMatC, control);
}

