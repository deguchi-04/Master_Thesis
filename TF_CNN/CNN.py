##### STEPS
## Ler o dataset
## treinar
## testar
## referenciar ines no trabalho

import os
import glob as glob
import matplotlib.pyplot as plt
import cv2
import requests
import random
import numpy as np
import shutil

import torch
import utils
import cv2
from ultralytics import YOLO
from lxml import etree as ET
import pandas as pd
np.random.seed(42)
from csv import reader




class_names = ['trunk', 'tiny_grape_Bunch', 'medium_grape_bunch']
column_name = ['filename', 'width', 'height', 'class', 'xmin', 'ymin', 'xmax', 'ymax']
colors = np.random.uniform(0, 255, size=(len(class_names), 4))

path_image_train = "/home/thaidy/Documents/grape_trunk_dataset_split/train/*.jpg"
path_annotations_train = "/home/thaidy/Documents/grape_trunk_dataset_split/image_train_labels.csv"
path_labels_txt = "/home/thaidy/Documents/grape_trunk_dataset_split/labels_train/*.txt"

def read_csv(convert):
    if(convert == True):
        print("Converting CSV into txt...")
        # skip first line i.e. read header first and then iterate over each row od csv as a list
        with open(path_annotations_train, 'r') as read_obj:
            csv_reader = reader(read_obj)
            header = next(csv_reader)
            # Check file as empty
            if header != None:
                # Iterate over each row after the header in the csv
                for row in csv_reader:
                    # row variable is a list that represents a row in csv
                    name_txt = row[0].replace(".jpg", ".txt")
                    size = len(name_txt)
                    # Slice string to remove last 3 characters from string
                    mod_string = name_txt[:size - 4]
                    f = open('/home/thaidy/Documents/grape_trunk_dataset_split/labels_train/' + mod_string + ".txt", "a")
                    
                    if (row[3]=='trunk'):
                        f.write('0' + ' ' + row[1]  + ' ' + row[2] + ' ' + row[4] + ' ' + row[5] + ' ' + row[6] + ' ' + row[7] + '\n')
                    elif (row[3]=='tiny_grape_bunch'):
                        f.write('1' + ' ' + row[1]  + ' ' + row[2] + ' ' + row[4] + ' ' + row[5] + ' ' + row[6] + ' ' + row[7] + '\n')
                    else:
                        f.write('2' + ' ' + row[1]  + ' ' + row[2] + ' ' + row[4] + ' ' + row[5] + ' ' + row[6] + ' ' + row[7] + '\n')
        f.close()
    else:
        return
                
    

def str2bool(v):
  return v.lower() in ("yes", "true", "t", "1")

def dataset_resize():
    glob.glob("/home/thaidy/Documents/grape_trunk_dataset_split/train/*.jpg")
    
    return

# Function to plot images with the bounding boxes.
def plot(image_paths, label_paths, num_samples):
    all_training_images = glob.glob(image_paths)
    all_training_labels = glob.glob(label_paths)
    all_training_images.sort()
    all_training_labels.sort()

    num_images = len(all_training_images)
    num_labels = len(all_training_labels)
    print("Total images in the dataset " + str(num_images))
    print("Total labels in the dataset " + str(num_labels))

    plt.figure(figsize=(15, 12))
    for i in range(num_samples):
        j = random.randint(0,num_labels-1)
        k = str(all_training_labels[j]).replace("labels_train", "train")
        k= k.replace(".txt", ".jpg")
        image = cv2.imread(k)
        # print(all_training_images[j])
        # print(all_training_labels[j])

        # print(k)
        
        with open(all_training_labels[j], 'r') as f:
            bboxes = []
            labels = []
            label_lines = f.readlines()
            for label_line in label_lines:
                label = label_line[0]
                bbox_string = label_line[2:]
                _, _ ,xmin, ymin, xmax, ymax= bbox_string.split(' ')
                bboxes.append([xmin, ymin, xmax, ymax])
                labels.append(label)
        result_image = plot_box(image, bboxes, labels)
        plt.subplot(2, 2, i+1)
        plt.imshow(result_image[:, :, ::-1])
        plt.axis('off')
    plt.subplots_adjust(wspace=0)
    plt.tight_layout()
    print("Saving Fig...")
    plt.savefig("fig.jpg")
    #plt.show()

def plot_box(image, bboxes, labels):
    # Need the image height and width to denormalize
    # the bounding box coordinates
    h, w, _ = image.shape
    for box_num, box in enumerate(bboxes):
        # denormalize the coordinates
        xmin = int(box[0])
        ymin = int(box[1])
        xmax = int(box[2])
        ymax = int(box[3])

        #print(xmin,ymin,xmax,ymax)
        
        class_name = class_names[int(labels[box_num])]
        #print(class_name)
        cv2.rectangle(
            image, 
            (xmin, ymin), (xmax, ymax),
            color=colors[class_names.index(class_name)],
            thickness=2
        ) 

        font_scale = min(1,max(3,int(w/500)))
        font_thickness = min(2, max(10,int(w/50)))
        
        p1, p2 = (int(xmin), int(ymin)), (int(xmax), int(ymax))
        # Text width and height
        tw, th = cv2.getTextSize(
            class_name, 
            0, fontScale=font_scale, thickness=font_thickness
        )[0]
        p2 = p1[0] + tw, p1[1] + -th - 10
        cv2.rectangle(
            image, 
            p1, p2,
            color=colors[class_names.index(class_name)],
            thickness=-1,
        )
        cv2.putText(
            image, 
            class_name,
            (xmin+1, ymin-10),
            cv2.FONT_HERSHEY_SIMPLEX,
            font_scale,
            (255, 255, 255),
            font_thickness
        )
    return image

def main():
    # conv = input("Convert the csv to txt? (yes/no) ")
    # conv = str2bool(conv)
    # read_csv(conv)
    print("Number of txt labels : ", str(len(glob.glob("/home/thaidy/Documents/grape_trunk_dataset_split/labels_train/*.txt"))))
    plot(image_paths=path_image_train, label_paths=path_labels_txt, num_samples=4)
    return

if __name__ == "__main__":
    main()
#python train.py --img 640 --batch 16 --epochs 3 --data coco128.yaml --weights yolov5s.pt

