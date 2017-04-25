
#include "LightMap.h"
#include "BakeMesh.h"
#include "RadianceMap.h"

namespace AtomicGlow
{

RadianceMap::RadianceMap(Context* context, BakeMesh* bakeMesh) : Object(context),
    bakeMesh_(bakeMesh),    
    packed_(false)
{
    int width = bakeMesh->GetRadianceWidth();
    int height = bakeMesh->GetRadianceHeight();

    image_ = new Image(context_);
    image_->SetSize(width, height, 2, 3);

    Vector3 rad;
    int triIndex;
    Color c;

    for (unsigned y = 0; y <height; y++)
    {
        for (unsigned x = 0; x < width; x++)
        {
            if (!bakeMesh->GetRadiance(x, y, rad, triIndex))
            {
                image_->SetPixel(x, y, 0, Color::MAGENTA);
                image_->SetPixel(x, y, 1, Color::BLACK);
                continue;
            }

            if (rad.Length() > 3.0f)
            {
                rad.Normalize();
                rad *= 3.0f;
            }

            c.r_ = rad.x_;
            c.g_ = rad.y_;
            c.b_ = rad.z_;

            image_->SetPixel(x, y, c);
            // mark as a valid pixel
            image_->SetPixel(x, y, 1, Color::RED);
        }
    }

    // blur before fill
    Blur();

    const int maxDist = 7;
    int d = 1;
    while (FillInvalidPixels(d) && d <= maxDist)
    {
        d++;
    }

    //Downsample();

}

bool RadianceMap::CheckValidPixel(int x, int y, Color &color)
{

    if (x < 0 || x >= image_->GetWidth())
        return false;

    if (y < 0 || y >= image_->GetHeight())
        return false;

    color = image_->GetPixel(x, y, 1);

    if (color == Color::BLACK)
        return false;

    color = image_->GetPixel(x, y, 0);

    return true;
}

bool RadianceMap::FillInvalidPixels(int searchDistance)
{
    bool result = false;

    PODVector<Vector2> coords;

    // left
    coords.Push(Vector2(-searchDistance, 0));
    // right
    coords.Push(Vector2(searchDistance, 0));

    // down
    coords.Push(Vector2(0, searchDistance));
    // up
    coords.Push(Vector2(0, -searchDistance));

    coords.Push(Vector2(-searchDistance, -searchDistance));
    coords.Push(Vector2(searchDistance, searchDistance));
    coords.Push(Vector2(-searchDistance, searchDistance));
    coords.Push(Vector2(searchDistance, -searchDistance));


    int width = image_->GetWidth();
    int height = image_->GetHeight();

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Color c;
            HashMap<int, PODVector<Color>> colors;

            if (!CheckValidPixel(x, y , c))
            {
                // we have an unitialized pixel, search for an initialized neighbor
                for (unsigned i = 0; i< coords.Size(); i++)
                {
                    const Vector2& coord = coords[i];

                    if (CheckValidPixel(x + coord.x_, y + coord.y_, c))
                    {
                        Vector3 rad;
                        int triIndex;

                        bakeMesh_->GetRadiance(x + coord.x_, y + coord.y_, rad, triIndex);

                        // triIndex can be -1, for a previously filled pixel
                        colors[triIndex].Push(c);
                    }
                }

                if (colors.Size())
                {
                    result = true;

                    HashMap<int, PODVector<Color>>::ConstIterator itr = colors.Begin();
                    int bestTri = -2;
                    int bestCount = 0;
                    while (itr != colors.End())
                    {
                        // only consider the previous fill colors, if we don't have any
                        // valid tri colors
                        if (itr->first_ < 0)
                        {
                            itr++;
                            continue;
                        }

                        if (itr->second_.Size() > bestCount)
                        {
                            bestCount = itr->second_.Size();
                            bestTri = itr->first_;
                        }

                        itr++;
                    }

                    if (bestTri < 0)
                    {
                        if (!colors.Contains(-1))
                        {
                            // shouldn't happen
                            continue;
                        }

                        // use the previous fill as we don't have a valid tri
                        bestTri = -1;

                    }

                    const PODVector<Color>& triColors = colors[bestTri];

                    c = Color::BLACK;
                    for (unsigned i = 0; i < triColors.Size(); i++)
                    {
                        c += triColors[i];
                    }

                    c.r_ /= triColors.Size();
                    c.g_ /= triColors.Size();
                    c.b_ /= triColors.Size();

                    image_->SetPixel(x, y, c);
                    image_->SetPixel(x, y, 1, Color::RED);
                }
            }
        }
    }

    return result;
}


void RadianceMap::Blur()
{
    int width = image_->GetWidth();
    int height = image_->GetHeight();

    SharedPtr<Image> target(new Image(context_));
    target->SetSize(width, height, 2, 3);
    target->SetData(image_->GetData());

    Color color;
    float validPixels;
    int minK, maxK, minL, maxL;
    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {

            Vector3 rad;
            int destTriIndex;

            if (!bakeMesh_->GetRadiance(i, j, rad, destTriIndex))
                continue;

            color = Color::BLACK;
            validPixels = 0;

            minK = i - 1;
            if (minK < 0)
                minK = 0;
            maxK = i + 1;
            if (maxK >= width)
                maxK = width - 1;
            minL = j - 1;
            if (minL < 0)
                minL = 0;
            maxL = j + 1;
            if (maxL >= height)
                maxL = height - 1;

            for (int k = minK; k <= maxK; ++k)
            {
                for (int l = minL - 1; l < maxL; ++l)
                {

                    int tindex;
                    if (!bakeMesh_->GetRadiance(i, j, rad, tindex))
                        continue;

                    if (tindex != destTriIndex)
                        continue;

                    Color c;
                    if (!CheckValidPixel(k, l, c))
                        continue;

                    color += c;
                    ++validPixels;
                }
            }

            if (validPixels)
            {
                color.r_ /= validPixels;
                color.g_ /= validPixels;
                color.b_ /= validPixels;
                target->SetPixel(i, j, color);
            }
        }
    }

    image_->SetData(target->GetData());
}

bool RadianceMap::Downsample()
{
    // Simple average downsample gives nice results

    SharedPtr<Image> tmp(new Image(context_));
    tmp->SetSize(image_->GetWidth()/2, image_->GetHeight()/2, 3);
    tmp->Clear(Color::BLACK);

    for (int y = 0; y < tmp->GetHeight(); y++)
    {
        for (int x = 0; x < tmp->GetWidth(); x++)
        {

            int validColors = 0;

            Color c = Color::BLACK;

            Color tc = image_->GetPixel(x * 2, y * 2);
            if (tc != Color::BLACK)
            {
                c += tc;
                validColors++;
            }

            tc = image_->GetPixel(x * 2 + 1, y * 2);
            if (tc != Color::BLACK)
            {
                c += tc;
                validColors++;
            }

            tc = image_->GetPixel(x * 2, y * 2 + 1);
            if (tc != Color::BLACK)
            {
                c += tc;
                validColors++;
            }

            tc = image_->GetPixel(x * 2 + 1, y * 2 + 1);
            if (tc != Color::BLACK)
            {
                c += tc;
                validColors++;
            }

            if (!validColors)
                continue;

            c.r_ /= validColors;
            c.g_ /= validColors;
            c.b_ /= validColors;
            c.a_ = 1.0f;
            tmp->SetPixel(x, y, c);
        }

    }

    image_->SetSize(tmp->GetWidth(), tmp->GetHeight(), 3);
    image_->SetData(tmp->GetData());

    return true;


}

RadianceMap::~RadianceMap()
{

}

}
