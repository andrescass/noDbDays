import sys
import json
import os 
import  time 
import datetime
from datetime import timedelta
from pandas import DataFrame
import pandas as pd

def incMonth(d, m, y):
    if m == 1 or m == 3 or m == 5 or m== 7 or m == 8 or m == 10 or m == 12 :
        if d == 31 :
            if m < 12:
                return m + 1
            else:
                return 1
    elif m == 2 :
        if y%4 != 0:
            if d == 28 :
                return 3
        else:
            if d == 29 :
                return 3
    else:
        if d == 30:
            return m+1
    
    return m

def incrementDate(currDate):
    result = ""
    dSplit = currDate.split("T")
    if dSplit[1] != "23:59:59":
        result += dSplit[0]
    else:
        m0 = int(dSplit[0].split("-")[1])
        m1 = incMonth(int(dSplit[0].split("-")[2]), int(dSplit[0].split("-")[1]), int(dSplit[0].split("-")[0]))
        if m0 != m1:
            if m1 < 10:
                result += dSplit[0].split("-")[0] + "-0" + str(m1) + "-01"
            else:
                result += dSplit[0].split("-")[0] + "-" + str(m1) + "-01"
        else:
            print("day")
            result += dSplit[0].split("-")[0] + "-" +dSplit[0].split("-")[1] + "-" + str(int(dSplit[0].split("-")[2])+1)
        result += "T00:00:00"
        return result
    result += "T"
    h = dSplit[1].split(":")
    if int(h[1]) < 59:
        result += h[0] + ":"
        if int(h[2]) < 59 :
            if int(h[2])+1 < 10:
                result += h[1] + ":0" + str(int(h[2])+1)
            else:
                result += h[1] + ":" + str(int(h[2])+1)
        else:
            if int(h[1])+1 < 10:
                result += "0" + str(int(h[1])+1) + ":00"
            else:
                result += str(int(h[1])+1) + ":00"
    else:
        if int(h[2]) < 59 :
            if int(h[2])+1 < 10:
                result += h[0] + ":" + h[1] + ":0" + str(int(h[2])+1)
            else:
                result += h[0] + ":" + h[1] + ":" + str(int(h[2])+1)
        else:
            if int(h[0])+1 < 10 :
                result += "0" + str(int(h[0])+1) + ":00:00"
            else:
                result += str(int(h[0])+1) + ":00:00"
    
    return result

def processJson(filename, filei, csvFile):
    #print("hey")
    #with open("jseconds.json" ,'r') as f:
    with open(filename ,'r') as f:
        data=json.load(f)
    #intervalinfo=data.get('intervalsDataPoints')
    data1=data.get('data')

    print("file {0} with {1} packets".format(filename, len(data1)))


    #print("length is :{}".format(len(datapoint)),datapoint)  
    #print(datapoint[1],type(datapoint[1]))#print(len(datapoint[1]))
    daystamp=[]
    timestamp=[]
    openPrice_ask=[]
    openPrice_bid=[]
    closePrice_ask=[]
    closePrice_bid=[]
    highPrice_ask=[]
    highPrice_bid=[]
    lowPrice_ask=[]
    lowPrice_bid=[]
    lastTradedVolume=[]
    lastline = None
    secs = 1
    newDate = ""
    idx = 0

    for d in range(len(data1)):
        intervalinfo=data1[d].get('intervalsDataPoints')
        datapoint=[item.get('dataPoints') for item in intervalinfo]
        for i in datapoint:
            #for j in i:
            while idx < len(i):
                #print(type(j),j)
                #print(j.get('timestamp'))
                j = i[idx]
                
                if lastline != None and (datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%dT%H:%M:%S")) > newDate:
                    if newDate.split("T")[1] < "21:00:00" or newDate.split("T")[1] > "22:00:00" :
                        #while (datetime.date.fromtimestamp(j.get('timestamp')/1000)) > newTStamp and :
                        t = datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%dT%H:%M:%S")
                        #print("t = {}".format(t))
                        #print(newDate)
                        openPrice_ask.append(lastline.get('openPrice').get('ask'))
                        openPrice_bid.append(lastline.get('openPrice').get('bid'))
                        closePrice_ask.append(lastline.get('closePrice').get('ask'))
                        closePrice_bid.append(lastline.get('closePrice').get('bid'))
                        highPrice_ask.append(lastline.get('closePrice').get('ask'))
                        highPrice_bid.append(lastline.get('closePrice').get('bid'))
                        lowPrice_bid.append(lastline.get('lowPrice').get('bid'))
                    
                        daystamp.append(newDate.split("T")[0])
                        timestamp.append(newDate)
                        
                        lastTradedVolume.append(lastline.get('lastTradedVolume'))
                        #print(datetime.date.fromtimestamp(j.get('timestamp')/1000).strftime("%d-%m-%yT%H:%M:%S"))
                        #print(datetime.date.fromtimestamp(lastline.get('timestamp')/1000).strftime("%d-%m-%yT%H:%M:%S"))
                        #print(newTStamp.strftime("%d-%m-%yT%H:%M:%S"))
                    newDate = incrementDate(newDate)

                elif len(j['openPrice']) > 0:
                    openPrice_ask.append(j.get('openPrice').get('ask'))
                    openPrice_bid.append(j.get('openPrice').get('bid'))
                    closePrice_ask.append(j.get('closePrice').get('ask'))
                    closePrice_bid.append(j.get('closePrice').get('bid'))
                    highPrice_ask.append(j.get('highPrice').get('ask'))
                    highPrice_bid.append(j.get('highPrice').get('bid'))
                    lowPrice_bid.append(j.get('lowPrice').get('bid'))
                
                    daystamp.append(datetime.date.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%d"))
                    timestamp.append(datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%dT%H:%M:%S"))
                    
                    lastTradedVolume.append(j.get('lastTradedVolume'))

                    lastline = j
                    newDate = incrementDate(datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%dT%H:%M:%S"))
                    idx += 1
                else:
                    if lastline != None:
                        openPrice_ask.append(lastline.get('openPrice').get('ask'))
                        openPrice_bid.append(lastline.get('openPrice').get('bid'))
                        closePrice_ask.append(lastline.get('closePrice').get('ask'))
                        closePrice_bid.append(lastline.get('closePrice').get('bid'))
                        highPrice_ask.append(lastline.get('closePrice').get('ask'))
                        highPrice_bid.append(lastline.get('closePrice').get('bid'))
                        lowPrice_bid.append(lastline.get('lowPrice').get('bid'))
                    
                        daystamp.append(datetime.date.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%d"))
                        timestamp.append(datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%dT%H:%M:%S"))
                        
                        lastTradedVolume.append(j.get('lastTradedVolume'))
                        newDate = incrementDate(datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%y-%m-%dT%H:%M:%S"))
                    idx += 1


    PackedData=[daystamp,
    timestamp,
    openPrice_ask,
    highPrice_ask,
    lowPrice_bid,
    closePrice_bid,
    ]
    df=DataFrame(PackedData).transpose()
    df.columns=['Day',
    ' Time',
    ' Open Price',
    ' High Price',
    ' Low Price',
    ' Close Price',
    ]

    from time import strftime,localtime
    t = time.localtime()
    timeop = time.strftime("%d-%m-%Y %H'%M", t)

    df.reset_index(drop=True, inplace=True)
    #df.to_csv("IsaacScrapper_Pumped{}.csv".format(timeop),index=False)
    #df.to_csv(str(sys.argv[2]),index=False)
    outputF = "c" + str(filei) + ".csv"
    df.to_csv(outputF,index=False)
    csvFile.write(outputF+'\n')

if __name__ == "__main__":
    fl = []
    idx = 0
    txtF = open("jsonFIles.txt", "r")
    csvF = open("csvfiles.txt", "w")
    for l in txtF :
        fl.append(l.rstrip('\n'))
        print(fl[len(fl)-1])
    for z in fl:
        if len(z) > 0:
            processJson(z, idx, csvF)
            idx += 1
    txtF.close()
    csvF.close()




