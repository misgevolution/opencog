import socket
import time

# This implements netcat in python.
def netcat(content, hostname = "localhost", port = 17001) :
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

  # If the cogserver is down, the connection will fail.
  try:
    s.connect((hostname, port))
  except socket.error as msg:
    print "Connect failed: ", msg
    s.close()
    return 1  # non-zero means failure

  s.sendall(content)
  s.shutdown(socket.SHUT_WR)
  while True:
    data = s.recv(1024)
    if not data or data == "":
      break
  # print "Received:", repr(data)
  # print "Connection closed."
  s.close()
  return 0  # zero means success

def scm_load(files) :
  for f in files:
    start_time = time.time()
    tmp = f.split('/')

    print "Loading %s" % (tmp[len(tmp)-1])
    netcat("(primitive-load \"" + f + "\")")
    print "Finished loading in %d sec" % (time.time() - start_time)

def load_ecan_module() :
  netcat("loadmodule opencog/attention/libattention.so")

def load_experiment_module() :
  netcat("loadmodule experiments/insect-poison/libinsect-poison-exp.so")

def start_ecan() :
  netcat("start-ecan")

def start_logger() :
  netcat("start-logger")

def topic_switched(is_on) :
  netcat("topic-switched "+(is_on==True and '1' or '0'))

def dump_af_stat(zfile) :
  netcat("dump-af-stat "+zfile)

def parse_sent_file(zfile) :
  netcat('(parse-all nlp-parse "'+zfile+'")')

def parse_sent(sent) :
  netcat('(nlp-parse "'+sent+'")')


def start_word_stimulation(stimulus) :
  netcat('(nlp-start-stimulation '+str(stimulus)+')')


load_files = ["/home/misgana/Desktop/ECAN/db/conceptnet4.scm",
              "/home/misgana/Desktop/ECAN/db/wordnet.scm",
              "/home/misgana/Desktop/ECAN/db/adagram_sm_links.scm"]

BASE_DIR = "/home/misgana/OPENCOG/opencog"

SENT_DIR = BASE_DIR+"/experiments/insect-poison/data/sentences"

# Atom(uuid), EnteredAt, LastSeenAt, STI, DurationInAF, IsNLPParseOutput, DirectSTI, GainFromSpreading
def extract_log(column, starting_row, file_name):
  col = []
  with open(file_namei, 'r') as log:
    start = 1
    for line in log:
      if(start != starting_row):
        start = start + 1
      else:
        values = line.split(',')
        col.push(values[column-1])

    return col

if __name__ == "__main__" :

  load_experiment_module()
  load_ecan_module()

  scm_load(load_files)
  
  print "Starting ecan and logger agents."
  start_logger()
  start_ecan()
  
  print "Starting parsing sentences."
  start_word_stimulation(100)
  parse_sent_file(SENT_DIR+"/insects.sent")


  print "Dumping log data."
  dump_af_stat("pydump")



  