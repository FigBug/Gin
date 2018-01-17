bool solveLine (double x1, double y1, double x2, double y2, double* m, double* b)
{
    if (x2 != x1)
    {
        *m = (y2 - y1) / (x2 - x1);
        *b = y2 - *m * x2;

        return true;
    }
    else
    {
        *m = 0;
        *b = 0;

        return false;
    }
}

Colour goldenRatioColor (int idx)
{
    double h = std::fmod (idx * 0.618033988749895, 1);
    return Colour (float (h), 0.8f, 0.95f, 1.0f);
}

