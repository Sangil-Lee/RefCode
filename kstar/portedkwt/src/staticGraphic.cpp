/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "staticGraphic.h"


class StaticGraphic::PrivateData
{
public:
    PrivateData():
		m_rf(QRectF( 2, 2, 46, 46)),
		m_gtype(Ellipse),
		m_gdir(Default),
		m_fillMode(Empty),
		m_fcol0(QColor(0,255,0)),
		//m_lcol(QColor(68,142,144)),
		m_lwid(2),
		m_blink(false),
		m_blinkstatus(false)
    {
    };

	QRectF m_rf;
	QPainterPath m_closedpath;
	QPainterPath m_openpath;
	Geometry m_gtype;
	GeomDirection m_gdir;
	FMode m_fillMode;

	QColor m_fcol;
	QColor m_fcol0;
	QColor m_lcol;
	QColor m_lcol0;
	int m_lwid;
	int m_timerid;
	bool m_blink, m_blinkstatus;
	QLabel m_tlabel;
	QLabel m_flabel;
};

StaticGraphic::StaticGraphic(QWidget *parent)
	:QWidget(parent, Qt::FramelessWindowHint)
{
    initStaticGraphic();
}

StaticGraphic::~StaticGraphic()
{
    delete d_data;
}

QSize StaticGraphic::sizeHint() const
{
	return minimumSizeHint();
}

void StaticGraphic::resizeEvent(QResizeEvent *)
{
	d_data->m_rf = QRectF(lineWidth()*0.5, lineWidth()*0.5, width()-lineWidth(), height()-lineWidth());
	QRegion maskedRegion(0, 0, width(), height());
	setMask (maskedRegion);

	setClosedPath(geometryType());
	setOpenPath(geometryType());

//	qDebug("****************************resizeEvent");
}

QSize StaticGraphic::minimumSizeHint() const
{
	int mw = 50;
	int mh = 50;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void StaticGraphic::setClosedPath(Geometry gtype) const
{
	QRectF r = d_data->m_rf;
	double x0 = r.x();
	double y0 = r.y();
	double w = r.width();
	double h = r.height();

	QPainterPath closedPath;

	QPainterPath p_rect;
	QPainterPath p_roundrect;
	QPainterPath p_ellipse;
	QPainterPath p_rarrow;
	QPainterPath p_larrow;
	QPainterPath p_uarrow;
	QPainterPath p_darrow;
	QPainterPath p_coil;
	QPainterPath p_leftcapsule;
	QPainterPath p_rightcapsule;

	QPainterPath p_hpvalve;
	QPainterPath p_vpvalve;
	QPainterPath p_hmvalve;
	QPainterPath p_vmvalve;
	QPainterPath p_hgvalve;
	QPainterPath p_vgvalve;
	QPainterPath p_hp2valve;
	QPainterPath p_vp2valve;
	QPainterPath p_havalve;
	QPainterPath p_vavalve;
	QPainterPath p_hsvalve;
	QPainterPath p_vsvalve;
	QPainterPath p_vpump;
	QPainterPath p_tmp;
	QPainterPath p_bp;
	QPainterPath p_drp;
	QPainterPath p_vgauge;
	QPainterPath p_vchamber;
	QPainterPath p_vv;
	QPainterPath p_difp;

	p_rect.addRect(r);
	p_roundrect.addRect(r);
	//p_ellipse.addEllipse(QRect(0,0,1,1));
	p_ellipse.addEllipse(r);
//	qDebug("width():%f, height():%f", r.width(), r.height());

	// Right Arrow
	p_rarrow.moveTo(x0, 0.3*h);
	p_rarrow.lineTo(0.5*w, 0.3*h);
	p_rarrow.lineTo(0.5*w, y0);
	p_rarrow.lineTo(x0+w, 0.5*h);
	p_rarrow.lineTo(0.5*w, y0+h);
	p_rarrow.lineTo(0.5*w, 0.7*h);
	p_rarrow.lineTo(x0, 0.7*h);
	p_rarrow.lineTo(x0, 0.3*h);

	// Left Arrow
	p_larrow.moveTo(x0+w, 0.3*h);
	p_larrow.lineTo(0.5*w, 0.3*h);
	p_larrow.lineTo(0.5*w, y0);
	p_larrow.lineTo(x0, 0.5*h);
	p_larrow.lineTo(0.5*w, y0+h);
	p_larrow.lineTo(0.5*w, 0.7*h);
	p_larrow.lineTo(x0+w, 0.7*h);
	p_larrow.lineTo(x0+w, 0.3*h);


	// Down Arrow
	p_darrow.moveTo(x0+0.3*w, y0+0.0*h);
	p_darrow.lineTo(x0+0.3*w, y0+0.5*h);
	p_darrow.lineTo(x0+0.0*w, y0+0.5*h);
	p_darrow.lineTo(x0+0.5*w, y0+1.0*h);
	p_darrow.lineTo(x0+1.0*w, y0+0.5*h);
	p_darrow.lineTo(x0+0.7*w, y0+0.5*h);
	p_darrow.lineTo(x0+0.7*w, y0+0.0*h);
	p_darrow.lineTo(x0+0.3*w, y0+0.0*h);

	// Coil
	p_coil.setFillRule(Qt::WindingFill);
	p_coil.addRect(r);
	p_coil.moveTo(x0+0.0*w, y0+0.0*h);
	p_coil.lineTo(x0+1.0*w, y0+1.0*h);
	p_coil.moveTo(x0+1.0*w, y0+0.0*h);
	p_coil.lineTo(x0+0.0*w, y0+1.0*h);

	// Left Capsule
	p_leftcapsule.moveTo(x0+0.5*w, y0+0.0*h);
	p_leftcapsule.lineTo(x0+1.0*w, y0+0.0*h);
	p_leftcapsule.lineTo(x0+1.0*w, y0+1.0*h);
	p_leftcapsule.lineTo(x0+0.5*w, y0+1.0*h);
	p_leftcapsule.arcTo(x0+0.0*w, y0+0.0*h, w, h, -90, -180);

	// Right Capsule
	p_rightcapsule.moveTo(x0+0.5*w, y0+0.0*h);
	p_rightcapsule.lineTo(x0+0.0*w, y0+0.0*h);
	p_rightcapsule.lineTo(x0+0.0*w, y0+1.0*h);
	p_rightcapsule.lineTo(x0+0.5*w, y0+1.0*h);
	p_rightcapsule.arcTo(x0+0.0*w, y0+0.0*h, w, h, -90, 180);

	// Horizontal Pneumatic Valve : Box
	p_hpvalve.moveTo(0.35*w, 0.4*h);
	p_hpvalve.lineTo(0.35*w, y0);
	p_hpvalve.lineTo(0.65*w, y0);
	p_hpvalve.lineTo(0.65*w, 0.4*h);
	p_hpvalve.lineTo(0.35*w, 0.4*h);
	// Horizontal Pneumatic Valve : Triangle 1
	p_hpvalve.moveTo(0.5*w, 0.75*h);
	p_hpvalve.lineTo(x0+w, 0.5*h);
	p_hpvalve.lineTo(x0+w, y0+h);
	p_hpvalve.lineTo(0.5*w, 0.75*h);
	// Horizontal Pneumatic Valve : Triangle 2
	p_hpvalve.moveTo(0.5*w, 0.75*h);
	p_hpvalve.lineTo(x0, 0.5*h);
	p_hpvalve.lineTo(x0, y0+h);
	p_hpvalve.lineTo(0.5*w, 0.75*h);

	// Vertical Pneumatic Valve : Box
	p_vpvalve.moveTo(0.6*w, 0.65*h);
	p_vpvalve.lineTo(x0+w, 0.65*h);
	p_vpvalve.lineTo(x0+w, 0.35*h);
	p_vpvalve.lineTo(0.6*w, 0.35*h);
	p_vpvalve.lineTo(0.6*w, 0.65*h);
	// Vertical Pneumatic Valve : Triangle 1
	p_vpvalve.moveTo(0.25*w, 0.5*h);
	p_vpvalve.lineTo(0.5*w, y0);
	p_vpvalve.lineTo(x0, y0);
	p_vpvalve.lineTo(0.25*w, 0.5*h);
	// Vertical Pneumatic Valve : Triangle 2
	p_vpvalve.moveTo(0.25*w, 0.5*h);
	p_vpvalve.lineTo(x0, y0+h);
	p_vpvalve.lineTo(0.5*w, y0+h);
	p_vpvalve.lineTo(0.25*w, 0.5*h);

	// Horizontal Manual Valve : Triangle 1
	p_hmvalve.moveTo(0.5*w, 0.75*h);
	p_hmvalve.lineTo(x0+w, 0.5*h);
	p_hmvalve.lineTo(x0+w, y0+h);
	p_hmvalve.lineTo(0.5*w, 0.75*h);
	// Horizontal Manual Valve : Triangle 2
	p_hmvalve.moveTo(0.5*w, 0.75*h);
	p_hmvalve.lineTo(x0, 0.5*h);
	p_hmvalve.lineTo(x0, y0+h);
	p_hmvalve.lineTo(0.5*w, 0.75*h);

	// Vertical Manual Valve : Triangle 1
	p_vmvalve.moveTo(0.25*w, 0.5*h);
	p_vmvalve.lineTo(0.5*w, y0);
	p_vmvalve.lineTo(x0, y0);
	p_vmvalve.lineTo(0.25*w, 0.5*h);
	// Vertical Manual Valve : Triangle 2
	p_vmvalve.moveTo(0.25*w, 0.5*h);
	p_vmvalve.lineTo(x0, y0+h);
	p_vmvalve.lineTo(0.5*w, y0+h);
	p_vmvalve.lineTo(0.25*w, 0.5*h);

	// Gate Valve
	p_hgvalve = p_hpvalve;
	p_vgvalve = p_vpvalve;
	
	// Horizontal Pneumatic2 Valve : Box
	p_hp2valve.moveTo(0.40*w, 0.47*h);
	p_hp2valve.lineTo(0.34*w, 0.15*h);
	p_hp2valve.lineTo(0.41*w, y0);
	p_hp2valve.lineTo(0.59*w, y0);
	p_hp2valve.lineTo(0.66*w, 0.15*h);
	p_hp2valve.lineTo(0.60*w, 0.47*h);
	p_hp2valve.lineTo(0.40*w, 0.47*h);
	
	// Horizontal Pneumatic2 Valve : Triangle 1
	p_hp2valve.moveTo(0.5*w, 0.75*h);
	p_hp2valve.lineTo(x0+w, 0.5*h);
	p_hp2valve.lineTo(x0+w, y0+h);
	p_hp2valve.lineTo(0.5*w, 0.75*h);
	// Horizontal Pneumatic2 Valve : Triangle 2
	p_hp2valve.moveTo(0.5*w, 0.75*h);
	p_hp2valve.lineTo(x0, 0.5*h);
	p_hp2valve.lineTo(x0, y0+h);
	p_hp2valve.lineTo(0.5*w, 0.75*h);

	// Vertical Pneumatic2 Valve : Box	
	// p_vp2valve.moveTo(x0, 0.41*h);
	// p_vp2valve.lineTo(0.15*w, 0.34*h );
	// p_vp2valve.lineTo(0.47*w, 0.4*h);
	// p_vp2valve.lineTo(0.47*w, 0.6*h);
	// p_vp2valve.lineTo(0.15*w, 0.66*h);
	// p_vp2valve.lineTo(x0, 0.59*h);
	// p_vp2valve.lineTo(x0, 0.41*h);
	// Vertical Pneumatic2 Valve : Triangle 1
	// p_vp2valve.moveTo(0.75*w, 0.5*h);
	// p_vp2valve.lineTo(0.5*w, y0+h);
	// p_vp2valve.lineTo(x0+w, y0+h);
	// p_vp2valve.lineTo(0.75*w, 0.5*h);
	// Vertical Pneumatic2 Valve : Triangle 2
	// p_vp2valve.moveTo(0.75*w, 0.5*h);
	// p_vp2valve.lineTo(0.5*w, y0);
	// p_vp2valve.lineTo(x0+w, y0);
	// p_vp2valve.lineTo(0.75*w, 0.5*h);
	// Vertical Pneumatic2 Valve : Box	
	p_vp2valve.moveTo(w, 0.41*h);
	p_vp2valve.lineTo(0.85*w, 0.34*h );
	p_vp2valve.lineTo(0.53*w, 0.4*h);
	p_vp2valve.lineTo(0.53*w, 0.6*h);
	p_vp2valve.lineTo(0.85*w, 0.66*h);
	p_vp2valve.lineTo(w, 0.59*h);
	p_vp2valve.lineTo(w, 0.41*h);
	// Vertical Pneumatic2 Valve : Triangle 1
	p_vp2valve.moveTo(0.25*w, 0.5*h);
	p_vp2valve.lineTo(0.5*w, y0+h);
	p_vp2valve.lineTo(x0, y0+h);
	p_vp2valve.lineTo(0.25*w, 0.5*h);
	// Vertical Pneumatic2 Valve : Triangle 2
	p_vp2valve.moveTo(0.25*w, 0.5*h);
	p_vp2valve.lineTo(0.5*w, y0);
	p_vp2valve.lineTo(x0, y0);
	p_vp2valve.lineTo(0.25*w, 0.5*h);

	// Horizontal Angle Valve : Triangle 1
	p_havalve.moveTo(0.25*w, y0);
	p_havalve.lineTo(0.75*w, y0);
	p_havalve.lineTo(0.5*w, 0.5*h);
	p_havalve.lineTo(0.25*w, y0);
	// Horizontal Angle Valve : Triangle 2
	p_havalve.moveTo(x0+w, 0.25*h);
	p_havalve.lineTo(x0+w, 0.75*h);
	p_havalve.lineTo(0.5*w, 0.5*h);
	p_havalve.lineTo(x0+w, 0.25*h);
	
	// Vertical Angle Valve : Triangle 1
    p_vavalve.moveTo(x0+w, 0.25*h);
	p_vavalve.lineTo(x0+w, 0.75*h);
	p_vavalve.lineTo(0.5*w, 0.5*h);
	p_vavalve.lineTo(x0+w, 0.25*h);		
	// Vertical Angle Valve : Triangle 2
	p_vavalve.moveTo(0.5*w, 0.5*h);
	p_vavalve.lineTo(0.25*w, y0+h);
	p_vavalve.lineTo(0.75*w, y0+h);
	p_vavalve.lineTo(0.5*w, 0.5*h);
		
	// Horizontal Simple Valve : Triangle 1
	p_hsvalve.moveTo(0.5*w, 0.5*h);
	p_hsvalve.lineTo(x0+w, y0);
	p_hsvalve.lineTo(x0+w, y0+h);
	p_hsvalve.lineTo(0.5*w, 0.5*h);
	// Horizontal Simple Valve : Triangle 2
	p_hsvalve.moveTo(0.5*w, 0.5*h);
	p_hsvalve.lineTo(x0, y0);
	p_hsvalve.lineTo(x0, y0+h);
	p_hsvalve.lineTo(0.5*w, 0.5*h);

	// Vertial Simple Valve : Triangle 1
	p_vsvalve.moveTo(0.5*w, 0.5*h);
	p_vsvalve.lineTo(x0, y0);
	p_vsvalve.lineTo(x0+w, y0);
	p_vsvalve.lineTo(0.5*w, 0.5*h);
	// Vertical Simple Valve : Triangle 2
	p_vsvalve.moveTo(0.5*w, 0.5*h);
	p_vsvalve.lineTo(x0, y0+h);
	p_vsvalve.lineTo(x0+w, y0+h);
	p_vsvalve.lineTo(0.5*w, 0.5*h);

	// Vacuum pump 
	p_vpump.setFillRule(Qt::WindingFill);
	p_vpump.addEllipse(r);

	// Turbo Molecular pump
	p_tmp = p_vpump;
	p_tmp.addEllipse(QRectF(x0+0.5*(1-0.4)*w, y0+0.5*(1-0.4)*h, 0.4*w, 0.4*h));
	p_tmp.addEllipse(QRectF(x0+0.5*(1-0.2)*w, y0+0.5*(1-0.2)*h, 0.2*w, 0.2*h));

	// Booster pump
	p_bp = p_vpump;
	p_bp.addEllipse(QRectF(x0+0.5*(1-0.7)*w, y0+0.5*(1-0.7)*h, 0.7*w, 0.7*h));

	// DRP
	p_drp = p_vpump;
	p_drp.addEllipse(QRectF(x0+0.5*(1-0.3)*w, y0+0.5*(1-0.3)*h, 0.3*w, 0.3*h));

	// Vacuum Gauge
	p_vgauge = p_vpump;

	// Vacuum Chamber
	p_vchamber.moveTo(x0+0.25*w, y0+0.75*h);
	p_vchamber.lineTo(x0+0.75*w, y0+0.75*h);
	p_vchamber.arcTo(QRectF(x0+0.5*w, y0+0.25*h, 0.5*w, 0.5*h), -90, 180);
	p_vchamber.lineTo(x0+0.25*w, y0+0.25*h);
	p_vchamber.arcTo(QRectF(x0, y0+0.25*h, 0.5*w, 0.5*h), 90, 180);

	// Vacuum Vessel
	p_vv.moveTo(x0+0.3*w, y0+h);
	p_vv.lineTo(x0+0.45*w, y0+h);
	p_vv.lineTo(x0+0.45*w, y0+0.9*h);
	p_vv.lineTo(x0+0.55*w, y0+0.9*h);
	p_vv.lineTo(x0+0.55*w, y0+h);
	p_vv.lineTo(x0+0.7*w, y0+h);
	p_vv.lineTo(x0+0.7*w, y0+0.9*h);
	p_vv.arcTo(QRectF(x0+0.1*w, y0+0.1*h, 0.8*w, 0.8*h), -60, 15);
	p_vv.lineTo(x0+0.9*w, y0+0.9*h);
	p_vv.lineTo(x0+0.97*w, y0+0.8*h);
	p_vv.lineTo(x0+0.85*w, y0+0.68*h);
	p_vv.arcTo(QRectF(x0+0.1*w, y0+0.1*h, 0.8*w, 0.8*h), -20, 15);
	p_vv.lineTo(x0+0.9*w, y0+0.55*h);
	p_vv.lineTo(x0+w, y0+0.55*h);
	p_vv.lineTo(x0+w, y0+0.45*h);
	p_vv.lineTo(x0+0.9*w, y0+0.45*h);

	// Diffusion Pump
	p_difp = p_vpump;
	p_difp.addEllipse(QRectF(x0+0.5*(1-0.3)*w, y0+0.5*(1-0.3)*h, 0.3*w, 0.3*h));



	switch(gtype)
	{
		case Rectangle:
			closedPath = p_rect;
			break;
		case RoundRectangle:
			closedPath = p_roundrect;
			break;
		case Ellipse:
			closedPath = p_ellipse;
			break;
		case RightArrow:
			closedPath = p_rarrow;
			break;
		case LeftArrow:
			closedPath = p_larrow;
			break;
		case DownArrow:
			closedPath = p_darrow;
			break;
		case Coil:
			closedPath = p_coil;
			break;
		case LeftCapsule:
			closedPath = p_leftcapsule;
			break;
		case RightCapsule:
			closedPath = p_rightcapsule;
			break;
		case HPValve:
			closedPath = p_hpvalve;
			break;
		case VPValve:
			closedPath = p_vpvalve;
			break;
		case HMValve:
			closedPath = p_hmvalve;
			break;
		case VMValve:
			closedPath = p_vmvalve;
			break;
		case HGValve:
			closedPath = p_hgvalve;
			break;
		case VGValve:
			closedPath = p_vgvalve;
			break;
		case HP2Valve:
			closedPath = p_hp2valve;
			break;
		case VP2Valve:
			closedPath = p_vp2valve;
			break;
		case HAValve:
			closedPath = p_havalve;
			break;
		case VAValve:
			closedPath = p_vavalve;
			break;
		case HSValve:
			closedPath = p_hsvalve;
			break;
		case VSValve:
			closedPath = p_vsvalve;
			break;
		case VacuumPump:
			closedPath = p_vpump;
			break;
		case CRP:
			closedPath = p_vpump;
			break;
		case TMP:
			closedPath = p_tmp;
			break;
		case BP:
			closedPath = p_bp;
			break;
		case DP:
			closedPath = p_vpump;
			break;
		case RP:
			closedPath = p_vpump;
			break;
		case DRP:
			closedPath = p_drp;
			break;
		case VacuumGauge:
			closedPath = p_vgauge;
			break;
		case VacuumChamber:
			closedPath = p_vchamber;
			break;
		case VacuumVessel:
			closedPath = p_vv;
			break;
		case DiffusionPump:
			closedPath = p_difp;
			break;
		default:
			break;
	}
	d_data->m_closedpath = closedPath;

//qDebug("*********************in setClosedPath");
	
}

const QPainterPath StaticGraphic::getClosedPath()
{
//	qDebug("width():%f, height():%f", d_data->m_rf.width(), d_data->m_rf.height());
	return d_data->m_closedpath;
}

void StaticGraphic::setOpenPath(Geometry gtype) const
{
	QRectF r = d_data->m_rf;
	double x0 = r.x();
	double y0 = r.y();
	double w = r.width();
	double h = r.height();
	double hw = 0.5*w;
	double hh = 0.5*h;
	double xc = 0.5*(2*x0+w);
	double yc = 0.5*(2*y0+h);

	QRectF cr = QRectF(x0+0.5*(1-0.5)*w, y0+0.5*(1-0.5)*h -0.1*h, 0.5*w, 0.5*h);
	
	double cx0 = cr.x();
	double cy0 = cr.y();
	double cw = cr.width();
	double ch = cr.height();
	double chw = 0.5*cw;
	double chh = 0.5*ch;
	double cxc = 0.5*(2*cx0+cw);
	double cyc = 0.5*(2*cy0+ch);
	double coa = 10.0; 

	QRectF br = QRectF(x0+0.5*(1-0.4)*w, y0+0.5*(1-0.4)*h, 0.4*w, 0.4*h);
	double bx0 = br.x();
	double by0 = br.y();
	double bw = br.width();
	double bh = br.height();
	double bhw = 0.5*bw;
	double bhh = 0.5*bh;
	double bxc = 0.5*(2*bx0+bw);
	double byc = 0.5*(2*by0+bh);


	QPainterPath openPath;

	QPainterPath p_hpvalveline;
	QPainterPath p_vpvalveline;
	QPainterPath p_hmvalveline;
	QPainterPath p_vmvalveline;
	QPainterPath p_hgvalveline;
	QPainterPath p_vgvalveline;
	QPainterPath p_hp2valveline;
	QPainterPath p_vp2valveline;
	QPainterPath p_havalveline;
	QPainterPath p_vavalveline;
	QPainterPath p_vpumpline;
	QPainterPath p_crpline;
	QPainterPath p_tmpline;
	QPainterPath p_bpline;
	QPainterPath p_dpline;
	QPainterPath p_rpline;
	QPainterPath p_drpline;
	QPainterPath p_vgaugeline;
	QPainterPath p_difpline;
	QPainterPath p_rdiagonalline;
	QPainterPath p_ldiagonalline;
	QPainterPath p_hline;
	QPainterPath p_vline;

	// Horizontal Pneumatic Valve : line
	p_hpvalveline.moveTo(0.5*w, 0.4*h);
	p_hpvalveline.lineTo(0.5*w, 0.75*h);

	// Vertical Pneumatic Valve : line
	p_vpvalveline.moveTo(0.6*w, 0.5*h);
	p_vpvalveline.lineTo(0.25*w, 0.5*h);

	// Horizontal Manual Valve : line
	p_hmvalveline.moveTo(0.5*w, 0.1*h);
	p_hmvalveline.lineTo(0.5*w, 0.75*h);
	p_hmvalveline.moveTo(0.25*w, 0.1*h);
	p_hmvalveline.lineTo(0.75*w, 0.1*h);

	// Vertical Manual Valve : line
	p_vmvalveline.moveTo(0.9*w, 0.5*h);
	p_vmvalveline.lineTo(0.25*w, 0.5*h);
	p_vmvalveline.moveTo(0.9*w, 0.25*h);
	p_vmvalveline.lineTo(0.9*w, 0.75*h);

	// Horizontal Gate Valve : line
	p_hgvalveline.moveTo(0.5*w, 0.4*h);
	p_hgvalveline.lineTo(0.5*w, 0.75*h);
	p_hgvalveline.moveTo(0.35*w, 0.1*h);
	p_hgvalveline.lineTo(0.65*w, 0.3*h);
	// Vertical Gate Valve : line
	p_vgvalveline.moveTo(0.6*w, 0.5*h);
	p_vgvalveline.lineTo(0.25*w, 0.5*h);
	p_vgvalveline.moveTo(0.9*w, 0.35*h);
	p_vgvalveline.lineTo(0.7*w, 0.65*h);
	
	// Horizontal Pneumatic2 Valve : line
	p_hp2valveline.moveTo(0.5*w, 0.47*h);
	p_hp2valveline.lineTo(0.5*w, 0.75*h);
	p_hp2valveline.moveTo(0.34*w, 0.15*h);
	p_hp2valveline.lineTo(0.61*w, 0.36*h);
	// Vertical Pneumatic2 Valve : line
	//p_vp2valveline.moveTo(0.47*w, 0.5*h);
	//p_vp2valveline.lineTo(0.75*w, 0.5*h);
	//p_vp2valveline.moveTo(0.15*w, 0.66*h);
	//p_vp2valveline.lineTo(0.36*w, 0.39*h);
	p_vp2valveline.moveTo(0.53*w, 0.5*h);
	p_vp2valveline.lineTo(0.25*w, 0.5*h);
	p_vp2valveline.moveTo(0.85*w, 0.66*h);
	p_vp2valveline.lineTo(0.64*w, 0.39*h);

	// Vacuum Pump : line
	p_vpumpline.moveTo(xc+0.85*hw, yc-sqrt(hh*hh*(1-0.85*0.85)));
	p_vpumpline.lineTo(xc+0.5*hw, yc+sqrt(hh*hh*(1-0.5*0.5)));
	p_vpumpline.moveTo(xc-0.85*hw, yc-sqrt(hh*hh*(1-0.85*0.85)));
	p_vpumpline.lineTo(xc-0.5*hw, yc+sqrt(hh*hh*(1-0.5*0.5)));

	// Cryo Pump : line
	p_crpline = p_vpumpline;
	p_crpline.moveTo(xc+0.5*hw, yc+sqrt(hh*hh*(1-0.5*0.5))-y0);
	p_crpline.lineTo(xc-0.5*hw, yc+sqrt(hh*hh*(1-0.5*0.5))-y0);

	//p_crpline.moveTo(cxc+0.1*chw, cyc-sqrt(chh*chh*(1-0.1*0.1)));
	//p_crpline.lineTo(cxc-sqrt(chw*chw*(1-0.1*0.1)), cyc-0.1*chh);

	p_crpline.moveTo(cxc+chw*cos((90-coa)*M_PI/180), cyc-chh*sin((90-coa)*M_PI/180));
	p_crpline.lineTo(cxc-chw*cos((90-60-coa)*M_PI/180), cyc-chh*sin((90-60-coa)*M_PI/180));

	p_crpline.moveTo(cxc-chw*cos((90-60+coa)*M_PI/180), cyc-chh*sin((90-60+coa)*M_PI/180));
	p_crpline.lineTo(cxc-chw*cos((90-60+coa)*M_PI/180), cyc+chh*sin((90-60+coa)*M_PI/180));

	p_crpline.moveTo(cxc-chw*cos((90-60-coa)*M_PI/180), cyc+chh*sin((90-60-coa)*M_PI/180));
	p_crpline.lineTo(cxc+chw*cos((90-coa)*M_PI/180), cyc+chh*sin((90-coa)*M_PI/180));

	p_crpline.moveTo(cxc-chw*cos((90-coa)*M_PI/180), cyc+chh*sin((90-coa)*M_PI/180));
	p_crpline.lineTo(cxc+chw*cos((90-60-coa)*M_PI/180), cyc+chh*sin((90-60-coa)*M_PI/180));

	p_crpline.moveTo(cxc+chw*cos((90-60+coa)*M_PI/180), cyc+chh*sin((90-60+coa)*M_PI/180));
	p_crpline.lineTo(cxc+chw*cos((90-60+coa)*M_PI/180), cyc-chh*sin((90-60+coa)*M_PI/180));

	p_crpline.moveTo(cxc+chw*cos((90-60-coa)*M_PI/180), cyc-chh*sin((90-60-coa)*M_PI/180));
	p_crpline.lineTo(cxc-chw*cos((90-coa)*M_PI/180), cyc-chh*sin((90-coa)*M_PI/180));


	// Turbo Molecular Pump : line
	p_tmpline = p_vpumpline;

	// Booster Pump : line
	p_bpline.moveTo(bxc+bhw*cos(30*M_PI/180), byc-bhh*sin(30*M_PI/180));
	//p_bpline.arcTo(br, 30, bhw*280*M_PI/180);
	p_bpline.arcTo(br, 30, 120);
	p_bpline.moveTo(bxc-bhw*cos(30*M_PI/180), byc+bhh*sin(30*M_PI/180));
	p_bpline.arcTo(br, -150, 120);
	//p_bpline.moveTo(bxc, byc);

	// Dry Pump : line
	p_dpline.moveTo(xc-sqrt(hw*hw*(1-0.85*0.85)),yc+0.85*hh);
	p_dpline.lineTo(xc+sqrt(hw*hw*(1-0.5*0.5)), yc+0.5*hh);
	p_dpline.moveTo(xc-sqrt(hw*hw*(1-0.85*0.85)), yc-0.85*hh);
	p_dpline.lineTo(xc+sqrt(hw*hw*(1-0.5*0.5)), yc-0.5*hh);

	// Rotary Pump : line
	p_rpline = p_vpumpline;
	p_rpline.moveTo(xc-0.7*hw, yc);
	p_rpline.lineTo(xc-0.3*hw, yc);
	p_rpline.moveTo(xc+0.7*hw, yc);
	p_rpline.lineTo(xc+0.3*hw, yc);

	// DRP
	p_drpline = p_vpumpline;

	// Vacuum gauge : line
	p_vgaugeline.moveTo(xc-0.5*hw, yc-0.5*hh);
	p_vgaugeline.lineTo(xc, yc+0.7*hh);
	p_vgaugeline.lineTo(xc+0.5*hw, yc-0.5*hh);
	p_vgaugeline.moveTo(xc-0.7*hw, yc-0.2*hh);
	p_vgaugeline.lineTo(xc+0.7*hw, yc-0.2*hh);

	// Diffusion pump : line
	p_difpline.moveTo(xc, yc-hh);
	p_difpline.lineTo(xc, yc-0.3*hh);
	p_difpline.moveTo(xc, yc+0.3*hh);
	p_difpline.lineTo(xc, yc+hh);
	p_difpline.moveTo(xc-0.5*hw, yc-0.3*hh);
	p_difpline.lineTo(xc, yc-0.7*hh);
	p_difpline.lineTo(xc+0.5*hw, yc-0.3*hh);
	/* 
	p_difpline.moveTo(xc-0.85*hw, yc-sqrt(hh*hh*(1-0.85*0.85)));
	p_difpline.lineTo(xc-0.5*hw, yc+sqrt(hh*hh*(1-0.5*0.5)));
	*/

	// Right diagonal line
	p_rdiagonalline.moveTo(x0, y0);
	p_rdiagonalline.lineTo(x0+w, y0+h);

	// Left diagonal line
	p_ldiagonalline.moveTo(x0, y0+h);
	p_ldiagonalline.lineTo(x0+w, y0);

	// Horizontal line
	p_hline.moveTo(x0, y0);
	p_hline.lineTo(x0+w, y0);

	// Vertical line
	p_vline.moveTo(x0, y0);
	p_vline.lineTo(x0, y0+h);

	switch(gtype)
	{
		case HPValve:
			openPath = p_hpvalveline;
			break;
		case VPValve:
			openPath = p_vpvalveline;
			break;
		case HMValve:
			openPath = p_hmvalveline;
			break;
		case VMValve:
			openPath = p_vmvalveline;
			break;
		case HGValve:
			openPath = p_hgvalveline;
			break;
		case VGValve:
			openPath = p_vgvalveline;
			break;
		case HP2Valve:
			openPath = p_hp2valveline;
			break;
		case VP2Valve:
			openPath = p_vp2valveline;
			break;
		case VacuumPump:
			openPath = p_vpumpline;
			break;
		case CRP:
			openPath = p_crpline;
			break;
		case TMP:
			openPath = p_tmpline;
			break;
		case BP:
			openPath = p_bpline;
			break;
		case DP:
			openPath = p_dpline;
			break;
		case RP:
			openPath = p_rpline;
			break;
		case DRP:
			openPath = p_drpline;
			break;
		case VacuumGauge:
			openPath = p_vgaugeline;
			break;
		case DiffusionPump:
			openPath = p_difpline;
			break;
		case RDiagonalLine:
			openPath = p_rdiagonalline;
			break;
		case LDiagonalLine:
			openPath = p_ldiagonalline;
			break;
		case HLine:
			openPath = p_hline;
			break;
		case VLine:
			openPath = p_vline;
			break;
		default:
			break;
	}
	d_data->m_openpath = openPath;
//qDebug("*********************in setOpenPath");
}

const QPainterPath StaticGraphic::getOpenPath()
{
	return d_data->m_openpath;
}

const QColor& StaticGraphic::fillColor() const
{
	return d_data->m_fcol;
}

void StaticGraphic::setFillColor(const QColor &fillColor)
{
	d_data->m_fcol = fillColor;
	//setFillMode(Solid);
	//memory leak(leesi)
	update();
}

const QColor& StaticGraphic::fillColor0() const
{
	return d_data->m_fcol0;
}

void StaticGraphic::setFillColor0(const QColor &fillColor)
{
	d_data->m_fcol0 = fillColor;
	d_data->m_fcol = fillColor;
	//setFillMode(Solid);
	//memory leak(leesi)
	update();
}

const QColor& StaticGraphic::lineColor() const
{
	return d_data->m_lcol;
}

void StaticGraphic::setLineColor(const QColor &lineColor)
{
	d_data->m_lcol = lineColor;
	update();
}

const QColor& StaticGraphic::lineColor0() const
{
	return d_data->m_lcol0;
}

void StaticGraphic::setLineColor0(const QColor &lineColor)
{
	d_data->m_lcol0 = lineColor;
	d_data->m_lcol = lineColor;
	update();
}

StaticGraphic::Geometry StaticGraphic::geometryType() const
{
	return (Geometry) d_data->m_gtype;
}

void StaticGraphic::setGeometryType(Geometry gtype)
{
	d_data->m_gtype = gtype;
	setClosedPath(gtype);
	setOpenPath(gtype);
	update();
}

StaticGraphic::GeomDirection StaticGraphic::geomDirection() const
{
	return (GeomDirection) d_data->m_gdir;
}

void StaticGraphic::setGeomDirection(GeomDirection gdir)
{
	d_data->m_gdir = gdir;
	update();
}

StaticGraphic::FMode StaticGraphic::fillMode() const
{
	return (FMode) d_data->m_fillMode;
}

void StaticGraphic::setFillMode(FMode fillmode)
{
	d_data->m_fillMode = fillmode;
	update();
}

const int StaticGraphic::lineWidth()
{
	return d_data->m_lwid;
}

void StaticGraphic::setLineWidth(const int lineWidth)
{
	int l = lineWidth;
	d_data->m_rf = QRectF(l*0.5, l*0.5, width()-l, height()-l);
	setClosedPath(geometryType());
	setOpenPath(geometryType());
	d_data->m_lwid = l;
	//update();
}

void StaticGraphic::initStaticGraphic()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;
	d_data->m_fcol=d_data->m_fcol0;
	d_data->m_lcol=d_data->m_lcol0;
	setGeometryType(d_data->m_gtype);
	setFillMode(d_data->m_fillMode);
	setFillColor(d_data->m_fcol);
	setVisible(true);
	//setAttribute(Qt::WA_PaintOutsidePaintEvent, true);

	setClosedPath(geometryType());
	setOpenPath(geometryType());

//	qDebug("*******************************init");

	//update();
}

void StaticGraphic::drawStaticGraphic(QPainter *p)
{
//qDebug("Debug *** 1: in drawStaticGraphic");
	// Add brush setting
	QPen pen(d_data->m_lcol, d_data->m_lwid, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	p->setPen(pen);
	p->setRenderHint(QPainter::Antialiasing);

	if (fillMode()==Solid)
	{
			p->setBrush(QBrush(fillColor(), Qt::SolidPattern));
	}
	else if (fillMode()==Empty)
	{
			p->setBrush(QBrush(QColor(0,0,0,0), Qt::SolidPattern));
	}
	p->drawPath(getClosedPath());
	p->drawPath(getOpenPath());

//qDebug("Debug *** 2: in drawStaticGraphic");
}

void StaticGraphic::paintEvent(QPaintEvent *)
{
//	qDebug("-----------------paintEvent");
	paint();
}

void StaticGraphic::paint()
{
//	qDebug("-----------------paint");
	QPainter p(this);
	drawStaticGraphic(&p);
}

void StaticGraphic::startBlink ()
{
	if(blinkStatus() != true)
	{
		setFillColor(fillColor0());
		d_data->m_timerid = startTimer(500);
		d_data->m_blinkstatus = true;
	}
}

void StaticGraphic::stopBlink ()
{
	setFillMode(Empty);

	if(blinkStatus() == true)
	{
		killTimer(d_data->m_timerid);
		d_data->m_blinkstatus = false;
	}
}

bool StaticGraphic::blinkStatus ()
{
	return d_data->m_blinkstatus;
}

bool StaticGraphic::getBlink()
{
	return d_data->m_blink;
}

void StaticGraphic::setBlink(bool blinkoption)
{
	d_data->m_blink = blinkoption;
}

void StaticGraphic::timerEvent (QTimerEvent *event)
{
	if (fillMode()==Solid)
	{
		setFillMode(Empty);
	} else if (fillMode()==Empty)
	{
		setFillMode(Solid);
	}

	// for Debuggint.
	//qDebug() << "Timer ID:" << event->timerId();
}
