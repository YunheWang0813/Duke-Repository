import math
from random import randint
from basic import checkKeyReturnValue
import progressbar

global l1_rhits
global l1_whits
global l1_irhits
global next_level

def denomi(i):
    return max(0.01, float(i))

def toNumber(s):
    l = len(s)
    sum = 0
    for i in range(l):
        j = int(s[l - 1 - i])
        sum += j* int(math.pow(2, i)) 
    return sum


def switchHex(h):
    return {
        '0': "0000",
        '1': "0001",
        '2': "0010",
        '3': "0011",
        '4': "0100",
        '5': "0101",
        '6': "0110",
        '7': "0111",
        '8': "1000",
        '9': "1001",
        'a': "1010",
        'b': "1011",
        'c': "1100",
        'd': "1101",
        'e': "1110",
        'f': "1111",
   }[h]


def parseHex(hx):
    bi = "" 
    for h in hx:
        bi += switchHex(h)
    return bi


def readInput(fname):
    with open (fname) as f:
        content = f.readlines()
    l = []
    print "Loading instructions:"
    bar = progressbar.ProgressBar(max_value = len(content))
    for e in bar(content):
        e_dict = e.split()
        l.append((int(e_dict[0]), parseHex(e_dict[1].zfill(8))))
    return l


class singeWay (object):
    def __init__(self, valid=False, tag = ""):
        self.LRU = False 
        self.valid = valid 
        self.tag = tag 




class cacheParameters(object):
    def __init__(self, config_path):

        with open(config_path) as f:
            content = f.readlines()
        c = [x for x in content]

        #associativity, num of ways 
        #A = 1 for direct map (C/B 1-way set, fast lookup)
        #A = (C/B) way for fully associative (one set only, slow, stores everything)
        #A*set_num = (C/B) 

        # name=0 for L1, name=1 for L2
        self.name = checkKeyReturnValue(c[0], "name", '=')
        # self.A = 1 
        self.A = checkKeyReturnValue(c[1], "A", '=')
        # B = block size
        self.B = checkKeyReturnValue(c[2], "B", '=')
        # C = capacity
        self.C = checkKeyReturnValue(c[3], "C", '=')
        # self.writeAllo = False
        self.writeAllo = checkKeyReturnValue(c[4], "writeAllo", '=')
        # 0 for LRU, 1 for RND,
        self.policy= checkKeyReturnValue(c[5], "policy", '=')
        # 0 for ins, 1 for data,
        # 2 for both
        self.insData = checkKeyReturnValue(c[6], "insData", '=')

        with open("system.txt") as f:
            content = f.readlines()
        s = [x for x in content]
        self.l1hit = checkKeyReturnValue(s[0], "l1hit", '=')
        self.l2hit = checkKeyReturnValue(s[1], "l2hit", '=')
        self.memory = checkKeyReturnValue(s[2], "memory", '=')

        set_num = self.C / self.B /self.A
        self.way = self.C / self.B
        self.cachelen = int( math.log(self.C, 2))
        self.offset = int(math.log(self.B, 2))
        self.setlen = int(math.log(set_num, 2))

        self.cheValid = range(self.way) 
        for i in self.cheValid:
            self.cheValid[i] = False

        self.cache = range(self.way) 

        self.LRU = range(set_num)
        for i in self.LRU:
            self.LRU[i] = 0 
        
        self.rmiss = 0 
        self.irmiss = 0 
        self.wmiss = 0 

        self.m0 = 0
        self.m1 = 0
        self.m2 = 0
        self.m3 = 0
        self.entries = 0 

    def decideNext(self, setNum):
        # 0 for LRU, 1 for RND,
        # 2 for NMRU, 3 for FIFO
        # Random 
        # Not most recent used
        
        # For LRU, it comes from 0 to A-1
        if self.policy == 0:
            for_rtn = self.LRU[setNum]
            self.LRU[setNum] += 1
            # 0 to self.A -1
            if self.LRU[setNum] > self.A-1:
                self.LRU[setNum] = 0

        elif self.policy == 1:
            for_rtn = randint(0, self.A-1) 

        return for_rtn 


    def calMode(self, mode): 
        m = int(mode)
        if m == 0:
            self.m0 += 1
        elif m == 1:
            self.m1 += 1
        elif m == 2:
            self.m2 += 1
        else:
            self.m3 += 1


    def addMiss(self, mode): # 0: Data Read, 1: Data Write, 2: Instruction Fetch
        if mode==0:
            self.rmiss += 1
        elif mode==1:
            self.wmiss += 1
        else:
            self.irmiss += 1


    def loadCache(self, input):
        self.entries = len(input)

        print "Cache simulating:"
        bar = progressbar.ProgressBar(max_value = len(input))
        for i in bar(input):

            if self.insData == 0 and i[0] != 2:
                continue
            elif self.insData == 1 and i[0] == 2:
                continue


            lenth = len(i[1])
            setid = i[1][(lenth - self.offset - self.setlen):(lenth - self.offset)]
            tag = i[1][:(lenth - self.offset - self.setlen)]
            self.calMode(i[0])

            setNum = toNumber(setid)
            start = self.A * setNum 

            find = False
            invalid_i = start+self.A
            for j in range (start, start+self.A):
                if self.cheValid[j] and self.cache[j] == tag: 
                    find = True
                    break
                if not self.cheValid[j]:
                    invalid_i = j
                    break

            if find == True:
                continue

            self.addMiss(i[0]) # miss!

            if i[0] == 1 and not self.writeAllo:
                continue

            if invalid_i < start+self.A: #There is still invalid way available in such set
                self.cache[invalid_i] = tag
                self.cheValid[invalid_i] = True
            else:
                k = self.decideNext(setNum)
                self.cache[start + k] = tag

    def printConfig (self):
        nameList = ["l1", "l2"]
        writeAlloList = ["False", "True"]
        policyList = ["LRU", "RND"]
        insDataList = ["instruction only", "data only", "unified"]

        print ""
        print("Cache type: %s_%s" %(nameList[self.name], insDataList[self.insData]))
        print("A: %d, B: %d, C: %d" %(self.A, self.B, self.C))
        print("Policy: %s, Write allocate: %s" %(policyList[self.policy], 
                                            writeAlloList[self.writeAllo]))
        print ""


    def printResult(self):
        global l1_rhits 
        global l1_whits 
        global l1_irhits 
        global next_level

        if self.name == 1: # L2
            self.m2 -= l1_irhits
            self.m0 -= l1_rhits
            self.m1 -= l1_whits  

        print ("Metrics                      Total           Instrn \
                Data            Read           Write            Misc") 
        #here to do
        print ("------                       -----           ------ \
                ----            ----           -----            ----")

        print ("Demand Fetches            %7d            %5d \
                %5d          %5d           %5d           %5d"
                %(self.m0+self.m1+self.m2, self.m2, self.m0+self.m1, \
                        self.m0, self.m1, self.m3))

        #l = float(self.entries)
        l = float(self.m0+self.m1+self.m2)
        print ("Fraction of total          %.4f           %.4f\
                %.4f         %.4f          %.4f          %.4f"
                %((self.m0+self.m1+self.m2)/l, self.m2/l, (self.m0+self.m1)/l, \
                        self.m0/l, self.m1/l, self.m3/l))
        print ""
        print ("Demand Misses             %7d            %5d \
                %5d          %5d           %5d           %5d"
                %(self.rmiss+self.wmiss+self.irmiss, self.irmiss, \
                        self.rmiss+self.wmiss, self.rmiss, self.wmiss, self.m3)) # Todo here

        print ("Demand Misses rate         %.4f           %.4f\
                %.4f         %.4f          %.4f          %.4f"
                %((self.rmiss+self.wmiss+self.irmiss)/denomi(self.m0+self.m1+self.m2), 
                    self.irmiss/denomi(self.m2), (self.rmiss+self.wmiss)/denomi(self.m0+self.m1), 
                    self.rmiss/denomi(self.m0), self.wmiss/denomi(self.m1), self.m3/denomi(self.m3)))

        if self.name == 1: # L2
            time = (self.m0+self.m1+self.m2)* self.l2hit
            time += (self.irmiss + self.rmiss)* self.memory 
            if self.writeAllo:
                time += self.wmiss* self.memory
            self.result = time

        if self.name == 0:
            l1_irhits = max(l1_irhits, self.m2 - self.irmiss)
            l1_rhits = max(l1_rhits, self.m0 - self.rmiss)
            l1_whits = max(l1_whits, self.m1 - self.wmiss)

            time = (self.m0+self.m1+self.m2)* self.l1hit

            time += (self.irmiss + self.rmiss)* next_level * self.memory
            if self.writeAllo:
                time += self.wmiss* next_level* self.memory
            self.result = time

        print ("Estimated time: %d" %(time))



def simuateSingleCache(config, input):
    l1 = cacheParameters(config)
    l1.printConfig()
    l1.loadCache(input)
    l1.printResult()
    return l1.result

if __name__ == "__main__":
    global l1_rhits
    global l1_whits
    global l1_irhits
    global next_level # set to 1 when no L2, access memory directly

    l1_rhits = 0
    l1_whits = 0
    l1_irhits = 0

    r1 = 0
    r2 = 0
    r3 = 0

#    next_level = 1 
#    input = readInput ("test1.txt")    
#    r1 = simuateSingleCache("configurationCase1.txt", input)


    next_level = 0 
    input = readInput ("test2.txt")
    r1 = simuateSingleCache("configurationCase2_1.txt", input)
    r2 = simuateSingleCache("configurationCase2_2.txt", input) 
    r3 = simuateSingleCache("configurationCase2_3.txt", input)

    print ("Overall access time: %s" %(r1 + r2 + r3))
