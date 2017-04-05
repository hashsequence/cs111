#!/usr/bin/python

import csv, sys, string, locale

def parse_files():
    output = open('lab3b_check.txt','w+')
    super_fd = open('super.csv', 'r') 
    super_r = csv.reader(super_fd)
    group_fd = open('group.csv', 'r') 
    group_r = csv.reader(group_fd)
    bitmap_fd = open('bitmap.csv', 'r') 
    bitmap_r = csv.reader(bitmap_fd)
    inode_fd = open('inode.csv', 'r') 
    inode_r = csv.reader(inode_fd)
    directory_fd = open('directory.csv', 'r') 
    directory_r = csv.reader(directory_fd)
    indirect_fd = open('indirect.csv', 'r')
    indirect_r = csv.reader(indirect_fd)

    super_arr = list(super_r)
    group_arr = list(group_r)
    inode_arr = list(inode_r)
    bitmap_arr = list(bitmap_r)
    directory_arr = list(directory_r)
    indirect_arr = list(indirect_r)
    
    free_blocks = []
    free_blocks_dict = {}
    duplicate_blocks_inodes = {}
    free_block_bitmap_block = []
    seen_blocks_dict = {}
    for row in group_arr:
       free_block_bitmap_block.append(row[5])
    for row in bitmap_arr:
        if row[0] in free_block_bitmap_block:
            free_blocks.append(row[1])   
    for i in range(0,int(super_arr[0][2])):
        free_blocks_dict[str(i)] = 'F'
        seen_blocks_dict[str(i)] = 0
    for i in free_blocks:
        free_blocks_dict[str(i)] = 'T'
    del free_blocks 
    # checking for unallocated and duplicate blocks
    unallocated_blocks = []
    unallocated_blocks_inodes = []
    global checker1
    for row1 in inode_arr:
        #direct
        for i in range(11, 23):
           block1 = str(int(row1[i],16))
           if block1 != '0':
               if free_blocks_dict[block1] == 'T':
                   unallocated_blocks.append("UNALLOCATED BLOCK < "+block1+ " > REFERENCED BY")
                   unallocated_blocks_inodes.append(" INODE < " + str(row1[0]) + " > ENTRY < " + str(i-11) + " >")
               seen_blocks_dict[block1] += 1
               if (seen_blocks_dict[block1] == 1):
                   duplicate_blocks_inodes[block1] = list()
                   duplicate_blocks_inodes[block1].append(" INODE < " + str(row1[0]) +  " > ENTRY < " + str(i-11) + " >")
               if (seen_blocks_dict[block1] > 1):
                   duplicate_blocks_inodes[block1].append(" INODE < " + str(row1[0]) +  " > ENTRY < " + str(i-11) + " >")
                
               
        #indirect
        for i in range(23,24):
            block1 = str(int(row1[i],16))
            checker1 = block1
            if block1 != '0':
                for row2 in indirect_arr:
                   block2 = str(int(row2[2],16))
                   if free_blocks_dict[block2] == 'T':
                       unallocated_blocks_inodes.append(" INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[23] + " > ENTRY < " + str(row2[1]) + " >")
                   seen_blocks_dict[block2] += 1
                   if (seen_blocks_dict[block2] == 1):
                       duplicate_blocks_inodes[block2] = list()
                   if (seen_blocks_dict[block2] > 1):
                        duplicate_blocks_inodes[block2].append(" INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[23] + " > ENTRY < " + str(row2[1]) + " >")
                    
        #indirect2
        if checker1 != '0':
            for i in range(24,25):
                block1 = str(int(row1[i],16))
                checker1 = block1
                if block1 != '0':
                    for row2 in indirect_arr:
                        block2 = str(int(row2[2],16))
                        if block2 != '0':
                            for row3 in indirect_arr:
                                block3 = str(int(row3[2],16))
                                if free_blocks_dict[block3] == 'T':
                                    unallocated_blocks_inodes.append(" INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[24] + " > ENTRY < " + str(int(row3[1]) + int(row2[1])*int(super_arr[0][3])/4) + " >")
                                    seen_blocks_dict[block3] += 1
                                    if (seen_blocks_dict[block3] == 1):
                                        duplicate_blocks_inodes[block3] = list()
                                        if (seen_blocks_dict[block3] > 1):
                                            duplicate_blocks_inodes[block3].append(" INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[24] + " > ENTRY < " + \
                                                                                   str(int(row3[1]) + int(row2[1])*int(super_arr[0][3])/4) + " >")

        
        #indirect3
        if  checker1 != '0':
            for i in range(25,26):
                block1 = str(int(row1[i],16))
                if block1 != '0':
                    for row2 in indirect_arr:
                        block2 = str(int(row2[2],16))
                        if block2 != '0':
                            for row3 in indirect_arr:
                                block3 = str(int(row3[2],16))
                                if block3 != '0':
                                    for row4 in indirect_arr:
                                        block4 = str(int(row4[2],16))
                                        if free_blocks_dict[block4] == 'T':
                                            unallocated_blocks_inodes.append(" INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[25] + " > ENTRY < " + str(int(row4[1]) + int(row3[1])*int(super_arr[0][3])/4*int(row2[1])*int(super_arr[0][3])/4) + " >")
                                            seen_blocks_dict[block4] += 1
                                            if (seen_blocks_dict[block4] == 1):
                                                duplicate_blocks_inodes[block4] = list()
                                                if (seen_blocks_dict[block4] > 1):
                                                    duplicate_blocks_inodes[block4].append(" INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[25] + " > ENTRY < " + str(int(row4[1]) + int(row3[1])*int(super_arr[0][3])/4*int(row2[1])*int(super_arr[0][3])/4) + " >")
    






    unallocated_blocks_inodes.sort()
    for row1 in unallocated_blocks:
        output.write(row1)
        buf = row1
        for row2 in unallocated_blocks_inodes:
            output.write(row2)
            buf += row2
        output.write("\n")
        print buf
    
    for key in seen_blocks_dict:
        if seen_blocks_dict[key] >= 2:
            output.write("MULTIPLY REFERENCED BLOCK < " +  key + " > BY ")
            buf = "MULTIPLY REFERENCED BLOCK < " +  key + " > BY "
            duplicate_blocks_inodes[key].sort()
            for i in duplicate_blocks_inodes[key]:
                output.write(i)
                buf+=i
            print buf
            output.write("\n")
 #   duplicate_blocks = set(duplicate_blocks)
 #   duplicate_blocks = list(duplicate_blocks)
 #   duplicate_blocks.sort()
    
    
    
    super_fd.close()
    group_fd.close()
    bitmap_fd.close()
    inode_fd.close()
    directory_fd.close()
    indirect_fd.close()
    output.close()
def main():
    parse_files()

main()

        
