import json, os
'''Parses json file and returns coordinate values as a string as a whole.'''
ylocs = []
xlocs = []
size = 0

def parsJson():    
    '''sets ylocs, xlocs, and size to values given by json'''

    
    #path = os.getcwd() + '\\resources\\' + 'file.json' # specifing relative path. #windows
    path = os.getcwd() + '/resources/file.json' #linux

    fileObj = open(path)                #Creating a fileObject                       
    fileReadObj = fileObj.read()        #Creating a fileReadObject
    py_values = json.loads(fileReadObj) #py_values are dictionary values.


    #typecasting dictionary py_values to tuple_values
    tuple_values = tuple(py_values.values())
    #storing x values and y values in coordinates
    ylocs = py_values.get("y coords")
    xlocs = py_values.get("x coords")
    size = int(py_values.get("size"))
    print(ylocs, xlocs, size)
    
    fileObj.close()

parsJson()
