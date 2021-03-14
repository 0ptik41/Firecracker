from threading import Thread
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
		self.data = StringVar()
		self.data.set(self.name+': '+self.value)
		self.color = '#1111c1'
		[self.x, self.y] = pos
		self.obj = Frame(window, width=200, height=100)
		self.obj.grid(row=self.x, column=self.y, padx=5, pady=5)
		self.obj.config(background=self.color)
		self.val = Label(self.obj, 
						textvariable=self.data,
						background=self.color).grid(row=self.x, column=self.y, padx=10, pady=2)


class ProgramRegisters:

	def __init__(self):
		self.PID = -1 # Initialize to -1
		self.rip = 0;	self.rax = 0
		self.rdx = 0;	self.rcx = 0
		self.rbp = 0;	self.rsp = 0
		self.rdi = 0;	self.rbx = 0
		self.r10 = 0;	self.r11 = 0
		self.r9 = 0;	self.r8 = 0
		self.cs = 0;	self.ss = 0

	def set_pid(p):
		self.PID = p



def debugger(program, r):
	STATE = ProgramRegisters()
	rip = Register(r, 'RIP', '0x00000000', [1,0])
	rax = Register(r, 'RAX', '0x00000000', [2,0])
	rcx = Register(r, 'RCX', '0x00000000', [3,0])
	rdx = Register(r, 'RDX', '0x00000000', [4,0])
	rbp = Register(r, 'RBP', '0x00000000', [5,0])
	cs  = Register(r,  'CS', ' 0x00000000', [6,0])

	r08 = Register(r, 'R08', '0x00000000', [1, 1])
	r09 = Register(r, 'R09', '0x00000000', [2, 1])
	r10 = Register(r, 'R10', '0x00000000', [3, 1])
	r11 = Register(r, 'R11', '0x00000000', [4, 1])
	rsp = Register(r, 'RSP', '0x00000000', [5, 1])
	ss  = Register(r,  'SS', ' 0x00000000', [6, 1])


	newpid = os.fork()
	while True:
		if newpid == 0:
			# Execute the debugger program
			dbg_pid = launch_program(program)
			lib.show_registers(dbg_pid)
			lib.get_rax.argtypes = [c_int]
			lib.get_rax.restypes = c_ulong
			STATE.rip = c_ulong(lib.get_rip(dbg_pid) & 0xFFFFFFFF).value
			STATE.rax = c_ulong(lib.get_rax(dbg_pid) & 0xFFFFFFFF).value
			print(STATE.rip)
			rip.data.set("RIP: 0x%08x" % STATE.rip)
			rax.data.set("RIP: 0x%08x" % STATE.rax)
			rax.val.pack()
			rip.val.pack()
			time.sleep(.3)
		else:
			pids = (os.getpid, newpid)
			time.sleep(.3)


def launch_program(p):
	cmd = [p]
	p = subprocess.Popen(cmd)
	return p.pid

def quit():
	c = "ps aux | grep gui.py | cut -d ' ' -f 2 | while read n ; do kill -9 $n; done"
	os.system(c)

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

	q = Button(root, text="Quit", command=quit)
	q.grid(row=0,column=0,padx=2,pady=2)
	Thread(target=debugger, args=(target, root,)).start()
	

	root.mainloop() # run it


if __name__ == '__main__':
	main()