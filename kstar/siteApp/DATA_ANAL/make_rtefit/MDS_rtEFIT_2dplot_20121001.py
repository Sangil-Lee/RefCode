#!/usr/bin/env python
""" 
  GEQDSK EXTRACTER FROM MDS-PLUS (By Hyunsun)

Example)
   
  run(shot,time_i,time_f);

"""

#------------------------------------------------------------------------------------#

import ctypes as _C
from MDSplus import Connection
from MDSplus._mdsshr import _load_library, MdsException 

ConnectToMds=_load_library('MdsIpShr').ConnectToMds
ConnectToMds.argtypes=[_C.c_char_p]
DisconnectFromMds = _load_library('MdsIpShr').DisconnectFromMds
DisconnectFromMds.argtypes = [_C.c_int]

class _Connection( Connection):
    """
    Updating 'Connection' class in 'MDSplus' to manange the connection to the server
    (1) hanging off the connection when termination
    (2) retry the connection by 'reconnect' method
    Written by D. K. Oh
    Last Modification : Aug 2012
    """
    def __del__(self):
        self.closeConnection()
        
    def closeConnection(self):
        if self.socket != -1:
             if DisconnectFromMds(self.socket) == 0: 
                raise Exception, "Error in disconnection"
             else:
                self.socket = -1
                
    def reconnect(self):
        if self.hostspec == None:
             raise MdsException, "Error: no host specified"
        else:
             if self.socket != -1:
                print self.socket
                try:
                    self.closeConnection()
                except:
                    raise Exception, "Error in resetting connection to %s" %(self.hostspec,)
             self.socket = ConnectToMds(self.hostspec)
             if self.socket == -1:
                raise Exception, "Error connecting to %s" %(self.hostspec,)   

class MDS(object):
    """
    Implementation of a connection to the MDSplus tree based on the class mds by Y. M. Jeon
    Written by D. K. Oh
    Last modification : Aug 2012
    """
    __DefaultTree = "KSTAR"
    __DefaultServer = "172.17.100.200:8005"

    def __init__(self, shot=None, tree =__DefaultTree, server=__DefaultServer):
        try:                    
            self.alist = {"tree":None, "shot":None, "server":None}
            self.__mds__ = _Connection(server)
            if shot is not None:
                self.open(shot, tree)
        except MdsException:
            raise MdsException, "Error in the connection %s" %(server)
        except:
            raise Exception, " Unknown error in the connection %s" %(server)
        else:
            self.alist = {"tree":tree, "shot":shot, "server":server}
            
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()
        self.disconnect()
        
    def reset(self, shot=None, tree=None, server=__DefaultServer):
        if tree is None:
            tree = self.alist["tree"]
        if shot is None:
            shot = self.alist["shot"]
        
        self.alist = {"tree":tree, "shot":shot, "server":server}
        alist = self.alist
        __mds = self.__mds__ 
        __mds.hostspec = alist["server"]
        __mds.reconnect()
        if (shot is not None) and (tree is not None):
            self.open(tree, shot)

    def open(self, shot, tree =__DefaultTree):
        if shot is None:
            shot = self.alist["shot"]
        if shot is None:
            self.alist["tree"] = None
            self.alist["shot"] = None
            raise MdsException, "Error in open : shot number is not specified"
        else:    
            self.close( self.alist["shot"], self.alist["tree"])
            try:
                self.__mds__.openTree(tree, shot)
            except:
                self.alist["tree"] = None
                self.alist["shot"] = None
                raise MdsException, "Error in open : unknown error"    
            else:
                self.alist["tree"] = tree
                self.alist["shot"] = shot
                return self.__mds__

    def close(self, shot=None, tree=None): 
        if tree is None:
            tree = self.alist["tree"]
        if shot is None:
            shot = self.alist["shot"]
        if (shot is not None) and (tree is not None):
            try:
                self.__mds__.closeTree(tree, shot)
            except:
                raise MdsException, "Error in close : unknown error"
            else:
                self.alist["shot"] = None
                self.alist["tree"] = None

    def disconnect(self):
        self.__mds__.closeConnection()
        
    def get_T0(self):
        try:
            ret_str = self.__mds__.get('\T0_STR').data()
        except:
            ret_str = None            
            raise MdsException, "Error in get"
        return ret_str

    def get_sig(self, sigstr):
        try:
            t = self.__mds__.get('dim_of(%s)' %(sigstr)).data();
            v = self.__mds__.get(sigstr).data();
        except:
            t = numpy.ndarray([])
            v = numpy.ndarray([])             
            raise MdsException, "Error in get"
        return t,v;


import os, sys
import numpy as np
import matplotlib.pyplot as plt



#--------------------------------------------------------------------------------#

def run(shot,time_i,time_f,treename='EFITRT1'):
   import os, sys
   import subprocess                 # For issuing commands to the OS.
   import numpy as np
   from matplotlib import pylab
   #import matplotlib.tri as tri


     
   PARAMETERS=['\\bdry','\\gtime','\\lim','\\limitr','\\mh','\\mw','\\nbdry',
               '\\psirz','\\r','\\rgrid1',
               '\\rhovn','\\rmaxis','\\rzero','\\xdim','\\z',
               '\\zdim','\\zmaxis','\\zmid'];
   
   geqdsk=[];

   treename='EFITRT1'

   with MDS(server="172.17.100.200:8005") as mds:
      try:
          eq=mds.open(shot=shot, tree=treename);
      except: 
          print "Error #1"
      else:
          try:
              print mds.alist
       
              for signame in PARAMETERS:
                  print 'reading ...',signame
                  temp = eq.get(signame).data();
                  geqdsk.append(temp); 
                  #print geqdsk[PARAMETERS.index(signame)];
              
          except:
              print "Can not reading the signal\n Quit the program";          
              sys.exit(0);
          else:
              print "END of reading"
              #plt.show();


   mds.close();
   
   index_time = PARAMETERS.index('\\gtime');

   if( time_i < geqdsk[index_time][0] ):
       time_i = geqdsk[index_time][0];
       print "the initial time set to", time_i;
   
   if( time_f > geqdsk[index_time][len(geqdsk[index_time])-1] ):
       time_f =  geqdsk[index_time][len(geqdsk[index_time])-1]
       print "the final time set to", time_f;
       
   
       
   nlim =  geqdsk[PARAMETERS.index('\\limitr')][0]
            
   rlim = np.zeros( (nlim,) ) # "R of limiter points in meter"
   zlim = np.zeros( (nlim,) ) # "Z of limiter points in meter" 
            
   for w in range(nlim): 
       rlim[w] = geqdsk[PARAMETERS.index('\\lim')][w][0]
       zlim[w] = geqdsk[PARAMETERS.index('\\lim')][w][1]

   for i in range(0,len(geqdsk[index_time])):
       
       time = geqdsk[index_time][i];      
                   
       #if(( time >= time_i) and ( time <= time_f )):
       if True:
            time_fileout = time*1000;
            #print '%06d'%time_fileout,time
            filename='kstar_%s_%05d_%06d'%(treename,shot,time_fileout);
            print 'writing..',filename;
                       
            
            nw=geqdsk[PARAMETERS.index('\\mw')][i];
            nh=geqdsk[PARAMETERS.index('\\mh')][i];

            rmin   = geqdsk[PARAMETERS.index('\\rgrid1')][i]; 
            rdim   = geqdsk[PARAMETERS.index('\\xdim')][i]; 
            rmax   = rdim+rmin;
            dr     = (rmax - rmin)/float(nw - 1) 
            
            zdim   = geqdsk[PARAMETERS.index('\\zdim')][i]; 
            zmid   = geqdsk[PARAMETERS.index('\\zmid')][i];
            
            zmin = zmid - zdim*0.5 
            zmax = zmid + zdim*0.5 
            dz = (zmax - zmin)/float(nh - 1) 

            rmaxis = geqdsk[PARAMETERS.index('\\rmaxis')][i];
            zmaxis = geqdsk[PARAMETERS.index('\\zmaxis')][i];
            
            rs = np.arange(rmin, rmin + (rmax-rmin)*(1.+1.e-10), dr) 
            zs = np.arange(zmin, zmin + (zmax-zmin)*(1.+1.e-10), dz) 
            
            nbbbs =  geqdsk[PARAMETERS.index('\\nbdry')][i]
       
            rbbbs = np.zeros( (nbbbs,) ) # "R of boundary points in meter"
            zbbbs = np.zeros( (nbbbs,) ) # "Z of boundary points in meter" 
                       
            for w in range(nbbbs): 
                 rbbbs[w] = geqdsk[PARAMETERS.index('\\bdry')][i][w][0]
                 zbbbs[w] = geqdsk[PARAMETERS.index('\\bdry')][i][w][1]
                 
            string_time = '#%06d t=%6.5f\n'%(shot,time)
            
            pylab.figure(1, figsize=(4,6))
            
            #pylab.pcolor(rs, zs, geqdsk[PARAMETERS.index('\\psirz')][i], shading='interp')

            #pylab.fill([1.0,2.0,2.0,1.0],[0.0,0.0,1.0,1.0], facecolor='b',alpha=1.0, edgecolor='black') 

            # Create the Triangulation; no triangles so Delaunay triangulation created.

            if i==0:
                rxx, zyy = np.meshgrid(rs,zs);
            

            plt.contour(rxx,zyy,geqdsk[PARAMETERS.index('\\psirz')][i],20,linewidths=1)

            pylab.plot(rbbbs,zbbbs, 'r') 
            pylab.plot(rlim, zlim, 'k',linewidth=3) 

            pylab.axis('scaled') 

            pylab.title(string_time) 

            pylab.xlabel('R'); pylab.xticks([1.2,1.6,2.0,2.4]);
            pylab.ylabel('Z')
                        
            pylab.savefig(filename+'.png',fmt='png',transparent=False);
            
            pylab.clf()
            
            
   # Merging to make .avi
   
   not_found_msg = """
       The mencoder command was not found;
       mencoder is used by this script to make an avi file from a set of pngs.
       It is typically not installed by default on linux distros because of
       legal restrictions, but it is widely available.
         """
   
   try:
    subprocess.check_call(['mencoder'])
   except subprocess.CalledProcessError:
    print "mencoder command was found"
    pass # mencoder is found, but returns non-zero exit as expected
    # This is a quick and dirty check; it leaves some spurious output
    # for the user to puzzle over.
   except OSError:
    print not_found_msg
    sys.exit("quitting\n")
    
   
   filelist='mf://kstar_%s_%05d_*'%(treename,shot);
   
   outfile_name='kstar_%s_%05d.avi'%(treename,shot);
   
   command = ('mencoder',
           'mf://*.png',
           '-mf',
           'type=png:w=400:h=600:fps=15',
           '-ovc',
           'lavc',
           '-lavcopts',
           'vcodec=mpeg4',
           '-oac',
           'copy',
           '-o',
           outfile_name)

#os.spawnvp(os.P_WAIT, 'mencoder', command)

   print "\n\nabout to execute:\n%s\n\n" % ' '.join(command)
   subprocess.check_call(command)

   print "\n\n The movie was written to %s"%(outfile_name);

   cmd_rm = "rm -f *.png"
   os.system(cmd_rm)

   cmd_rm = "scp " + outfile_name + " kstar@172.17.100.137:~/rtEFITMovie/rtEFIT.avi"
   os.system(cmd_rm)

   webfile_name='rt%06d.avi'%(shot);
   cmd_rm = "scp " + outfile_name + " root@172.17.100.204:/usr/local/tomcat5/webapps/kstarweb/intranet/efitmovie/movies/"+webfile_name
   os.system(cmd_rm)




            
   

            
     
            
      
         
            
#--------------------------------------------------------------------------------#
if __name__=='__main__':
   """ """
   import os, sys
   import numpy as np
   import matplotlib.pyplot as plt
   
   exename=os.path.basename(__file__);
   nargs=len(sys.argv);
   
   """   if( (nargs==1) or (nargs != 3) ):
      print "  Usages : %s shot signal" %(exename);
      print "           ex) %s 5947 \\\\pol_ha04" %(exename);
      sys.exit()
   """
   shot=int(sys.argv[1]);

   time_i = 0.;
   time_f = 1000.;

   if ((nargs >= 3)): time_i = float(sys.argv[2]);
   if ((nargs >= 4)): time_f = float(sys.argv[3]);

   run(shot,time_i,time_f);
   
 

