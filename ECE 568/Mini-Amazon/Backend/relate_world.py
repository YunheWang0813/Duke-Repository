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

seqnum = 1
ack_set = set()

def sendHandler(world_sock, amazon_db):
    while 1:
        #print("AAAAAAAAAAAAAAA")
        c = amazon_db.cursor()
        # send world APack message when needed
        c.execute("SELECT id FROM MAIN_ORDER WHERE status = 'to pack' AND s_status = 'not packed';")
        orders = c.fetchall()
        if len(orders) != 0:
            for order in orders:
                topack_helper(world_sock, amazon_db, order[0])

        # send world APutOnTruck message when needed
        c.execute("SELECT id FROM MAIN_ORDER WHERE status = 'load' AND s_status = 'not loaded';")
        orders2 = c.fetchall()
        if len(orders2) != 0:
            for order in orders2:
                load_helper(world_sock, amazon_db, order[0])


def recvHandler(world_sock, amazon_db):
    global ack_set
    while 1:
        ackflag = False
        recv_msg = get_message(world_sock, world_amazon_pb2.AResponses)
        cm_ack = world_amazon_pb2.ACommands()
        for APurchasemore_resp in recv_msg.arrived:
            arrived_helper(world_sock, amazon_db, APurchasemore_resp)
            cm_ack.acks.append(APurchasemore_resp.seqnum)
            ackflag = True
        for APacked_resp in recv_msg.ready:
            ready_helper(world_sock, amazon_db, APacked_resp)
            cm_ack.acks.append(APacked_resp.seqnum)
            ackflag = True
        for ALoaded_resp in recv_msg.loaded:
            loaded_helper(world_sock, amazon_db, ALoaded_resp)
            cm_ack.acks.append(ALoaded_resp.seqnum)
            ackflag = True
        for ack_obj in recv_msg.acks:
            ack_set.add(ack_obj)
            print("FOR DEBUGGING ACK_SET:")
            print(ack_set)

        # send acks to world
        if ackflag:
            send_message(world_sock, cm_ack)

        if recv_msg.error:
            print("Something wrong!")
        if recv_msg.finished:
            print("close world connection")
            world_sock.close()

def load_helper(world_sock, amazon_db, shipid):
    c = amazon_db.cursor()
    c.execute("SELECT * FROM main_order WHERE id = {0};".format(shipid))
    the_order = c.fetchone()
    cm = world_amazon_pb2.ACommands()

    #print("SHIPID: " + str(shipid))
    print(the_order)
    print ("TRUCK_ID:",the_order[9])
    load = cm.load.add()
    load.whnum = 1
    load.truckid = the_order[9]
    load.shipid = shipid
    global seqnum
    load.seqnum = seqnum


    global ack_set
    tmp_seqnum = seqnum
    while True:
        send_message(world_sock, cm)
        time.sleep(3)
        if tmp_seqnum in ack_set:
            break
        print("still sending msg in load_helper")


    seqnum += 1
    c.execute("UPDATE main_order SET s_status = 'loading' WHERE id = {0};".format(shipid)) ###########currently assume world absolutely return ack
    amazon_db.commit()
    c.close()

def topack_helper(world_sock, amazon_db, shipid):
    c = amazon_db.cursor()
    #print("shipid is: " + str(shipid))
    c.execute("SELECT bought_quantity, item_id_id FROM main_order WHERE id = {0};".format(shipid))
    the_order_c = c.fetchone()
    cm = world_amazon_pb2.ACommands()
    topack = cm.topack.add()
    topack.whnum = 1
    topack.shipid = shipid
    global seqnum
    topack.seqnum = seqnum
    order = topack.things.add()
    order_item_id = the_order_c[1]
    c.execute("SELECT name, quantity FROM main_item WHERE id = {0};".format(order_item_id))
    item = c.fetchone()
    item_name = item[0]
    item_quant = item[1]
    order.id = order_item_id
    order.description = item_name
    order.count = the_order_c[0] #bought_quantity

    c.execute("UPDATE main_item SET quantity = {0} WHERE id = {1};".format(item_quant - the_order_c[0], order_item_id))
    print("FOR TEST item_quant: " + str(item_quant))
    print("FOR TEST the_order_c[0]: " + str(the_order_c[0]))
    print("FOR TEST item_quant - the_order_c[0]: " + str(item_quant - the_order_c[0]))
    # try really fast simspeed
    # cm.simspeed = 99999


    global ack_set
    tmp_seqnum = seqnum
    while True:
        send_message(world_sock, cm)
        time.sleep(3)
        if tmp_seqnum in ack_set:
            break
        print("still sending msg in topack_helper")


    seqnum += 1
    c.execute("UPDATE main_order SET s_status = 'packing' WHERE id = {0};".format(shipid)) ###########currently assume world absolutely return ack
    amazon_db.commit()
    c.close()

def arrived_helper(world_sock, amazon_db, APurchasemore_resp):
    c = amazon_db.cursor()
    #whnum = APurchasemore_resp.whnum
    for item in APurchasemore_resp.things:
        c.execute("SELECT quantity FROM MAIN_ITEM WHERE name = '{0}';".format(item.description))
        curr_quantity = c.fetchone()
        print("CURR_QUANTITY: " + str(curr_quantity[0]))
        if len(curr_quantity) == 0:
            c.execute("INSERT INTO MAIN_ITEM (name, quantity) SELECT {0}, {1};".format(item.description, item.count))
        else:
            c.execute("UPDATE MAIN_ITEM SET quantity = {0} WHERE name = '{1}';".format(item.count + curr_quantity[0], item.description))
    amazon_db.commit()

    # send world APack message
    '''
    c.execute("SELECT * FROM MAIN_ORDER WHERE status = 'to pack';")
    orders = c.fetchall()
    if orders is not None:
        for order in orders:
            topack_helper(world_sock, amazon_db, order[0], seqnum)
    '''
    c.close()

def ready_helper(world_sock, amazon_db, APacked_resp):
    c = amazon_db.cursor()
    tmp_shipid = APacked_resp.shipid
    c.execute("SELECT STATUS FROM MAIN_ORDER WHERE id = {0};".format(tmp_shipid))
    tmp_status = c.fetchone()
    print("ready_helper: tmp_status[0]: " + tmp_status[0])
    print("tmp_shipid: " + str(tmp_shipid))
    if tmp_status[0] == "to pack":
        c.execute("UPDATE MAIN_ORDER SET status = 'ready', s_status = 'not loaded' WHERE id = {0};".format(tmp_shipid))
        amazon_db.commit()

    '''
    if tmp_status[0] is "load":
        load_helper(world_sock, amazon_db, tmp_shipid, seqnum)
    c.close()
    '''

def loaded_helper(world_sock, amazon_db, ALoaded_resp):
    c = amazon_db.cursor()
    tmp_shipid = ALoaded_resp.shipid
    c.execute("SELECT STATUS FROM MAIN_ORDER WHERE id = {0};".format(tmp_shipid))
    tmp_status = c.fetchone()
    if tmp_status[0] == "load":
        c.execute("UPDATE MAIN_ORDER SET status = 'loaded' WHERE id = {0};".format(tmp_shipid))
        amazon_db.commit()
    c.close()

def Amazon_World(world_sock, amazon_db):
    print("AMAZON_WORLD function")

    t_send = threading.Thread(target=sendHandler, args=(world_sock, amazon_db))
    t_recv = threading.Thread(target=recvHandler, args=(world_sock, amazon_db))
    t_send.start()
    t_recv.start()