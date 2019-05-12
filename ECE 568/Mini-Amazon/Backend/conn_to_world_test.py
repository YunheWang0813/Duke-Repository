import psycopg2
import socket
from multiprocessing.pool import ThreadPool  
from multiprocessing import Pool, cpu_count
import threading
import time
from threading import Thread
import sys
from google.protobuf.internal.decoder import _DecodeVarint32
from google.protobuf.internal.encoder import _VarintEncoder

import A_U_pb2
import world_amazon_pb2
import send_get_msg

#global_db = psycopg2.connect(host="vcm-7703.vm.duke.edu", database="amazon_db", user="amazonboy", port= "10000")

def init_world_socket():
    HOST = 'vcm-7703.vm.duke.edu'    # The remote host
    PORT = 23456              # The same port as used by the server
    c = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    c.connect((HOST, PORT))
    print ("connect success" )
    return c

def send_Aconnect(sock):

    cm=world_amazon_pb2.AConnect()
    cm.worldid=10
    cm.isAmazon=True
    '''
    iw=cm.initwh.add()
    iw.id=1
    iw.x=223
    iw.y=134
    '''
   
    send_message(sock,cm)

def send_ACommands(sock):
    cm = world_amazon_pb2.ACommands()

    # buy case
    buy = cm.buy.add()
    buy.whnum = 1
    buy.seqnum = 1
    product = buy.things.add()
    product.id = 1
    product.description = "basketball"
    product.count = 10

    send_message(sock, cm)

if __name__ == '__main__':
    world_sock=init_world_socket()

    send_Aconnect(world_sock)
    recv_Aconnected=get_message(world_sock, world_amazon_pb2.AConnected)

    send_ACommands(world_sock)
    recv_AResponse=get_message(world_sock, world_amazon_pb2.AResponses)
