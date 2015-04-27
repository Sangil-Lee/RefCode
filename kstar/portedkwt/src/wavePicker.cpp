#include "wavePicker.h"


WavePicker::WavePicker(CAWaveformPut *plot):QObject(plot)
{
	QwtPlotCanvas *canvas = plot->canvas(); 
	canvas->installEventFilter(this);
	canvas->setCursor(Qt::PointingHandCursor);
};

bool WavePicker::eventFilter(QObject *o, QEvent *e)
{
	if(o != (QObject*)plot()->canvas()) return false;

	QMouseEvent *pE = (QMouseEvent*)e;
	if (e->type() == QEvent::MouseButtonPress)
	{
		if(pE->button() == Qt::LeftButton ) 
		{
			select(pE->pos());
		}
		else if(pE->button() == Qt::RightButton ) 
		{
			plot()->ShowProperty(pE->pos());

			remove(pE->pos());
#if 0
			//Mouse Right Button
			if(d_data->mxData.count() == 0) return false;
			d_data->mxData.remove(d_data->mxData.count()-1);
			d_data->myData.remove(d_data->myData.count()-1);
			d_data->mpCurve->setData(d_data->mxData, d_data->myData);
#endif
		};
	}
	else if(e->type() == QEvent::MouseMove)
	{
		move(((QMouseEvent*) e) -> pos());
	}
	else if(e->type() == QEvent::MouseButtonDblClick)
	{
		if(pE->button() == Qt::LeftButton ) 
		{
			insertPoint(((QMouseEvent*) e) -> pos());
		};
	}
	else if(e->type() == QEvent::KeyPress)
	{
#if 0
		const int delta = 1;
		switch(((const QKeyEvent*)e)->key())
		{
			case Qt::Key_Up:
				movePointBy(0, -delta);
				break;

			case Qt::Key_Down:
				movePointBy(0, delta);
				break;

			case Qt::Key_Left:
				movePointBy(-delta, 0);
				break;

			case Qt::Key_Right:
				movePointBy(delta, 0);
				break;

			default:
				break;
		};
#else
		double delta = 0.001;
		switch(((const QKeyEvent*)e)->key())
		{
			case Qt::Key_Up:
				if(((const QKeyEvent*)e)->modifiers()&Qt::ShiftModifier)
					delta *= 10; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::AltModifier)
					delta *= 100; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::ControlModifier)
					delta *= 1000; 
				//else delta *= 100000; 
				else delta = plot()->getYFactor();
				movePointValue(0, delta);
				break;
			case Qt::Key_Down:
				if(((const QKeyEvent*)e)->modifiers()&Qt::ShiftModifier)
					delta *= 10; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::AltModifier)
					delta *= 100; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::ControlModifier)
					delta *= 1000; 
				//else delta *= 100000; 
				else delta = plot()->getYFactor(); 
				movePointValue(0, -delta);
				break;

			case Qt::Key_Left:
				if(((const QKeyEvent*)e)->modifiers()&Qt::ShiftModifier)
					delta *= 10; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::AltModifier)
					delta *= 100; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::ControlModifier)
					delta *= 1000; 
				else delta = plot()->getXFactor(); 
				movePointValue(-delta, 0);
				break;

			case Qt::Key_Right:
				if(((const QKeyEvent*)e)->modifiers()&Qt::ShiftModifier)
					delta *= 10; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::AltModifier)
					delta *= 100; 
				else if(((const QKeyEvent*)e)->modifiers()&Qt::ControlModifier)
					delta *= 1000; 
				else delta = plot()->getXFactor(); 
				movePointValue(delta, 0);
				break;

			default:
				break;
		};
#endif
	}
	return QObject::eventFilter(o, e);
};

// Move the selected point
void WavePicker::move(const QPoint &pos)
{
	if ( !plot()->curve() ) return;
	plot()->MovePoint(pos);
}
void WavePicker::movePointBy(int dx, int dy)
{
	if(dx == 0 && dy == 0 ) return;
	//if(!plot()->curve()) return;
	if(plot()->DataCheck() < 0 ) return;

	const int x = plot()->TransformX(dx);
	const int y = plot()->TransformY(dy);

	plot()->MovePoint(QPoint(x, y));
};

void WavePicker::movePointValue(double dx, double dy)
{
	if(dx == 0 && dy == 0 ) return;
	//if(!plot()->curve()) return;
	if(plot()->DataCheck() < 0 ) return;

	double xval = plot()->getSelectedPointXValue() + dx;
	double yval = plot()->getSelectedPointYValue() + dy;

	plot()->MoveValue(xval, yval);
};

// Select the point at a position. If there is no point
// deselect the selected point
void WavePicker::select(const QPoint &pos)
{
	QwtPlotCurve *curve = NULL;
	double dist = 10e10;
	int index = -1;

	const QwtPlotItemList& itmList = plot()->itemList();
	for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it )
	{
		if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotCurve )
		{
			QwtPlotCurve *c = (QwtPlotCurve*)(*it);
			double d;
			int idx = c->closestPoint(pos, &d);
			if ( d < dist )
			{
				curve = c;
				index = idx;
				dist = d;
			};
		};
	};

	if( curve && dist < 10)
		plot()->SetSelectedPoint(index);
	else
		plot()->SetSelectedPoint(-1);
}

void WavePicker::remove(const QPoint &pos)
{
	QwtPlotCurve *curve = NULL;
	double dist = 10e10;
	int index = -1;

	const QwtPlotItemList& itmList = plot()->itemList();
	for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it )
	{
		if ( (*it)->rtti() == QwtPlotItem::Rtti_PlotCurve )
		{
			QwtPlotCurve *c = (QwtPlotCurve*)(*it);
			double d;
			int idx = c->closestPoint(pos, &d);
			//qDebug ("index: %d, distance:%f", idx, d);
			if ( d < dist )
			{
				curve = c;
				index = idx;
				dist = d;
			};
		};
	};

	if ( curve && dist < 10 ) // 10 pixels tolerance
	{
		qDebug("Selected Point index: %d", index);
		plot()->RemovePoint(index);
	}
}

void WavePicker::insertPoint(const QPoint &pos)
{
	plot()->InsertPoint(pos);
}

bool WavePicker::event(QEvent *)
{
	return false;
};

