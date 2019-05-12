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
import relate_world

'''======================= Connection between amazon & django through DB, handle buy ======================='''
def Amazon_Django(world_sock, amazon_db):
    print("AMAZON_DJANGO function")
    while 1:
        c = amazon_db.cursor()
        c.execute("SELECT * FROM MAIN_FILL WHERE status = 'unfilled';")
        fills = c.fetchall()
        if len(fills) != 0:
            print(fills)
            cm = world_amazon_pb2.ACommands()
            buy = cm.buy.add()
            buy.whnum = 1
            buy.seqnum = relate_world.seqnum
            for fill in fills:
                c.execute("SELECT name, quantity FROM main_item WHERE id = {0};".format(fill[4]))
                item = c.fetchone()
                item_name = item[0]
                #item_quant = item[1]
                product = buy.things.add()
                product.id = fill[4]
                product.description = item_name
                product.count = fill[2] #fill_quantity
            c.execute("UPDATE MAIN_FILL SET status = 'filled' WHERE id = {0};".format(fill[0]))
            amazon_db.commit()
            send_message(world_sock, cm)

            tmp_seqnum = relate_world.seqnum
            while True:
                send_message(world_sock, cm)
                time.sleep(2)
                if tmp_seqnum in relate_world.ack_set:

                    break
                print("still sending msg in amazon-django")


            relate_world.seqnum += 1

        c.close()