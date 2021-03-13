import multiprocessing
from Tkinter import *
from ctypes import *
import subprocess
import time
import sys 
import os 

lib = cdll.LoadLibrary("./dlib.so")

class Register:

	def __init__(self, window, label, data, pos):
		self.name = label
		self.value = data
		self.color = '#1111c1'
		[self.x, self.y] = pos
		self.obj = Frame(window, width=200, height=100)
		self.obj.grid(row=self.x, column=self.y, padx=5, pady=5)
		self.obj.config(background=self.color)
		self.val = Label(self.obj, 
						text=str(self.name+': '+self.value),
						background=self.color).grid(row=self.x, column=self.y, padx=10, pady=2)

class RegStruct(Structure):
	_fields_ = [("rip", c_ulong), 
				("rax", c_ulong),
				("rcx", c_ulong),
				("rdx", c_ulong),
				("rbp", c_ulong),
				("rsp", c_ulong),
				("cs", c_ulong),
				("ss", c_ulong),
				("r08", c_ulong),
				("r09", c_ulong),
				("r10", c_ulong),
				("r11", c_ulong)]



def launch_program(p):
	cmd = [p]
	p = subprocess.Popen(cmd)
	return p.pid

def debugger(t):
	newpid = os.fork()
	while True:
		if newpid == 0:
			dbg_pid = launch_program(t)
			lib.show_registers(dbg_pid)
			regs = RegStruct()
			lib.get_rax.argtypes = [c_int]
			lib.get_rax.restypes = c_ulong
			print('%08x' % c_ulong(lib.get_rax(dbg_pid)& 0xFFFFFFFF).value)
			time.sleep(3)
		else:
			pids = (os.getpid, newpid)
			time.sleep(3)


def main():
	# Load The C Library for Stepping through ELF binaries
	if not os.path.isfile('firecracker.so'):
		os.system('gcc -shared -fPIC -o firecracker.so bugger.c')
	
	
	# Load in program to debug
	if len(sys.argv) >=2:
		target = sys.argv[1]

	# Create GUI 
	root = Tk()
	root.wm_title("~ Firecracker ~") 
	root.config(background = "#b1b1b1b1b1b1")

	rip = Register(root, 'RIP', '0x00000000', [0,0])
	rax = Register(root, 'RAX', '0x00000000', [1,0])
	rcx = Register(root, 'RCX', '0x00000000', [2,0])
	rdx = Register(root, 'RDX', '0x00000000', [3,0])
	rbp = Register(root, 'RBP', '0x00000000', [4,0])
	cs  = Register(root,  'CS', ' 0x00000000', [5,0])

	r08 = Register(root, 'R08', '0x00000000', [0, 1])
	r09 = Register(root, 'R09', '0x00000000', [1, 1])
	r10 = Register(root, 'R10', '0x00000000', [2, 1])
	r11 = Register(root, 'R11', '0x00000000', [3, 1])
	rsp = Register(root, 'RSP', '0x00000000', [4, 1])
	ss  = Register(root,  'SS', ' 0x00000000', [5, 1])
	
	# Execute the debugger  program
	debugger(target) # This needs to be a daemon probably

	root.mainloop() # run it


if __name__ == '__main__':
	main()