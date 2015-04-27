#!/bin/sh
echo "12--->THEMALSHIELD_CTS"
./CAGetMathPV -f thermalshield_cts.txt -M -m -a
echo "13--->THEMALSHIELD_VVTS"
./CAGetMathPV -f thermalshield_vvts.txt -M -m -a
echo "14--->"
./CAGetMathPV -f tfstructure_strain.txt -M -m -a
echo "15--->"
./CAGetMathPV -f csstructure_strain.txt -M -m -a
echo "16--->"
./CAGetMathPV -f pfstructure_strain.txt -M -m -a
echo "17--->"
./CAGetMathPV -f bussupport_strain.txt -M -m -a
echo "18--->"
./CAGetMathPV -f gs_strain.txt -M -m -a
echo "19--->"
./CAGetMathPV -f leafstring_strain.txt -M -m -a
echo "20--->"
./CAGetMathPV -f trraidal_disp.txt -M -m -a
echo "21--->"
./CAGetMathPV -f trsvertical_disp.txt -M -m -a
echo "22--->"
./CAGetMathPV -f tfstructure_22.txt -M -m -a
