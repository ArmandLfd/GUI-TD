import os

def readFiles(path):
    i = 0
    for root, dirs, files in os.walk(path, topdown=False):
        for name in files:
            pathFile = os.path.join(root,name)
            
            # Loop through the file line by line
            try:
                # opening a text file
                file1 = open(pathFile, "r")
                for line in file1:
                    # checking string is present in line or not
                    if string1 in line:
                        print(pathFile)
                        break
                file1.close()
            except Exception:
                i += 1
        for name in dirs:
            readFiles(pathFile)

string1 = 'HEAD'
yourpath = "C:\\Users\\arman\\Desktop\\GUI TD"
readFiles(yourpath)



 

