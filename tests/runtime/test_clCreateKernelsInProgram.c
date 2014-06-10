#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <CL/opencl.h>
#include "poclu.h"
#include "config.h"


int main(int argc, char **argv)
{
  cl_int err;
  const char *krn_src;
  cl_program program;
  cl_context ctx;
  cl_device_id did;
  cl_command_queue queue;
  cl_uint num_krn;
  cl_kernel kernels[2];
  size_t src_size;

  poclu_get_any_device(&ctx, &did, &queue);
  assert( ctx );
  assert( did );
  assert( queue );

  krn_src = poclu_read_file(SRCDIR "/tests/runtime/test_clCreateKernelsInProgram.cl", NULL);
  assert(krn_src);

  program = clCreateProgramWithSource(ctx, 1, &krn_src, NULL, NULL);
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  assert(err == CL_SUCCESS);
  
  err = clCreateKernelsInProgram(program, 0, NULL, &num_krn);
  assert(err == CL_SUCCESS);
  // test_clCreateKernelsInProgram.cl has two kernel functions.
  assert(num_krn == 2);

  err = clCreateKernelsInProgram(program, 2, kernels, NULL);
  assert(err == CL_SUCCESS);
  
  // make sure the kernels were actually created 
  // Note: nothing in the specification says which kernel function
  // is kernels[0], which is kernels[1]. For now assume pocl/LLVM
  // orders these deterministacally
  err = clEnqueueTask(queue, kernels[0], 0, NULL, NULL); 
  assert(err == CL_SUCCESS);
  err = clEnqueueTask(queue, kernels[1], 0, NULL, NULL);
  assert(err == CL_SUCCESS);
  clFinish(queue);

  clReleaseProgram(program);
  free((void*)krn_src);

  /* Create same program, but from SPIR */
  krn_src = poclu_read_file(SRCDIR "/tests/runtime/test_clCreateKernelsInProgram.spir",
                            &src_size);
  assert(krn_src);
  program = clCreateProgramWithBinary(ctx, 1, &did, &src_size, 
                                      (const unsigned char**)(&krn_src), NULL, NULL);
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  assert(err == CL_SUCCESS);

  err = clCreateKernelsInProgram(program, 0, NULL, &num_krn);
  assert(err == CL_SUCCESS);
  assert(num_krn == 2);

  err = clCreateKernelsInProgram(program, 2, kernels, NULL);
  assert(err == CL_SUCCESS);

  clReleaseProgram(program);
  free((void*)krn_src);
  clReleaseContext(ctx);
  clReleaseDevice(did);
  clReleaseCommandQueue(queue);

}


