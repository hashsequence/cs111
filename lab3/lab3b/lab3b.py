#!/usr/bin/python

import csv, sys, string, locale, math

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
    # checking for (1)unallocated and (2)duplicate blocks and (7)invalid blocks
    unallocated_blocks = []
    unallocated_blocks_inodes = []
    invalid_block_pointers = []
    unallocated_block_inodes = []
    
    for row1 in inode_arr:
        #direct
        for i in range(11, 23):
           block1 = str(int(row1[i],16))
           if int(row1[10],10) > 0 and int(row1[i],16) == 0 and (i - 10) <= int(row1[10]):
               invalid_block_pointers.append("INVALID BLOCK < " + block1 + " > IN INODE < " + str(row1[0]) +  " > ENTRY < " + str(i-11) + " >")
           if block1 != '0':
               if free_blocks_dict[block1] == 'T':
                   unallocated_blocks.append("UNALLOCATED BLOCK < "+block1+ " > REFERENCED BY")
                   unallocated_blocks_inodes.append("INODE < " + str(row1[0]) + " > ENTRY < " + str(i-11) + " >")
               seen_blocks_dict[block1] += 1
               if (seen_blocks_dict[block1] == 1):
                   duplicate_blocks_inodes[block1] = list()
                   duplicate_blocks_inodes[block1].append("INODE < " + str(row1[0]) +  " > ENTRY < " + str(i-11) + " >")
               if (seen_blocks_dict[block1] > 1):
                   duplicate_blocks_inodes[block1].append("INODE < " + str(row1[0]) +  " > ENTRY < " + str(i-11) + " >")
                
               
        #indirect
        for i in range(23,24):
            block1 = str(int(row1[i],16))
            checker1 = block1
            for row2 in indirect_arr:
                if block1 == str(int(row2[0],16)): #checks if the block number is the containing block of the indirect entry
                    block2 = str(int(row2[2],16))
                    if free_blocks_dict[block2] == 'T':
                        unallocated_blocks_inodes.append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[23] + " > ENTRY < " + str(row2[1]) + " >")
                    seen_blocks_dict[block2] += 1
                    if (seen_blocks_dict[block2] == 1):
                        duplicate_blocks_inodes[block2] = list()
                        duplicate_blocks_inodes[block2].append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[23] + " > ENTRY < " + str(row2[1]) + " >")
                    if (seen_blocks_dict[block2] > 1):
                        duplicate_blocks_inodes[block2].append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[23] + " > ENTRY < " + str(row2[1]) + " >")
                    
        #indirect2
        #if checker1 != '0':
            for i in range(24,25):
                block1 = str(int(row1[i],16))
                checker1 = block1
                if block1 != '0':
                    for row2 in indirect_arr:
                        if block1 == str(int(row2[0],16)):
                            block2 = str(int(row2[2],16))
                            if block2 != '0':
                                for row3 in indirect_arr:
                                    if block2 == str(int(row3[0],16)):
                                        block3 = str(int(row3[2],16))
                                        if free_blocks_dict[block3] == 'T':
                                            unallocated_blocks_inodes.append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[24] + " > ENTRY < " + str(int(row3[1]) + int(row2[1])*int(super_arr[0][3])/4) + " >")
                                        seen_blocks_dict[block3] += 1
                                        if (seen_blocks_dict[block3] == 1):
                                            duplicate_blocks_inodes[block3] = list()
                                            duplicate_blocks_inodes[block3].append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[24] + " > ENTRY < " + \
                                                                                           str(int(row3[1]) + int(row2[1])*int(super_arr[0][3])/4) + " >")
                                        if (seen_blocks_dict[block3] > 1):
                                            duplicate_blocks_inodes[block3].append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[24] + " > ENTRY < " + \
                                                                                           str(int(row3[1]) + int(row2[1])*int(super_arr[0][3])/4) + " >")

        
        #indirect3
        #if  checker1 != '0':
            for i in range(25,26):
                block1 = str(int(row1[i],16))
                if block1 != '0':
                    for row2 in indirect_arr:
                        if block1 == str(int(row2[0],16)):
                            block2 = str(int(row2[2],16))
                            if block2 != '0':
                                for row3 in indirect_arr:
                                    if block2 == str(int(row3[0],16)):
                                        block3 = str(int(row3[2],16))
                                        if block3 != '0':
                                            for row4 in indirect_arr:
                                                if block3 == str(int(row4[0],16)):
                                                    block4 = str(int(row4[2],16))
                                                    if free_blocks_dict[block4] == 'T':
                                                        unallocated_blocks_inodes.append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[25] + " > ENTRY < " + str(int(row4[1])\
                                                          + int(row3[1])*int(super_arr[0][3])/4*int(row2[1])*int(super_arr[0][3])/4) + " >")
                                                    seen_blocks_dict[block4] += 1
                                                    if (seen_blocks_dict[block4] == 1):
                                                        duplicate_blocks_inodes[block4] = list()
                                                        duplicate_blocks_inodes[block4].append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[25] + " > ENTRY < " + \
                                                                                               str(int(row4[1]) + int(row3[1])*int(super_arr[0][3])/4*int(row2[1])*int(super_arr[0][3])/4) + " >")
                                                    if (seen_blocks_dict[block4] > 1):
                                                        duplicate_blocks_inodes[block4].append("INODE < " + str(row1[0]) + " > INDIRECT BLOCK < " + row1[25] + " > ENTRY < " + \
                                                                                               str(int(row4[1]) + int(row3[1])*int(super_arr[0][3])/4*int(row2[1])*int(super_arr[0][3])/4) + " >")
                                                        

                                            
    free_inodes_dict = {}
    inode_bitmap_block = []
    free_inodes = []
    for row in group_arr:
        inode_bitmap_block.append(row[4])
    for row in bitmap_arr:
        if row[0] in inode_bitmap_block:
            free_inodes.append(row[1])
    for i in range(0,int(super_arr[0][1])):
        free_inodes_dict[str(i)] = 'F' # false it is not in inode bitmap
    for i in free_inodes:
        free_inodes_dict[i] = 'T' #true it is used in bitmap
    del free_inodes
    
    alloc_inodes = []
    alloc_inodes_dict = {}
    for row in inode_arr:
        alloc_inodes.append(row[0]);
    for i in range(0,int(super_arr[0][1])):
        alloc_inodes_dict[str(i)] = 'F'
    for i in alloc_inodes:
        alloc_inodes_dict[i] = 'T'
    # check for missing inodes(4)
    # if the link count is zero and it is not in free bitmap, then it is missing from bitmap
    #I calcualted the free list < block_num > by using its inode number and since each group has a inode table that holds 2048 inodes I find the correct group by dividing
    #it by inodes per group and taking the floor. if the inode nubmer is the max inode number then it is in the last group 
    '''On ext2 there are more than two inodes reserved for some special purpose as follows:
Inode 0 is used as a senital value to indicate null or no inode. similar to how pointers can be NULL in C. without a sentinel, you'd need an extra bit to test if an inode in a struct was set or not.
Inode 1 is the bad blocks inode - I believe that its data blocks contain a list of the bad blocks in the filesystem, which should not be allocated.
Inode 2 is the root inode - The inode of the root directory. It is the starting point for reaching a known path in the filesystem.
Inode 3 is the acl index inode. Access control lists are currently not supported by the ext2 filesystem, so I believe this inode is not used.
Inode 4 is the acl data inode. Of course, the above applies here too.
Inode 5 is the boot loader inode.
Inode 6 is the undelete directory inode. It is also a foundation for future enhancements, and is currently not used.
Inodes 7-10 are reserved and currently not used.
    '''
    missing_inodes = []
    for row in inode_arr:
        if (int(row[0],10) > 10) and (row[5] == '0') and (free_inodes_dict[row[5]] == 'F'):
            num = math.floor(int(row[0],10)/int(super_arr[0][6],10))
            if num >= 25:
                num = 24
            missing_inodes.append("MISSING INODE < " + row[0] + " > SHOULD BE IN FREE LIST < " + str(int(group_arr[int(num)][4],16)) + " >")
         
    for i in range(11,int(super_arr[0][1],10)):
        if (free_inodes_dict[str(i)] == 'F' and alloc_inodes_dict[str(i)] == 'F'): #if inode number is not in bitmap nor in allocated inodes then it is also missing
            num = math.floor(int(row[0],10)/int(super_arr[0][6],10))
            if num >= 25:
                num = 24
            missing_inodes.append("MISSING INODE < " + row[0] + " > SHOULD BE IN FREE LIST < " + str(int(group_arr[int(num)][4],16)) + " >")
    
    # calculating incorrect link counts(5)
    # basically check directory.csv and count how many times it has been referenced and if it matches the link count then you are good, otherwise print to txt
    incorrect_links = []
    for row1 in inode_arr:
        count = 0
        for row2 in directory_arr:
            if row2[4] == row1[0]:
                count+=1
        if int(row1[5],10) != count:
            incorrect_links.append("LINKCOUNT < " + row1[0] + " > IS < " + row1[5] + " > SHOULD BE < " + str(count) + " >")
    #incorrect indirectory entries(6)
    incorrect_directory = []
    for row1 in directory_arr:
        if row1[5] == ".":
            if row1[0] != row1[4]:
                incorrect_directory.append("INCORRECT ENTRY IN < " + row1[0] + " > NAME < "+ row1[5] + " > LINK TO < " + row1[4] + " > SHOULD BE < " + row1[0] + " >")
        elif row1[5] == "..":
            for parent in directory_arr:
                if parent[4] == row1[0]: # looks for the parent of the file
                    if parent[0] != row1[4]: # parent's parent should be the file because they are the same
                        if parent[5] != "." and parent[5] != "..": #if parents name is not . .. then there is an error b/c .. is broken
                            incorrect_directory.append("INCORRECT ENTRY IN < " + row1[0] + " > NAME < "+ row1[5] + " > LINK TO < " + row1[4] + " > SHOULD BE < " + parent[0] + " >")
                            
    #unallocated_inodes
    unalloc_inodes = []
    unalloc_inodes_directories = {}
    for row1 in directory_arr:
            if alloc_inodes_dict[row1[4]] == 'F':
                if row1[4] not in unalloc_inodes:
                    unalloc_inodes.append(row1[4])
                    unalloc_inodes_directories[row1[4]] = list()
                    unalloc_inodes_directories[row1[4]].append("DIRECTORY < " + row1[0] + " > ENTRY < " +  row1[1] + " >")
                if row1[4] in unalloc_inodes:
                    unalloc_inodes_directories[row1[4]].append("DIRECTORY < " + row1[0] + " > ENTRY < " +  row1[1] + " >")

                
                
    #write into lab3b_check.txt
    unallocated_blocks_inodes.sort()
    for row1 in unallocated_blocks:
        output.write(row1)
#        buf = row1
        for row2 in unallocated_blocks_inodes:
            output.write(" " + row2)
 #           buf += " "
  #          buf += row2
        output.write("\n")
   #     print buf
    
    for key in seen_blocks_dict:
        if seen_blocks_dict[key] >= 2:
            output.write("MULTIPLY REFERENCED BLOCK < " +  key + " > BY")
    #        buf = "MULTIPLY REFERENCED BLOCK < " +  key + " > BY"
            duplicate_blocks_inodes[key].sort()
            for i in duplicate_blocks_inodes[key]:
                output.write(" " +i)
     #           buf+=" "
      #          buf+= i
       #     print buf
            output.write("\n")
    for i in invalid_block_pointers:
        output.write(i+"\n")
       # print i
    for i in missing_inodes:
        output.write(i+"\n")
       # print i
    for i in incorrect_links:
        output.write(i+"\n")
       # print i
    for i in incorrect_directory:
        output.write(i+"\n")
        #print i

    for row1 in unalloc_inodes:
        output.write("UNALLOCATED INODE < " + row1 + "> REFERENCED BY")
        sorted(unalloc_inodes_directories[row1])
       # buf = "UNALLOCATED INODE < " + row1 + "> REFERENCED BY"
        for row2 in unalloc_inodes_directories[row1]:
            output.write(" "+ row2)
        #    buf += " "
         #   buf+= row2
        output.write("\n")
       # print buf
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

        
