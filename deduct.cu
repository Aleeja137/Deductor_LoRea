#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> 
#include <cuda_runtime.h>

__global__ void unifier_matrices_cuda(int *mat0, int *mat1, int n0, int n1, int *unifiers){

    // Launch n0*n1 threads in 1D, unifiers must have space for all threads to store a unifier
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    int i, j, m, row_size, unifier_size, code, last_unifier;
    int *unifier;

    m = mat0[0];
    last_unifier = 0;
    row_size = 1+m;
    unifier_size = 1+(2*m)+2;

    unifier = (int*) malloc (unifier_size*sizeof(int));

    i = gid/n1;
    j = gid%n1;
    
    memset(unifier,0,unifier_size*sizeof(int));  
    code = unifier_rows(&mat0[row_size * i], &mat1[row_size * j], unifier);
    if (code != 0) continue; // Rows cannot be unified

    code = correct_unifier(&mat0[row_size * i], &mat1[row_size * j], unifier);
    if (code != 0) continue; // Rows cannot be unified
    
    unifier[1+(2*m)]   = i;
    unifier[1+(2*m)+1] = j;

    // Here another kernel or host function will need to compress unifiers list
    memcpy(&unifiers[gid*unifier_size],unifier,unifier_size*sizeof(int));
    last_unifier++;

    // Free the extra space
    // unifiers = realloc(unifiers,last_unifier*unifier_size*sizeof(int));
    // free(unifier);
}


__global__ void unify_matrices_cuda(int *unified){
    // Launch n0 threads in 1D, each thread will handle one unifier
    int gid = blockIdx.x * blockDim.x + threadIdx.x;

    int line_len = 1+m0;
    int *line_A  = (int*) malloc (line_len*sizeof(int));
    int *line_B  = (int*) malloc (line_len*sizeof(int));
    // int *unified = (int*) malloc (unif_count*line_len*sizeof(int));

    int i = gid;
    int ind_A, ind_B;

    if (i < unif_count)
    {
        ind_A = unifiers[i*unifier_size+unifier_size-2];
        ind_B = unifiers[i*unifier_size+unifier_size-1];
        memcpy(line_A,&mat0[ind_A*line_len],line_len*sizeof(int));
        memcpy(line_B,&mat1[ind_B*line_len],line_len*sizeof(int));
        apply_unifier(line_A,line_B,&unifiers[i*unifier_size]);
        memcpy(&unified[i*line_len],line_A,line_len*sizeof(int));
    }

}
int main() {
    
    int n_blk = 1;
    int threads_per_blk = 1;

    apply_unifier_cuda <<<n_blk,threads_per_blk>>>(); 
    unify_matrices_cuda<<<n_blk,threads_per_blk>>>(); 

    return 0;
}