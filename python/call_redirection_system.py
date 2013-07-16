#!/usr/bin/python

"""
this is a system for calaulate call drecirction , First of all ,
what is call-redirection , I means assume this sence : This is sunday , I go to
a hospital to find a doctor , and I need to call the service number to check , BUT 
the number I called is having a holiday(admire him) , SO this call will redirect to 
the one who take the place of him , BUT , the new one maybe on vaction too...SO , 
this system is created to find the number you will call in the end...

the input has three section : first line is the call-redrection number and next many lines
is every item of a worker number / replacement number / vaction start day / vaction days
the last lines is the number for test...for every number you need output the route...
every line is a number/day combination...
"""

import string
def get_inputs() :
    dic = {}
    try : 
        first_line = raw_input()
    except EOFError: 
        return dic;

    if(not first_line.isdigit()) :
        print "first line is not digit number..."
        return {};
        
    lines = string.atoi(first_line)
    for i in range(lines) :
        line = raw_input()
        items = line.split(" ")
        if(len(items) != 4) :
            print "line %d is invalid !!!" %(i + 1)
            return {}

        k = 0
        for item in items : 
            if(not item.isdigit()) : 
                print "line %d must be all digit number ..." %(i + 1)
                return {}
            items[k] = items[k].strip()
            k += 1

        items[2] = string.atoi(items[2]);
        items[3] = string.atoi(items[3]) + items[2]

        if(items[0] in dic) :
            print "Can not has the same number in the first colunm in line %d !!!" %(i + 1)
            return {}

        dic[items[0]] = items[1 : ]

    return dic

def get_all_requests() :
    lst = []
    while(True) : 
        try :
            line = raw_input()
        except EOFError:
            return lst;

        tmp = line.split()
        sz = len(tmp)
        if(sz == 0) :
            continue ;

        if(sz > 2) : 
            print "Wrong line !!!";
            continue ;
        
        item = []
        if(sz == 1) :
            tmp[0] = tmp[0].strip()
            if(not tmp[0].isdigit()) :
                print "Wrong line !!!"
                continue ;

            day = string.atoi(tmp[0])
            lst.append((0 , day))
        if(sz == 2) :
            tmp[0] = tmp[0].strip()
            tmp[1] = tmp[1].strip()
            if(not tmp[0].isdigit() or not tmp[1].isdigit()) :
                print "Wrong line !!!"
                continue ;

            day = string.atoi(tmp[1])
            lst.append((1 , tmp[0] , day))

    return lst

def do_find_day(day , all) :
    counter = 0
    max_len = 0
    all_items = all.items()
    for item in all_items :
        if(day >= item[1][1] and day < item[1][2]) :
            counter += 1
        
        call = item[0]
        replace = item[0]
        this_len = 1
        while(True) :
            if(replace in all) :
                replace = all[replace][0]
            else :
                break;
        
            this_len += 1
    
        if(this_len > max_len) :
            max_len = this_len;
            start = call

    print "There are %d call redrection in day %d..." %(counter , day)
    print "The max length of call redrection id %d and started from %s ..." %(max_len , start)


def do_find_number(number , day , all) :
    if(number not in all) : 
        print "This number should not be redrected..."
        return ;

    result = []
    replace = number;
    while(True) :
        if(replace in all and day >= all[replace][1] and day < all[replace][2]) :
            result.append(replace)
            replace = all[replace][0]
        else :
            break;

   
    print "the route of call %s will redrection in day %d is : " %(number , day)
    for i in result :
        print "%s  " %i

import sys

def main() :
    all_items = get_inputs()
    if(len(all_items) == 0) :
        print "Error input...exiting..."
        sys.exit(-1)
    all_requests = get_all_requests()

    print "All items : \n"
    print str(all_items)
    print "All request : \n"
    print str(all_requests)


    for request in all_requests : 
        if(request[0] == 0) :
            do_find_day(request[1] , all_items)
        else :
            do_find_number(request[1] , request[2] , all_items)

    print "all request completed..."

if __name__ == "__main__" :
    main()
