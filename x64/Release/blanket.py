import sys
import json
import os 
import  time 
import datetime
from pandas import DataFrame
import pandas as pd

print("hey")
#with open("jseconds.json" ,'r') as f:
with open(str(sys.argv[1]) ,'r') as f:
    data=json.load(f)
#intervalinfo=data.get('intervalsDataPoints')
data1=data.get('data')

print(len(data1))



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
for d in range(len(data1)):
    intervalinfo=data1[d].get('intervalsDataPoints')
    datapoint=[item.get('dataPoints') for item in intervalinfo]
    for i in datapoint:
        for j in i:
            #print(type(j),j)
            #print(j.get('timestamp'))
            if len(j['openPrice']) > 0:
                openPrice_ask.append(j.get('openPrice').get('ask'))
                openPrice_bid.append(j.get('openPrice').get('bid'))
                closePrice_ask.append(j.get('closePrice').get('ask'))
                closePrice_bid.append(j.get('closePrice').get('bid'))
                highPrice_ask.append(j.get('highPrice').get('ask'))
                highPrice_bid.append(j.get('highPrice').get('bid'))
                lowPrice_bid.append(j.get('lowPrice').get('bid'))
            
                daystamp.append(datetime.date.fromtimestamp(j.get('timestamp')/1000).strftime("%d-%m-%y"))
                timestamp.append(datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%d-%m-%yT%H:%M:%S"))
                
                lastTradedVolume.append(j.get('lastTradedVolume'))

                lastline = j
            elif lastline != None:
                openPrice_ask.append(lastline.get('openPrice').get('ask'))
                openPrice_bid.append(lastline.get('openPrice').get('bid'))
                closePrice_ask.append(lastline.get('closePrice').get('ask'))
                closePrice_bid.append(lastline.get('closePrice').get('bid'))
                highPrice_ask.append(lastline.get('closePrice').get('ask'))
                highPrice_bid.append(lastline.get('closePrice').get('bid'))
                lowPrice_bid.append(lastline.get('lowPrice').get('bid'))
            
                daystamp.append(datetime.date.fromtimestamp(j.get('timestamp')/1000).strftime("%d-%m-%y"))
                timestamp.append(datetime.datetime.fromtimestamp(j.get('timestamp')/1000).strftime("%d-%m-%yT%H:%M:%S"))
                
                lastTradedVolume.append(j.get('lastTradedVolume'))


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
df.to_csv(str(sys.argv[2]),index=False)



