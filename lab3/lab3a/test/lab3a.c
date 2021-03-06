#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>

char* diskimage;
int disk_fd;
FILE* superblock_fd;

struct superblock_t
{
  uint32_t s_inodes_count; //
  uint32_t s_blocks_count; //
  uint32_t s_r_blocks_count; 
  uint32_t s_free_blocks_count;
  uint32_t s_free_inodes_count;
  uint32_t s_first_data_block;  //
  uint32_t s_log_block_size; //
  uint32_t s_log_frag_size; //
  uint32_t s_blocks_per_group; //
  uint32_t s_frags_per_group; //
  uint32_t s_inodes_per_group; //
  uint32_t s_mtime; 
  uint32_t s_wtime; 
  uint16_t s_mnt_count; 
  uint16_t s_max_mnt_count; 
  uint16_t s_magic; // 
};

typedef struct superblock_t superblock_t;

superblock_t* my_superblock;

void superblock_parser();

//int gdes_fd;
FILE* gdes_fd;
struct group_descriptor_t
{
  uint32_t bg_block_bitmap;
  uint32_t bg_inode_bitmap;
  uint32_t bg_inode_table;
  uint16_t bg_free_blocks_count;
  uint16_t bg_free_inodes_count;
  uint16_t bg_used_dirs_count;
  uint16_t bg_pad;
  uint16_t num_contained_blocks;
  char buf[10];
};

typedef struct group_descriptor_t g_descript_t;

g_descript_t** my_gdes;
int group_num;
void group_descriptor_parser();

FILE* bitmap_fd;
void bitmap_parser();

typedef union { // you can use union or struct here
  __uint128_t i : 96;
} __attribute__((packed)) uint96_t;


struct dir_entry_t
{
  uint32_t inode; //inode number
  uint16_t rec_len; //directory entry length
  uint8_t name_len; // name length
  uint8_t file_type; 
  char name[255]; //file name
};

typedef struct dir_entry_t dir_entry_t;

struct inode_t
{
  uint16_t i_mode; // mode and file type
  uint16_t i_uid; // owner
  uint32_t i_size; //file size
  uint32_t i_atime; //add time
  uint32_t i_ctime; //creation time
  uint32_t i_mtime; //modification time
  uint32_t i_dtime;
  uint16_t i_gid; //group
  uint16_t i_links_counts; //link counts
  uint32_t i_blocks; //block number
  uint32_t i_flags;
  uint32_t i_osdl;
  uint32_t i_block[15]; //block pointers
  uint32_t i_generation;
  uint32_t i_file_acl;
  uint32_t i_dir_acl;
  uint32_t i_faddr;
  uint96_t i_osd2;
};

typedef struct inode_t inode_t;

inode_t* my_inode;
FILE* inode_fd;

void inode_parser();

FILE* directory_fd;
//int* directory_offset;
//int* directory_inode_number;
//int directory_offset_counter = 0;



void directory_parser();
void directory_parser2();
//int* indirect_offset;
//int indirect_offset_counter = 0;
FILE* indirect_fd;
void indirect_parser();
void indirect_parser2();
void free_and_close();

int main(int argc, char**argv)
{
  if (argc != 2)
    {
      fprintf(stderr, "ERROR: name of disk image required\n");
      exit(0);
    }
  diskimage = argv[1];
  disk_fd = open(diskimage,O_RDONLY);
  if (disk_fd < 0)
    {
      fprintf(stderr, "Error opening diskimage\n");
    }
  superblock_parser();
  group_descriptor_parser();  
  bitmap_parser();
  inode_parser();
  //  directory_parser();
  //indirect_parser();
  directory_parser2();
  indirect_parser2();
  free_and_close();
  //  printf("done\n");
  exit(0);
}

void superblock_parser()
{
  my_superblock = (superblock_t*)malloc(sizeof(superblock_t)); 
  //superblock_fd = open("super.csv",O_TRUNC | O_CREAT | O_WRONLY, 0666);
  superblock_fd = fopen("super.csv","w");
  if ( superblock_fd == NULL)
    {
       fprintf(stderr, "Superblock: cannot open super.csv \n");
    }
  int check  = pread(disk_fd, my_superblock, sizeof(superblock_t), 1024);
  if ( check < 0)
    {
        fprintf(stderr, "Superblock: cannot pread diskimage \n");
    }
  my_superblock->s_log_block_size = 1024 << my_superblock->s_log_block_size;
  if (my_superblock->s_log_frag_size > 0)
    {
      my_superblock->s_log_frag_size = 1024 << my_superblock->s_log_frag_size;
    }
  else
    {
      my_superblock->s_log_frag_size = 1024 >> -my_superblock->s_log_frag_size;
    }
  //printf("%04X\n",my_superblock->s_magic);

  //sanity checking
  if (my_superblock->s_magic != 0xEF53)
    {
      fprintf(stderr, "Superblock - invalid magic: %04X\n",my_superblock->s_magic);
      exit(1);
    }
  if (my_superblock->s_blocks_count % my_superblock->s_blocks_per_group)
    {
        fprintf(stderr, "Superblock - Blocks per group must evenly divide into total blocks\n");
	exit(1);
    }
  if (my_superblock->s_inodes_count % my_superblock->s_inodes_per_group)
    {
        fprintf(stderr, "Superblock - Inodes per group must evenly divide into total Inodes\n");
	exit(1);
    }
  fprintf(superblock_fd,"%x,%d,%d,%d,%d,%d,%d,%d,%d\n",
	  my_superblock->s_magic,
	  my_superblock->s_inodes_count,
          my_superblock->s_blocks_count, 
          my_superblock->s_log_block_size,
          my_superblock->s_log_frag_size,
          my_superblock->s_blocks_per_group,
	  my_superblock->s_inodes_per_group,
	  my_superblock->s_frags_per_group,
	  my_superblock->s_first_data_block);
  // double group_num1 = my_superblock->s_blocks_count / my_superblock->s_blocks_per_group;
  //printf("%g\n", group_num1);
  fclose(superblock_fd);  
}

void group_descriptor_parser()
{
  group_num = (my_superblock->s_blocks_count / my_superblock->s_blocks_per_group);
  my_gdes = (g_descript_t**)malloc(sizeof(g_descript_t*)*(group_num));
  
  //gdes_fd = open("group.csv",O_TRUNC | O_CREAT | O_WRONLY, 0666);
  gdes_fd = fopen("group.csv","w");   
   if ( gdes_fd == NULL)
       {
         fprintf(stderr, "Group Descriptor: cannot open group.csv \n");
       }

 
  int i = 0;
  for (i = 0; i < group_num; i++)
    {
      my_gdes[i] = (g_descript_t*)malloc(sizeof(g_descript_t));
      int check  = pread(disk_fd, my_gdes[i], sizeof(g_descript_t), my_superblock->s_log_block_size + 1024 + (i*sizeof(g_descript_t)));
      if ( check < 0)
       {
         fprintf(stderr, "Group Descriptor: cannot pread diskimage \n");
       }
    }
  for (i = 0; i < group_num; i++)
    {
      fprintf(gdes_fd,"%d,%d,%d,%d,%x,%x,%x\n",
              my_superblock->s_blocks_per_group,
	      my_gdes[i]->bg_free_blocks_count,
	      my_gdes[i]->bg_free_inodes_count,
	      my_gdes[i]->bg_used_dirs_count,
	      my_gdes[i]->bg_inode_bitmap,
	      my_gdes[i]->bg_block_bitmap,
	      my_gdes[i]->bg_inode_table);
    }
  fclose(gdes_fd);
}

void bitmap_parser()
{
  uint8_t buf;
  uint8_t bitmask = 1;
  bitmap_fd = fopen("bitmap.csv","w");
  if (bitmap_fd == NULL)
    {
       fprintf(stderr, "bitmap: cannot open group.csv \n");
    }

  int i = 0;
  int j = 0;
  int k = 0;

  for (i = 0; i < group_num; i++)
    {
      for (j = 0; j < my_superblock->s_log_block_size; j++)
	{
	  //A bit value of 0 indicates that the block/inode is free, while a value of 1 indicates that the block/inode is being used
	  //block size is 1024 and each byte is 8 bits so we need to look at 1024 * 8 = 8192 blocks
	  //since pread can only read at least 1 byte we make a third for loop to read each bit
	  //all values are stored in little endian order, so use a bit mask and go from left to right bitmask << i
	  //bitmap offset can be calulated using this: BASE_OFFSET + (bg_block_bitmap-1)*block_size)
	  pread(disk_fd,&buf,1,(1024 + (my_gdes[i]->bg_block_bitmap-1)*my_superblock->s_log_block_size)+j);
	  for (k = 0; k < 8; k++)
	    {
	      if ((buf & (bitmask << (k))) == 0)
		{
		  fprintf(bitmap_fd, "%x,",my_gdes[i]->bg_block_bitmap);
		  fprintf(bitmap_fd, "%d\n", j * 8 + k + 1 + (i*my_superblock->s_blocks_per_group));
		}
	    }
	}

      for (j = 0; j <my_superblock->s_inodes_per_group/8; j++)
	{
	  pread(disk_fd,&buf,1,(1024 + (my_gdes[i]->bg_inode_bitmap-1)*my_superblock->s_log_block_size)+j);
	  for (k = 0; k < 8; k++)
	    {
	      if ((buf & (bitmask << (k))) == 0)
		{
		  fprintf(bitmap_fd, "%x,",my_gdes[i]->bg_inode_bitmap);
		  fprintf(bitmap_fd, "%d\n", j * 8 + k + 1 + (i*my_superblock->s_inodes_per_group));
		}
	    }
	}
    }  
  fclose(bitmap_fd);
}

void inode_parser()
{
  my_inode = (inode_t*)malloc(sizeof(inode_t));
  inode_fd = fopen("inode.csv","w");
  if (inode_fd == NULL)
    {
       fprintf(stderr, "inode: cannot open inode.csv \n");
    }

  int i = 0;
  int j = 0;
  int k = 0;
  int m = 0;  
  //indirect_offset = (int*)malloc(sizeof(int)*my_superblock->s_inodes_count);
  //directory_offset = (int*)malloc(sizeof(int)*my_superblock->s_inodes_count);
  //directory_inode_number = (int*)malloc(sizeof(int)*my_superblock->s_inodes_count);
  
  uint8_t buf;
  uint8_t bitmask = 1;
  for (i = 0; i < group_num; i++)
    {
      for (j = 0; j < my_superblock->s_inodes_per_group/8; j++) 
	{
	  pread(disk_fd,&buf,1,((my_gdes[i]->bg_inode_bitmap)*my_superblock->s_log_block_size)+j);
	  for (k = 0; k < 8; k++)
	    {
	      if ((buf & (bitmask << (k))) != 0)
		{
		  //if (/*(j*8+k+1) <= (my_superblock->s_inodes_per_group)*/1) //the inode number cannot exceed the inodes per group
		    // {
		      //inode number since each bit refers to inode number and the first inode is 1 I can calculate it with the following
		      //since I wanted to read one inode struct at a time, I has to load 1 byte at a time into a buffer which
		  //contains 8 inodes so I iterated j from 0 to 1024/8 times and for each bit in buffer, I  check if it is valid
		  //and if it is I read the data into a inode struct and print its arguments
		      pread(disk_fd, my_inode,sizeof(inode_t), my_gdes[i]->bg_inode_table * my_superblock->s_log_block_size + (j * 8 + k) * sizeof(inode_t));
		      fprintf(inode_fd, "%d,",j*8+k+1+(i*my_superblock->s_inodes_per_group));
		      //		      indirect_offset[indirect_offset_counter++] = my_gdes[i]->bg_inode_table * my_superblock->s_log_block_size + (j * 8 + k) * sizeof(inode_t);
		      if (my_inode->i_mode & 0x4000)
			{
			  fprintf(inode_fd, "d,");
			  //  directory_inode_number[directory_offset_counter] = j*8+k+1+(i*my_superblock->s_inodes_per_group);
			  //directory_offset[directory_offset_counter++] = my_gdes[i]->bg_inode_table * my_superblock->s_log_block_size + (j * 8 + k) * sizeof(inode_t);
			}
		      else if (my_inode->i_mode & 0x8000)
			{
			  fprintf(inode_fd, "f,");
			}
		      else if (my_inode->i_mode & 0xA000)
			{
			  fprintf(inode_fd,"s,");
			}
		      else
			{
                          fprintf(inode_fd,"?,");
			}
		      fprintf(inode_fd,"%o,%d,%d,%d,%x,%x,%x,%d,%d,",my_inode->i_mode,
			      my_inode->i_uid,
			      my_inode->i_gid,
			      my_inode->i_links_counts,
			      my_inode->i_ctime,
			      my_inode->i_mtime,
			      my_inode->i_atime,
 			      my_inode->i_size,
			      my_inode->i_blocks /(( my_superblock->s_log_block_size)/512)); //calculation is from nongnu in i_blocks section
		      for (m = 0; m < 14; m++)
			{
			  fprintf(inode_fd,"%x,",my_inode->i_block[m]);
			}
		      fprintf(inode_fd,"%x\n",my_inode->i_block[14]);
		      // }
		 
		      // else
		      // {
		      // fprintf(stderr,"Inode %d: out of range\n",j*8+k+1+(i*my_superblock->s_inodes_per_group));
		      //}
		}
	    }
	}
    }

  //  printf("director_offset_counter: %d\n",directory_offset_counter);
  fclose(inode_fd);
}
/*
void directory_parser()
{
  int i = 0;
  int j = 0;
  unsigned int s = 0;
  inode_t temp_inode;
  int entry_num;
  directory_fd = fopen("directory.csv","w");
  if (directory_fd == NULL)
    {
       fprintf(stderr, "directory: cannot open inode.csv \n");
    }
  //fprintf(directory_fd,"Avery Wong\n");

  for (i = 0; i < directory_offset_counter; i++)
    {
      entry_num = 0;
      pread(disk_fd, &temp_inode, sizeof(inode_t), directory_offset[i]);
		      //printing the direct block pointers
		      for (j = 0; j < 12; j++)
			{
			  if ( temp_inode.i_block[j] != 0)
			    {
			      unsigned char block[my_superblock->s_log_block_size];
			      //dir_entry_t temp_dir_entry;
			      pread(disk_fd, block, my_superblock->s_log_block_size ,temp_inode.i_block[j]*my_superblock->s_log_block_size);
			      dir_entry_t* entry = (dir_entry_t*)block;
			      for(s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
				{
				  char buf[256];
				  memcpy(buf,entry->name,entry->name_len);
				  buf[entry->name_len] = '\0';
				  if (entry->inode > 0)
				    {
				      fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
					      directory_inode_number[i], 
					      entry_num, entry->rec_len,
					      entry->name_len, entry->inode,
					      buf);
				    }
				  entry_num++;
				  entry = (void*) entry + entry->rec_len;      
				  if (entry->rec_len == 0)
				    break;
				}
			    }
			}
		      //printing indirect block pointers
		      //if the block size is 1024 bytes then there are 1024/4 block pointers because 4 bytes per pointer
		      
		      if (temp_inode.i_block[12] != 0)
			{
			  uint32_t indirect1;
			  
			  for (j = 0; j < my_superblock->s_log_block_size/4; j++)
			    {
			       pread(disk_fd, &indirect1, 4 ,temp_inode.i_block[12]*my_superblock->s_log_block_size + (j * 4) );
			       if (indirect1 != 0)
				 {
			           unsigned char block[1024];
				   pread(disk_fd, block, 1024, indirect1*my_superblock->s_log_block_size); 
				   dir_entry_t* entry = (dir_entry_t*)block;
				   for (s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
				     {
				       char buf[256];
				       memcpy(buf,entry->name,entry->name_len);
				       buf[entry->name_len] = '\0';
				       if (entry->inode > 0)
					 {
					   fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
						   directory_inode_number[i],
						   entry_num, 
						   entry->rec_len,
						   entry->name_len,
						   entry->inode,
						   buf);
					 }
				       entry_num++;
				       entry = (void*) entry + entry->rec_len;  
				       if (entry->rec_len == 0)
					 break;
				     }
				 }
			    }
			}

		      		      //printing double t block pointers
		      //if the block size is 1024 bytes then there are 1024/4 block pointers because there are 4 bytes per pointer
		      int k = 0;
		      if (temp_inode.i_block[13] != 0)
			{
			  uint32_t indirect1;
			  
			  for (j = 0; j < my_superblock->s_log_block_size/4; j++)
			    {
			       pread(disk_fd, &indirect1, 4 ,temp_inode.i_block[13]*my_superblock->s_log_block_size + (j * 4) );
			       if (indirect1 != 0)
				 {
				   uint32_t indirect2;
				   for (k = 0; k < my_superblock->s_log_block_size/4; k++)
				     {
				       pread(disk_fd, &indirect2, 4 ,indirect1*my_superblock->s_log_block_size + (k * 4) );
				       if (indirect2 != 0)
					 {
			                    unsigned char block[1024];
					    pread(disk_fd, block, 1024, indirect2*my_superblock->s_log_block_size); 
			                    dir_entry_t* entry = (dir_entry_t*)block;
			                    for (s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
			                    {
			                       char buf[256];
			                       memcpy(buf,entry->name,entry->name_len);
			                       buf[entry->name_len] = '\0';
			                       if (entry->inode > 0)
				                 {
				                   fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
							   directory_inode_number[i],
							   entry_num,
							   entry->rec_len,
							   entry->name_len,
							   entry->inode,
							   buf);
			      	                 }
			                         entry_num++;
			                         entry = (void*) entry + entry->rec_len;  
			                        if (entry->rec_len == 0)
			                        break;
				              }
					 }
				     }
				 }
			    }
			}



		      		      //printing triple t block pointers
		      //if the block size is 1024 bytes then there are 1024/4 block pointers since each ptr is of 4 bytes

		      int m = 0;
		      if (temp_inode.i_block[14] != 0)
			{
			  uint32_t indirect1;
			  
			  for (j = 0; j < my_superblock->s_log_block_size/4; j++)
			    {
			       pread(disk_fd, &indirect1, 4 ,temp_inode.i_block[14]*my_superblock->s_log_block_size + (j * 4) );
			       if (indirect1 != 0)
				 {
				   uint32_t indirect2;
				   for (k = 0; k < my_superblock->s_log_block_size/4; k++)
				     {
				       pread(disk_fd, &indirect2, 4 ,indirect1*my_superblock->s_log_block_size + (k * 4) );
				       if (indirect2 != 0)
					 {
					   uint32_t indirect3;
					   for (m = 0; m < my_superblock->s_log_block_size/4; m++)
					     {
					       pread(disk_fd, &indirect3, 4 ,indirect2*my_superblock->s_log_block_size + (m * 4) );
					       if (indirect3 != 0)
						 {
			                           unsigned char block[1024];
						   pread(disk_fd, block, 1024, indirect3*my_superblock->s_log_block_size);  
						   dir_entry_t* entry = (dir_entry_t*)block;
						   for (s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
						     {
						       char buf[256];
						       memcpy(buf,entry->name,entry->name_len);
						       buf[entry->name_len] = '\0';
						       if (entry->inode > 0)
							 {
							   fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
								   directory_inode_number[i], 
								   entry_num, entry->rec_len, 
								   entry->name_len, entry->inode,
								   buf);
							 }
						       entry_num++;
						       entry = (void*) entry + entry->rec_len;  
						       if (entry->rec_len == 0)
							 break;
						     }
						 }
					     }
					 }
				     }
				 }
			    }
			}


		      		      
    }
                        

  fclose(directory_fd);
}

void indirect_parser()
{
  
  indirect_fd = fopen("indirect.csv","w");
  if (indirect_fd == NULL)
    {
       fprintf(stderr, "indirect: cannot open inode.csv \n");
    }
  inode_t temp_inode;
  int entry_num = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int m = 0;
  
  for (i = 0; i < directory_offset_counter; i++)
    {
      uint32_t indirect1;
      uint32_t indirect2;
      uint32_t indirect3;
      
      pread(disk_fd, &temp_inode, sizeof(inode_t), directory_offset[i]); 
      //indirect
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[12]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      fprintf(indirect_fd,"%x,%d,%x\n",temp_inode.i_block[12], entry_num++ ,indirect1);
	    }
	}
      //double indirect
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[13]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      fprintf(indirect_fd,"%x,%d,%x\n",temp_inode.i_block[13], entry_num++ ,indirect1);
	    }
	}

      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[13]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      entry_num = 0;
	      for (k = 0; k < my_superblock->s_log_block_size/4; k++)
		{
		  pread(disk_fd, &indirect2, 4 , indirect1*my_superblock->s_log_block_size+(k*4));
		  if (indirect2 != 0)
		    {
		      fprintf(indirect_fd,"%x,%d,%x\n",indirect1, entry_num++ ,indirect2);
		    }
		}
	    }
	}
      


      //triple indirect
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[14]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      fprintf(indirect_fd,"%x,%d,%x\n",temp_inode.i_block[14], entry_num++ ,indirect1);
	    }
	}

      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[14]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      entry_num = 0;
	      for (k = 0; k < my_superblock->s_log_block_size/4; k++)
		{
		  pread(disk_fd, &indirect2, 4 , indirect1*my_superblock->s_log_block_size+(k*4));
		  if (indirect2 != 0)
		    {
		      fprintf(indirect_fd,"%x,%d,%x\n",indirect1, entry_num++ ,indirect2);
		    }
		}
	    }
	}
      
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[14]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      entry_num = 0;
	      for (k = 0; k < my_superblock->s_log_block_size/4; k++)
		{
		  pread(disk_fd, &indirect2, 4 , indirect1*my_superblock->s_log_block_size+(k*4));
		  if (indirect2 != 0)
		    {
		      entry_num = 0;
		      for (m = 0; m < my_superblock->s_log_block_size/4; m++)
			{
			 pread(disk_fd, &indirect3, 4 , indirect2*my_superblock->s_log_block_size+(m*4));
			 if (indirect3 != 0)
			   {
			     fprintf(indirect_fd,"%x,%d,%x\n",indirect2, entry_num++ ,indirect3);
			   }
			}
		    }
		}
	    }
	}
      
    }

  

  fclose(indirect_fd);
}
*/
void free_and_close()
{
  
  int i = 0;
  //  free(indirect_offset);
  //free(directory_offset);
  //free(directory_inode_number);
  free(my_inode);
  for (i = 0; i < group_num; i++)
    {
      free(my_gdes[i]);
    } 
  free(my_gdes);
  free(my_superblock);

  close(disk_fd);
}

/*
  int f = 0;
  int g = 0;
  int h = 0;
  uint8_t buf;
  uint8_t bitmask = 1;
  for (f = 0; f < group_num; f++)
    {
      for (g = 0; g < my_superblock->s_inodes_per_group/8; g++) 
	{
	  pread(disk_fd,&buf,1,((my_gdes[f]->bg_inode_bitmap)*my_superblock->s_log_block_size)+g);
	  for (h = 0; h < 8; h++)
	    {
	      if ((buf & (bitmask << (h))) != 0)
		{
		      //inode number since each bit refers to inode number and the first inode is 1 I can calculate it with the following
		   
		      pread(disk_fd, my_inode,sizeof(inode_t), my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t));
		      indirect_off = my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t);
		       if (my_inode->i_mode & 0x4000)
			{
			  directory_off = my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t);
			}
*/
void directory_parser2()
{
  //  int i = 0;
  int j = 0;
  unsigned int s = 0;
  inode_t temp_inode;
  int entry_num;
  directory_fd = fopen("directory.csv","w");
  if (directory_fd == NULL)
    {
       fprintf(stderr, "directory: cannot open inode.csv \n");
    }
  //fprintf(directory_fd,"Avery Wong\n");
  int f = 0;
  int g = 0;
  int h = 0;
  uint8_t buf;
  uint8_t bitmask = 1;
  int directory_off;
  int directory_inode_n;
  for (f = 0; f < group_num; f++)
    {
      for (g = 0; g < my_superblock->s_inodes_per_group/8; g++) 
	{
	  pread(disk_fd,&buf,1,((my_gdes[f]->bg_inode_bitmap)*my_superblock->s_log_block_size)+g);
	  for (h = 0; h < 8; h++)
	    {
	      if ((buf & (bitmask << (h))) != 0)
		{
		      //inode number since each bit refers to inode number and the first inode is 1 I can calculate it with the following
		      //I want to pread each inode data into my inode struct and I can do this by using the group descriptor table to find the
		  //offset for inode table and iterate through it
		      pread(disk_fd, my_inode,sizeof(inode_t), my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t));
		      
		       if (my_inode->i_mode & 0x4000)
			{
			  directory_off = my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t);
			  directory_inode_n = g*8+h+1+(f*my_superblock->s_inodes_per_group);
      entry_num = 0;
      pread(disk_fd, &temp_inode, sizeof(inode_t), directory_off);
		      //printing the direct block pointers
		      for (j = 0; j < 12; j++)
			{
			  if ( temp_inode.i_block[j] != 0)
			    {
			      unsigned char block[my_superblock->s_log_block_size];
			      //dir_entry_t temp_dir_entry;
			      pread(disk_fd, block, my_superblock->s_log_block_size ,temp_inode.i_block[j]*my_superblock->s_log_block_size);
			      dir_entry_t* entry = (dir_entry_t*)block;
			      for(s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
				{
				  char buf[256];
				  memcpy(buf,entry->name,entry->name_len);
				  buf[entry->name_len] = '\0';
				  if (entry->inode > 0)
				    {
				      fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
					      directory_inode_n, 
					      entry_num, entry->rec_len,
					      entry->name_len, entry->inode,
					      buf);
				    }
				  entry_num++;
				  entry = (void*) entry + entry->rec_len;      
				  if (entry->rec_len == 0)
				    break;
				}
			    }
			}
		      //printing indirect block pointers
		      //if the block size is 1024 bytes then there are 1024/4 block pointers because 4 bytes per pointer
		      
		      if (temp_inode.i_block[12] != 0)
			{
			  uint32_t indirect1;
			  
			  for (j = 0; j < my_superblock->s_log_block_size/4; j++)
			    {
			       pread(disk_fd, &indirect1, 4 ,temp_inode.i_block[12]*my_superblock->s_log_block_size + (j * 4) );
			       if (indirect1 != 0)
				 {
			           unsigned char block[1024];
				   pread(disk_fd, block, 1024, indirect1*my_superblock->s_log_block_size); 
				   dir_entry_t* entry = (dir_entry_t*)block;
				   for (s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
				     {
				       char buf[256];
				       memcpy(buf,entry->name,entry->name_len);
				       buf[entry->name_len] = '\0';
				       if (entry->inode > 0)
					 {
					   fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
						   directory_inode_n,
						   entry_num, 
						   entry->rec_len,
						   entry->name_len,
						   entry->inode,
						   buf);
					 }
				       entry_num++;
				       entry = (void*) entry + entry->rec_len;  
				       if (entry->rec_len == 0)
					 break;
				     }
				 }
			    }
			}
		      /**********************************************************************************************************/
		      		      //printing double t block pointers
		      //if the block size is 1024 bytes then there are 1024/4 block pointers because there are 4 bytes per pointer
		      int k = 0;
		      if (temp_inode.i_block[13] != 0)
			{
			  uint32_t indirect1;
			  
			  for (j = 0; j < my_superblock->s_log_block_size/4; j++)
			    {
			       pread(disk_fd, &indirect1, 4 ,temp_inode.i_block[13]*my_superblock->s_log_block_size + (j * 4) );
			       if (indirect1 != 0)
				 {
				   uint32_t indirect2;
				   for (k = 0; k < my_superblock->s_log_block_size/4; k++)
				     {
				       pread(disk_fd, &indirect2, 4 ,indirect1*my_superblock->s_log_block_size + (k * 4) );
				       if (indirect2 != 0)
					 {
			                    unsigned char block[1024];
					    pread(disk_fd, block, 1024, indirect2*my_superblock->s_log_block_size); 
			                    dir_entry_t* entry = (dir_entry_t*)block;
			                    for (s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
			                    {
			                       char buf[256];
			                       memcpy(buf,entry->name,entry->name_len);
			                       buf[entry->name_len] = '\0';
			                       if (entry->inode > 0)
				                 {
				                   fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
							   directory_inode_n,
							   entry_num,
							   entry->rec_len,
							   entry->name_len,
							   entry->inode,
							   buf);
			      	                 }
			                         entry_num++;
			                         entry = (void*) entry + entry->rec_len;  
			                        if (entry->rec_len == 0)
			                        break;
				              }
					 }
				     }
				 }
			    }
			}
		      /****************************************************************************************************/

/**********************************************************************************************************/
		      		      //printing triple t block pointers
		      //if the block size is 1024 bytes then there are 1024/4 block pointers since each ptr is of 4 bytes

		      int m = 0;
		      if (temp_inode.i_block[14] != 0)
			{
			  uint32_t indirect1;
			  
			  for (j = 0; j < my_superblock->s_log_block_size/4; j++)
			    {
			       pread(disk_fd, &indirect1, 4 ,temp_inode.i_block[14]*my_superblock->s_log_block_size + (j * 4) );
			       if (indirect1 != 0)
				 {
				   uint32_t indirect2;
				   for (k = 0; k < my_superblock->s_log_block_size/4; k++)
				     {
				       pread(disk_fd, &indirect2, 4 ,indirect1*my_superblock->s_log_block_size + (k * 4) );
				       if (indirect2 != 0)
					 {
					   uint32_t indirect3;
					   for (m = 0; m < my_superblock->s_log_block_size/4; m++)
					     {
					       pread(disk_fd, &indirect3, 4 ,indirect2*my_superblock->s_log_block_size + (m * 4) );
					       if (indirect3 != 0)
						 {
			                           unsigned char block[1024];
						   pread(disk_fd, block, 1024, indirect3*my_superblock->s_log_block_size);  
						   dir_entry_t* entry = (dir_entry_t*)block;
						   for (s = 0; s < my_superblock->s_log_block_size; s += entry->rec_len)
						     {
						       char buf[256];
						       memcpy(buf,entry->name,entry->name_len);
						       buf[entry->name_len] = '\0';
						       if (entry->inode > 0)
							 {
							   fprintf(directory_fd,"%d,%d,%d,%d,%d,\"%s\"\n",
								   directory_inode_n, 
								   entry_num, entry->rec_len, 
								   entry->name_len, entry->inode,
								   buf);
							 }
						       entry_num++;
						       entry = (void*) entry + entry->rec_len;  
						       if (entry->rec_len == 0)
							 break;
						     }
						 }
					     }
					 }
				     }
				 }
			    }
			}
		      /****************************************************************************************************/

			}
		}
	    }
	}
    }		      		      

                        

  fclose(directory_fd);
}

void indirect_parser2()
{
  
  indirect_fd = fopen("indirect.csv","w");
  if (indirect_fd == NULL)
    {
       fprintf(stderr, "indirect: cannot open inode.csv \n");
    }
  inode_t temp_inode;
  int entry_num = 0;
  //  int i = 0;
  int j = 0;
  int k = 0;
  int m = 0;
  
  int f = 0;
  int g = 0;
  int h = 0;
  uint8_t buf;
  uint8_t bitmask = 1;
  int indirect_off;
  //int directory_off;
  for (f = 0; f < group_num; f++)
    {
      for (g = 0; g < my_superblock->s_inodes_per_group/8; g++) 
	{
	  pread(disk_fd,&buf,1,((my_gdes[f]->bg_inode_bitmap)*my_superblock->s_log_block_size)+g);
	  for (h = 0; h < 8; h++)
	    {
	      if ((buf & (bitmask << (h))) != 0)
		{
		      //inode number since each bit refers to inode number and the first inode is 1 I can calculate it with the following
		   
		      pread(disk_fd, my_inode,sizeof(inode_t), my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t));
		      indirect_off = my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t);
		      /* 
		      if (my_inode->i_mode & 0x4000)
			{
			  directory_off = my_gdes[f]->bg_inode_table * my_superblock->s_log_block_size + (g * 8 + h) * sizeof(inode_t);
		      */
      uint32_t indirect1;
      uint32_t indirect2;
      uint32_t indirect3;
      
      pread(disk_fd, &temp_inode, sizeof(inode_t), indirect_off); 
      //indirect
      if (temp_inode.i_block[12] != 0)
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[12]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      fprintf(indirect_fd,"%x,%d,%x\n",temp_inode.i_block[12], entry_num++ ,indirect1);
	    }
	}
      
      //double indirect
      if (temp_inode.i_block[12] != 0)
	{ 
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[13]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	        fprintf(indirect_fd,"%x,%d,%x\n",temp_inode.i_block[13], entry_num++ ,indirect1);
	    }
	}

      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[13]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      entry_num = 0;
	      for (k = 0; k < my_superblock->s_log_block_size/4; k++)
		{
		  pread(disk_fd, &indirect2, 4 , indirect1*my_superblock->s_log_block_size+(k*4));
		  if (indirect2 != 0)
		    {
		            fprintf(indirect_fd,"%x,%d,%x\n",indirect1, entry_num++ ,indirect2);
		    }
		}
	    }
	}
      

	}
      //triple indirect
      if (temp_inode.i_block[13] != 0)
	{
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[14]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      	      fprintf(indirect_fd,"%x,%d,%x\n",temp_inode.i_block[14], entry_num++ ,indirect1);
	    }
	}

      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[14]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      entry_num = 0;
	      for (k = 0; k < my_superblock->s_log_block_size/4; k++)
		{
		  pread(disk_fd, &indirect2, 4 , indirect1*my_superblock->s_log_block_size+(k*4));
		  if (indirect2 != 0)
		    {
		            fprintf(indirect_fd,"%x,%d,%x\n",indirect1, entry_num++ ,indirect2);
		    }
		}
	    }
	}
      
      for (j = 0; j < my_superblock->s_log_block_size/4; j++)
	{
	  pread(disk_fd, &indirect1, 4 , temp_inode.i_block[14]*my_superblock->s_log_block_size+(j*4));
	  if (indirect1 != 0)
	    {
	      entry_num = 0;
	      for (k = 0; k < my_superblock->s_log_block_size/4; k++)
		{
		  pread(disk_fd, &indirect2, 4 , indirect1*my_superblock->s_log_block_size+(k*4));
		  if (indirect2 != 0)
		    {
		      entry_num = 0;
		      for (m = 0; m < my_superblock->s_log_block_size/4; m++)
			{
			 pread(disk_fd, &indirect3, 4 , indirect2*my_superblock->s_log_block_size+(m*4));
			 if (indirect3 != 0)
			   {
			          fprintf(indirect_fd,"%x,%d,%x\n",indirect2, entry_num++ ,indirect3);
			   }
			}
		    }
		}
	    }
	}
	}
      //			}		
		}
	    }
	}
    }


  fclose(indirect_fd);
}
