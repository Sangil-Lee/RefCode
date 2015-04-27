create table SESSIONT (
		idx int(10) NOT NULL AUTO_INCREMENT,
		sesnum datetime DEFAULT NULL, 
		sestitle varchar(100) DEFAULT NULL, 
		sesleader varchar(30) DEFAULT NULL,
		sescmo varchar(30) DEFAULT NULL,
		proposal varchar(100) DEFAULT NULL,
		startshot bigint(20) DEFAULT NULL,
		endshot bigint(20) DEFAULT NULL,
		PRIMARY KEY (idx)
		)ENGINE=InnoDB DEFAULT CHARSET=euckr;

create table REMARKT (
		idx int(10) NOT NULL AUTO_INCREMENT,
		shotnum bigint(20) DEFAULT NULL, 
		writer varchar(32) DEFAULT NULL, 
		remark varchar(300) DEFAULT NULL,
		PRIMARY KEY (idx)
		)ENGINE=InnoDB DEFAULT CHARSET=euckr;


create table FAULTLISTT (
		num int(10) NOT NULL AUTO_INCREMENT,
		subsystem enum('GEP','CWF','HRS','HDS_VPS','HDS_HCS','CLS_CLB','CLS_HCS','TMS_TEMP','TMS_STRT','VMS_VV','VMS_CR','TF','PF1','PF2'
		,'PF3','PF4','PF5','PF6','PF7','ECH','ICRH','DDS1','DDS2','MD','ECE_HR','MMW_I','HAM','FS','VSS','Diag_TV','SIS'
		,'PSI','RMS','CCS','PCS','ICS','ETC') DEFAULT NULL,
		fdatetime datetime DEFAULT NULL, 
		ftype enum('HW','SW','OP','ETC') DEFAULT NULL,
		fdesc varchar(200) DEFAULT NULL,
		rdatetime datetime DEFAULT NULL,
		rdesc varchar(200) DEFAULT NULL,
		intlevel enum('L1','L2','L3','L4','WARNING','NONE') DEFAULT NULL,
		intdesc varchar(200) DEFAULT NULL,
		writer  varchar(32) DEFAULT NULL,
		comment varchar(200) DEFAULT NULL,
		refshotnum bigint(20) DEFAULT NULL, 
		wdate datetime DEFAULT NULL, 
		PRIMARY KEY (num)
		)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table FAULTLISTT (
		num int(10) NOT NULL AUTO_INCREMENT,
		sname enum('GEP','CWF','HRS','HDS_VPS','HDS_HCS','CLS_VPS','CLS_HCS','TMS_TEMP','TMS_STRT','VMS_VV',
			'VMS_CR','MPS_TF','MPS_PF1','MPS_PF2','MPS_PF3','MPS_PF4','MPS_PF5','MPS_PF6','MPS_PF7','ECH',
			'ICRH','DDS1','DDS2','MD','ECER','MMW_I','HAM','FS','VSS','DTV','SIS',
			'PSI','RMS','CCS','PCS','ICS','ETC') NOT NULL,
		scomponent varchar(64) DEFAULT NULL,
		sfunction varchar(200) DEFAULT NULL,
		fdatetime datetime DEFAULT NULL, 
		fclass enum('HW','SW','HW&SW','Operator','Sequence','B-Sag','B-Int','Utility','Installation',
			'U&C','Unknown','Non-Classified','Secondary') DEFAULT NULL,
		fname  varchar(100) DEFAULT NULL,
		fseverity int DEFAULT NULL,
		fcause  varchar(600) DEFAULT NULL,
		feffect varchar(200) DEFAULT NULL,
		rdatetime datetime DEFAULT NULL,
		rdesc varchar(200) DEFAULT NULL,
		rduration varchar(24) DEFAULT NULL,
		intlevel enum('L1','L2','L3','L4','WARNNING','NONE') DEFAULT NULL,
		intdesc varchar(200) DEFAULT NULL,
		refshotnum int DEFAULT NULL, 
		mtbf int DEFAULT NULL,
		writer  varchar(32) DEFAULT NULL,
		comment varchar(600) DEFAULT NULL,
		wdate datetime DEFAULT NULL, 
		PRIMARY KEY (num)
		)ENGINE=InnoDB DEFAULT CHARSET=utf8;

#2008_08_26
create table FAULTLISTT (
		num int(10) NOT NULL AUTO_INCREMENT,

		sname enum('CCS','CLS','CLS_HCS','CLS_VPS','CWF','DDS','DDS1','DDS2','DTV','ECER','ECH','FS','GEP','HAM',
			'HDS','HDS_HCS','HDS_RGA','HDS_VPS','HRS','ICS','ICRH','MD','MMW_I','MPS','MPS_PF','MPS_PF1','MPS_PF2','MPS_PF3','MPS_PF4',
			'MPS_PF5','MPS_PF6','MPS_PF7','MPS_TF','PCS','PSI','QDS','RMS','SIS','TMS','TMS_STRT','TMS_TEMP',
			'VMS','VMS_CR','VMS_VV','VSS','ETC') NOT NULL,

		scomponent varchar(64) DEFAULT NULL,
		sfunction varchar(200) DEFAULT NULL,
		fdatetime datetime DEFAULT NULL, 
		fclass enum('HW','SW','HW&SW','Operator','Operator&SW','Sequence','E-Sag','E-Int','Utility',
			'Installation','U&C','Experiment','Unknown','Non-Classified','Secondary') DEFAULT NULL,
		fname  varchar(100) DEFAULT NULL,
		fseverity int DEFAULT NULL,
		fcause  varchar(600) DEFAULT NULL,
		feffect varchar(200) DEFAULT NULL,
		rdatetime datetime DEFAULT NULL,
		rdesc varchar(200) DEFAULT NULL,
		rduration varchar(24) DEFAULT NULL,
		intlevel enum('L1','L2','L3','L4','WARNNING','NONE') DEFAULT NULL,
		intdesc varchar(200) DEFAULT NULL,
		refshotnum int DEFAULT NULL, 
		mtbf int DEFAULT NULL,
		writer  varchar(32) DEFAULT NULL,
		comment varchar(600) DEFAULT NULL,
		wdate datetime DEFAULT NULL, 
		PRIMARY KEY (num)
		)ENGINE=InnoDB DEFAULT CHARSET=utf8;


#table name change
alter table FAULTLISTT rename FLISTT;

insert into FAULTLISTT (num, sname,scomponent,sfunction,fdatetime,fclass,fname,fseverity,fcause,feffect,rdatetime,rdesc,rduration,
		intlevel,intdesc,refshotnum,mtbf,writer,comment,wdate) 
	values(0,1,'M27 온도센서','Temperatuer Monitoring',curdate(),1,'High Temperature',5,'Sensor fault','하드웨어 Error',curdate(),'Recover',
			300,4,'INT Level warning',1024,200,'Lee SI','Test Comment',curdate());

create table SHOTRESULTT (
		idx int(10) NOT NULL AUTO_INCREMENT,
		title varchar(100) DEFAULT NULL,
		contents varchar(500) DEFAULT NULL,
		shotnum bigint(20) DEFAULT NULL, 
		shotdate datetime DEFAULT NULL, 
		PRIMARY KEY (idx)
		)ENGINE=InnoDB DEFAULT CHARSET=utf8;

create table PLASMAMOVIET (
		idx int(10) NOT NULL AUTO_INCREMENT,
		title varchar(50) DEFAULT NULL,
		contents varchar(300) DEFAULT NULL,
		shotnum bigint(20) DEFAULT NULL, 
		shotdate datetime DEFAULT NULL, 
		shotmoviefile varchar(50) DEFAULT NULL,
		PRIMARY KEY (idx)
		)ENGINE=InnoDB DEFAULT CHARSET=utf8;

insert into FAULTLISTT (num, subsystem, fdatetime, ftype, fdesc, rdatetime, rdesc, intlevel,intdesc, comment) 
	values(0,1,curdate(),1,'HW Error',curdate(),'Recovery',3,'INT Level4','Test Comment');



