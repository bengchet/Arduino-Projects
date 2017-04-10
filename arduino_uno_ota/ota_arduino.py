#!/usr/bin/python
#
# this script will push an OTA update to the ESP
#
# use it like: python ota_server.py <ESP_IP_address> <sketch.bin>
#
# on the ESP side you need code like this: https://gist.github.com/igrr/43d5c52328e955bb6b09 to handle the update
#

import socket
import sys
import os
import binascii

class Data:
    '''
    Class containing the data from non-contiguous memory allocations
    '''
    def __init__(self, begin, data):
        self.begin = begin
        self.data = data
        self.count = len(data)


def parse_line(line):
    '''
    Parameters:
        line: The line to parse
    Returns:
        The size of data. The address of data. The type of data. The line checksum.
        True if the checksum is correct, otherwise False.
    Description:
        It parses a line from the .hex file.
    '''
    ok = False
    size = int(line[1:3], 16)
    address = int(line[3:7], 16)
    type = int(line[7:9], 16)
    next_index = (9 + size * 2)
    data = binascii.a2b_hex(line[9:next_index])
    checksum = int(line[next_index:], 16)

    # checking if checksum is correct
    sum = size + (address >> 8) + (address & 0xFF) + type
    for byte in data:
        sum += ord(byte)

    if (~(sum & 0xFF) + 1) & 0xFF == checksum:
        ok = True

    return (size, address, type, data, checksum, ok)
#------------------------------------------------------------------------------


def read_hex_file(chunks, file):
    '''
    Parameters:
        chunks: An array with different chunks of data.
        path: The path to the .hex file to read
    Returns:
        True if the reading was successfully, otherwise False.
    Description:
        It reads a .hex file and stores the data in memory.
    '''
    line = file.readline()
    if line[0] != ':':
        print "The file seems to be a not valid .hex file"
        file.close()
        return False

    size, address, type, data, checksum, ok = parse_line(line.strip())
    if not ok:
        print "The checksum in line 1 is wrong"
        file.close()
        return False

    chunks.append(Data(address, data))

    # Read the other lines
    index = 0
    count = 2
    for line in file:
        size, address, type, data, checksum, ok = parse_line(line.strip())
        if not ok:
            print "The checksum in line", count, "is wrong"
            file.close()
            return False

        if chunks[index].begin + chunks[index].count == address:
            chunks[index].count += size
            for code in data:
                chunks[index].data += code
        else:
            chunks.append(Data(address, data))
            index += 1
        count += 1

    return True

#------------------------------------------------------------------------------

def serve(remoteAddr, filename):
  # Create a TCP/IP socket
  chunks = []
  total_size = 0
  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
  serverPort = 48266
  server_address = ('0.0.0.0', serverPort)
  print >>sys.stderr, 'starting up on %s port %s' % server_address
  sock.bind(server_address)
  sock.settimeout(2)
  sock.listen(1)

  sock2 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  remote_address = (remoteAddr, 8266)
  content_size = os.path.getsize(filename)
  print >>sys.stderr, 'upload size: %d' % content_size
  message = '%d %d %d\n' % (2, serverPort, content_size)
  print >>sys.stderr, 'sending invitation'
  sent = sock2.sendto(message, remote_address)


  while True:
    try:
      # Wait for a connection
      print >>sys.stderr, 'waiting for a connection'
      connection, client_address = sock.accept()
      connection.settimeout(2)
      print >>sys.stderr, 'connection from', client_address

      while True:
        data = connection.recv(4096)
        print >>sys.stderr, '%s' % data
        if (data=='ready'): break

      print >>sys.stderr, 'opening file %s' % filename
      f = open(filename, "rb")
      read_hex_file(chunks, f)
      for chunk in chunks:
        if (chunk.count==0): break
        total_size = chunk.count
        print >>sys.stderr, 'sending %d' % total_size
        connection.send(chunk.data)
      #digits = len(str(101 - 1))
      print >>sys.stderr, 'Uploading...'
      while True:
        data = connection.recv(4096)
        if(data=='end'):
          #print "%s%.2f%%" % ("\b"*9, 100)
          print >>sys.stderr, '#'
          break
        if(data=='fail'):
          print >>sys.stderr,'Upload failed'
          break
        #a = float(data)*128*100/total_size
        #print "%s%.2f%%" % ("\b"*(len(str(a))+5), a),
        sys.stderr.write('#')

      while True:
        data = connection.recv(4096)
        if(data=='end'): 
          print >>sys.stderr, 'done!'
          break
        if(data=='fail'):
          print >>sys.stderr,'Upload failed'
          break
        print >>sys.stderr, '%s' % data

      connection.close()
      f.close()
      sock.close()
      return 0

    except socket.timeout:
      print 'no connection found'
      return 1

    except:
      print 'unknown error'
      return 1

    finally:
      #connection.close()
      #f.close()
      sock.close()

def main(args):
  return serve(args[1], args[2])



if __name__ == '__main__':
  sys.exit(main(sys.argv))

