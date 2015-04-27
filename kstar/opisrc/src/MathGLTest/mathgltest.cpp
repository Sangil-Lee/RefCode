#include <mgl2/mgl.h>
#include <mgl2/window.h>

void mgls_prepare1d(mglData *y, mglData *y1=0, mglData *y2=0, mglData *x1=0, mglData *x2=0);

void mgls_prepare1d(mglData *y, mglData *y1, mglData *y2, mglData *x1, mglData *x2)
{
	register long i,n=50;
	if(y)   y->Create(n,3);
	if(x1)  x1->Create(n);      if(x2)  x2->Create(n);
	if(y1)  y1->Create(n);      if(y2)  y2->Create(n);
	double xx;
	for(i=0;i<n;i++)
	{
		xx = i/(n-1.);
		if(y)
		{
			y->a[i] = 0.7*sin(2*M_PI*xx) + 0.5*cos(3*M_PI*xx) + 0.2*sin(M_PI*xx);
			y->a[i+n] = sin(2*M_PI*xx);
			y->a[i+2*n] = cos(2*M_PI*xx);
		}
		if(y1)  y1->a[i] = 0.5+0.3*cos(2*M_PI*xx);
		if(y2)  y2->a[i] = 0.3*sin(2*M_PI*xx);
		if(x1)  x1->a[i] = xx*2-1;
		if(x2)  x2->a[i] = 0.05+0.03*cos(2*M_PI*xx);
	}
}

int sample(mglGraph *gr)
{
	mglData y1,y2;  
	mgls_prepare1d(0,&y1,&y2);
	gr->SubPlot(2,2,0); gr->Title("Torus plot (default)");
	gr->Light(true);  gr->Rotate(50,60);  gr->Box();  gr->Torus(y1,y2);
	//if(mini)  return;

	gr->SubPlot(2,2,1); gr->Title("'x' style"); gr->Rotate(50,60);
	gr->Box();  gr->Torus(y1,y2,"x");

	gr->SubPlot(2,2,2); gr->Title("'z' style"); gr->Rotate(50,60);
	gr->Box();  gr->Torus(y1,y2,"z");

	gr->SubPlot(2,2,3); gr->Title("'\\#' style"); gr->Rotate(50,60);
	gr->Box();  gr->Torus(y1,y2,"#");
	return 0;
}

int sample_plot(mglGraph *gr)
{
	mglData y;  mgls_prepare1d(&y); gr->SetOrigin(0,0,0);
	gr->SubPlot(2,2,0,"");  gr->Title("Plot plot (default)");
	gr->Box();  gr->Plot(y);

	gr->SubPlot(2,2,2,"");  gr->Title("'!' style; 'rgb' palette");
	gr->Box();  gr->Plot(y,"o!rgb");

	gr->SubPlot(2,2,3,"");  gr->Title("just markers");
	gr->Box();  gr->Plot(y," +");

	gr->SubPlot(2,2,1); gr->Title("3d variant");
	gr->Rotate(50,60);  gr->Box();
	mglData yc(30), xc(30), z(30);  z.Modify("2*x-1");
	yc.Modify("sin(pi*(2*x-1))"); xc.Modify("cos(pi*2*x-pi)");
	gr->Plot(xc,yc,z,"rs");
	return 0;
}


int main ()
{
	//mglGraph gr;

	//gr.FPlot("sin(pi*x)");

#if 0
	mglGraph *gr = new mglGraph;
#if 0
	sample(gr);
	gr->WriteFrame("test.png");
#else
	sample_plot(gr);
	gr->WriteFrame("sample_plot.png");
#endif
#endif
	mglWindow gr(sample_plot, "MathGL example", 1);

	return gr.Run();

}
