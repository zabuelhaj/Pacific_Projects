''' This script will determin the path of both negative and positive images.
    Positive images path will be in info.dat and negative images will be in
    bg.txt.

    Required:   Python 3, os
    Group:      Zachariah Abuelahaj and Norlan Prudente
'''
# library required
import os

def descriptor():
    ''' Description of this function is up top.
    '''
    # traverse through directories
    for file_type in ['resources']:
        for folder in os.listdir(file_type):
            # ignor .DS_Store file
            if folder == '.DS_Store':
                continue
            # search for positive and negative images
            for img in os.listdir(file_type+'/'+folder):
                if img == '.DS_Store':
                    continue
                elif folder == 'frame':
                    # save positive by appending it to info.dat with
                    # path + how many object, position x and y, and
                    # width and height.
                    line = file_type+'/'+folder+'/'+img+' 1 0 0 35 45\n'
                    with open('info.dat','a') as f:
                        f.write(line)
                elif folder == 'image' or folder == 'neg':
                    # save the negative with just the path name
                    line = file_type+'/'+folder+'/'+img+'\n'
                    with open('bg.txt','a') as f:
                        f.write(line)

def main():
    descriptor()

if __name__ == '__main__':
    main()
