# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: A_U.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='A_U.proto',
  package='',
  syntax='proto2',
  serialized_pb=_b('\n\tA_U.proto\"\xa6\x01\n\x07\x41\x43reate\x12\x11\n\tpackageid\x18\x01 \x02(\x05\x12\x0c\n\x04whid\x18\x02 \x02(\x05\x12\x0c\n\x04\x64\x65sX\x18\x03 \x02(\x05\x12\x0c\n\x04\x64\x65sY\x18\x04 \x02(\x05\x12\x0c\n\x04wh_x\x18\x05 \x02(\x05\x12\x0c\n\x04wh_y\x18\x06 \x02(\x05\x12\r\n\x05upsid\x18\x07 \x01(\x05\x12\x10\n\x08password\x18\x08 \x01(\t\x12\x10\n\x08itemname\x18\t \x02(\t\x12\x0f\n\x07itemnum\x18\n \x02(\x05\".\n\x08UArrived\x12\x0f\n\x07truckid\x18\x01 \x02(\x05\x12\x11\n\tpackageid\x18\x02 \x02(\x05\"+\n\x05\x41Load\x12\x0f\n\x07truckid\x18\x01 \x02(\x05\x12\x11\n\tpackageid\x18\x02 \x02(\x05\"L\n\nUDelivered\x12\x0f\n\x07truckid\x18\x01 \x02(\x05\x12\x11\n\tpackageid\x18\x02 \x02(\x05\x12\x0c\n\x04\x64\x65sX\x18\x03 \x02(\x05\x12\x0c\n\x04\x64\x65sY\x18\x04 \x02(\x05\"y\n\tUAMessage\x12\x19\n\x07\x63reates\x18\x01 \x03(\x0b\x32\x08.ACreate\x12\x1a\n\x07\x61rrives\x18\x02 \x03(\x0b\x32\t.UArrived\x12\x15\n\x05loads\x18\x03 \x03(\x0b\x32\x06.ALoad\x12\x1e\n\tdelivered\x18\x04 \x03(\x0b\x32\x0b.UDelivered')
)
_sym_db.RegisterFileDescriptor(DESCRIPTOR)




_ACREATE = _descriptor.Descriptor(
  name='ACreate',
  full_name='ACreate',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='packageid', full_name='ACreate.packageid', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='whid', full_name='ACreate.whid', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='desX', full_name='ACreate.desX', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='desY', full_name='ACreate.desY', index=3,
      number=4, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='wh_x', full_name='ACreate.wh_x', index=4,
      number=5, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='wh_y', full_name='ACreate.wh_y', index=5,
      number=6, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='upsid', full_name='ACreate.upsid', index=6,
      number=7, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='password', full_name='ACreate.password', index=7,
      number=8, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='itemname', full_name='ACreate.itemname', index=8,
      number=9, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='itemnum', full_name='ACreate.itemnum', index=9,
      number=10, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=14,
  serialized_end=180,
)


_UARRIVED = _descriptor.Descriptor(
  name='UArrived',
  full_name='UArrived',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='truckid', full_name='UArrived.truckid', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='packageid', full_name='UArrived.packageid', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=182,
  serialized_end=228,
)


_ALOAD = _descriptor.Descriptor(
  name='ALoad',
  full_name='ALoad',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='truckid', full_name='ALoad.truckid', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='packageid', full_name='ALoad.packageid', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=230,
  serialized_end=273,
)


_UDELIVERED = _descriptor.Descriptor(
  name='UDelivered',
  full_name='UDelivered',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='truckid', full_name='UDelivered.truckid', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='packageid', full_name='UDelivered.packageid', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='desX', full_name='UDelivered.desX', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='desY', full_name='UDelivered.desY', index=3,
      number=4, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=275,
  serialized_end=351,
)


_UAMESSAGE = _descriptor.Descriptor(
  name='UAMessage',
  full_name='UAMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='creates', full_name='UAMessage.creates', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='arrives', full_name='UAMessage.arrives', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='loads', full_name='UAMessage.loads', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='delivered', full_name='UAMessage.delivered', index=3,
      number=4, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=353,
  serialized_end=474,
)

_UAMESSAGE.fields_by_name['creates'].message_type = _ACREATE
_UAMESSAGE.fields_by_name['arrives'].message_type = _UARRIVED
_UAMESSAGE.fields_by_name['loads'].message_type = _ALOAD
_UAMESSAGE.fields_by_name['delivered'].message_type = _UDELIVERED
DESCRIPTOR.message_types_by_name['ACreate'] = _ACREATE
DESCRIPTOR.message_types_by_name['UArrived'] = _UARRIVED
DESCRIPTOR.message_types_by_name['ALoad'] = _ALOAD
DESCRIPTOR.message_types_by_name['UDelivered'] = _UDELIVERED
DESCRIPTOR.message_types_by_name['UAMessage'] = _UAMESSAGE

ACreate = _reflection.GeneratedProtocolMessageType('ACreate', (_message.Message,), dict(
  DESCRIPTOR = _ACREATE,
  __module__ = 'A_U_pb2'
  # @@protoc_insertion_point(class_scope:ACreate)
  ))
_sym_db.RegisterMessage(ACreate)

UArrived = _reflection.GeneratedProtocolMessageType('UArrived', (_message.Message,), dict(
  DESCRIPTOR = _UARRIVED,
  __module__ = 'A_U_pb2'
  # @@protoc_insertion_point(class_scope:UArrived)
  ))
_sym_db.RegisterMessage(UArrived)

ALoad = _reflection.GeneratedProtocolMessageType('ALoad', (_message.Message,), dict(
  DESCRIPTOR = _ALOAD,
  __module__ = 'A_U_pb2'
  # @@protoc_insertion_point(class_scope:ALoad)
  ))
_sym_db.RegisterMessage(ALoad)

UDelivered = _reflection.GeneratedProtocolMessageType('UDelivered', (_message.Message,), dict(
  DESCRIPTOR = _UDELIVERED,
  __module__ = 'A_U_pb2'
  # @@protoc_insertion_point(class_scope:UDelivered)
  ))
_sym_db.RegisterMessage(UDelivered)

UAMessage = _reflection.GeneratedProtocolMessageType('UAMessage', (_message.Message,), dict(
  DESCRIPTOR = _UAMESSAGE,
  __module__ = 'A_U_pb2'
  # @@protoc_insertion_point(class_scope:UAMessage)
  ))
_sym_db.RegisterMessage(UAMessage)


# @@protoc_insertion_point(module_scope)
