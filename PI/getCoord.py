import json, os
'''Parses json file and returns coordinate values as a string as a whole.'''

def parsJson():    
    '''Returns the x_values and y_values as a tuple within a tuple
       from the supplied .json file.
       The first tuple has x_values and the second has the y_values.'''

    
    path = os.getcwd() + '\\resources\\' + 'file.json' # specifing relative path.

    fileObj = open(path)				#Creating a fileObject                       
    fileReadObj = fileObj.read() 		#Creating a fileReadObject
    py_values = json.loads(fileReadObj) #py_values are dictionary values.


    #typecasting dictionary py_values to tuple_values
    tuple_values = tuple(py_values.values())
    #storing x values and y values in coordinates

    coordinates = (tuple(tuple_values[1]), tuple(tuple_values[2])) #makes a tuple inside a tuple
    
    fileObj.close()

    return coordinates

def tuplesTostrList(tuple_value):
    '''Returns a list of string of x values.
        Must pass a tupe value in.'''
    str_list = []
    for value in tuple_value:
        str_list.append(str(value))

    return str_list


def strListTostr(str_x_list,str_y_list):
    '''uses ''.join(str_list) to convert str_list into pure string.
        Returns pure string with information about both x and y coordinates.
        xvalues comes first yvalues comes last. '''

    string_x_coordinates = ''.join(str_x_list)
    string_y_coordinates = ''.join(str_y_list)

    return (string_x_coordinates, string_y_coordinates)

def getCoord(): #MAIN FUNCTION IN THE SCRIPT.
    '''Takes input from the ouput of parsJson and.
    Returns tuples of strings with x values first and y values in the end'''

    tuples_in_tuples = parsJson() #store output from parsJson into list_tuples.

#GET INPUT:
    str_x_list = tuplesTostrList(tuples_in_tuples[0]) #gets a list of strings of x_values
    str_y_list = tuplesTostrList(tuples_in_tuples[1]) #gets a list of strings of y_values

#CONVERT INTO STRING AS A WHOLE
    # use strListTostr() to get our final string product.
    return strListTostr(str_x_list,str_y_list)


#test:
#Sample json input: {"x coords" : [3, 5, 2, 7, 2, 7],  "y coords" : [4, 5, 1, 3, 0, 6]}
#ouput returned when getCoord() called: ('352727', '451306')
