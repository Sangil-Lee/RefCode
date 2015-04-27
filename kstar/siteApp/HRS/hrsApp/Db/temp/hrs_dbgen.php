<?

# $query=$_GET["query"];
# $table_name="hrs_sensor_name_all";
$table_name="hrs_sensor_name_all";
$query_bo="select concat('HRSBO_',sensor_name), offset, bit, datatype, concat('HRSBI_',inpvname) from ".$table_name." where recordtype like 'bo' order by offset, bit";
$query_bi="select concat('HRSBI_',sensor_name), offset, bit, datatype from ".$table_name." where recordtype like 'bi' order by offset, bit";
$query_ao="select concat('HRSAO_',sensor_name), offset, datatype, concat('HRSAI_',inpvname) from ".$table_name." where recordtype like 'ao' order by offset";
# $query_ao="select concat('HRSAO_',sensor_name), offset, datatype from ".$table_name." where recordtype like 'ao' order by offset";
$query_ai="select concat('HRSAI_',sensor_name), offset, datatype from ".$table_name." where recordtype like 'ai' order by offset";
$conn = @mysql_connect("172.17.100.101","root","kstar2004") 
or die ("Failed to connect to the server ...");

$status = mysql_select_db("signalDB");

if(!$status) {
	$errorNo = mysql_errno();
	$errorMsg = mysql_error();
	echo ("Failed to connect to the database in the server.");
	exit;
}

$fp=fopen("./hrs_io.template", "w");

if(!$fp) {
		echo("Failed to open the file");
} else {
		echo("Successfully opened the file!");
}


#############################
# Binary Input
#############################
$result = mysql_query($query_bi);
if (!$result) {
	$errorNo = mysql_errno();
	$errorMsg = mysql_error();

	echo "Error code".$errorNo.":".$errorMsg;
	exit;
}

$fields = mysql_num_fields($result);

$str="file db/s7plc_bi.db\n{\npattern\n";
fwrite($fp, $str);

$str="\t{PVNAME\t\tPLCNAME\tOFFSET\tBIT\tTYPE}\n";
fwrite($fp, $str);
$count=0;
while($row = mysql_fetch_row ($result)) {
	for ($i=0; $i<$fields; $i++) {
		if ($i==0) {
			$str="\t{".$row[$i]."\tDB1PLC";
			fwrite($fp, $str);
		} else {
			$str="\t".$row[$i];
			fwrite($fp, $str);
		}
	}
	$count++;
	$str="}\n";
	fwrite($fp, $str);
}
$str="}\n\n";
fwrite($fp, $str);


#############################
# Binary Output
#############################
$result = mysql_query($query_bo);
if (!$result) {
	$errorNo = mysql_errno();
	$errorMsg = mysql_error();

	echo "Error code".$errorNo.":".$errorMsg;
	exit;
}

$fields = mysql_num_fields($result);

$str="file db/s7plc_bo.db\n{\npattern\n";
fwrite($fp, $str);

$str="\t{PVNAME\t\tPLCNAME\tOFFSET\tBIT\tTYPE\tINPVNAME}\n";
fwrite($fp, $str);
$count=0;
while($row = mysql_fetch_row ($result)) {
	for ($i=0; $i<$fields; $i++) {
		if ($i==0) {
			$str="\t{".$row[$i]."\tDB1PLC";
			fwrite($fp, $str);
		} else {
			$str="\t".$row[$i];
			fwrite($fp, $str);
		}
	}
	$count++;
	$str="}\n";
	fwrite($fp, $str);
}
$str="}\n\n";
fwrite($fp, $str);


#############################
# Analog Input
#############################
$result = mysql_query($query_ai);
if (!$result) {
	$errorNo = mysql_errno();
	$errorMsg = mysql_error();

	echo "Error code".$errorNo.":".$errorMsg;
	exit;
}

$fields = mysql_num_fields($result);

$str="file db/s7plc_ai.db\n{\npattern\n";
fwrite($fp, $str);

$str="\t{PVNAME\t\tPLCNAME\tOFFSET\tTYPE}\n";
fwrite($fp, $str);
$count=0;
while($row = mysql_fetch_row ($result)) {
	for ($i=0; $i<$fields; $i++) {
		if ($i==0) {
			$str="\t{".$row[$i]."\tDB1PLC";
			fwrite($fp, $str);
		} else {
			$str="\t".$row[$i];
			fwrite($fp, $str);
		}
	}
	$count++;
	$str="}\n";
	fwrite($fp, $str);
}
$str="}\n\n";
fwrite($fp, $str);


#############################
# Analog Output
#############################
$result = mysql_query($query_ao);
if (!$result) {
	$errorNo = mysql_errno();
	$errorMsg = mysql_error();

	echo "Error code".$errNo.":".$errorMsg;
	exit;
}

$fields = mysql_num_fields($result);

$str="file db/s7plc_ao.db\n{\npattern\n";
fwrite($fp, $str);

$str="\t{PVNAME\t\tPLCNAME\tOFFSET\tTYPE\tINPVNAME}\n";
fwrite($fp, $str);
$count=0;
while($row = mysql_fetch_row ($result)) {
	for ($i=0; $i<$fields; $i++) {
		if ($i==0) {
			$str="\t{".$row[$i]."\tDB1PLC";
			fwrite($fp, $str);
		} else {
			$str="\t".$row[$i];
			fwrite($fp, $str);
		}
	}
	$count++;
	$str="}\n";
	fwrite($fp, $str);
}
$str="}\n\n";
fwrite($fp, $str);



fclose($fp);

mysql_close ($conn);
?>
