#!/usr/bin/env python
""" 
  GEQDSK EXTRACTER FROM MDS-PLUS (By Hyunsun)

Example)
   
  run(shot,time_i,time_f,image_ok);

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
    #__DefaultServer = "172.17.250.100:8005"
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



#--------------------------------------------------------------------------------#

def run(shot,time_i,time_f,image_ok,treename='EFITRT1'):
   import os, sys
   import subprocess                 # For issuing commands to the OS.
   import numpy as np
   
   from matplotlib import pylab
   
   import matplotlib.gridspec as gridspec
   import matplotlib.patches as patches
  
   try:
     from PIL import Image
   except ImportError, exc:
     raise SystemExit("PIL must be installed to run this example")

   import matplotlib.cbook as cbook


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
       print 'Count: ',i;

   TimeLen = len((geqdsk[index_time]));
   print 'TimeLen: ', TimeLen;

   SkipCnt = 2
   if TimeLen >= 300 & TimeLen < 500 : SkipCnt = 2
   if TimeLen >= 500 & TimeLen < 700 : SkipCnt = 3
   if TimeLen >= 700  : SkipCnt = 4

   print 'SkipCnt: ', SkipCnt;


   for i in range(0,len(geqdsk[index_time])):
       print 'Count: ',i;
       time = geqdsk[index_time][i];      
       print 'Time: ', time;
                   
       #if(( time >= time_i) and ( time <= time_f )):
       if True:
            time_fileout = time*1000;
            #print '%06d'%time_fileout,time
            filename='kstar_%s_%05d_%05d'%(treename,shot,time_fileout);
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
                 
            string_time = 't=%6.3f'%(time);
            shot_number = '#%06d'%(shot);
            
            #pylab.figure(1, figsize=(12,36))
            #pylab.figure(1, figsize=(12,36), dpi=180)
            #pylab.figure(1, figsize=(15,45), dpi=180)
            pylab.figure(1, figsize=(18,54), dpi=90)

            #pylab.title(shot_number,fontsize=32)
            #pylab.title(string_time,fontsize=32)

            #gs = gridspec.GridSpec(1, 3, width_ratios=[1, 1.89]) 
            #gs = gridspec.GridSpec(1, 3, width_ratios=[2.35, 1, 1.87]) 
            #gs = gridspec.GridSpec(1, 3, width_ratios=[1.87, 1, 1.87]) 
            #gs = gridspec.GridSpec(1, 3, width_ratios=[1.65, 1, 1.65])
            gs = gridspec.GridSpec(1, 3, width_ratios=[1.38, 1, 1.51])

            gs.update(wspace=0.01, hspace=0.01)
            #gs = gridspec.GridSpec(1, 3) 

            ax1 = pylab.subplot(gs[1])
            
            #pylab.pcolor(rs, zs, geqdsk[PARAMETERS.index('\\psirz')][i], shading='interp')

            #pylab.fill([1.0,2.0,2.0,1.0],[0.0,0.0,1.0,1.0], facecolor='b',alpha=1.0, edgecolor='black') 

            # Create the Triangulation; no triangles so Delaunay triangulation created.

            if i==0:
                rxx, zyy = np.meshgrid(rs,zs);
            

            ax1.contour(rxx,zyy,geqdsk[PARAMETERS.index('\\psirz')][i],20,linewidths=1)

            ax1.plot(rbbbs,zbbbs, 'r') 
            ax1.plot(rlim, zlim, 'k',linewidth=3) 

            ax1.axis('scaled') 

            pylab.title(shot_number,fontsize=40)

            #pylab.text(2.0, 1.6, string_time, fontsize=16, alpha=1.0, va='bottom', ha='left')

            #pylab.xlabel('R'); 
            ax1.get_xaxis().set_ticks([]); # xticks([1.2,1.6,2.0,2.4],fontsize=0);
            ax1.get_yaxis().set_ticks([]); # pylab.yticks(fontsize=0);
            #pylab.ylabel('Z')

            ax2 = pylab.subplot(gs[2])

            pylab.title(string_time,fontsize=40)

            #set time ...
            if image_ok == 1:
               print 'File Choice: %d'%int((time*1000+100)*210/1000)
               bmp_time = '/home/rtefitbdry/diag_TV_2012/2012C05/TV01/%06d/%06d-%05d.bmp'%(shot,shot,int((time*1000+100)*210/1000));
               tv03_file = '/home/rtefitbdry/diag_TV_2012/2012C05/TV03/%06d/%06d_%06d.png'%(shot,shot,int((time*1000)*100/1000));
            else:
               bmp_time = './empty.bmp'

            file_ok = os.path.exists(bmp_time);
            if file_ok:
               print 'File Existed: ', file_ok;
            else:
               print 'File Existed: ', file_ok;
               print 'Loop continue...'
               continue

            file_ok = os.path.exists(tv03_file);
            if file_ok:
               print 'File Existed: ', file_ok;
            else:
               print 'File Existed: ', file_ok;

            #print 'TV03 File Choice: %d'%int((time*1000)*100/1000)

            print 'reading ... %s'%(bmp_time);
            print 'reading ... %s'%(tv03_file);
            #if i%2 == 0 : continue # for saving to spend time making image.

            if i%SkipCnt != 0 : continue # for saving to spend time making image.

            image = Image.open(tv03_file)
            image2 = Image.open(bmp_time)

            #img1 = image.crop((64,0,512,512)) #TV03 Image cropping
            img1 = image.crop((210,80,512,492)) #TV03 Image cropping
            img1.save('tv03_tmp.png')

            #img2 = image2.crop((80,54,528,566)) #TV03 Image cropping
            img2 = image2.crop((150,80,520,540)) #TV01 Image cropping
            img2.save('tv01_tmp.png')
            
            #a = pylab.imread(bmp_time)
            a = pylab.imread('tv01_tmp.png')

            #generates a RGB image, so do
            fig_bmp=pylab.mean(a,2) # to get a 2-D array
            #DPI = pylab.gcf().get_dpi()
            #Size = fig_bmp.shape
            #print 'Size: ', Size

            #GCF = pylab.gcf()
            #DPI = GCF.get_dpi()
            #print "DPI: ", DPI

            #b_box_1 = (top, left, bottom,right) 
            #b_box_1 = (10,10,200,200)
            #a = a.crop(b_box_1);

            ax3 = pylab.subplot(gs[0])
            #b = pylab.imread(tv03_file)   
            b = pylab.imread('tv03_tmp.png')

            fig_tv03=pylab.mean(b,2)
           
            print 'done'
            
            #fig_bmp = Image.open(datafile)
            #dpi = pylab.rcParams['figure.dpi']
            
            #figsize = fig_bmp.size[0]/dpi, fig_bmp.size[1]/dpi

            #figure(figsize=figsize)
            
            #ax2.axis('scaled')
            
            ax2.get_xaxis().set_ticks([]);
            ax2.get_yaxis().set_ticks([]);

            ax3.get_xaxis().set_ticks([]);
            ax3.get_yaxis().set_ticks([]);

            #im2 = ax2.imshow(fig_bmp, origin='lower')
            #im2 = ax2.imshow(fig_bmp, origin='lower', cmap=pylab.gray())
            im2 = ax2.imshow(fig_bmp, cmap=pylab.gray())
            im3 = ax3.imshow(fig_tv03)
            
            pylab.savefig(filename+'.png',fmt='png',transparent=False,bbox_inches='tight');
            
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
   
   outfile_name='kstar_%s_%05d_tv1.avi'%(treename,shot);
   
   command = ('mencoder',
           filelist,
           '-mf',
           'type=png:w=900:h=800:fps=6',
           '-ovc',
           'lavc',
           '-lavcopts',
           #'vcodec=mpeg4:mbd=2:mv0:trell:v4mv:cbp:last_pred=3:predia=2:dia=2:vmax_b_frames=2:vb_strategy=1:precmp=2:cmp=2:subcmp=2:preme=2:qns=2',
           'vcodec=mpeg4',
           '-oac',
           'copy',
           '-o',
           outfile_name)

#os.spawnvp(os.P_WAIT, 'mencoder', command)

   print "\n\nabout to execute:\n%s\n\n" % ' '.join(command)
   subprocess.check_call(command)

   print "\n\n The movie was written to %s"%(outfile_name);

   filelist='kstar_%s_%05d_*'%(treename,shot);
   cmd_rm = 'rm -f *%05d_*.png'%(shot)
   os.system(cmd_rm)

   #cmd_rm = "scp " + outfile_name + " kstar@172.17.100.116:~/rtEFITMovie/rtEFIT.avi"
   #os.system(cmd_rm)

   cmd_rm = "scp " + outfile_name + " kstar@172.17.100.137:~/rtEFITMovie/rtEFIT.avi"
   os.system(cmd_rm)

   webfile_name='rt%06d.avi'%(shot);
   cmd_rm = "scp " + outfile_name + " root@172.17.100.204:/usr/local/tomcat5/webapps/kstarweb/intranet/efitmovie/movies/"+webfile_name
   os.system(cmd_rm)

            
      
         
            
#--------------------------------------------------------------------------------#
if __name__=='__main__':
   """ """
   import os, sys, time
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


   # if there is ok.txt file, then run~
   count = 0
   image_ok = 1
   while True:
        #file_ok = os.path.exists('/home/rtefitbdry/diag_TV_2012/2012C05/TV01/%06d/ok.txt'%(shot));
        file_ok = os.path.exists('/home/rtefitbdry/diag_TV_2012/2012C05/TV03/%06d/ok.txt'%(shot));
        if file_ok:
           print file_ok;
           #print 'Wait for 60 seconds'
           #time.sleep(60)
           break
        else:
           print file_ok;
           print 'Count: ', count
           count = count + 1
           #if count == 180: sys.exit("Time Out\n")
           if count == 180: 
              print 'Time out.....10 minutes'
              image_ok = 0
              break

        time.sleep(5)


   run(shot,time_i,time_f,image_ok);
   
