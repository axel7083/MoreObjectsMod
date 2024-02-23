#include "SM64DS_2.h"

bool BezierPathIter::Advance()
{
	Vector3 a, b, c, d, n;

	while (true)
	{
		do
		{
			a = pathPtr.GetNode(currSplineX3);
			b = pathPtr.GetNode(currSplineX3 + 1);
			c = pathPtr.GetNode(currSplineX3 + 2);
			d = pathPtr.GetNode(currSplineX3 + 3);
			
			Vec3_InterpCubic(n, a, b, c, d, currTime);
			
			Fix12i dist = n.Dist(pos);
			
			if (step <= dist)
			{
				pos -= n;
				pos *= 1._f - step / dist;
				pos += n;
				
				return true;
			}
			else if (currTime >= 1._f && pathPtr.ptr->numNodes <= currSplineX3 + 6)
			{
				// finished
				pos = pathPtr.GetNode(currSplineX3 + 3);
				return false;
			}
			
			currTime += tinyStep;
			
		} while (currTime < 1._f || pathPtr.ptr->numNodes <= currSplineX3 + 6);
		
		do
		{
			currTime -= 1._f;
			currSplineX3 += 3;
			
			if (currTime < 1._f)
				break;
			
		} while (currSplineX3 + 6 < pathPtr.ptr->numNodes);
	}
}