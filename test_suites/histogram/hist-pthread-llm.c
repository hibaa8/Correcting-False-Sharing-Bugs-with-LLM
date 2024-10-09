/* Copyright (c) 2007, Stanford University
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materialas provided with the distribution.
*     * Neither the name of Stanford University nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/ 

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>
#include <stdalign.h>

#include "stddefines.h"

#define IMG_DATA_OFFSET_POS 10
#define BITS_PER_PIXEL_POS 28

int swap;      // to indicate if we need to swap byte order of header information

typedef struct {
   unsigned char *data;
   long data_pos;
   long data_len;
   alignas(64) int red[256];   // Aligning to avoid false sharing
   alignas(64) int green[256]; // Aligning to avoid false sharing
   alignas(64) int blue[256];  // Aligning to avoid false sharing
   // int red[256];
   // char padding1[64];  // Padding to avoid false sharing
   // int green[256];
   // char padding2[64];  // Padding to avoid false sharing
   // int blue[256];
   // char padding3[64];  // Padding to avoid false sharing
} thread_arg_t;

/* test_endianess
 *
 */
void test_endianess() {
   unsigned int num = 0x12345678;
   char *low = (char *)(&(num));
   if (*low ==  0x78) {
      dprintf("No need to swap\n");
      swap = 0;
   }
   else if (*low == 0x12) {
      dprintf("Need to swap\n");
      swap = 1;
   }
   else {
      printf("Error: Invalid value found in memory\n");
      exit(1);
   } 
}

/* swap_bytes
 *
 */
void swap_bytes(char *bytes, int num_bytes) {
   int i;
   char tmp;
   
   for (i = 0; i < num_bytes/2; i++) {
      dprintf("Swapping %d and %d\n", bytes[i], bytes[num_bytes - i - 1]);
      tmp = bytes[i];
      bytes[i] = bytes[num_bytes - i - 1];
      bytes[num_bytes - i - 1] = tmp;   
   }
}

/* calc_hist
 * Function that computes the histogram for the region
 * assigned to each thread
 */
void *calc_hist(void *arg) {
   
   int *red;
   int *green;
   int *blue;
   int i,j;
   thread_arg_t *thread_arg = (thread_arg_t *)arg;
   unsigned char *val;
   
   red = thread_arg->red;
   green = thread_arg->green;
   blue = thread_arg->blue;
   
   for(j=0; j<60; j++){
   for (i= thread_arg->data_pos; 
        i < thread_arg->data_pos + thread_arg->data_len; 
        i+=3) {
               
      val = &(thread_arg->data[i]);
      blue[*val]++;
      
      val = &(thread_arg->data[i+1]);
      green[*val]++;
      
      val = &(thread_arg->data[i+2]);
      red[*val]++;   
   }
   }
   
   return (void *)0;
}

int main(int argc, char *argv[]) {
      
   int i, j;
   int fd;
   char *fdata;
   struct stat finfo;
   char * fname;
   pthread_t *pid;
   pthread_attr_t attr;
   thread_arg_t *arg;
   int red[256];
   int green[256];
   int blue[256];
   int num_procs = 4;
   int num_per_thread;
   int excess;

#ifdef THREADS
   num_procs = THREADS;
#endif

   if (argv[1] == NULL) {
      printf("USAGE: %s <bitmap filename>\n", argv[0]);
      exit(1);
   }
   
   fname = argv[1];
   
   CHECK_ERROR((fd = open(fname, O_RDONLY)) < 0);
   CHECK_ERROR(fstat(fd, &finfo) < 0);
   CHECK_ERROR((fdata = mmap(0, finfo.st_size + 1, 
      PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == NULL);
   
   if ((fdata[0] != 'B') || (fdata[1] != 'M')) {
      printf("File is not a valid bitmap file. Exiting\n");
      exit(1);
   }
   
   test_endianess();   
   
   unsigned short *bitsperpixel = (unsigned short *)(&(fdata[BITS_PER_PIXEL_POS]));
   if (swap) {
      swap_bytes((char *)(bitsperpixel), sizeof(*bitsperpixel));
   }
   if (*bitsperpixel != 24) {
      printf("Error: Invalid bitmap format - This application only accepts 24-bit pictures. Exiting\n");
      exit(1);
   }
   
   unsigned short *data_pos = (unsigned short *)(&(fdata[IMG_DATA_OFFSET_POS]));
   if (swap) {
      swap_bytes((char *)(data_pos), sizeof(*data_pos));
   }
   
   int imgdata_bytes = (int)finfo.st_size - (int)(*(data_pos));
   int num_pixels = ((int)finfo.st_size - (int)(*(data_pos))) / 3;
   printf("This file has %d bytes of image data, %d pixels\n", imgdata_bytes,
                                                            num_pixels);

   printf("Starting pthreads histogram\n");

   memset(&(red[0]), 0, sizeof(int) * 256);
   memset(&(green[0]), 0, sizeof(int) * 256);
   memset(&(blue[0]), 0, sizeof(int) * 256);
   
   pthread_attr_init(&attr);
   pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
   
   num_per_thread = num_pixels / num_procs;
   excess = num_pixels % num_procs;
   
   CHECK_ERROR( (pid = (pthread_t *)malloc(sizeof(pthread_t) * num_procs)) == NULL);
   CHECK_ERROR( (arg = (thread_arg_t *)malloc(sizeof(thread_arg_t)* num_procs)) == NULL);
   memset(arg, 0, sizeof(thread_arg_t)*num_procs);
   
   long curr_pos = (long)(*data_pos);
   for (i = 0; i < num_procs; i++) {
      arg[i].data = (unsigned char *)fdata;
      arg[i].data_pos = curr_pos;
      long tmp_data_len = num_per_thread;
      if (excess > 0) {
         tmp_data_len++;
         excess--;
      }
      arg[i].data_len = tmp_data_len;
      
      arg[i].data_len *= 3;
      curr_pos += arg[i].data_len;
      
      pthread_create(&(pid[i]), &attr, calc_hist, (void *)(&(arg[i])));   
   }
   
   for (i = 0; i < num_procs; i++) {
      pthread_join(pid[i] , NULL);   
   }
   
   for (i = 0; i < num_procs; i++) {
      for (j = 0; j < 256; j++) {
         red[j] += arg[i].red[j];
         green[j] += arg[i].green[j];
         blue[j] += arg[i].blue[j];
      }
   }

   dprintf("\n\nBlue\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, blue[i]);        
   }

   dprintf("\n\nGreen\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, green[i]);        
   }
   
   dprintf("\n\nRed\n");
   dprintf("----------\n\n");
   for (i = 0; i < 256; i++) {
      dprintf("%d - %d\n", i, red[i]);        
   }

   CHECK_ERROR(munmap(fdata, finfo.st_size + 1) < 0);
   CHECK_ERROR(close(fd) < 0);
   
   free(pid);
   pthread_attr_destroy(&attr);
   
   return 0;
}
