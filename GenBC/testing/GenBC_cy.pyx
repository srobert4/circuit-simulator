'''
This Cython file generates the GenBC_cy executable that
should eventually perform the same functionality as GenBC

Generate GenBC_cy.c with cython --embed GenBC_cy.pyx, then 
compile GenBC_cy.c with gcc. Or use Makefile, and edit the
ENVDIR variable to contain the path to your virtual env (packages 
in requirements.txt)
'''

from libc.stdlib cimport exit
from libc.stdio cimport printf
from libc.string cimport memcpy
import time
import numpy as np
from scipy.io import FortranFile

# Use C bool type for Ngspice lib/callbacks
cdef extern from "stdbool.h": 
        ctypedef bint bool 

#======================#
# SHARED SPICE LIBRARY #
#======================#
cdef extern from "../sharedspice.h":
'''
The following block imports/includes the
ngspice shared library
'''
    ctypedef struct ngcomplex_t:
        double cx_real,
        double cx_imag

    struct vector_info:
        char *v_name;       # Same as so_vname
        int v_type;         # Same as so_vtype
        short v_flags;      # Flags (a combination of VF_*)
        double *v_realdata;     # Real data
        ngcomplex_t *v_compdata;    # Complex data
        int v_length;       # Length of the vector
    ctypedef vector_info* pvector_info

    struct vecvalues:
        char* name; #name of a specific vector
        double creal; #actual data value
        double cimag; #actual data value
        bool is_scale;#if 'name' is the scale vector
        bool is_complex;#if the data are complex numbers
    ctypedef vecvalues* pvecvalues

    struct vecvaluesall:
        int veccount; #number of vectors in plot
        int vecindex; #index of actual set of vectors. i.e. the number of accepted data points
        pvecvalues *vecsa; #values of actual set of vectors, indexed from 0 to veccount - 1
    ctypedef vecvaluesall* pvecvaluesall

    #info for a specific vector
    struct vecinfo:
        int number;     #number of vector, as postion in the linked list of vectors, starts with 0
        char *vecname;  #name of the actual vector
        bool is_real;   #TRUE if the actual vector has real data
        void *pdvec;    #a void pointer to struct dvec *d, the actual vector
        void *pdvecscale; #a void pointer to struct dvec *ds, the scale vector
    ctypedef vecinfo* pvecinfo

    #info for the current plot
    struct vecinfoall:
        #the plot
        char *name;
        char *title;
        char *date;
        char *type;
        int veccount;

        #the data as an array of vecinfo with length equal to the number of vectors in the plot
        pvecinfo *vecs;
    ctypedef vecinfoall* pvecinfoall

    ctypedef int (*SendChar)(char*, int, void*);
    ctypedef int (*SendStat)(char*, int, void*);
    ctypedef int (*ControlledExit)(int, bool, bool, int, void*);
    ctypedef int (*SendData)(pvecvaluesall, int, int, void*);
    ctypedef int (*SendInitData)(pvecinfoall, int, void*);
    ctypedef int (*BGThreadRunning)(bool, int, void*);
    ctypedef int (*GetVSRCData)(double*, double, char*, int, void*);
    ctypedef int (*GetISRCData)(double*, double, char*, int, void*);
    ctypedef int (*GetSyncData)(double, double*, double, int, int, int, void*);
    
    int  ngSpice_Init(SendChar* printfcn, SendStat* statfcn, ControlledExit* ngexit, 
                      SendData* sdata, SendInitData* sinitdata, BGThreadRunning* bgtrun, void* userData)
    int  ngSpice_Init_Sync(GetVSRCData* vsrcdat, GetISRCData* isrcdat, GetSyncData* syncdat, int* ident, void* userData)
    int  ngSpice_Command(char* command)
    pvector_info ngGet_Vec_Info(char* vecname)
    int ngSpice_Circ(char** circarray)
    char* ngSpice_CurPlot()
    char** ngSpice_AllPlots()
    char** ngSpice_AllVecs(char* plotname)
    bint ngSpice_running()

cdef vecvaluesall vecValues
cdef pvecvalues curVec

#============================#
# NGSPICE CALLBACK FUNCTIONS #
#============================#
'''
The Ngspice callback functions must be implemented without the python interpreter (nogil)
because they are called in the background thread. This means that only pure C functions
can be called. (Python syntax is ok e.g. no semi-colons or curly braces)

Modify these functions to modify the behavior of Ngspice
'''
cdef int ng_getchar(char* outputreturn, int ident, void* userdata) nogil:
'''
Used to return any output from Ngspice generated by printf or puts
Currently just prints to console.
'''
    printf("%s\n", outputreturn)
    return 0

cdef int ng_getstat(char* outputreturn, int ident, void* userdata) nogil:
'''
Used to return simulation status.
Currently just prints to console.
'''
    printf("%s\n", outputreturn)
    return 0

cdef int ng_thread_runs(bool noruns, int ident, void* userdata) nogil:
'''
Called when the background thread starts/stops
Currently prints status to console
'''
    if (noruns):
        printf("bg thread not running\n")
    else:
        printf("bg thread running\n")
    return 0

cdef int ng_data(pvecvaluesall vdata, int numvecs, int ident, void* userdata) nogil:
'''
Called when a new timestep of data is available.
Copies the data to vecValues so that the final result is available at the end
of the simulation.
'''
    memcpy(&vecValues, vdata, sizeof(vecvaluesall))
    return 0

cdef int ng_exit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata) nogil:
'''
Called when the function ControlledExit is hit.
Currently exits is quitexit is true, else returns exitstatus
'''
    if quitexit:
        exit(exitstatus)
    return exitstatus


#==================#
# PYTHON FUNCTIONS #
#==================#
def readGenBCFile():
'''
Reads the GenBC.int file, stored in INT_FILE
The assumed structure is:
flag (character literal)
timestep (double)
nDirichletSurfaces (int)
nNeumannSurfaces (int)
for each Dirichlet surface:
    value_initial value_final (double, double)
for each Neumann surface:
    value_initial value_final (double, double)

Returns (flag, timestep, dirichlet, neumann) as a tuple,
where dirichlet and neumann are maps from surface name (as
defined in DIRICHLET_SRFS and NEUMANN_SRFS) to value
'''
    genBC_reader = FortranFile(INT_FILE, 'r')
    flag = chr(genBC_reader.read_record('uint8')) # unicode string length 1
    # print "Read flag: " + flag
    timestep = genBC_reader.read_reals()[0]
    # print "Read timestep ", timestep
    nDirichletSurfaces = genBC_reader.read_ints()[0]
    # print "Read nDir ", nDirichletSurfaces
    if not nDirichletSurfaces == len(DIRICHLET_SRFS):
        raise ValueError("Inconsistent number of Dirichlet surfaces")

    nNeumannSurfaces = genBC_reader.read_ints()[0]
    # print "Read nNeu ", nNeumannSurfaces
    if not nNeumannSurfaces == len(NEUMANN_SRFS):
        raise ValueError("Inconsistent number of Neumann surfaces")

    dirichlet = {}
    neumann = {}

    for i in range(nDirichletSurfaces):
        dirichlet[DIRICHLET_SRFS[i]] = genBC_reader.read_reals()[0] / 1334
        # print "Read Dir val: ", dirichlet[DIRICHLET_SRFS[i]]

    for i in range(nNeumannSurfaces):
        neumann[NEUMANN_SRFS[i]] = genBC_reader.read_reals()[0]
        # print "Read Neu val: ", neumann[NEUMANN_SRFS[i]]

    return flag, timestep, dirichlet, neumann

def loadNetlist(surface, value):
'''
Opens up the file surface.cir and loads the netlist
into ngspice using the function ngSpice_Command to send
the command circbyline, which loads the netlist by sending
one line at a time.

This function skips comment and blank lines, and sets the value
of any external elements to the given value

TODO: is this the right netlist to be loading? Is setting this value correct?
'''
    filename = surface + ".cir"
    with open(filename, 'r') as circuit_reader:
        for line in circuit_reader:
            if line == "\n" or line.startswith("*"): continue
            if line.find("external") != -1:
                if line.startswith("V"): # Dirichlet Surface
                    # get value from dirichlet
                    line = line.replace("external", str(value))
                elif line.startswith("I"):
                    # get value from neumann
                    line = line.replace("external", str(value))
            command = "circbyline " + line
            ngSpice_Command(command.encode("utf-8"))



def getResult(surface, value):
'''
Loads the netlist for the given surface
and runs the simulation with the given value

Returns the final pressure at the node 1

TODO: Return the value of interest
TODO: Define the value of interest
TODO: Figure out unit conversion
'''
    loadNetlist(surface, value)
    ret = ngSpice_Command("bg_run")

    time.sleep(0.1)
    while ngSpice_running():
        time.sleep(0.1)

    print "Simulation for surface ", surface, " complete"
    # Write the QOI to file here

    for i in range(vecValues.veccount):
        curVec = vecValues.vecsa[i]
        name = curVec.name.decode('utf-8')
        if name == "V(1)":
            return curVec.creal * 1334


def writeGenBCFile():
'''
Writes the results for each surface (stored at neumann/dirichlet_results[surface_name])
to the file stored at INT_FILE.

TODO: Figure out the order
'''
    genBC_writer = FortranFile(INT_FILE,'w')
    genBC_writer.write_record(neumann_results["const"])
    genBC_writer.write_record(dirichlet_results["rcr"])

#==================#
# PYTHON CONSTANTS #
#==================#
INT_FILE = "GenBC.int" # File to read/write
DIRICHLET_SRFS = ["rcr"] # get dirichlet surfaces (from yaml?)
NEUMANN_SRFS = ["const"] # get neumann surface names (from yaml?)

#=============#
# MAIN SCRIPT #
#=============#
# Read GenBC.int
flag, timestep, dirichlet, neumann = readGenBCFile()

# Initialize SPICE
ret = ngSpice_Init(<SendChar*>ng_getchar, <SendStat*>ng_getstat, <ControlledExit*>ng_exit,  
    <SendData*>ng_data, NULL <BGThreadRunning*>ng_thread_runs, NULL);

# Run simulation for each Dirichlet surface
dirichlet_results = {}
for surf, val in dirichlet.items():
    dirichlet_results[surf] = getResult(surf, val)

# Run simulation for each Neumann surface
# TODO: actually run simulation
neumann_results = {}
for surf, val in neumann.items():
    neumann_results[surf] = -val # constant flow

# Write results to GenBC.int
writeGenBCFile()
