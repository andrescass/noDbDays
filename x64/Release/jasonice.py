import sys

#states
DISC = 0
WR = 1
CLOSING = 2
DISC1 = 3

#fi = open("seconds.txt", "r")
fi = open(str(sys.argv[1]), "r")
fl = fi.readlines()

#of = open("jseconds.json", "w")
of = open(str(sys.argv[2]), "w")

print("file has {0} lines \n".format(len(fl)))

i = 0
j = 0
k = 0
state = DISC1
output = ""

of.write("{\"data\":[")

for l in fl:
    while i < len(l):
        if state == DISC1:
            if l[i] != "{":
                i = i+1
            else:
                if l[i+1] == "\"":
                    if l[i+2] == "i":
                        output += l[i] + l[i+1] + l[i+2]
                        i = i+3
                        state = WR
                    else:
                        i = i+1
                else:
                    i = i+1
        if state == DISC:
            if l[i] != "{":
                i = i+1
            else:
                if l[i+1] == "\"":
                    if l[i+2] == "i" and l[i+3] == "n" and l[i+4] == "t":
                        output += "," + l[i] + l[i+1] + l[i+2]
                        i = i+3
                        state = WR
                    else:
                        i = i+1
                else:
                    i = i+1
        if state == WR:
            if l[i] == "n" and l[i+1] == "I" and l[i+2] == "d":
                state = CLOSING
            if l[i] == ":" and l[i+1] == "t" and l[i+2] == "r" and l[i+3] == "u" and l[i+4] == "e":
                output += l[i] + "\"" + l[i+1] + l[i+2] + l[i-3] + l[i+4] + "\""
                i = i + 5
            output += l[i]
            i = i+1
        if state == CLOSING:
            if l[i] == "}" or k > 60:
                if k > 59 and l[i] != "\"":
                    output += "\"}"
                else:
                    output += l[i]
                state = DISC
                k = 0 
                i = i+1
            else:           
                output += l[i]
                i = i+1
                k = k+1
    
    of.write(output)
    output = ""
    i = 0
    j = j + 1

of.write("]}")
of.close()
fi.close()



