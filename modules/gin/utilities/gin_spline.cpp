/*==============================================================================

 Copyright 2018 by Roland Rabien, Devin Lane
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

/* "THE BEER-WARE LICENSE" (Revision 42): Devin Lane wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return. */


Spline::Spline (const juce::Array<Point<double>>& points)
{
    jassert (points.size() >= 3); // "Must have at least three points for interpolation"
    points.size ();
    int n = points.size() - 1;

    juce::Array<double> b, d, a, c, l, u, z, h;

    a.resize (n);
    b.resize (n);
    c.resize (n + 1);
    d.resize (n);
    h.resize (n + 1);
    l.resize (n + 1);
    u.resize (n + 1);
    z.resize (n + 1);

    l.set (0, 1.0);
    u.set (0, 0.0);
    z.set (0, 0.0);
    h.set (0, points[1].getX() - points[0].getX());

    for (int i = 1; i < n; i++)
    {
        h.set (i, points[i+1].getX() - points[i].getX());
        l.set (i, (2 * (points[i+1].getX() - points[i-1].getX())) - (h[i-1]) * u[i-1]);
        u.set (i, juce::approximatelyEqual (l[i], 0.0) ? 0 : h[i] / l[i]);
        a.set (i, juce::approximatelyEqual (h[i], 0.0) || juce::approximatelyEqual (h[i-1], 0.0) ? 0 : (3.0 / (h[i])) * (points[i+1].getY() - points[i].getY()) - (3.0 / (h[i-1])) * (points[i].getY() - points[i-1].getY()));
        z.set (i, juce::approximatelyEqual (l[i], 0.0) ? 0 : (a[i] - (h[i-1]) * z[i-1]) / l[i]);
    }

    l.set (n, 1.0);
    z.set (n, 0.0);
    c.set (n, 0.0);

    for (int j = n - 1; j >= 0; j--)
    {
        c.set (j, z[j] - u[j] * c[j + 1]);
        b.set (j, juce::approximatelyEqual (h[j], 0.0) ? 0 : (points[j+1].getY() - points[j].getY()) / (h[j]) - ((h[j]) * (c[j+1] + 2.0 * c[j])) / 3.0);
        d.set (j, juce::approximatelyEqual (h[j], 0.0) ? 0 : (c[j+1] - c[j]) / (3.0 * h[j]));
    }

    for (int i = 0; i < n; i++)
        elements.add (Element (points[i].getX(), points[i].getY(), b[i], c[i], d[i]));
}

double Spline::operator[] (double x) const
{
    return interpolate (x);
}

double Spline::interpolate (double x) const
{
    if (elements.size() == 0)
        return 0.0;

    int i;
    for (i = 0; i < elements.size(); i++)
    {
        if (! (elements[i] < x))
            break;
    }
    if (i != 0) i--;

    return elements[i].eval (x);
}
