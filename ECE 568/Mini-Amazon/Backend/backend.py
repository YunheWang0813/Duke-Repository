import psycopg2
import socket
from multiprocessing.pool import ThreadPool
from multiprocessing import Pool, cpu_count
import threading
import time
from threading import Thread
import sys
from send_get_msg import send_message, get_message
import A_U_pb2
import world_amazon_pb2
from relate_world import Amazon_World
from relate_ups import Amazon_Ups
from relate_django import Amazon_Django
import relate_world

def init_world_socket():
    HOST = 'vcm-8003.vm.duke.edu'  # The remote host
    PORT = 23456  # The same port as used by the server
    c = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    c.connect((HOST, PORT))
    print("connect success")
    return c

def init_operation(world_sock, amazon_db):
    cm = world_amazon_pb2.AConnect()
    cm.worldid = 1
    cm.isAmazon = True

    # create warehouse
    iw=cm.initwh.add()
    iw.id=1
    iw.x=2
    iw.y=4

    send_message(world_sock, cm)
    recv_Aconnected = get_message(world_sock, world_amazon_pb2.AConnected)


    '''
    # buy initial items to the warehouse
    cm = world_amazon_pb2.ACommands()
    buy = cm.buy.add()
    buy.whnum = 1
    buy.seqnum = relate_world.seqnum
    product1 = buy.things.add()
    product1.id = 1
    product1.description = "Apple"
    product1.count = 20
    product2 = buy.things.add()
    product2.id = 2
    product2.description = "Ball"
    product2.count = 10
    product3 = buy.things.add()
    product3.id = 3
    product3.description = "Candy"
    product3.count = 5

    send_message(world_sock, cm)

    relate_world.seqnum += 1
    recv_AResponse = get_message(world_sock, world_amazon_pb2.AResponses)

    # send Amazon side's ack
    cm2 = world_amazon_pb2.ACommands()
    if len(recv_AResponse.arrived) != 0:
        for arrived in recv_AResponse.arrived:
            cm2.acks.append(arrived.seqnum)
    send_message(world_sock, cm2)
    '''



    c = amazon_db.cursor()
    c.execute('TRUNCATE TABLE MAIN_WAREHOUSE, MAIN_FILL, MAIN_ITEM, MAIN_ORDER;')
    c.execute("INSERT INTO MAIN_WAREHOUSE (id, x, y) SELECT 1, 2, 4 WHERE NOT EXISTS (SELECT id FROM MAIN_WAREHOUSE WHERE id = 1);")
    #c.execute("INSERT INTO MAIN_ITEM (id, name, quantity) SELECT 1, 'Apple', 20 WHERE NOT EXISTS (SELECT id FROM MAIN_ITEM WHERE id = 1);")
    #c.execute("INSERT INTO MAIN_ITEM (id, name, quantity) SELECT 2, 'Ball', 10 WHERE NOT EXISTS (SELECT id FROM MAIN_ITEM WHERE id = 2);")
    #c.execute("INSERT INTO MAIN_ITEM (id, name, quantity) SELECT 3, 'Candy', 5 WHERE NOT EXISTS (SELECT id FROM MAIN_ITEM WHERE id = 3);")
    amazon_db.commit()
    c.close()


if __name__ == '__main__':
    amazon_db = psycopg2.connect(dbname='amazon_db', user='amazonboy', password='passw0rd', host='localhost', port='5432')
    world_sock = init_world_socket()
    init_operation(world_sock, amazon_db)


    t_world = threading.Thread(target=Amazon_World, args=(world_sock, amazon_db))
    t_ups = threading.Thread(target=Amazon_Ups)
    t_web = threading.Thread(target=Amazon_Django, args=(world_sock, amazon_db))

    t_world.start()
    t_ups.start()
    t_web.start()

    #t_world.join()
    #t_ups.join()
    #t_web.join()


    #amazon_db.close()