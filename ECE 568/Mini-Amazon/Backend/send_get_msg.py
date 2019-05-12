from google.protobuf.internal.encoder import _VarintEncoder
from google.protobuf.internal.decoder import _DecodeVarint32

def encode_varint(value):
    data = []
    _VarintEncoder()(data.append, value, None)
    return b''.join(data)

def decode_varint(data):
    return _DecodeVarint32(data, 0)[0]

def send_message(conn, msg):
    print("sending:\n", msg)
    data = msg.SerializeToString()
    size = encode_varint(len(data))
    conn.sendall(size + data)

def get_message(conn, msg_type):
    data = b''
    while True:
        try:
            data += conn.recv(1)
            size = decode_varint(data)
            break
        except IndexError:
            pass
    data = conn.recv(size)
    msg = msg_type()
    msg.ParseFromString(data)
    print ("getting:\n", msg)
    return msg