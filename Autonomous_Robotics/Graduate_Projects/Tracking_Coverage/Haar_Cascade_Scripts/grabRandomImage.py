''' This script will grab images from the given sites below, which can be found
    in image-net.org and get rid of the invalid one.

    Required:   Python 3, cv2, numpy, urllib, and os
    Group:      Zachariah Abuelahaj and Norlan Prudente
'''
import urllib.request
import cv2
import numpy as np
import os

def GrabRandomImage():
    ''' This function will grab image from the url provided, resize it to
        100x100, grayscale it, and save it.

        Required:   NA
        Return:     Saved images that is 100x100 and grayscaled
    '''
    # link for people images
    # neg_images_link = 'http://image-net.org/api/text/imagenet.synset.geturls?wnid=n07942152'
    # link for wall images
    neg_images_link = 'http://image-net.org/api/text/imagenet.synset.geturls?wnid=n03038685'
    neg_image_urls = urllib.request.urlopen(neg_images_link).read().decode()
    # image name would be an increment of number and this is where we change it
    # so it wont override old images
    pic_num = 2584

    # if the 'neg' folder don't exist creates it
    if not os.path.exists('resources/neg'):
        os.makedirs('resources/neg')

    # start getting images and sving it to 'neg' folder
    for i in neg_image_urls.split('\n'):
        # if the image is not from flickr.com, then skip it
        if not ('flickr.com' in str(i)):
            continue
        try:
            print(pic_num, i)
            # grab the image from the site
            urllib.request.urlretrieve(i, "resources/neg/"+str(pic_num)+".jpg")
            # grayscale it
            img = cv2.imread("resources/neg/"+str(pic_num)+".jpg",cv2.IMREAD_GRAYSCALE)
            # should be larger than samples / pos pic (so we can place our image on it)
            # resize it to 100x100 pixels
            resized_image = cv2.resize(img, (100, 100))
            # save it
            cv2.imwrite("resources/neg/"+str(pic_num)+".jpg",resized_image)
            # increment the pic_num for the next image name
            pic_num += 1

        except Exception as e:
            print(str(e))

def find_uglies():
    ''' Clean up the folder of negative images and get rid of the one that is
        not valid.
    '''
    match = False
    # for each image in neg
    for file_type in ['resources/neg']:
        for img in os.listdir(file_type):
            # compare it to an invalid image
            for ugly in os.listdir('resources/uglies'):
                try:
                    current_image_path = str(file_type)+'/'+str(img)
                    ugly = cv2.imread('resources/uglies/'+str(ugly))
                    question = cv2.imread(current_image_path)
                    # if the both the image in 'neg' and 'uglies' image are the
                    # same, get rid of it.
                    if ugly.shape == question.shape and not(np.bitwise_xor(ugly,question).any()):
                        print('That is one ugly pic! Deleting!')
                        print(current_image_path)
                        os.remove(current_image_path)
                except Exception as e:
                    print(str(e))


GrabRandomImage()
find_uglies()
