import psycopg2
import socket
from multiprocessing.pool import ThreadPool  
from multiprocessing import Pool, cpu_count
import threading
import A_U_pb2
import time
from threading import Thread

from google.protobuf.internal.decoder import _DecodeVarint32
from google.protobuf.internal.encoder import _VarintEncoder
from send_get_msg import send_message, get_message


def recv_UArrived(msg):
    truckid=msg.truckid
    packageid=msg.packageid
    conn = psycopg2.connect(dbname='amazon_db', user='amazonboy', password='passw0rd', host='localhost', port='5432')#need to change specific info
    c=conn.cursor()
   # updateflag=False
    while True:
        c.execute("SELECT id FROM main_order WHERE id ={0} AND status='ready'".format(packageid))
        result=c.fetchone()
        if result is not None:
            break
        
    try:
        c.execute("UPDATE main_order SET status = 'load', truck_id = {0} WHERE id={1}".format(truckid, packageid))
        conn.commit()
        print("execute pending")
    except psycopg2.Error as e:
        conn.rollback()
        print("update order status from 'ready' to 'load' error:{0}".format(e))
    '''
    try:    
        c.execute("UPDATE main_order SET truck_id= {0} WHERE id = {1}".format(truckid,packageid))
        conn.commit()
    except psycopg2.Error as e1:
        conn.rollback()
        print("update truckid error:{0}".format(e1))
        '''

    c.close()
    conn.close()
    
    #        c=conn.cursor()
#       c.execute('INSERT')

    pass

def recv_UDlivered(msg):
    truckid=msg.truckid
    packageid=msg.packageid
    desx=msg.desX
    desy=msg.desY
    conn = psycopg2.connect(dbname='amazon_db', user='amazonboy', password='passw0rd', host='localhost', port='5432')#need to change specific info
    c=conn.cursor()
    while True:
        c.execute("SELECT id FROM main_order WHERE id = {0} AND status ='delivering'".format(packageid))
        result=c.fetchone()
        if result is not None:
            break
    try:
        c.execute("UPDATE main_order SET status = 'delivered', x ={0},y={1} WHERE id= {2} AND truck_id={3} AND status ='delivering' ".format(desx,desy,packageid,truckid))
        conn.commit()
    except psycopg2.Error as e2:
        conn.rollback()
        print("update main_order status from delivering to delivered error:{0}".format(e2))
    c.close()
    conn.close()
    pass

def send_acreate(ausend, result_p):  # fill the message_acreate to send
    result_pa = result_p
    cmacreate = A_U_pb2.UAMessage()
    connect_msg = cmacreate.creates.add()
    connect_msg.packageid = result_pa[0]
    connect_msg.whid = 1
    connect_msg.desX = result_pa[1]
    connect_msg.desY = result_pa[2]
    connsac = psycopg2.connect(dbname='amazon_db', user='amazonboy', password='passw0rd', host='localhost', port='5432')  # need to change specific info
    c = connsac.cursor()
    c.execute("SELECT * FROM users_profile WHERE user_id_id={0}".format(result_pa[7]))
    upsinfo_res = c.fetchone()

    connect_msg.wh_x = 2
    connect_msg.wh_y = 4
    if upsinfo_res is not None:
        connect_msg.upsid = upsinfo_res[1]  # first,  need to figure out how to arrange the upsid in the frontend
        connect_msg.password = upsinfo_res[2]  # first,  need to figure out how to arrange the upsid in the frontend

    c.execute("SELECT name, quantity FROM main_item WHERE id={0}".format(result_pa[6]))
    iteminfo_res = c.fetchone()
    connect_msg.itemname = iteminfo_res[0]
    connect_msg.itemnum = result_pa[4]

    send_message(ausend, cmacreate)

    pass

def send_aloaded(ausend,result_l):#fill the message_aload to send
    result_la=result_l
    cmaload=A_U_pb2.UAMessage()
    connect_msg=cmaload.loads.add()
    connect_msg.truckid=result_la[9]
    connect_msg.packageid=result_la[0]
    send_message(ausend,cmaload)
    
    pass

def init_socket_server_for_ups():
    serverau_send = socket.socket()
    serverau_send.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    serverau_send.bind(('0.0.0.0', 45678))#may need to change an  argument
    serverau_send.listen(5)
    return serverau_send

        
    pass

def init_socket_client_for_ups():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    port = 34567
    host_ip = "vcm-8265.vm.duke.edu"
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.connect((host_ip, port))#need to change arguments
    print("the socket has successfully connected to %s \
        on port %s\n" % (host_ip, port))
    return s


def checkdb_tosend(ausend):
    
    while True:
        conn = psycopg2.connect(dbname='amazon_db', user='amazonboy', password='passw0rd', host='localhost', port='5432')#need to change specific info
        c=conn.cursor()
        try:
            c.execute("SELECT * FROM main_order where status = 'pending';") ## to send <acreate>
            result_p=c.fetchone()
            if result_p is not None:
                send_acreate(ausend,result_p) ### think about the argument to pass and may need to change to another function
                print("sent to ups")
                try:
                    c.execute("UPDATE main_order SET status = 'to pack' WHERE id ={0}".format(result_p[0]))
                    conn.commit()
                except psycopg2.Error as e1:
                    conn.rollback()
                    print("update order status from 'pending' to 'to load' error:{0}".format(e1))
            #else:
                #print("result_p is None")
            
        except psycopg2.Error as e:
            print("select order_pending error:{0}".format(e))

        try:
            c.execute("SELECT * FROM main_order where status = 'loaded';") ## to send <aloaded>
            result_l=c.fetchone()
            if result_l is not None:
                send_aloaded(ausend,result_l) ### think about the argument to pass and may need to change to another function
                try:
                    c.execute("UPDATE main_order SET status='delivering' WHERE id = {0}".format(result_l[0]))
                    conn.commit()
                except psycopg2.Error as e2:
                    conn.rollback()
                    print("update order status from 'loaded' to delivering error:{0}".format(e2))
                    
        except psycopg2.Error as e3:
            print("select order_loaded error:{0}".format(e3))
        c.close()    
        conn.close()    

def handlerecvfromups(aurecv):
    while True:
        msg=get_message(aurecv,A_U_pb2.UAMessage)
        if len(msg.arrives) != 0:
            print("ARRRRRRRRRRRRRRRRRRRRRRRRRRIVES")
            for arrived in msg.arrives:
                recv_UArrived(arrived)
        elif len(msg.delivered) != 0:
            for deliver in msg.delivered:
                recv_UDlivered(deliver)


    pass
        
def Amazon_Ups():

    print("AMAZON_UPS function")
    aurecv = init_socket_client_for_ups()  ###### only use this socket to recv message from ups

    serverau_send= init_socket_server_for_ups()
    print("TEST")
    ausend,addr= serverau_send.accept()##### only use this socket to send message to ups 
    print('get connection from ',addr)

    # need to sleep(3)?????
    # print("TEST")

#    aurecv=init_socket_client_for_ups()###### only use this socket to recv message from ups
    t1= threading.Thread(target=checkdb_tosend,args=(ausend,))
    t2= threading.Thread(target=handlerecvfromups,args=(aurecv,))
    t1.start()
    t2.start()
    #t1.join()
    #t2.join()
    
    #checkdb_tosend(ausend)
    
    #  checkdb_torecv(aurecv)
  

    pass
   
