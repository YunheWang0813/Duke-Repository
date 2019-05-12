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

def encode_varint(value):
    """ Encode an int as a protobuf varint """
    data = []
    _VarintEncoder()(data.append, value, None)
    return b''.join(data)


# decode the data received from the "world"
def decode_varint(data):
    """ Decode a protobuf varint to an int """
    return _DecodeVarint32(data, 0)[0]

def send_message(conn, msg):
    """ Send a message, prefixed with its size, to a TPC/IP socket """

    print("sending:", msg)

    data = msg.SerializeToString()
    size = encode_varint(len(data))
    conn.sendall(size + data)

def get_message(conn, msg_type):
    """ Receive a message, prefixed with its size, from a TCP/IP socket """
    # Receive the size of the message data
    data = b''
    while True:
        try:
            data += conn.recv(1)
            size = decode_varint(data)
            break
        except IndexError:
            pass
    # Receive the message data
    data = conn.recv(size)
    # Decode the message
    msg = msg_type()
    msg.ParseFromString(data)

    print ("getting:", msg)
    return msg
#send server

def serverSend():
    """server = socket.socket()
    server.bind(('0.0.0.0', 45678))
    server.listen(5)"""
    conn1,addr = server.accept()
    cm=A_U_pb2.UAMessage()
    connect_mag =cm.loads.add()
    connect_mag.seqnum = 145
    connect_mag.truckid = 167
    

    send_message(conn1, cm)
    time.sleep(1)
    conn1.close()

def clientRecv():
    click = socket.socket()
    click.connect(('vcm-8265.vm.duke.edu', 34567))
    connected = get_message(click, A_U_pb2.UAMessage)
    print(connected.seqnum)
   # click.send(str(data2).encode())
   # time.sleep(1) 
    click.close()





if __name__ == '__main__':
    server = socket.socket()
    server.bind(('0.0.0.0', 45678))
    server.listen(5)
    time.sleep(5)
    t1=threading.Thread(target=serverSend,args=())
    t2=threading.Thread(target= clientRecv,args=())
    t1.start()
    t2.start()
    t1.join()
    t2.join()
