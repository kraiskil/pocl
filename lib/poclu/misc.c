/* poclu_misc - misc generic OpenCL helper functions

   Copyright (c) 2013 Pekka Jääskeläinen / Tampere University of Technology
   Copyright (c) 2014 Kalle Raiskila
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include "poclu.h"
#include <CL/opencl.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"

cl_context
poclu_create_any_context() 
{
  cl_uint i;
  cl_platform_id* platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id));

  clGetPlatformIDs(1, platforms, &i);
  if (i == 0)
    return (cl_context)0;

  cl_context_properties properties[] = 
    {CL_CONTEXT_PLATFORM, 
     (cl_context_properties)platforms[0], 
     0};

  // create the OpenCL context on any available OCL device 
  cl_context context = clCreateContextFromType(
      properties, 
      CL_DEVICE_TYPE_ALL,
      NULL, NULL, NULL); 

  free (platforms);
  return context;
}

cl_int
poclu_get_any_device( cl_context *context, cl_device_id *device, cl_command_queue *queue)
{
  cl_int err;  
  cl_platform_id platform;

  if (context == NULL ||
      device  == NULL ||
      queue   == NULL)
    return CL_INVALID_VALUE;  

  err = clGetPlatformIDs(1, &platform, NULL);
  if (err != CL_SUCCESS)
    return err;

  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, device, NULL);
  if (err != CL_SUCCESS)
    return err;

  *context = clCreateContext(NULL, 1, device, NULL, NULL, &err);
  if (err != CL_SUCCESS)
    return err;
  
  *queue = clCreateCommandQueue(*context, *device, 0, &err); 
  if (err != CL_SUCCESS)
    return err;

  return CL_SUCCESS;
}

char *
poclu_read_file(char *filename, size_t* bytes_read)
{
  FILE *file;
  long size;
  char* src;
  
  file = fopen(filename, "r");
  if (file == NULL)
    return NULL;
  
  fseek( file, 0, SEEK_END);
  size = ftell(file);
  src = (char*)malloc(size+1);
  if (src == NULL)
    return NULL;

  fseek(file, 0, SEEK_SET);
  fread(src, size, 1, file);
  fclose(file);
  src[size]=0;
  
  if (bytes_read != NULL)
    *bytes_read = (size_t)size;
  return src;
}
